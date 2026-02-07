/**
 * @file NodeWidget.cpp
 * @brief Implementation of NodeWidget
 */

#include "NodeWidget.h"
#include "../core/SubsystemNode.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QCursor>
#include <QDebug>

NodeWidget::NodeWidget(SubsystemNode* node, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_node(node)
    , m_size(180, 120)
    , m_highlighted(false)
    , m_hovered(false)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
    setCursor(Qt::ArrowCursor);
    
    // Setup fonts
    m_titleFont = QFont("Arial", 10, QFont::Bold);
    m_textFont = QFont("Arial", 8);
    
    // Connect to node signals for live updates
    if (m_node) {
        connect(m_node, &SubsystemNode::healthStatusChanged,
                this, [this]() { update(); });
        connect(m_node, &SubsystemNode::nodeNameChanged,
                this, [this]() { update(); });
    }
}

NodeWidget::~NodeWidget()
{
}

QRectF NodeWidget::boundingRect() const
{
    return QRectF(-BORDER_WIDTH, -BORDER_WIDTH,
                  m_size.width() + BORDER_WIDTH * 2,
                  m_size.height() + BORDER_WIDTH * 2);
}

void NodeWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    drawNode(painter);
    drawPorts(painter);
}

QPainterPath NodeWidget::shape() const
{
    QPainterPath path;
    path.addRoundedRect(boundingRect(), CORNER_RADIUS, CORNER_RADIUS);
    return path;
}

QPointF NodeWidget::getPortPosition(const QString& portName, bool isOutput) const
{
    // Get port list
    QList<NodePort> ports = isOutput ? m_node->outputPorts() : m_node->inputPorts();
    
    // Find port index
    int index = -1;
    for (int i = 0; i < ports.size(); ++i) {
        if (ports[i].name == portName) {
            index = i;
            break;
        }
    }
    
    if (index < 0) {
        return QPointF(0, 0);
    }
    
    // Calculate position
    qreal y = HEADER_HEIGHT + PORT_SPACING * (index + 1);
    qreal x = isOutput ? m_size.width() : 0;
    
    return QPointF(x, y);
}

QList<QPointF> NodeWidget::inputPortPositions() const
{
    QList<QPointF> positions;
    QList<NodePort> ports = m_node->inputPorts();
    
    for (int i = 0; i < ports.size(); ++i) {
        qreal y = HEADER_HEIGHT + PORT_SPACING * (i + 1);
        positions.append(QPointF(0, y));
    }
    
    return positions;
}

QList<QPointF> NodeWidget::outputPortPositions() const
{
    QList<QPointF> positions;
    QList<NodePort> ports = m_node->outputPorts();
    
    for (int i = 0; i < ports.size(); ++i) {
        qreal y = HEADER_HEIGHT + PORT_SPACING * (i + 1);
        positions.append(QPointF(m_size.width(), y));
    }
    
    return positions;
}

void NodeWidget::setHighlighted(bool highlighted)
{
    if (m_highlighted != highlighted) {
        m_highlighted = highlighted;
        update();
    }
}

void NodeWidget::setNodeSize(const QSizeF& size)
{
    prepareGeometryChange();
    m_size = size;
    update();
}

void NodeWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsItem::mousePressEvent(event);
}

void NodeWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void NodeWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(Qt::ArrowCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}

void NodeWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    // Double-click to drill down into hierarchical nodes
    if (m_node && m_node->hasChildGraph()) {
        qDebug() << "Double-clicked node with child graph:" << m_node->nodeName();
        // Emit signal for hierarchical navigation (handled by MainWindow)
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void NodeWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    m_hovered = true;
    update();
    QGraphicsItem::hoverEnterEvent(event);
}

void NodeWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    m_hovered = false;
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

QVariant NodeWidget::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged && scene()) {
        // Notify that position changed (connections need update)
        // This is handled by NodeGraphScene
    }
    
    return QGraphicsItem::itemChange(change, value);
}

void NodeWidget::drawNode(QPainter* painter)
{
    QRectF rect(0, 0, m_size.width(), m_size.height());
    
    // Draw shadow
    if (!isSelected()) {
        QRectF shadowRect = rect.translated(2, 2);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 0, 50));
        painter->drawRoundedRect(shadowRect, CORNER_RADIUS, CORNER_RADIUS);
    }
    
    // Draw body
    painter->setPen(QPen(getBorderColor(), BORDER_WIDTH));
    painter->setBrush(QColor(60, 60, 63));
    painter->drawRoundedRect(rect, CORNER_RADIUS, CORNER_RADIUS);
    
    // Draw header
    QRectF headerRect(0, 0, m_size.width(), HEADER_HEIGHT);
    drawHeader(painter, headerRect);
    
    // Draw body content
    QRectF bodyRect(0, HEADER_HEIGHT, m_size.width(), m_size.height() - HEADER_HEIGHT);
    drawBody(painter, bodyRect);
}

