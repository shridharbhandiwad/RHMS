/**
 * @file EmbeddedControllerNode.cpp
 * @brief Implementation of EmbeddedControllerNode
 */

#include "EmbeddedControllerNode.h"
#include <QDebug>

EmbeddedControllerNode::EmbeddedControllerNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("Embedded Controller");
    setupPorts();
    initializeProperties();
}

EmbeddedControllerNode::~EmbeddedControllerNode()
{
}

void EmbeddedControllerNode::setupPorts()
{
    // Input ports
    addInputPort("Power In", PortType::PowerInput, "power");
    
    // Output ports (controller manages other subsystems)
    addOutputPort("Control Out 1", PortType::ControlOutput, "control");
    addOutputPort("Control Out 2", PortType::ControlOutput, "control");
    addOutputPort("Control Out 3", PortType::ControlOutput, "control");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void EmbeddedControllerNode::initializeProperties()
{
    setProperty("cpu_load", 0.0);
    setProperty("memory_usage", 0.0);
    setProperty("uptime", 0);  // seconds
    setProperty("watchdog_status", "OK");
    setProperty("temperature", 25.0);
    setProperty("firmware_version", "1.0.0");
}

QColor EmbeddedControllerNode::nodeColor() const
{
    if (healthStatus().isHealthy()) {
        return QColor(180, 140, 200);  // Purple for controller
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);
    } else {
        return QColor(128, 128, 128);
    }
}

void EmbeddedControllerNode::onHealthUpdate(const TelemetryPacket& packet)
{
    if (packet.hasParameter("cpu_load")) {
        double cpuLoad = packet.parameter("cpu_load").toDouble();
        setProperty("cpu_load", cpuLoad);
        
        if (cpuLoad > 95.0) {
            qWarning() << "Embedded controller CPU load critical:" << cpuLoad << "%";
        }
    }
    
    if (packet.hasParameter("memory_usage")) {
        double memUsage = packet.parameter("memory_usage").toDouble();
        setProperty("memory_usage", memUsage);
        
        if (memUsage > 90.0) {
            qWarning() << "Embedded controller memory usage high:" << memUsage << "%";
        }
    }
    
    if (packet.hasParameter("uptime")) {
        setProperty("uptime", packet.parameter("uptime"));
    }
    
    if (packet.hasParameter("watchdog_status")) {
        QString watchdogStatus = packet.parameter("watchdog_status").toString();
        setProperty("watchdog_status", watchdogStatus);
        
        if (watchdogStatus != "OK") {
            qWarning() << "Watchdog status:" << watchdogStatus;
        }
    }
    
    if (packet.hasParameter("temperature")) {
        setProperty("temperature", packet.temperature());
    }
}
