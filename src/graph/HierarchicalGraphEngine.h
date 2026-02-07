/**
 * @file HierarchicalGraphEngine.h
 * @brief Engine for managing hierarchical node graphs
 */

#ifndef HIERARCHICALGRAPHENGINE_H
#define HIERARCHICALGRAPHENGINE_H

#include <QObject>
#include <QStack>
#include <memory>

class NodeGraphScene;
class SubsystemNode;

/**
 * @struct GraphLevel
 * @brief Represents a level in the graph hierarchy
 */
struct GraphLevel {
    NodeGraphScene* scene;
    SubsystemNode* parentNode;
    int depth;
    
    GraphLevel()
        : scene(nullptr), parentNode(nullptr), depth(0) {}
    
    GraphLevel(NodeGraphScene* s, SubsystemNode* p, int d)
        : scene(s), parentNode(p), depth(d) {}
};

/**
 * @class HierarchicalGraphEngine
 * @brief Manages navigation through hierarchical node graphs
 * 
 * Supports drilling down into subsystem nodes and navigating
 * the hierarchy with breadcrumb navigation.
 */
class HierarchicalGraphEngine : public QObject
{
    Q_OBJECT
    
public:
    explicit HierarchicalGraphEngine(QObject* parent = nullptr);
    ~HierarchicalGraphEngine();
    
    // Root scene management
    void setRootScene(NodeGraphScene* scene);
    NodeGraphScene* rootScene() const { return m_rootScene; }
    
    // Navigation
    bool drillDown(SubsystemNode* node);
    bool drillUp();
    void jumpToRoot();
    void jumpToLevel(int level);
    
    // Current state
    NodeGraphScene* currentScene() const;
    SubsystemNode* currentParentNode() const;
    int currentDepth() const { return m_navigationStack.size(); }
    bool canDrillUp() const { return !m_navigationStack.isEmpty(); }
    
    // Hierarchy information
    QList<SubsystemNode*> breadcrumbPath() const;
    QString breadcrumbString() const;
    
signals:
    void sceneChanged(NodeGraphScene* scene);
    void depthChanged(int depth);
    void breadcrumbChanged(const QList<SubsystemNode*>& path);
    
private:
    NodeGraphScene* m_rootScene;
    QStack<GraphLevel> m_navigationStack;
};

#endif // HIERARCHICALGRAPHENGINE_H
