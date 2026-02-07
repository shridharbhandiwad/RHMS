/**
 * @file TrackerNode.cpp
 * @brief Implementation of TrackerNode
 */

#include "TrackerNode.h"
#include <QDebug>

TrackerNode::TrackerNode(QObject* parent)
    : SubsystemNode(parent)
{
    setNodeName("Tracker");
    setupPorts();
    initializeProperties();
}

TrackerNode::~TrackerNode()
{
}

void TrackerNode::setupPorts()
{
    // Input ports
    addInputPort("Power In", PortType::PowerInput, "power");
    addInputPort("Control", PortType::ControlInput, "control");
    addInputPort("Detection Data", PortType::DataInput, "detection_data");
    
    // Output ports
    addOutputPort("Track Data", PortType::DataOutput, "track_data");
    addOutputPort("Status", PortType::DataOutput, "status");
    addOutputPort("Telemetry", PortType::DataOutput, "telemetry");
}

void TrackerNode::initializeProperties()
{
    setProperty("track_count", 0);
    setProperty("update_rate", 10.0);  // Hz
    setProperty("track_quality", 100.0);
    setProperty("cpu_load", 0.0);
    setProperty("memory_usage", 0.0);
    setProperty("max_tracks", 200);
}

QColor TrackerNode::nodeColor() const
{
    if (healthStatus().isHealthy()) {
        return QColor(255, 200, 80);  // Yellow/Orange for tracker
    } else if (healthStatus().isWarning()) {
        return QColor(255, 165, 0);
    } else if (healthStatus().isError()) {
        return QColor(255, 60, 60);
    } else {
        return QColor(128, 128, 128);
    }
}

void TrackerNode::onHealthUpdate(const TelemetryPacket& packet)
{
    if (packet.hasParameter("track_count")) {
        int trackCount = packet.parameter("track_count").toInt();
        setProperty("track_count", trackCount);
        
        int maxTracks = property("max_tracks").toInt();
        if (trackCount > maxTracks * 0.9) {
            qWarning() << "Tracker approaching maximum capacity:" 
                      << trackCount << "/" << maxTracks;
        }
    }
    
    if (packet.hasParameter("update_rate")) {
        setProperty("update_rate", packet.parameter("update_rate"));
    }
    
    if (packet.hasParameter("track_quality")) {
        double quality = packet.parameter("track_quality").toDouble();
        setProperty("track_quality", quality);
        
        if (quality < 70.0) {
            qWarning() << "Track quality degraded:" << quality << "%";
        }
    }
    
    if (packet.hasParameter("cpu_load")) {
        setProperty("cpu_load", packet.parameter("cpu_load"));
    }
    
    if (packet.hasParameter("memory_usage")) {
        setProperty("memory_usage", packet.parameter("memory_usage"));
    }
}
