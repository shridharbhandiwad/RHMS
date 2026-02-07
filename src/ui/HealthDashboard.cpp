/**
 * @file HealthDashboard.cpp
 * @brief Implementation of HealthDashboard
 */

#include "HealthDashboard.h"
#include "../core/SubsystemNode.h"
#include "../graph/NodeGraphScene.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDateTime>

HealthDashboard::HealthDashboard(QWidget* parent)
    : QDockWidget("Health Dashboard", parent)
    , m_tableWidget(nullptr)
    , m_scene(nullptr)
    , m_refreshTimer(nullptr)
{
    setupUI();
    
    // Auto-refresh every 1 second
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &HealthDashboard::onRefreshTimer);
    m_refreshTimer->start(1000);
}

HealthDashboard::~HealthDashboard()
{
}

void HealthDashboard::setupUI()
{
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(5);
    m_tableWidget->setHorizontalHeaderLabels(
        QStringList() << "Status" << "Subsystem" << "Type" << "Message" << "Last Update"
    );
    
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Style
    m_tableWidget->setStyleSheet(R"(
        QTableWidget {
            background-color: #2d2d30;
            color: #e0e0e0;
            gridline-color: #3e3e42;
            border: none;
        }
        QTableWidget::item {
            padding: 5px;
        }
        QTableWidget::item:selected {
            background-color: #007acc;
        }
        QHeaderView::section {
            background-color: #3e3e42;
            color: #e0e0e0;
            padding: 5px;
            border: none;
        }
    )");
    
    setWidget(m_tableWidget);
}

void HealthDashboard::setNodeScene(NodeGraphScene* scene)
{
    m_scene = scene;
    updateDashboard();
}

void HealthDashboard::updateDashboard()
{
    populateHealthTable();
}

void HealthDashboard::clearDashboard()
{
    m_tableWidget->setRowCount(0);
}

void HealthDashboard::onRefreshTimer()
{
    if (m_scene) {
        updateDashboard();
    }
}

void HealthDashboard::populateHealthTable()
{
    if (!m_scene) {
        return;
    }
    
    m_tableWidget->setRowCount(0);
    
    QList<SubsystemNode*> nodes = m_scene->allNodes();
    
    // Sort by health status (errors first, then warnings, then OK)
    std::sort(nodes.begin(), nodes.end(), [](SubsystemNode* a, SubsystemNode* b) {
        int priorityA = static_cast<int>(a->healthStatus().code());
        int priorityB = static_cast<int>(b->healthStatus().code());
        return priorityA > priorityB;  // Higher priority (ERROR=2) comes first
    });
    
    for (SubsystemNode* node : nodes) {
        addHealthRow(node);
    }
}

void HealthDashboard::addHealthRow(SubsystemNode* node)
{
    if (!node) return;
    
    int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);
    
    HealthStatus status = node->healthStatus();
    
    // Status indicator
    QTableWidgetItem* statusItem = new QTableWidgetItem(status.statusIcon());
    statusItem->setBackground(getStatusColor(status.code()));
    statusItem->setTextAlignment(Qt::AlignCenter);
    m_tableWidget->setItem(row, 0, statusItem);
    
    // Subsystem name
    QTableWidgetItem* nameItem = new QTableWidgetItem(node->nodeName());
    m_tableWidget->setItem(row, 1, nameItem);
    
    // Subsystem type
    QTableWidgetItem* typeItem = new QTableWidgetItem(node->subsystemType());
    m_tableWidget->setItem(row, 2, typeItem);
    
    // Health message
    QTableWidgetItem* msgItem = new QTableWidgetItem(status.message());
    m_tableWidget->setItem(row, 3, msgItem);
    
    // Last update time
    qint64 lastUpdate = status.lastUpdateTime();
    QString timeStr;
    if (lastUpdate > 0) {
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(lastUpdate);
        timeStr = dt.toString("hh:mm:ss");
    } else {
        timeStr = "Never";
    }
    QTableWidgetItem* timeItem = new QTableWidgetItem(timeStr);
    m_tableWidget->setItem(row, 4, timeItem);
}

QColor HealthDashboard::getStatusColor(HealthCode code) const
{
    switch (code) {
        case HealthCode::OK:
            return QColor(0, 150, 0);      // Dark green
        case HealthCode::WARNING:
            return QColor(200, 140, 0);    // Dark orange
        case HealthCode::ERROR:
            return QColor(180, 0, 0);      // Dark red
        case HealthCode::OFFLINE:
            return QColor(80, 80, 80);     // Gray
        case HealthCode::UNKNOWN:
        default:
            return QColor(100, 100, 0);    // Dark yellow
    }
}
