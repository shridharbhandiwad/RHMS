/**
 * @file SignalProcessorNode.cpp
 * @brief Implementation of SignalProcessorNode
 */

#include "SignalProcessorNode.h"
#include <QDebug>

SignalProcessorNode::SignalProcessorNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("Signal Processor");
    setupPorts();
    initializeProperties();
}

SignalProcessorNode::~SignalProcessorNode()
{
}

void SignalProcessorNode::setupPorts()
{
    // Input ports
    addInputPort("Power In", PortType::PowerInput, "power");
    addInputPort("Control", PortType::ControlInput, "control");
    addInputPort("RF Data In", PortType::DataInput, "rf_data");
    
    // Output ports
    addOutputPort("Processed Data", PortType::DataOutput, "processed_data");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void SignalProcessorNode::initializeProperties()
{
    setProperty("cpu_load", 0.0);
    setProperty("latency", 0);
    setProperty("buffer_utilization", 0.0);
    setProperty("temperature", 25.0);
    setProperty("error_rate", 0.0);
    setProperty("throughput", 0.0);
}

QColor SignalProcessorNode::nodeColor() const
{
    if (healthStatus().isHealthy()) {
        return QColor(120, 200, 120);  // Green for processing
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);
    } else {
        return QColor(128, 128, 128);
    }
}

void SignalProcessorNode::onHealthUpdate(const TelemetryPacket& packet)
{
    // Update processing-specific properties
    if (packet.hasParameter("cpu_load")) {
        double load = packet.parameter("cpu_load").toDouble();
        setProperty("cpu_load", load);
        
        if (load > 90.0) {
            qWarning() << "Signal processor CPU load critical:" << load << "%";
        }
    }
    
    if (packet.hasParameter("latency")) {
        int latency = packet.latency();
        setProperty("latency", latency);
        
        if (latency > 100) {  // ms
            qWarning() << "Signal processor latency high:" << latency << "ms";
        }
    }
    
    if (packet.hasParameter("buffer_utilization")) {
        double bufferUtil = packet.parameter("buffer_utilization").toDouble();
        setProperty("buffer_utilization", bufferUtil);
        
        if (bufferUtil > 85.0) {
            qWarning() << "Signal processor buffer utilization high:" << bufferUtil << "%";
        }
    }
    
    if (packet.hasParameter("temperature")) {
        setProperty("temperature", packet.temperature());
    }
    
    if (packet.hasParameter("error_rate")) {
        setProperty("error_rate", packet.parameter("error_rate"));
    }
}
