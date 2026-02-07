/**
 * @file PowerSupplyNode.cpp
 * @brief Implementation of PowerSupplyNode
 */

#include "PowerSupplyNode.h"
#include <QDebug>

PowerSupplyNode::PowerSupplyNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("Power Supply");
    setupPorts();
    initializeProperties();
}

PowerSupplyNode::~PowerSupplyNode()
{
}

void PowerSupplyNode::setupPorts()
{
    // Input ports
    addInputPort("AC Input", PortType::PowerInput, "ac_power");
    addInputPort("Control", PortType::ControlInput, "control");
    
    // Output ports (multiple power rails)
    addOutputPort("28V Out", PortType::PowerOutput, "power");
    addOutputPort("12V Out", PortType::PowerOutput, "power");
    addOutputPort("5V Out", PortType::PowerOutput, "power");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void PowerSupplyNode::initializeProperties()
{
    setProperty("voltage_28v", 28.0);
    setProperty("voltage_12v", 12.0);
    setProperty("voltage_5v", 5.0);
    setProperty("current", 0.0);
    setProperty("power", 0.0);
    setProperty("efficiency", 95.0);
    setProperty("temperature", 25.0);
}

QColor PowerSupplyNode::nodeColor() const
{
    if (healthStatus().isHealthy()) {
        return QColor(255, 220, 0);  // Yellow for power
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);
    } else {
        return QColor(128, 128, 128);
    }
}

void PowerSupplyNode::onHealthUpdate(const TelemetryPacket& packet)
{
    if (packet.hasParameter("voltage")) {
        double voltage = packet.voltage();
        setProperty("voltage_28v", voltage);
        
        // Check voltage regulation
        if (voltage < 26.0 || voltage > 30.0) {
            qWarning() << "Power supply voltage out of range:" << voltage << "V";
        }
    }
    
    if (packet.hasParameter("current")) {
        double current = packet.current();
        setProperty("current", current);
        
        // Calculate power
        double voltage = property("voltage_28v").toDouble();
        setProperty("power", voltage * current);
    }
    
    if (packet.hasParameter("efficiency")) {
        double efficiency = packet.parameter("efficiency").toDouble();
        setProperty("efficiency", efficiency);
        
        if (efficiency < 80.0) {
            qWarning() << "Power supply efficiency low:" << efficiency << "%";
        }
    }
    
    if (packet.hasParameter("temperature")) {
        double temp = packet.temperature();
        setProperty("temperature", temp);
        
        if (temp > 70.0) {
            qWarning() << "Power supply temperature high:" << temp << "°C";
        }
    }
}
