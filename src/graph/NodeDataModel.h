/**
 * @file NodeDataModel.h
 * @brief Data model for node graph representation
 */

#ifndef NODEDATAMODEL_H
#define NODEDATAMODEL_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QPointF>
#include <QList>
#include <memory>

class SubsystemNode;

/**
 * @struct NodeConnection
 * @brief Represents a connection between two nodes
 */
struct NodeConnection {
    QString connectionId;
    QString sourceNodeId;
    QString sourcePort;
    QString targetNodeId;
    QString targetPort;
    bool isValid;
    
    NodeConnection()
        : isValid(false) {}
    
    NodeConnection(const QString& srcNode, const QString& srcPort,
                   const QString& tgtNode, const QString& tgtPort)
        : connectionId(QUuid::createUuid().toString(QUuid::WithoutBraces))
        , sourceNodeId(srcNode)
        , sourcePort(srcPort)
        , targetNodeId(tgtNode)
        , targetPort(tgtPort)
        , isValid(true) {}
};

/**
 * @struct NodeLayout
 * @brief Layout information for a node in the graph
 */
struct NodeLayout {
    QString nodeId;
    QPointF position;
    QSizeF size;
    bool isExpanded;
    int zIndex;
    
    NodeLayout()
        : position(0, 0)
        , size(150, 100)
        , isExpanded(false)
        , zIndex(0) {}
};

/**
 * @class NodeDataModel
 * @brief Data model for node graph
 * 
 * Manages nodes, connections, and layout in a graph scene.
 * Separate from visual representation for clean architecture.
 */
class NodeDataModel : public QObject
{
    Q_OBJECT
    
public:
    explicit NodeDataModel(QObject* parent = nullptr);
    ~NodeDataModel();
    
    // Node management
    void addNode(SubsystemNode* node, const QPointF& position = QPointF(0, 0));
    void removeNode(const QString& nodeId);
    SubsystemNode* getNode(const QString& nodeId) const;
    QList<SubsystemNode*> allNodes() const;
    int nodeCount() const { return m_nodes.size(); }
    
    // Connection management
    QString addConnection(const QString& srcNode, const QString& srcPort,
                         const QString& tgtNode, const QString& tgtPort);
    void removeConnection(const QString& connectionId);
    NodeConnection* getConnection(const QString& connectionId);
    QList<NodeConnection> allConnections() const;
    QList<NodeConnection> connectionsForNode(const QString& nodeId) const;
    int connectionCount() const { return m_connections.size(); }
    
    // Layout management
    void setNodePosition(const QString& nodeId, const QPointF& position);
    QPointF nodePosition(const QString& nodeId) const;
    void setNodeSize(const QString& nodeId, const QSizeF& size);
    QSizeF nodeSize(const QString& nodeId) const;
    NodeLayout nodeLayout(const QString& nodeId) const;
    
    // Validation
    bool canConnect(const QString& srcNode, const QString& srcPort,
                   const QString& tgtNode, const QString& tgtPort) const;
    bool hasConnection(const QString& srcNode, const QString& srcPort,
                      const QString& tgtNode, const QString& tgtPort) const;
    
    // Clear all data
    void clear();
    
    // Serialization
    QString serialize() const;
    bool deserialize(const QString& data);
    
signals:
    void nodeAdded(const QString& nodeId);
    void nodeRemoved(const QString& nodeId);
    void connectionAdded(const QString& connectionId);
    void connectionRemoved(const QString& connectionId);
    void nodePositionChanged(const QString& nodeId, const QPointF& position);
    void modelCleared();
    
private:
    QMap<QString, SubsystemNode*> m_nodes;
    QMap<QString, NodeConnection> m_connections;
    QMap<QString, NodeLayout> m_layouts;
};

#endif // NODEDATAMODEL_H
