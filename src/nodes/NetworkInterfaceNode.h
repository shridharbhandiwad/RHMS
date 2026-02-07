/**
 * @file NetworkInterfaceNode.h
 * @brief Network interface subsystem node
 */

#ifndef NETWORKINTERFACENODE_H
#define NETWORKINTERFACENODE_H

#include "../core/SubsystemNode.h"

/**
 * @class NetworkInterfaceNode
 * @brief Network communication subsystem
 * 
 * Monitors:
 * - Link status
 * - Bandwidth utilization
 * - Packet loss rate
 * - Latency
 * - Error count
 */
class NetworkInterfaceNode : public SubsystemNode
{
    Q_OBJECT
    
public:
    explicit NetworkInterfaceNode(QObject* parent = nullptr);
    ~NetworkInterfaceNode() override;
    
    QString subsystemType() const override { return "NetworkInterface"; }
    QString subsystemCategory() const override { return "Communication"; }
    
    QColor nodeColor() const override;
    QString nodeIcon() const override { return "🌐"; }
    
protected:
    void onHealthUpdate(const TelemetryPacket& packet) override;
    
private:
    void setupPorts();
    void initializeProperties();
};

#endif // NETWORKINTERFACENODE_H
