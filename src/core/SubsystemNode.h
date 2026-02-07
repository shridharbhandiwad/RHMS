/**
 * @file SubsystemNode.h
 * @brief Abstract base class for all radar subsystem nodes
 */

#ifndef SUBSYSTEMNODE_H
#define SUBSYSTEMNODE_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QMap>
#include <QVariant>
#include <memory>
#include "HealthStatus.h"
#include "TelemetryPacket.h"

class NodeGraphScene;

/**
 * @enum PortType
 * @brief Types of data ports for node connections
 */
enum class PortType {
    DataInput,       ///< Receives data
    DataOutput,      ///< Sends data
    PowerInput,      ///< Receives power
    PowerOutput,     ///< Provides power
    SignalInput,     ///< Receives RF/signal data
    SignalOutput,    ///< Sends RF/signal data
    ControlInput,    ///< Receives control commands
    ControlOutput    ///< Sends control commands
};

/**
 * @struct NodePort
 * @brief Represents an input/output port on a subsystem node
 */
struct NodePort {
    QString name;
    PortType type;
    QString dataType;       ///< Data type identifier for validation
    bool connected;
    QVariant currentValue;
    
    NodePort() : type(PortType::DataInput), connected(false) {}
    NodePort(const QString& n, PortType t, const QString& dt = "any")
        : name(n), type(t), dataType(dt), connected(false) {}
};

/**
 * @class SubsystemNode
 * @brief Abstract base class for all radar subsystem nodes
 * 
 * Provides core functionality for:
 * - Health monitoring and status updates
 * - Hierarchical graph support
 * - Port management for connections
 * - Telemetry data binding
 * - Serialization/deserialization
 * - Real-time visual updates
 */
class SubsystemNode : public QObject
{
    Q_OBJECT
    
public:
    explicit SubsystemNode(QObject* parent = nullptr);
    virtual ~SubsystemNode();
    
    // Core identification
    QString nodeId() const { return m_nodeId; }
    void setNodeId(const QString& id) { m_nodeId = id; }
    
    QString nodeName() const { return m_nodeName; }
    void setNodeName(const QString& name);
    
    virtual QString subsystemType() const = 0;
    virtual QString subsystemCategory() const = 0;
    
    // Health monitoring
    HealthStatus healthStatus() const { return m_healthStatus; }
    void setHealthStatus(const HealthStatus& status);
    
    virtual void updateHealth(const TelemetryPacket& packet);
    virtual void updateHealth(HealthCode code, const QString& message = "");
    
    // Port management
    void addInputPort(const QString& name, PortType type, const QString& dataType = "any");
    void addOutputPort(const QString& name, PortType type, const QString& dataType = "any");
    QList<NodePort> inputPorts() const { return m_inputPorts.values(); }
    QList<NodePort> outputPorts() const { return m_outputPorts.values(); }
    NodePort* getInputPort(const QString& name);
    NodePort* getOutputPort(const QString& name);
    
    // Port validation for connections
    virtual bool canConnectTo(const SubsystemNode* other, 
                             const QString& fromPort, 
                             const QString& toPort) const;
    
    // Hierarchical graph support
    bool hasChildGraph() const { return m_hasChildGraph; }
    void setHasChildGraph(bool has) { m_hasChildGraph = has; }
    NodeGraphScene* childGraph() const { return m_childGraph.get(); }
    void createChildGraph();
    
    bool isExpanded() const { return m_expanded; }
    void setExpanded(bool expanded);
    
    // Properties and metadata
    void setProperty(const QString& key, const QVariant& value);
    QVariant property(const QString& key) const;
    QMap<QString, QVariant> allProperties() const { return m_properties; }
    
    // Telemetry data binding
    void bindTelemetryPacket(const TelemetryPacket& packet);
    TelemetryPacket telemetryData() const { return m_telemetryData; }
    
    // Visual rendering (implemented by derived classes)
    virtual QColor nodeColor() const;
    virtual QString nodeIcon() const { return ""; }
    
    // Serialization
    virtual QString serialize() const;
    virtual bool deserialize(const QString& data);
    
    // Factory method for creating nodes from type
    static SubsystemNode* createNode(const QString& type, QObject* parent = nullptr);
    
signals:
    void healthStatusChanged(const HealthStatus& status);
    void nodeNameChanged(const QString& name);
    void propertyChanged(const QString& key, const QVariant& value);
    void telemetryUpdated(const TelemetryPacket& packet);
    void expandedStateChanged(bool expanded);
    
protected:
    // Derived classes implement these for custom behavior
    virtual void onHealthUpdate(const TelemetryPacket& packet);
    virtual void onPropertyChanged(const QString& key, const QVariant& value);
    
    // Helper to generate unique port IDs
    QString generatePortId(const QString& portName) const;
    
private:
    QString m_nodeId;
    QString m_nodeName;
    HealthStatus m_healthStatus;
    
    // Port management
    QMap<QString, NodePort> m_inputPorts;
    QMap<QString, NodePort> m_outputPorts;
    
    // Hierarchical support
    bool m_hasChildGraph;
    bool m_expanded;
    std::unique_ptr<NodeGraphScene> m_childGraph;
    
    // Properties and telemetry
    QMap<QString, QVariant> m_properties;
    TelemetryPacket m_telemetryData;
};

#endif // SUBSYSTEMNODE_H
