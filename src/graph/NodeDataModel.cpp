/**
 * @file NodeDataModel.cpp
 * @brief Implementation of NodeDataModel
 */

#include "NodeDataModel.h"
#include "../core/SubsystemNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

NodeDataModel::NodeDataModel(QObject* parent)
    : QObject(parent)
{
}

NodeDataModel::~NodeDataModel()
{
    clear();
}

void NodeDataModel::addNode(SubsystemNode* node, const QPointF& position)
{
    if (!node) {
        qWarning() << "Cannot add null node";
        return;
    }
    
    QString nodeId = node->nodeId();
    if (m_nodes.contains(nodeId)) {
        qWarning() << "Node already exists:" << nodeId;
        return;
    }
    
    m_nodes[nodeId] = node;
    
    NodeLayout layout;
    layout.nodeId = nodeId;
    layout.position = position;
    m_layouts[nodeId] = layout;
    
    emit nodeAdded(nodeId);
    qDebug() << "Added node to model:" << nodeId << node->nodeName();
}

void NodeDataModel::removeNode(const QString& nodeId)
{
    if (!m_nodes.contains(nodeId)) {
        qWarning() << "Node not found:" << nodeId;
        return;
    }
    
    // Remove all connections involving this node
    QList<QString> connectionsToRemove;
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        if (it->sourceNodeId == nodeId || it->targetNodeId == nodeId) {
            connectionsToRemove.append(it.key());
        }
    }
    
    for (const QString& connId : connectionsToRemove) {
        removeConnection(connId);
    }
    
    // Remove node
    m_nodes.remove(nodeId);
    m_layouts.remove(nodeId);
    
    emit nodeRemoved(nodeId);
    qDebug() << "Removed node from model:" << nodeId;
}

SubsystemNode* NodeDataModel::getNode(const QString& nodeId) const
{
    return m_nodes.value(nodeId, nullptr);
}

QList<SubsystemNode*> NodeDataModel::allNodes() const
{
    return m_nodes.values();
}

QString NodeDataModel::addConnection(const QString& srcNode, const QString& srcPort,
                                      const QString& tgtNode, const QString& tgtPort)
{
    // Validate connection
    if (!canConnect(srcNode, srcPort, tgtNode, tgtPort)) {
        qWarning() << "Cannot create connection";
        return QString();
    }
    
    // Check for duplicate
    if (hasConnection(srcNode, srcPort, tgtNode, tgtPort)) {
        qWarning() << "Connection already exists";
        return QString();
    }
    
    // Create connection
    NodeConnection conn(srcNode, srcPort, tgtNode, tgtPort);
    m_connections[conn.connectionId] = conn;
    
    emit connectionAdded(conn.connectionId);
    qDebug() << "Added connection:" << conn.connectionId;
    
    return conn.connectionId;
}

void NodeDataModel::removeConnection(const QString& connectionId)
{
    if (m_connections.remove(connectionId) > 0) {
        emit connectionRemoved(connectionId);
        qDebug() << "Removed connection:" << connectionId;
    }
}

NodeConnection* NodeDataModel::getConnection(const QString& connectionId)
{
    if (m_connections.contains(connectionId)) {
        return &m_connections[connectionId];
    }
    return nullptr;
}

QList<NodeConnection> NodeDataModel::allConnections() const
{
    return m_connections.values();
}

QList<NodeConnection> NodeDataModel::connectionsForNode(const QString& nodeId) const
{
    QList<NodeConnection> result;
    for (const auto& conn : m_connections) {
        if (conn.sourceNodeId == nodeId || conn.targetNodeId == nodeId) {
            result.append(conn);
        }
    }
    return result;
}

void NodeDataModel::setNodePosition(const QString& nodeId, const QPointF& position)
{
    if (!m_layouts.contains(nodeId)) {
        qWarning() << "Node layout not found:" << nodeId;
        return;
    }
    
    m_layouts[nodeId].position = position;
    emit nodePositionChanged(nodeId, position);
}

QPointF NodeDataModel::nodePosition(const QString& nodeId) const
{
    return m_layouts.value(nodeId).position;
}

void NodeDataModel::setNodeSize(const QString& nodeId, const QSizeF& size)
{
    if (m_layouts.contains(nodeId)) {
        m_layouts[nodeId].size = size;
    }
}

