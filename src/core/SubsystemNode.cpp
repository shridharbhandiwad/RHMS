/**
 * @file SubsystemNode.cpp
 * @brief Implementation of SubsystemNode base class
 */

#include "SubsystemNode.h"
#include "../graph/NodeGraphScene.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

SubsystemNode::SubsystemNode(QObject* parent)
    : QObject(parent)
    , m_nodeId(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_nodeName("Unnamed Subsystem")
    , m_hasChildGraph(false)
    , m_expanded(false)
{
    // Initialize with unknown health status
    m_healthStatus = HealthStatus(HealthCode::UNKNOWN, "Node created");
}

SubsystemNode::~SubsystemNode()
{
}

void SubsystemNode::setNodeName(const QString& name)
{
    if (m_nodeName != name) {
        m_nodeName = name;
        emit nodeNameChanged(name);
    }
}

void SubsystemNode::setHealthStatus(const HealthStatus& status)
{
    m_healthStatus = status;
    emit healthStatusChanged(status);
}

void SubsystemNode::updateHealth(const TelemetryPacket& packet)
{
    // Update health status from telemetry packet
    m_healthStatus.update(packet.healthCode(), packet.healthMessage());
    m_telemetryData = packet;
    
    // Call virtual method for derived class customization
    onHealthUpdate(packet);
    
    emit healthStatusChanged(m_healthStatus);
    emit telemetryUpdated(packet);
}

void SubsystemNode::updateHealth(HealthCode code, const QString& message)
{
    m_healthStatus.update(code, message);
    emit healthStatusChanged(m_healthStatus);
}

void SubsystemNode::addInputPort(const QString& name, PortType type, const QString& dataType)
{
    QString portId = generatePortId(name);
    NodePort port(name, type, dataType);
    m_inputPorts[portId] = port;
}

void SubsystemNode::addOutputPort(const QString& name, PortType type, const QString& dataType)
{
    QString portId = generatePortId(name);
    NodePort port(name, type, dataType);
    m_outputPorts[portId] = port;
}

NodePort* SubsystemNode::getInputPort(const QString& name)
{
    QString portId = generatePortId(name);
    if (m_inputPorts.contains(portId)) {
        return &m_inputPorts[portId];
    }
    return nullptr;
}

NodePort* SubsystemNode::getOutputPort(const QString& name)
{
    QString portId = generatePortId(name);
    if (m_outputPorts.contains(portId)) {
        return &m_outputPorts[portId];
    }
    return nullptr;
}

bool SubsystemNode::canConnectTo(const SubsystemNode* other, 
                                 const QString& fromPort, 
                                 const QString& toPort) const
{
    if (!other) return false;
    
    // Find the output port on this node
    QString fromId = generatePortId(fromPort);
    if (!m_outputPorts.contains(fromId)) {
        return false;
    }
    
    // Find the input port on the other node
    QString toId = other->generatePortId(toPort);
    const auto& otherInputs = other->m_inputPorts;
    if (!otherInputs.contains(toId)) {
        return false;
    }
    
    // Check if data types are compatible
    const NodePort& outputPort = m_outputPorts[fromId];
    const NodePort& inputPort = otherInputs[toId];
    
    // "any" type can connect to anything
    if (outputPort.dataType == "any" || inputPort.dataType == "any") {
        return true;
    }
    
    return outputPort.dataType == inputPort.dataType;
}

void SubsystemNode::createChildGraph()
{
    if (!m_childGraph) {
        m_childGraph = std::make_unique<NodeGraphScene>(this);
        m_hasChildGraph = true;
    }
}

void SubsystemNode::setExpanded(bool expanded)
{
    if (m_expanded != expanded) {
        m_expanded = expanded;
        emit expandedStateChanged(expanded);
    }
}

void SubsystemNode::setProperty(const QString& key, const QVariant& value)
{
    m_properties[key] = value;
    onPropertyChanged(key, value);
    emit propertyChanged(key, value);
}

QVariant SubsystemNode::property(const QString& key) const
{
    return m_properties.value(key, QVariant());
}

void SubsystemNode::bindTelemetryPacket(const TelemetryPacket& packet)
{
    m_telemetryData = packet;
    updateHealth(packet);
}

QColor SubsystemNode::nodeColor() const
{
    // Default color based on health status
    return m_healthStatus.statusColor();
}

QString SubsystemNode::serialize() const
{
    QJsonObject json;
    json["nodeId"] = m_nodeId;
    json["nodeName"] = m_nodeName;
    json["subsystemType"] = subsystemType();
    json["healthStatus"] = m_healthStatus.serialize();
    json["hasChildGraph"] = m_hasChildGraph;
    json["expanded"] = m_expanded;
    
    // Serialize properties
    QJsonObject props;
    for (auto it = m_properties.begin(); it != m_properties.end(); ++it) {
        props[it.key()] = QJsonValue::fromVariant(it.value());
    }
    json["properties"] = props;
    
    // Serialize ports
    QJsonArray inputPorts;
    for (const auto& port : m_inputPorts) {
        QJsonObject portObj;
        portObj["name"] = port.name;
        portObj["type"] = static_cast<int>(port.type);
        portObj["dataType"] = port.dataType;
        inputPorts.append(portObj);
    }
    json["inputPorts"] = inputPorts;
    
    QJsonArray outputPorts;
    for (const auto& port : m_outputPorts) {
        QJsonObject portObj;
        portObj["name"] = port.name;
        portObj["type"] = static_cast<int>(port.type);
        portObj["dataType"] = port.dataType;
        outputPorts.append(portObj);
    }
    json["outputPorts"] = outputPorts;
    
    QJsonDocument doc(json);
    return doc.toJson(QJsonDocument::Compact);
}

bool SubsystemNode::deserialize(const QString& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }
    
    QJsonObject json = doc.object();
    m_nodeId = json["nodeId"].toString();
    m_nodeName = json["nodeName"].toString();
    m_hasChildGraph = json["hasChildGraph"].toBool();
    m_expanded = json["expanded"].toBool();
    
    // Deserialize health status
    QString healthData = json["healthStatus"].toString();
    m_healthStatus = HealthStatus::deserialize(healthData);
    
    // Deserialize properties
    QJsonObject props = json["properties"].toObject();
    for (auto it = props.begin(); it != props.end(); ++it) {
        m_properties[it.key()] = it.value().toVariant();
    }
    
    return true;
}

void SubsystemNode::onHealthUpdate(const TelemetryPacket& packet)
{
    // Default implementation - derived classes can override
    Q_UNUSED(packet)
}

void SubsystemNode::onPropertyChanged(const QString& key, const QVariant& value)
{
    // Default implementation - derived classes can override
    Q_UNUSED(key)
    Q_UNUSED(value)
}

QString SubsystemNode::generatePortId(const QString& portName) const
{
    return portName.toLower().replace(" ", "_");
}
