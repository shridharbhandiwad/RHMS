/**
 * @file ConnectionManager.cpp
 * @brief Implementation of ConnectionManager
 */

#include "ConnectionManager.h"
#include "NodeGraphScene.h"
#include "../ui/NodeWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <cmath>

// ============================================================================
// ConnectionPath Implementation
// ============================================================================

ConnectionPath::ConnectionPath(const QString& connectionId, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , m_connectionId(connectionId)
    , m_color(100, 200, 100)
    , m_width(2.0)
    , m_highlighted(false)
{
    setZValue(-1); // Draw connections behind nodes
    setPen(QPen(m_color, m_width));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void ConnectionPath::setSourcePoint(const QPointF& point)
{
    m_sourcePoint = point;
    updatePath();
}

void ConnectionPath::setTargetPoint(const QPointF& point)
{
    m_targetPoint = point;
    updatePath();
}

void ConnectionPath::updatePath()
{
    QPainterPath path = createBezierPath();
    setPath(path);
}

void ConnectionPath::setConnectionColor(const QColor& color)
{
    m_color = color;
    setPen(QPen(m_color, m_width));
}

void ConnectionPath::setConnectionWidth(qreal width)
{
    m_width = width;
    setPen(QPen(m_color, m_width));
}

void ConnectionPath::setHighlighted(bool highlighted)
{
    m_highlighted = highlighted;
    if (highlighted) {
        setPen(QPen(QColor(255, 255, 0), m_width * 1.5));
    } else {
        setPen(QPen(m_color, m_width));
    }
}

void ConnectionPath::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    // Draw connection line
    QPen linePen = pen();
    if (isSelected()) {
        linePen.setColor(QColor(255, 255, 0));
        linePen.setWidth(m_width * 1.5);
    }
    painter->setPen(linePen);
    painter->drawPath(path());
    
    // Draw arrowhead at target
    qreal arrowSize = 10.0;
    QPointF direction = m_targetPoint - m_sourcePoint;
    qreal length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0) {
        direction /= length;
        
        QPointF perpendicular(-direction.y(), direction.x());
        QPointF arrowP1 = m_targetPoint - direction * arrowSize + perpendicular * (arrowSize / 2);
        QPointF arrowP2 = m_targetPoint - direction * arrowSize - perpendicular * (arrowSize / 2);
        
        QPainterPath arrow;
        arrow.moveTo(m_targetPoint);
        arrow.lineTo(arrowP1);
        arrow.lineTo(arrowP2);
        arrow.closeSubpath();
        
        painter->setBrush(linePen.color());
        painter->drawPath(arrow);
    }
}

QPainterPath ConnectionPath::createBezierPath() const
{
    QPainterPath path;
    path.moveTo(m_sourcePoint);
    
    // Create bezier curve for smooth connection
    qreal dx = m_targetPoint.x() - m_sourcePoint.x();
    qreal controlOffsetX = std::abs(dx) * 0.5;
    
    QPointF control1(m_sourcePoint.x() + controlOffsetX, m_sourcePoint.y());
    QPointF control2(m_targetPoint.x() - controlOffsetX, m_targetPoint.y());
    
    path.cubicTo(control1, control2, m_targetPoint);
    
    return path;
}

// ============================================================================
// ConnectionManager Implementation
// ============================================================================

ConnectionManager::ConnectionManager(NodeGraphScene* scene, NodeDataModel* dataModel, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_dataModel(dataModel)
    , m_defaultColor(100, 200, 100)
    , m_defaultWidth(2.0)
{
}

ConnectionManager::~ConnectionManager()
{
    clearConnections();
}

void ConnectionManager::createVisualConnection(const QString& connectionId)
{
    if (m_connectionPaths.contains(connectionId)) {
        qWarning() << "Connection path already exists:" << connectionId;
        return;
    }
    
    ConnectionPath* path = new ConnectionPath(connectionId);
    path->setConnectionColor(m_defaultColor);
    path->setConnectionWidth(m_defaultWidth);
    
    m_scene->addItem(path);
    m_connectionPaths[connectionId] = path;
    
    // Update the path geometry
    updateConnectionPath(connectionId);
    
    qDebug() << "Created visual connection:" << connectionId;
}

void ConnectionManager::removeVisualConnection(const QString& connectionId)
{
    if (!m_connectionPaths.contains(connectionId)) {
        return;
    }
    
    ConnectionPath* path = m_connectionPaths[connectionId];
    m_scene->removeItem(path);
    delete path;
    m_connectionPaths.remove(connectionId);
    
    qDebug() << "Removed visual connection:" << connectionId;
}

void ConnectionManager::updateConnection(const QString& connectionId)
{
    updateConnectionPath(connectionId);
}

void ConnectionManager::updateConnectionsForNode(const QString& nodeId)
{
    QList<NodeConnection> connections = m_dataModel->connectionsForNode(nodeId);
    
    for (const NodeConnection& conn : connections) {
        updateConnectionPath(conn.connectionId);
    }
}

void ConnectionManager::clearConnections()
{
    for (auto path : m_connectionPaths) {
        m_scene->removeItem(path);
        delete path;
    }
    m_connectionPaths.clear();
}

void ConnectionManager::setDefaultConnectionColor(const QColor& color)
{
    m_defaultColor = color;
}

void ConnectionManager::setDefaultConnectionWidth(qreal width)
{
    m_defaultWidth = width;
}

void ConnectionManager::highlightConnection(const QString& connectionId, bool highlight)
{
    ConnectionPath* path = getConnectionPath(connectionId);
    if (path) {
        path->setHighlighted(highlight);
    }
}

ConnectionPath* ConnectionManager::getConnectionPath(const QString& connectionId) const
{
    return m_connectionPaths.value(connectionId, nullptr);
}

QList<QString> ConnectionManager::connectionsForNode(const QString& nodeId) const
{
    QList<QString> result;
    QList<NodeConnection> connections = m_dataModel->connectionsForNode(nodeId);
    
    for (const NodeConnection& conn : connections) {
        result.append(conn.connectionId);
    }
    
    return result;
}

QPointF ConnectionManager::getNodePortPosition(const QString& nodeId, const QString& portName, bool isOutput)
{
    NodeWidget* widget = m_scene->getNodeWidget(nodeId);
    if (!widget) {
        qWarning() << "Node widget not found:" << nodeId;
        return QPointF(0, 0);
    }
    
    // Get port position relative to node
    QPointF portPos = widget->getPortPosition(portName, isOutput);
    
    // Convert to scene coordinates
    return widget->mapToScene(portPos);
}

void ConnectionManager::updateConnectionPath(const QString& connectionId)
{
    NodeConnection* conn = m_dataModel->getConnection(connectionId);
    if (!conn) {
        qWarning() << "Connection not found in data model:" << connectionId;
        return;
    }
    
    ConnectionPath* path = getConnectionPath(connectionId);
    if (!path) {
        qWarning() << "Connection path not found:" << connectionId;
        return;
    }
    
    // Get source and target positions
    QPointF sourcePos = getNodePortPosition(conn->sourceNodeId, conn->sourcePort, true);
    QPointF targetPos = getNodePortPosition(conn->targetNodeId, conn->targetPort, false);
    
    // Update path geometry
    path->setSourcePoint(sourcePos);
    path->setTargetPoint(targetPos);
}
