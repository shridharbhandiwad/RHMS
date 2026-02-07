/**
 * @file DataFusionNode.h
 * @brief Data fusion subsystem node
 */

#ifndef DATAFUSIONNODE_H
#define DATAFUSIONNODE_H

#include "../core/SubsystemNode.h"

/**
 * @class DataFusionNode
 * @brief Multi-sensor data fusion subsystem
 * 
 * Monitors:
 * - Input sources count
 * - Fusion quality
 * - Processing latency
 * - CPU load
 * - Output rate
 */
class DataFusionNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit DataFusionNode(QObject* parent = nullptr);
    ~DataFusionNode() override;
    
    QString subsystemType() const override { return "DataFusion"; }
    QString subsystemCategory() const override { return "Processing"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "🔗"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // DATAFUSIONNODE_H
