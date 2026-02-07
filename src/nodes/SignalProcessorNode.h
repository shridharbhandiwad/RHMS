/**
 * @file SignalProcessorNode.h
 * @brief Signal Processor subsystem node
 */

#ifndef SIGNALPROCESSORNODE_H
#define SIGNALPROCESSORNODE_H

#include "../core/SubsystemNode.h"

/**
 * @class SignalProcessorNode
 * @brief Signal processing subsystem for radar data
 * 
 * Monitors:
 * - Processing load
 * - Latency
 * - Buffer utilization
 * - Temperature
 * - Error rate
 */
class SignalProcessorNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit SignalProcessorNode(QObject* parent = nullptr);
    ~SignalProcessorNode() override;
    
    QString subsystemType() const override { return "SignalProcessor"; }
    QString subsystemCategory() const override { return "Processing"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "⚙️"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // SIGNALPROCESSORNODE_H