QSizeF NodeDataModel::nodeSize(const QString& nodeId) const
{
    return m_layouts.value(nodeId).size;
}

NodeLayout NodeDataModel::nodeLayout(const QString& nodeId) const
{
    return m_layouts.value(nodeId);
}

bool NodeDataModel::canConnect(const QString& srcNode, const QString& srcPort,
                                const QString& tgtNode, const QString& tgtPort) const
{
    // Get nodes
    SubsystemNode* source = getNode(srcNode);
    SubsystemNode* target = getNode(tgtNode);
    
    if (!source || !target) {
        return false;
    }
    
    // Cannot connect node to itself
    if (srcNode == tgtNode) {
        return false;
    }
    
    // Use node's built-in validation
    return source->canConnectTo(target, srcPort, tgtPort);
}

bool NodeDataModel::hasConnection(const QString& srcNode, const QString& srcPort,
                                   const QString& tgtNode, const QString& tgtPort) const
{
    for (const auto& conn : m_connections) {
        if (conn.sourceNodeId == srcNode && conn.sourcePort == srcPort &&
            conn.targetNodeId == tgtNode && conn.targetPort == tgtPort) {
            return true;
        }
    }
    return false;
}

void NodeDataModel::clear()
{
    m_connections.clear();
    m_layouts.clear();
    
    // Don't delete nodes, they're managed elsewhere
    m_nodes.clear();
    
    emit modelCleared();
    qDebug() << "Cleared node data model";
}

QString NodeDataModel::serialize() const
{
    QJsonObject root;
    
    // Serialize nodes (just IDs and layouts)
    QJsonArray nodesArray;
    for (auto it = m_layouts.begin(); it != m_layouts.end(); ++it) {
        QJsonObject nodeObj;
        nodeObj["nodeId"] = it.value().nodeId;
        nodeObj["posX"] = it.value().position.x();
        nodeObj["posY"] = it.value().position.y();
        nodeObj["width"] = it.value().size.width();
        nodeObj["height"] = it.value().size.height();
        nodeObj["expanded"] = it.value().isExpanded;
        nodeObj["zIndex"] = it.value().zIndex;
        nodesArray.append(nodeObj);
    }
    root["nodes"] = nodesArray;
    
    // Serialize connections
    QJsonArray connectionsArray;
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        QJsonObject connObj;
        connObj["id"] = it.value().connectionId;
        connObj["sourceNode"] = it.value().sourceNodeId;
        connObj["sourcePort"] = it.value().sourcePort;
        connObj["targetNode"] = it.value().targetNodeId;
        connObj["targetPort"] = it.value().targetPort;
        connectionsArray.append(connObj);
    }
    root["connections"] = connectionsArray;
    
    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Compact);
}

bool NodeDataModel::deserialize(const QString& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Deserialize nodes layouts
    QJsonArray nodesArray = root["nodes"].toArray();
    for (const QJsonValue& val : nodesArray) {
        QJsonObject nodeObj = val.toObject();
        NodeLayout layout;
        layout.nodeId = nodeObj["nodeId"].toString();
        layout.position = QPointF(nodeObj["posX"].toDouble(), nodeObj["posY"].toDouble());
        layout.size = QSizeF(nodeObj["width"].toDouble(), nodeObj["height"].toDouble());
        layout.isExpanded = nodeObj["expanded"].toBool();
        layout.zIndex = nodeObj["zIndex"].toInt();
        
        // Only restore if node exists
        if (m_nodes.contains(layout.nodeId)) {
            m_layouts[layout.nodeId] = layout;
        }
    }
    
    // Deserialize connections
    QJsonArray connectionsArray = root["connections"].toArray();
    for (const QJsonValue& val : connectionsArray) {
        QJsonObject connObj = val.toObject();
        NodeConnection conn;
        conn.connectionId = connObj["id"].toString();
        conn.sourceNodeId = connObj["sourceNode"].toString();
        conn.sourcePort = connObj["sourcePort"].toString();
        conn.targetNodeId = connObj["targetNode"].toString();
        conn.targetPort = connObj["targetPort"].toString();
        conn.isValid = true;
        
        // Validate that nodes exist
        if (m_nodes.contains(conn.sourceNodeId) && m_nodes.contains(conn.targetNodeId)) {
            m_connections[conn.connectionId] = conn;
        }
    }
    
    return true;
}
