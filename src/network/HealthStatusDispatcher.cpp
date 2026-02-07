/**
 * @file HealthStatusDispatcher.cpp
 * @brief Implementation of health status dispatcher
 */

#include "HealthStatusDispatcher.h"
#include "UdpTelemetryReceiver.h"
#include "../core/SubsystemNode.h"
#include <QMutexLocker>
#include <QDebug>

HealthStatusDispatcher::HealthStatusDispatcher(QObject* parent)
    : QObject(parent)
    , m_receiver(nullptr)
    , m_packetsDispatched(0)
    , m_packetsUnrouted(0)
{
}

HealthStatusDispatcher::~HealthStatusDispatcher()
{
    clearNodes();
}

void HealthStatusDispatcher::registerNode(SubsystemNode* node)
{
    if (!node) {
        qWarning() << "Cannot register null node";
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    QString nodeId = node->nodeId();
    if (m_nodeRegistry.contains(nodeId)) {
        qWarning() << "Node already registered:" << nodeId;
        return;
    }
    
    m_nodeRegistry[nodeId] = node;
    qDebug() << "Registered node for telemetry:" << nodeId << node->nodeName();
}

void HealthStatusDispatcher::unregisterNode(SubsystemNode* node)
{
    if (!node) {
        return;
    }
    
    unregisterNode(node->nodeId());
}

void HealthStatusDispatcher::unregisterNode(const QString& nodeId)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_nodeRegistry.remove(nodeId) > 0) {
        qDebug() << "Unregistered node:" << nodeId;
    }
}

void HealthStatusDispatcher::clearNodes()
{
    QMutexLocker locker(&m_mutex);
    m_nodeRegistry.clear();
    qDebug() << "Cleared all registered nodes";
}

void HealthStatusDispatcher::setTelemetryReceiver(UdpTelemetryReceiver* receiver)
{
    // Disconnect old receiver
    if (m_receiver) {
        disconnect(m_receiver, &UdpTelemetryReceiver::telemetryReceived,
                   this, &HealthStatusDispatcher::handleTelemetryPacket);
    }
    
    m_receiver = receiver;
    
    // Connect new receiver
    if (m_receiver) {
        connect(m_receiver, &UdpTelemetryReceiver::telemetryReceived,
                this, &HealthStatusDispatcher::handleTelemetryPacket,
                Qt::QueuedConnection);
        qInfo() << "Connected telemetry receiver to dispatcher";
    }
}

void HealthStatusDispatcher::resetStatistics()
{
    QMutexLocker locker(&m_mutex);
    m_packetsDispatched = 0;
    m_packetsUnrouted = 0;
}

void HealthStatusDispatcher::handleTelemetryPacket(const TelemetryPacket& packet)
{
    if (!packet.isValid()) {
        qWarning() << "Received invalid telemetry packet";
        return;
    }
    
    QString subsystemId = packet.subsystemId();
    SubsystemNode* targetNode = nullptr;
    
    {
        QMutexLocker locker(&m_mutex);
        targetNode = m_nodeRegistry.value(subsystemId, nullptr);
    }
    
    if (targetNode) {
        // Dispatch to node (thread-safe via signal/slot)
        QMetaObject::invokeMethod(targetNode, [targetNode, packet]() {
            targetNode->updateHealth(packet);
        }, Qt::QueuedConnection);
        
        {
            QMutexLocker locker(&m_mutex);
            m_packetsDispatched++;
        }
        
        emit packetDispatched(subsystemId, packet);
    } else {
        {
            QMutexLocker locker(&m_mutex);
            m_packetsUnrouted++;
        }
        
        qDebug() << "No registered node for subsystem:" << subsystemId;
        emit unroutedPacket(packet);
    }
}
