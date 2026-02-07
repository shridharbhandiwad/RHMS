/**
 * @file NodeGraphScene.cpp
 * @brief Implementation of NodeGraphScene
 */

#include "NodeGraphScene.h"
#include "ConnectionManager.h"
#include "../ui/NodeWidget.h"
#include "../core/SubsystemNode.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <cmath>

NodeGraphScene::NodeGraphScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_isDragging(false)
{
    m_dataModel = std::make_unique<NodeDataModel>(this);
    m_connectionManager = std::make_unique<ConnectionManager>(this, m_dataModel.get());
    
    // Set scene properties
    setSceneRect(-5000, -5000, 10000, 10000);
    setBackgroundBrush(QBrush(QColor(45, 45, 48)));
    
    // Connect signals
    connect(m_dataModel.get(), &NodeDataModel::nodePositionChanged,
            this, &NodeGraphScene::handleNodePositionChanged);
}

NodeGraphScene::~NodeGraphScene()
{
    clearScene();
}

void NodeGraphScene::addNode(SubsystemNode* node, const QPointF& position)
{
    if (!node) {
        qWarning() << "Cannot add null node to scene";
        return;
    }
    
    // Add to data model
    m_dataModel->addNode(node, position);
    
    // Create visual widget
    createNodeWidget(node, position);
    
    emit nodeAdded(node);
}

void NodeGraphScene::removeNode(const QString& nodeId)
{
    // Remove visual widget
    removeNodeWidget(nodeId);
    
    // Remove from data model (this also removes connections)
    m_dataModel->removeNode(nodeId);
    
    emit nodeRemoved(nodeId);
}

SubsystemNode* NodeGraphScene::getNode(const QString& nodeId) const
{
    return m_dataModel->getNode(nodeId);
}

QList<SubsystemNode*> NodeGraphScene::allNodes() const
{
    return m_dataModel->allNodes();
}

NodeWidget* NodeGraphScene::getNodeWidget(const QString& nodeId) const
{
    return m_nodeWidgets.value(nodeId, nullptr);
}

QString NodeGraphScene::createConnection(const QString& srcNode, const QString& srcPort,
                                         const QString& tgtNode, const QString& tgtPort)
{
    QString connId = m_dataModel->addConnection(srcNode, srcPort, tgtNode, tgtPort);
    
    if (!connId.isEmpty()) {
        // Create visual connection
        m_connectionManager->createVisualConnection(connId);
        emit connectionCreated(connId);
    }
    
    return connId;
}

void NodeGraphScene::removeConnection(const QString& connectionId)
{
    // Remove visual connection
    m_connectionManager->removeVisualConnection(connectionId);
    
    // Remove from data model
    m_dataModel->removeConnection(connectionId);
    
    emit connectionRemoved(connectionId);
}

QList<NodeConnection> NodeGraphScene::allConnections() const
{
    return m_dataModel->allConnections();
}

void NodeGraphScene::clearScene()
{
    // Remove all visual widgets
    for (auto widget : m_nodeWidgets) {
        removeItem(widget);
        delete widget;
    }
    m_nodeWidgets.clear();
    
    // Clear connections
    m_connectionManager->clearConnections();
    
    // Clear data model
    m_dataModel->clear();
    
    QGraphicsScene::clear();
}

void NodeGraphScene::centerOnNode(const QString& nodeId)
{
    NodeWidget* widget = getNodeWidget(nodeId);
    if (widget) {
        views().first()->centerOn(widget);
    }
}

void NodeGraphScene::autoLayout()
{
    // Simple grid layout for now
    arrangeNodesGrid();
}

void NodeGraphScene::arrangeNodesGrid()
{
    QList<SubsystemNode*> nodes = allNodes();
    if (nodes.isEmpty()) {
        return;
    }
    
    int cols = std::ceil(std::sqrt(nodes.size()));
    qreal spacing = 200.0;
    
    for (int i = 0; i < nodes.size(); ++i) {
        int row = i / cols;
        int col = i % cols;
        QPointF pos(col * spacing, row * spacing);
        
        m_dataModel->setNodePosition(nodes[i]->nodeId(), pos);
        
        NodeWidget* widget = getNodeWidget(nodes[i]->nodeId());
        if (widget) {
            widget->setPos(pos);
        }
    }
}

void NodeGraphScene::arrangeNodesHierarchical()
{
    // TODO: Implement hierarchical layout algorithm
    // For now, fall back to grid layout
    arrangeNodesGrid();
}

QString NodeGraphScene::serialize() const
{
    // Serialize the data model
    return m_dataModel->serialize();
}

bool NodeGraphScene::deserialize(const QString& data)
{
    // Deserialize the data model
    return m_dataModel->deserialize(data);
}

void NodeGraphScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);
    
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (!item) {
            clearSelection();
            emit selectionCleared();
        }
    }
}

void NodeGraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void NodeGraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void NodeGraphScene::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        // Delete selected nodes
        QList<QGraphicsItem*> selected = selectedItems();
        for (QGraphicsItem* item : selected) {
            NodeWidget* nodeWidget = dynamic_cast<NodeWidget*>(item);
            if (nodeWidget) {
                removeNode(nodeWidget->subsystemNode()->nodeId());
            }
        }
    }
    
    QGraphicsScene::keyPressEvent(event);
}

void NodeGraphScene::handleNodePositionChanged(const QString& nodeId, const QPointF& position)
{
    // Update visual widget position
    NodeWidget* widget = getNodeWidget(nodeId);
    if (widget) {
        widget->setPos(position);
    }
    
    // Update connections
    m_connectionManager->updateConnectionsForNode(nodeId);
}

void NodeGraphScene::createNodeWidget(SubsystemNode* node, const QPointF& position)
{
    QString nodeId = node->nodeId();
    
    if (m_nodeWidgets.contains(nodeId)) {
        qWarning() << "Node widget already exists:" << nodeId;
        return;
    }
    
    NodeWidget* widget = new NodeWidget(node);
    widget->setPos(position);
    addItem(widget);
    
    m_nodeWidgets[nodeId] = widget;
    
    qDebug() << "Created node widget:" << nodeId << "at" << position;
}

void NodeGraphScene::removeNodeWidget(const QString& nodeId)
{
    if (!m_nodeWidgets.contains(nodeId)) {
        return;
    }
    
    NodeWidget* widget = m_nodeWidgets[nodeId];
    removeItem(widget);
    delete widget;
    m_nodeWidgets.remove(nodeId);
    
    qDebug() << "Removed node widget:" << nodeId;
}
