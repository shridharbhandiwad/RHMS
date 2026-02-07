/**
 * @file NetworkInterfaceNode.cpp
 * @brief Implementation of NetworkInterfaceNode
 */

#include "NetworkInterfaceNode.h"
#include <QDebug>

NetworkInterfaceNode::NetworkInterfaceNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("Network Interface");
    setupPorts();
    initializeProperties();
}

NetworkInterfaceNode::~NetworkInterfaceNode()
{
}

void NetworkInterfaceNode::setupPorts()
{
    // Input ports
    addInputPort("Power In", PortType::PowerInput, "power");
    addInputPort("Control", PortType::ControlInput, "control");
    addInputPort("Data In", PortType::DataInput, "network_data");
    
    // Output ports
    addOutputPort("Data Out", PortType::DataOutput, "network_data");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void NetworkInterfaceNode::initializeProperties()
{
    setProperty("link_status", "Up");
    setProperty("bandwidth_utilization", 0.0);
    setProperty("packet_loss", 0.0);
    setProperty("latency", 0);
    setProperty("error_count", 0);
    setProperty("tx_rate", 0.0);  // Mbps
    setProperty("rx_rate", 0.0);  // Mbps
}

QColor NetworkInterfaceNode::nodeColor() const
{
    if (healthStatus().isHealthy()) {
        return QColor(100, 180, 220);  // Cyan for network
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);
    } else {
        return QColor(128, 128, 128);
    }
}

void NetworkInterfaceNode::onHealthUpdate(const TelemetryPacket& packet)
{
    if (packet.hasParameter("link_status")) {
        QString linkStatus = packet.parameter("link_status").toString();
        setProperty("link_status", linkStatus);
        
        if (linkStatus != "Up") {
            qWarning() << "Network link down";
        }
    }
    
    if (packet.hasParameter("bandwidth_utilization")) {
        double bwUtil = packet.parameter("bandwidth_utilization").toDouble();
        setProperty("bandwidth_utilization", bwUtil);
        
        if (bwUtil > 85.0) {
            qWarning() << "Network bandwidth high:" << bwUtil << "%";
        }
    }
    
    if (packet.hasParameter("packet_loss")) {
        double packetLoss = packet.parameter("packet_loss").toDouble();
        setProperty("packet_loss", packetLoss);
        
        if (packetLoss > 1.0) {
            qWarning() << "Network packet loss high:" << packetLoss << "%";
        }
    }
    
    if (packet.hasParameter("latency")) {
        setProperty("latency", packet.latency());
    }
    
    if (packet.hasParameter("error_count")) {
        setProperty("error_count", packet.errorCount());
    }
}
