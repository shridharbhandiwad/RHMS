/**
 * @file HierarchicalGraphEngine.cpp
 * @brief Implementation of HierarchicalGraphEngine
 */

#include "HierarchicalGraphEngine.h"
#include "NodeGraphScene.h"
#include "../core/SubsystemNode.h"
#include <QDebug>

HierarchicalGraphEngine::HierarchicalGraphEngine(QObject* parent)
    : QObject(parent)
    , m_rootScene(nullptr)
{
}

HierarchicalGraphEngine::~HierarchicalGraphEngine()
{
}

void HierarchicalGraphEngine::setRootScene(NodeGraphScene* scene)
{
    m_rootScene = scene;
    m_navigationStack.clear();
    
    emit sceneChanged(scene);
    emit depthChanged(0);
    emit breadcrumbChanged(QList<SubsystemNode*>());
}

bool HierarchicalGraphEngine::drillDown(SubsystemNode* node)
{
    if (!node) {
        qWarning() << "Cannot drill down into null node";
        return false;
    }
    
    if (!node->hasChildGraph()) {
        qWarning() << "Node has no child graph:" << node->nodeName();
        return false;
    }
    
    NodeGraphScene* childScene = node->childGraph();
    if (!childScene) {
        qWarning() << "Child graph is null";
        return false;
    }
    
    // Push current level onto stack
    NodeGraphScene* currentScene = this->currentScene();
    int currentDepth = m_navigationStack.size();
    
    GraphLevel level(currentScene, node, currentDepth);
    m_navigationStack.push(level);
    
    // Set node as expanded
    node->setExpanded(true);
    
    qInfo() << "Drilled down into:" << node->nodeName() 
            << "Depth:" << m_navigationStack.size();
    
    emit sceneChanged(childScene);
    emit depthChanged(m_navigationStack.size());
    emit breadcrumbChanged(breadcrumbPath());
    
    return true;
}

bool HierarchicalGraphEngine::drillUp()
{
    if (m_navigationStack.isEmpty()) {
        qWarning() << "Already at root level";
        return false;
    }
    
    GraphLevel level = m_navigationStack.pop();
    
    // Collapse the parent node
    if (level.parentNode) {
        level.parentNode->setExpanded(false);
    }
    
    NodeGraphScene* newScene = currentScene();
    
    qInfo() << "Drilled up to depth:" << m_navigationStack.size();
    
    emit sceneChanged(newScene);
    emit depthChanged(m_navigationStack.size());
    emit breadcrumbChanged(breadcrumbPath());
    
    return true;
}

void HierarchicalGraphEngine::jumpToRoot()
{
    while (!m_navigationStack.isEmpty()) {
        GraphLevel level = m_navigationStack.pop();
        if (level.parentNode) {
            level.parentNode->setExpanded(false);
        }
    }
    
    qInfo() << "Jumped to root";
    
    emit sceneChanged(m_rootScene);
    emit depthChanged(0);
    emit breadcrumbChanged(QList<SubsystemNode*>());
}

void HierarchicalGraphEngine::jumpToLevel(int level)
{
    if (level < 0 || level > m_navigationStack.size()) {
        qWarning() << "Invalid level:" << level;
        return;
    }
    
    if (level == 0) {
        jumpToRoot();
        return;
    }
    
    // Pop until we reach the desired level
    while (m_navigationStack.size() > level) {
        drillUp();
    }
}

NodeGraphScene* HierarchicalGraphEngine::currentScene() const
{
    if (m_navigationStack.isEmpty()) {
        return m_rootScene;
    }
    
    const GraphLevel& level = m_navigationStack.top();
    if (level.parentNode) {
        return level.parentNode->childGraph();
    }
    
    return m_rootScene;
}

SubsystemNode* HierarchicalGraphEngine::currentParentNode() const
{
    if (m_navigationStack.isEmpty()) {
        return nullptr;
    }
    
    return m_navigationStack.top().parentNode;
}

QList<SubsystemNode*> HierarchicalGraphEngine::breadcrumbPath() const
{
    QList<SubsystemNode*> path;
    
    for (const GraphLevel& level : m_navigationStack) {
        if (level.parentNode) {
            path.append(level.parentNode);
        }
    }
    
    return path;
}

QString HierarchicalGraphEngine::breadcrumbString() const
{
    QList<SubsystemNode*> path = breadcrumbPath();
    
    if (path.isEmpty()) {
        return "Root";
    }
    
    QStringList names;
    names.append("Root");
    
    for (SubsystemNode* node : path) {
        names.append(node->nodeName());
    }
    
    return names.join(" > ");
}
