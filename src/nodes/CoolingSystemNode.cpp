/**
 * @file CoolingSystemNode.cpp
 * @brief Implementation of CoolingSystemNode
 */

#include "CoolingSystemNode.h"
#include <QDebug>

CoolingSystemNode::CoolingSystemNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("Cooling System");
    setupPorts();
    initializeProperties();
}

CoolingSystemNode::~CoolingSystemNode()
{
}

void CoolingSystemNode::setupPorts()
{
    // Input ports
    addInputPort("Power In", PortType::PowerInput, "power");
    addInputPort("Control", PortType::ControlInput, "control");
    
    // Output ports
    addOutputPort("Cooling", PortType::DataOutput, "thermal");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void CoolingSystemNode::initializeProperties()
{
    setProperty("fan_speed", 0);  // RPM
    setProperty("coolant_temp", 25.0);
    setProperty("flow_rate", 0.0);  // L/min
    setProperty("pump_status", "Running");
    setProperty("ambient_temp", 25.0);
}

QColor CoolingSystemNode::nodeColor() const
{
    if (healthStatus().isHealthy()) {
        return QColor(100, 200, 255);  // Light blue for cooling
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);
    } else {
        return QColor(128, 128, 128);
    }
}

void CoolingSystemNode::onHealthUpdate(const TelemetryPacket& packet)
{
    if (packet.hasParameter("fan_speed")) {
        int fanSpeed = packet.parameter("fan_speed").toInt();
        setProperty("fan_speed", fanSpeed);
        
        if (fanSpeed < 500) {
            qWarning() << "Cooling fan speed low:" << fanSpeed << "RPM";
        }
    }
    
    if (packet.hasParameter("coolant_temp")) {
        double coolantTemp = packet.parameter("coolant_temp").toDouble();
        setProperty("coolant_temp", coolantTemp);
        
        if (coolantTemp > 60.0) {
            qWarning() << "Coolant temperature high:" << coolantTemp << "°C";
        }
    }
    
    if (packet.hasParameter("flow_rate")) {
        double flowRate = packet.parameter("flow_rate").toDouble();
        setProperty("flow_rate", flowRate);
        
        if (flowRate < 1.0) {
            qWarning() << "Coolant flow rate low:" << flowRate << "L/min";
        }
    }
    
    if (packet.hasParameter("pump_status")) {
        QString pumpStatus = packet.parameter("pump_status").toString();
        setProperty("pump_status", pumpStatus);
        
        if (pumpStatus != "Running") {
            qWarning() << "Cooling pump not running:" << pumpStatus;
        }
    }
    
    if (packet.hasParameter("ambient_temp")) {
        setProperty("ambient_temp", packet.parameter("ambient_temp"));
    }
}