void NodeWidget::drawHeader(QPainter* painter, const QRectF& rect)
{
    // Header background
    QPainterPath headerPath;
    headerPath.moveTo(rect.topLeft());
    headerPath.lineTo(rect.topRight());
    headerPath.lineTo(rect.bottomRight());
    headerPath.lineTo(rect.bottomLeft());
    
    QColor headerColor = getNodeColor();
    painter->setPen(Qt::NoPen);
    painter->setBrush(headerColor);
    painter->drawPath(headerPath);
    
    // Health LED
    QRectF ledRect(rect.width() - LED_SIZE - 5, 
                   (rect.height() - LED_SIZE) / 2,
                   LED_SIZE, LED_SIZE);
    drawHealthIndicator(painter, ledRect);
    
    // Node name
    painter->setPen(Qt::white);
    painter->setFont(m_titleFont);
    QRectF textRect = rect.adjusted(5, 0, -LED_SIZE - 10, 0);
    QString displayName = m_node ? m_node->nodeName() : "Unknown";
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, displayName);
}

void NodeWidget::drawBody(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect)
    
    // Draw node type
    if (m_node) {
        painter->setPen(QColor(200, 200, 200));
        painter->setFont(m_textFont);
        QRectF typeRect(5, HEADER_HEIGHT + 5, m_size.width() - 10, 15);
        painter->drawText(typeRect, Qt::AlignCenter, m_node->subsystemType());
    }
}

void NodeWidget::drawPorts(QPainter* painter)
{
    painter->setPen(Qt::NoPen);
    
    // Draw input ports
    QList<NodePort> inputPorts = m_node->inputPorts();
    for (int i = 0; i < inputPorts.size(); ++i) {
        QPointF portPos(0, HEADER_HEIGHT + PORT_SPACING * (i + 1));
        
        QColor portColor(100, 150, 255);  // Blue for inputs
        painter->setBrush(portColor);
        painter->drawEllipse(portPos, PORT_RADIUS, PORT_RADIUS);
        
        // Port label
        painter->setPen(QColor(180, 180, 180));
        painter->setFont(m_textFont);
        QRectF labelRect(PORT_RADIUS * 2, portPos.y() - 8, 60, 16);
        painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter,
                         inputPorts[i].name);
        painter->setPen(Qt::NoPen);
    }
    
    // Draw output ports
    QList<NodePort> outputPorts = m_node->outputPorts();
    for (int i = 0; i < outputPorts.size(); ++i) {
        QPointF portPos(m_size.width(), HEADER_HEIGHT + PORT_SPACING * (i + 1));
        
        QColor portColor(255, 150, 100);  // Orange for outputs
        painter->setBrush(portColor);
        painter->drawEllipse(portPos, PORT_RADIUS, PORT_RADIUS);
        
        // Port label
        painter->setPen(QColor(180, 180, 180));
        painter->setFont(m_textFont);
        QRectF labelRect(m_size.width() - 70, portPos.y() - 8, 60, 16);
        painter->drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter,
                         outputPorts[i].name);
        painter->setPen(Qt::NoPen);
    }
}

void NodeWidget::drawHealthIndicator(QPainter* painter, const QRectF& rect)
{
    if (!m_node) {
        return;
    }
    
    QColor ledColor = m_node->healthStatus().statusColor();
    
    // Draw LED with glow effect
    painter->setPen(Qt::NoPen);
    
    // Outer glow
    QRadialGradient glow(rect.center(), LED_SIZE / 2);
    glow.setColorAt(0, ledColor);
    glow.setColorAt(1, QColor(ledColor.red(), ledColor.green(), ledColor.blue(), 0));
    painter->setBrush(glow);
    painter->drawEllipse(rect);
    
    // Inner LED
    QRectF innerRect = rect.adjusted(2, 2, -2, -2);
    QRadialGradient led(innerRect.center(), LED_SIZE / 4);
    led.setColorAt(0, ledColor.lighter(150));
    led.setColorAt(1, ledColor);
    painter->setBrush(led);
    painter->drawEllipse(innerRect);
}

QColor NodeWidget::getNodeColor() const
{
    if (!m_node) {
        return QColor(100, 100, 100);
    }
    
    return m_node->nodeColor();
}

QColor NodeWidget::getBorderColor() const
{
    if (isSelected()) {
        return QColor(255, 255, 0);  // Yellow for selection
    } else if (m_highlighted) {
        return QColor(255, 165, 0);  // Orange for highlight
    } else if (m_hovered) {
        return QColor(150, 150, 150);
    } else {
        return QColor(80, 80, 80);
    }
}
