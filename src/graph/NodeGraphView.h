/**
 * @file NodeGraphView.h
 * @brief Qt Graphics View for node graph with pan/zoom
 */

#ifndef NODEGRAPHVIEW_H
#define NODEGRAPHVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>

class NodeGraphScene;

/**
 * @class NodeGraphView
 * @brief Interactive view for node graph with zoom and pan
 * 
 * Provides infinite canvas-like behavior for large radar architectures.
 * Supports smooth zooming, panning, and keyboard shortcuts.
 */
class NodeGraphView : public QGraphicsView
{
    Q_OBJECT
    
public:
    explicit NodeGraphView(QWidget* parent = nullptr);
    explicit NodeGraphView(NodeGraphScene* scene, QWidget* parent = nullptr);
    ~NodeGraphView();
    
    // View control
    void setNodeScene(NodeGraphScene* scene);
    NodeGraphScene* nodeScene() const { return m_nodeScene; }
    
    // Zoom operations
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void zoomToFit();
    void setZoomLevel(qreal level);
    qreal zoomLevel() const { return m_zoomLevel; }
    
    // Pan operations
    void panToCenter();
    void setEnablePanning(bool enable) { m_panningEnabled = enable; }
    bool isPanningEnabled() const { return m_panningEnabled; }
    
signals:
    void zoomChanged(qreal zoomLevel);
    void viewportChanged();
    
protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
private:
    void setupView();
    void zoom(qreal factor, const QPoint& centerPos);
    
    NodeGraphScene* m_nodeScene;
    qreal m_zoomLevel;
    qreal m_minZoom;
    qreal m_maxZoom;
    
    bool m_panningEnabled;
    bool m_isPanning;
    QPoint m_lastPanPos;
};

#endif // NODEGRAPHVIEW_H
