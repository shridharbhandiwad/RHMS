/**
 * @file TrackerNode.h
 * @brief Tracker subsystem node
 */

#ifndef TRACKERNODE_H
#define TRACKERNODE_H

#include "../core/SubsystemNode.h"

/**
 * @class TrackerNode
 * @brief Target tracking subsystem
 * 
 * Monitors:
 * - Track count
 * - Update rate
 * - Track quality
 * - CPU load
 * - Memory usage
 */
class TrackerNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit TrackerNode(QObject* parent = nullptr);
    ~TrackerNode() override;
    
    QString subsystemType() const override { return "Tracker"; }
    QString subsystemCategory() const override { return "Processing"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "🎯"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // TRACKERNODE_H
