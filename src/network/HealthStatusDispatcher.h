/**
 * @file HealthStatusDispatcher.h
 * @brief Dispatches health updates to subsystem nodes
 */

#ifndef HEALTHSTATUSDISPATCHER_H
#define HEALTHSTATUSDISPATCHER_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include "../core/TelemetryPacket.h"

class SubsystemNode;
class UdpTelemetryReceiver;

/**
 * @class HealthStatusDispatcher
 * @brief Thread-safe dispatcher for telemetry updates to nodes
 * 
 * Routes incoming telemetry packets to the appropriate subsystem nodes
 * based on subsystem ID. Maintains registry of active nodes.
 */
class HealthStatusDispatcher : public QObject
{
    Q_OBJECT
    
public:
    explicit HealthStatusDispatcher(QObject* parent = nullptr);
    ~HealthStatusDispatcher();
    
    // Node registration
    void registerNode(SubsystemNode* node);
    void unregisterNode(SubsystemNode* node);
    void unregisterNode(const QString& nodeId);
    void clearNodes();
    
    // Telemetry receiver integration
    void setTelemetryReceiver(UdpTelemetryReceiver* receiver);
    UdpTelemetryReceiver* telemetryReceiver() const { return m_receiver; }
    
    // Statistics
    quint64 packetsDispatched() const { return m_packetsDispatched; }
    quint64 packetsUnrouted() const { return m_packetsUnrouted; }
    void resetStatistics();
    
signals:
    void packetDispatched(const QString& nodeId, const TelemetryPacket& packet);
    void unroutedPacket(const TelemetryPacket& packet);
    
public slots:
    void handleTelemetryPacket(const TelemetryPacket& packet);
    
private:
    QMap<QString, SubsystemNode*> m_nodeRegistry;
    UdpTelemetryReceiver* m_receiver;
    quint64 m_packetsDispatched;
    quint64 m_packetsUnrouted;
    mutable QMutex m_mutex;
};

#endif // HEALTHSTATUSDISPATCHER_H
