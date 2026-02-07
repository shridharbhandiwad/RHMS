/**
 * @file AntennaServoNode.cpp
 * @brief Implementation of AntennaServoNode
 */

#include "AntennaServoNode.h"
#include <QDebug>

AntennaServoNode::AntennaServoNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("Antenna Servo");
    setupPorts();
    initializeProperties();
}

AntennaServoNode::~AntennaServoNode()
{
}

void AntennaServoNode::setupPorts()
{
    // Input ports
    addInputPort("Power In", PortType::PowerInput, "power");
    addInputPort("Control", PortType::ControlInput, "control");
    addInputPort("Position Cmd", PortType::ControlInput, "position_cmd");
    
    // Output ports
    addOutputPort("Position", PortType::DataOutput, "position");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void AntennaServoNode::initializeProperties()
{
    setProperty("azimuth", 0.0);
    setProperty("elevation", 0.0);
    setProperty("azimuth_rate", 0.0);
    setProperty("elevation_rate", 0.0);
    setProperty("motor_current", 0.0);
    setProperty("position_error", 0.0);
    setProperty("temperature", 25.0);
}

QColor AntennaServoNode::nodeColor() const
{
    if (healthStatus().isHealthy()) {
        return QColor(150, 100, 200);  // Purple for mechanical
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);
    } else {
        return QColor(128, 128, 128);
    }
}

void AntennaServoNode::onHealthUpdate(const TelemetryPacket& packet)
{
    if (packet.hasParameter("azimuth")) {
        setProperty("azimuth", packet.parameter("azimuth"));
    }
    
    if (packet.hasParameter("elevation")) {
        setProperty("elevation", packet.parameter("elevation"));
    }
    
    if (packet.hasParameter("motor_current")) {
        double current = packet.current();
        setProperty("motor_current", current);
        
        if (current > 10.0) {  // Amps
            qWarning() << "Antenna servo motor current high:" << current << "A";
        }
    }
    
    if (packet.hasParameter("position_error")) {
        double error = packet.parameter("position_error").toDouble();
        setProperty("position_error", error);
        
        if (error > 0.5) {  // degrees
            qWarning() << "Antenna position error high:" << error << "deg";
        }
    }
    
    if (packet.hasParameter("temperature")) {
        setProperty("temperature", packet.temperature());
    }
}
