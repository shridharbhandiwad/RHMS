/**
 * @file AntennaServoNode.h
 * @brief Antenna servo control subsystem node
 */

#ifndef ANTENNASERVONODE_H
#define ANTENNASERVONODE_H

#include "../core/SubsystemNode.h"

/**
 * @class AntennaServoNode
 * @brief Antenna positioning and servo control
 * 
 * Monitors:
 * - Azimuth position
 * - Elevation position
 * - Servo motor current
 * - Position error
 * - Temperature
 */
class AntennaServoNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit AntennaServoNode(QObject* parent = nullptr);
    ~AntennaServoNode() override;
    
    QString subsystemType() const override { return "AntennaServo"; }
    QString subsystemCategory() const override { return "Mechanical"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "📐"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // ANTENNASERVONODE_H
