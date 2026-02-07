/**
 * @file DataFusionNode.cpp
 * @brief Implementation of DataFusionNode
 */

#include "DataFusionNode.h"
#include <QDebug>

DataFusionNode::DataFusionNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("Data Fusion");
    setupPorts();
    initializeProperties();
}

DataFusionNode::~DataFusionNode()
{
}

void DataFusionNode::setupPorts()
{
    // Input ports (multiple sources)
    addInputPort("Power In", PortType::PowerInput, "power");
    addInputPort("Control", PortType::ControlInput, "control");
    addInputPort("Sensor 1", PortType::DataInput, "sensor_data");
    addInputPort("Sensor 2", PortType::DataInput, "sensor_data");
    addInputPort("Sensor 3", PortType::DataInput, "sensor_data");
    
    // Output ports
    addOutputPort("Fused Data", PortType::DataOutput, "fused_data");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void DataFusionNode::initializeProperties()
{
    setProperty("active_sources", 0);
    setProperty("fusion_quality", 100.0);
    setProperty("latency", 0);
    setProperty("cpu_load", 0.0);
    setProperty("output_rate", 10.0);  // Hz
}

QColor DataFusionNode::nodeColor() const
{
    if (healthStatus().isHealthy()) {
        return QColor(200, 150, 100);  // Brown/orange for fusion
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);
    } else {
        return QColor(128, 128, 128);
    }
}

void DataFusionNode::onHealthUpdate(const TelemetryPacket& packet)
{
    if (packet.hasParameter("active_sources")) {
        setProperty("active_sources", packet.parameter("active_sources"));
    }
    
    if (packet.hasParameter("fusion_quality")) {
        double quality = packet.parameter("fusion_quality").toDouble();
        setProperty("fusion_quality", quality);
        
        if (quality < 75.0) {
            qWarning() << "Data fusion quality degraded:" << quality << "%";
        }
    }
    
    if (packet.hasParameter("latency")) {
        setProperty("latency", packet.latency());
    }
    
    if (packet.hasParameter("cpu_load")) {
        setProperty("cpu_load", packet.parameter("cpu_load"));
    }
}
