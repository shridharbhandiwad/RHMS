/**
 * @file NodeWidget.h
 * @brief Visual widget for subsystem nodes in the graph
 */

#ifndef NODEWIDGET_H
#define NODEWIDGET_H

#include <QGraphicsItem>
#include <QColor>
#include <QFont>

class SubsystemNode;

/**
 * @class NodeWidget
 * @brief Visual representation of a subsystem node
 * 
 * Provides interactive graphical representation with:
 * - Health status indicator
 * - Port visualizations
 * - Node information display
 * - Selection and hover effects
 */
class NodeWidget : public QGraphicsItem
{
public:
    explicit NodeWidget(SubsystemNode* node, QGraphicsItem* parent = nullptr);
    ~NodeWidget();
    
    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
    QPainterPath shape() const override;
    
    // Node access
    SubsystemNode* subsystemNode() const { return m_node; }
    
    // Port position calculation
    QPointF getPortPosition(const QString& portName, bool isOutput) const;
    QList<QPointF> inputPortPositions() const;
    QList<QPointF> outputPortPositions() const;
    
    // Visual customization
    void setHighlighted(bool highlighted);
    bool isHighlighted() const { return m_highlighted; }
    
    // Size
    void setNodeSize(const QSizeF& size);
    QSizeF nodeSize() const { return m_size; }
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    
private:
    void updateAppearance();
    void drawNode(QPainter* painter);
    void drawHeader(QPainter* painter, const QRectF& rect);
    void drawBody(QPainter* painter, const QRectF& rect);
    void drawPorts(QPainter* painter);
    void drawHealthIndicator(QPainter* painter, const QRectF& rect);
    
    QColor getNodeColor() const;
    QColor getBorderColor() const;
    
    SubsystemNode* m_node;
    QSizeF m_size;
    bool m_highlighted;
    bool m_hovered;
    QFont m_titleFont;
    QFont m_textFont;
    
    // Visual parameters
    static constexpr qreal CORNER_RADIUS = 5.0;
    static constexpr qreal BORDER_WIDTH = 2.0;
    static constexpr qreal HEADER_HEIGHT = 30.0;
    static constexpr qreal PORT_RADIUS = 6.0;
    static constexpr qreal PORT_SPACING = 20.0;
    static constexpr qreal LED_SIZE = 12.0;
};

#endif // NODEWIDGET_H
