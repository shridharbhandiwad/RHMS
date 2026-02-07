/**
 * @file CoolingSystemNode.h
 * @brief Cooling system subsystem node
 */

#ifndef COOLINGSYSTEMNODE_H
#define COOLINGSYSTEMNODE_H

#include "../core/SubsystemNode.h"

/**
 * @class CoolingSystemNode
 * @brief Thermal management subsystem
 * 
 * Monitors:
 * - Fan speed
 * - Coolant temperature
 * - Flow rate
 * - Pump status
 * - Ambient temperature
 */
class CoolingSystemNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit CoolingSystemNode(QObject* parent = nullptr);
    ~CoolingSystemNode() override;
    
    QString subsystemType() const override { return "CoolingSystem"; }
    QString subsystemCategory() const override { return "Thermal"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "❄️"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // COOLINGSYSTEMNODE_H
