/**
 * @file RFFrontendNode.cpp
 * @brief Implementation of RFFrontendNode
 */

#include "RFFrontendNode.h"
#include <QDebug>

RFFrontendNode::RFFrontendNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("RF Frontend");
    setupPorts();
    initializeProperties();
}

RFFrontendNode::~RFFrontendNode()
{
}

void RFFrontendNode::setupPorts()
{
    // Input ports
    addInputPort("Power In", PortType::PowerInput, "power");
    addInputPort("Control", PortType::ControlInput, "control");
    addInputPort("IF Signal", PortType::SignalInput, "if_signal");
    
    // Output ports
    addOutputPort("RF Out", PortType::SignalOutput, "rf_signal");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void RFFrontendNode::initializeProperties()
{
    setProperty("frequency", 9.5e9);  // 9.5 GHz (X-band)
    setProperty("tx_power", 100.0);   // 100W
    setProperty("rx_sensitivity", -110.0);  // -110 dBm
    setProperty("temperature", 25.0);
    setProperty("vswr", 1.5);
}

QColor RFFrontendNode::nodeColor() const
{
    // Color based on health, but use RF-specific base color
    if (healthStatus().isHealthy()) {
        return QColor(80, 120, 255);  // Blue for RF
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);   // Orange
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);   // Red
    } else {
        return QColor(128, 128, 128); // Gray
    }
}

void RFFrontendNode::onHealthUpdate(const TelemetryPacket& packet)
{
    // Update RF-specific properties from telemetry
    if (packet.hasParameter("frequency")) {
        setProperty("frequency", packet.parameter("frequency"));
    }
    if (packet.hasParameter("tx_power")) {
        setProperty("tx_power", packet.parameter("tx_power"));
    }
    if (packet.hasParameter("temperature")) {
        setProperty("temperature", packet.parameter("temperature"));
        
        // Check temperature limits
        double temp = packet.temperature();
        if (temp > 85.0) {
            qWarning() << "RF Frontend temperature critical:" << temp;
        }
    }
    if (packet.hasParameter("vswr")) {
        setProperty("vswr", packet.parameter("vswr"));
        
        // Check VSWR
        double vswr = packet.parameter("vswr").toDouble();
        if (vswr > 2.0) {
            qWarning() << "RF Frontend VSWR high:" << vswr;
        }
    }
}
