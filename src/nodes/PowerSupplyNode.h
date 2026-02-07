/**
 * @file PowerSupplyNode.h
 * @brief Power supply subsystem node
 */

#ifndef POWERSUPPLYNODE_H
#define POWERSUPPLYNODE_H

#include "../core/SubsystemNode.h"

/**
 * @class PowerSupplyNode
 * @brief Power distribution subsystem
 * 
 * Monitors:
 * - Output voltage
 * - Output current
 * - Power consumption
 * - Efficiency
 * - Temperature
 */
class PowerSupplyNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit PowerSupplyNode(QObject* parent = nullptr);
    ~PowerSupplyNode() override;
    
    QString subsystemType() const override { return "PowerSupply"; }
    QString subsystemCategory() const override { return "Power"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "⚡"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // POWERSUPPLYNODE_H
