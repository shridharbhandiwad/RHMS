/**
 * @file ConnectionManager.h
 * @brief Manages visual connections between nodes
 */

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QGraphicsPathItem>
#include <QMap>
#include <QPen>
#include "NodeDataModel.h"

class NodeGraphScene;

/**
 * @class ConnectionPath
 * @brief Visual representation of a connection between nodes
 */
class ConnectionPath : public QGraphicsPathItem
{
public:
    ConnectionPath(const QString& connectionId, QGraphicsItem* parent = nullptr);
    
    void setSourcePoint(const QPointF& point);
    void setTargetPoint(const QPointF& point);
    void updatePath();
    
    QString connectionId() const { return m_connectionId; }
    
    // Visual customization
    void setConnectionColor(const QColor& color);
    void setConnectionWidth(qreal width);
    void setHighlighted(bool highlighted);
    
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
    
private:
    QString m_connectionId;
    QPointF m_sourcePoint;
    QPointF m_targetPoint;
    QColor m_color;
    qreal m_width;
    bool m_highlighted;
    
    QPainterPath createBezierPath() const;
};

/**
 * @class ConnectionManager
 * @brief Manages creation, deletion, and rendering of node connections
 * 
 * Handles visual connection paths and updates them when nodes move.
 */
class ConnectionManager : public QObject
{
    Q_OBJECT
    
public:
    ConnectionManager(NodeGraphScene* scene, NodeDataModel* dataModel, QObject* parent = nullptr);
    ~ConnectionManager();
    
    // Connection visual management
    void createVisualConnection(const QString& connectionId);
    void removeVisualConnection(const QString& connectionId);
    void updateConnection(const QString& connectionId);
    void updateConnectionsForNode(const QString& nodeId);
    void clearConnections();
    
    // Visual customization
    void setDefaultConnectionColor(const QColor& color);
    void setDefaultConnectionWidth(qreal width);
    void highlightConnection(const QString& connectionId, bool highlight = true);
    
    // Query
    ConnectionPath* getConnectionPath(const QString& connectionId) const;
    QList<QString> connectionsForNode(const QString& nodeId) const;
    
signals:
    void connectionClicked(const QString& connectionId);
    void connectionHovered(const QString& connectionId);
    
private:
    QPointF getNodePortPosition(const QString& nodeId, const QString& portName, bool isOutput);
    void updateConnectionPath(const QString& connectionId);
    
    NodeGraphScene* m_scene;
    NodeDataModel* m_dataModel;
    QMap<QString, ConnectionPath*> m_connectionPaths;
    
    QColor m_defaultColor;
    qreal m_defaultWidth;
};

#endif // CONNECTIONMANAGER_H
