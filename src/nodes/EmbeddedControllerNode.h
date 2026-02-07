/**
 * @file EmbeddedControllerNode.h
 * @brief Embedded controller subsystem node
 */

#ifndef EMBEDDEDCONTROLLERNODE_H
#define EMBEDDEDCONTROLLERNODE_H

#include "../core/SubsystemNode.h"

/**
 * @class EmbeddedControllerNode
 * @brief Embedded system controller
 * 
 * Monitors:
 * - CPU load
 * - Memory usage
 * - Uptime
 * - Watchdog status
 * - Temperature
 */
class EmbeddedControllerNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit EmbeddedControllerNode(QObject* parent = nullptr);
    ~EmbeddedControllerNode() override;
    
    QString subsystemType() const override { return "EmbeddedController"; }
    QString subsystemCategory() const override { return "Control"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "🖥️"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // EMBEDDEDCONTROLLERNODE_H
