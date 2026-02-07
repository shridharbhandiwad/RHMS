/**
 * @file NodeGraphView.cpp
 * @brief Implementation of NodeGraphView
 */

#include "NodeGraphView.h"
#include "NodeGraphScene.h"
#include <QScrollBar>
#include <QDebug>
#include <cmath>

NodeGraphView::NodeGraphView(QWidget* parent)
    : QGraphicsView(parent)
    , m_nodeScene(nullptr)
    , m_zoomLevel(1.0)
    , m_minZoom(0.1)
    , m_maxZoom(5.0)
    , m_panningEnabled(true)
    , m_isPanning(false)
{
    setupView();
}

NodeGraphView::NodeGraphView(NodeGraphScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
    , m_nodeScene(scene)
    , m_zoomLevel(1.0)
    , m_minZoom(0.1)
    , m_maxZoom(5.0)
    , m_panningEnabled(true)
    , m_isPanning(false)
{
    setupView();
}

NodeGraphView::~NodeGraphView()
{
}

void NodeGraphView::setupView()
{
    // Enable anti-aliasing for smooth rendering
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::TextAntialiasing, true);
    
    // Set view properties
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setDragMode(QGraphicsView::RubberBandDrag);
    
    // Enable mouse tracking for hover effects
    setMouseTracking(true);
}

void NodeGraphView::setNodeScene(NodeGraphScene* scene)
{
    m_nodeScene = scene;
    setScene(scene);
}

void NodeGraphView::zoomIn()
{
    zoom(1.15, viewport()->rect().center());
}

void NodeGraphView::zoomOut()
{
    zoom(1.0 / 1.15, viewport()->rect().center());
}

void NodeGraphView::zoomReset()
{
    resetTransform();
    m_zoomLevel = 1.0;
    emit zoomChanged(m_zoomLevel);
}

void NodeGraphView::zoomToFit()
{
    if (!m_nodeScene) {
        return;
    }
    
    QRectF bounds = m_nodeScene->itemsBoundingRect();
    if (bounds.isEmpty()) {
        return;
    }
    
    fitInView(bounds, Qt::KeepAspectRatio);
    m_zoomLevel = transform().m11();
    emit zoomChanged(m_zoomLevel);
}

void NodeGraphView::setZoomLevel(qreal level)
{
    qreal clampedLevel = qBound(m_minZoom, level, m_maxZoom);
    qreal factor = clampedLevel / m_zoomLevel;
    zoom(factor, viewport()->rect().center());
}

void NodeGraphView::panToCenter()
{
    if (m_nodeScene) {
        centerOn(0, 0);
    }
}

void NodeGraphView::wheelEvent(QWheelEvent* event)
{
    // Zoom with mouse wheel
    if (event->modifiers() & Qt::ControlModifier) {
        qreal factor = std::pow(1.15, event->angleDelta().y() / 120.0);
        zoom(factor, event->pos());
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void NodeGraphView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton && m_panningEnabled) {
        // Start panning with middle mouse button
        m_isPanning = true;
        m_lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else if (event->button() == Qt::RightButton && m_panningEnabled) {
        // Alternative panning with right mouse button
        m_isPanning = true;
        m_lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void NodeGraphView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isPanning) {
        // Pan the view
        QPoint delta = event->pos() - m_lastPanPos;
        m_lastPanPos = event->pos();
        
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        
        emit viewportChanged();
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void NodeGraphView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_isPanning && (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton)) {
        // Stop panning
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void NodeGraphView::keyPressEvent(QKeyEvent* event)
{
    // Keyboard shortcuts for navigation
    switch (event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomIn();
                event->accept();
                return;
            }
            break;
            
        case Qt::Key_Minus:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomOut();
                event->accept();
                return;
            }
            break;
            
        case Qt::Key_0:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomReset();
                event->accept();
                return;
            }
            break;
            
        case Qt::Key_F:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomToFit();
                event->accept();
                return;
            }
            break;
            
        case Qt::Key_Home:
            panToCenter();
            event->accept();
            return;
    }
    
    QGraphicsView::keyPressEvent(event);
}

void NodeGraphView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    emit viewportChanged();
}

void NodeGraphView::zoom(qreal factor, const QPoint& /*centerPos*/)
{
    qreal newZoom = m_zoomLevel * factor;
    
    // Clamp zoom level
    if (newZoom < m_minZoom || newZoom > m_maxZoom) {
        return;
    }
    
    // Apply zoom
    scale(factor, factor);
    m_zoomLevel = newZoom;
    
    emit zoomChanged(m_zoomLevel);
    emit viewportChanged();
}
