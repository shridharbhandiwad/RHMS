/**
 * @file NodeGraphScene.h
 * @brief Qt Graphics Scene for node graph visualization
 */

#ifndef NODEGRAPHSCENE_H
#define NODEGRAPHSCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <memory>
#include "NodeDataModel.h"

class SubsystemNode;
class QGraphicsItem;
class NodeWidget;
class ConnectionManager;

/**
 * @class NodeGraphScene
 * @brief Qt Graphics Scene for displaying and editing node graphs
 * 
 * Provides visual representation of radar subsystem architecture
 * with interactive node placement and connection editing.
 */
class NodeGraphScene : public QGraphicsScene
{
    Q_OBJECT
    
public:
    explicit NodeGraphScene(QObject* parent = nullptr);
    ~NodeGraphScene();
    
    // Node management
    void addNode(SubsystemNode* node, const QPointF& position = QPointF(0, 0));
    void removeNode(const QString& nodeId);
    SubsystemNode* getNode(const QString& nodeId) const;
    QList<SubsystemNode*> allNodes() const;
    
    // Visual node items
    NodeWidget* getNodeWidget(const QString& nodeId) const;
    
    // Connection management
    QString createConnection(const QString& srcNode, const QString& srcPort,
                            const QString& tgtNode, const QString& tgtPort);
    void removeConnection(const QString& connectionId);
    QList<NodeConnection> allConnections() const;
    
    // Data model access
    NodeDataModel* dataModel() { return m_dataModel.get(); }
    const NodeDataModel* dataModel() const { return m_dataModel.get(); }
    
    // Connection manager access
    ConnectionManager* connectionManager() { return m_connectionManager.get(); }
    
    // Scene operations
    void clearScene();
    void centerOnNode(const QString& nodeId);
    
    // Layout algorithms
    void autoLayout();
    void arrangeNodesGrid();
    void arrangeNodesHierarchical();
    
    // Serialization
    QString serialize() const;
    bool deserialize(const QString& data);
    
signals:
    void nodeAdded(SubsystemNode* node);
    void nodeRemoved(const QString& nodeId);
    void connectionCreated(const QString& connectionId);
    void connectionRemoved(const QString& connectionId);
    void nodeSelected(SubsystemNode* node);
    void selectionCleared();
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    
private slots:
    void handleNodePositionChanged(const QString& nodeId, const QPointF& position);
    
private:
    void createNodeWidget(SubsystemNode* node, const QPointF& position);
    void removeNodeWidget(const QString& nodeId);
    
    std::unique_ptr<NodeDataModel> m_dataModel;
    std::unique_ptr<ConnectionManager> m_connectionManager;
    QMap<QString, NodeWidget*> m_nodeWidgets;
    
    // Interaction state
    bool m_isDragging;
    QPointF m_dragStartPos;
};

#endif // NODEGRAPHSCENE_H
