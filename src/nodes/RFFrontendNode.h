/**
 * @file RFFrontendNode.h
 * @brief RF Frontend subsystem node
 */

#ifndef RFFRONTENDNODE_H
#define RFFRONTENDNODE_H

#include "../core/SubsystemNode.h"

/**
 * @class RFFrontendNode
 * @brief RF Frontend subsystem for radar signal transmission/reception
 * 
 * Monitors:
 * - Transmit power
 * - Receiver sensitivity
 * - Frequency stability
 * - Temperature
 * - VSWR (Voltage Standing Wave Ratio)
 */
class RFFrontendNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit RFFrontendNode(QObject* parent = nullptr);
    ~RFFrontendNode() override;
    
    QString subsystemType() const override { return "RFFrontend"; }
    QString subsystemCategory() const override { return "RF Systems"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "📡"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // RFFRONTENDNODE_H
