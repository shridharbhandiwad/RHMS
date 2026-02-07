/**
 * @file ToolboxPanel.cpp
 * @brief Implementation of ToolboxPanel
 */

#include "ToolboxPanel.h"
#include "../core/RadarSubsystem.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>

ToolboxPanel::ToolboxPanel(QWidget* parent)
    : QDockWidget("Subsystem Toolbox", parent)
    , m_treeWidget(nullptr)
{
    setupUI();
    populateSubsystems();
}

ToolboxPanel::~ToolboxPanel()
{
}

void ToolboxPanel::setupUI()
{
    // Create tree widget
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabel("Radar Subsystems");
    m_treeWidget->setHeaderHidden(false);
    m_treeWidget->setIndentation(15);
    m_treeWidget->setAnimated(true);
    
    // Enable drag and drop
    m_treeWidget->setDragEnabled(true);
    m_treeWidget->setDragDropMode(QAbstractItemView::DragOnly);
    
    // Style
    m_treeWidget->setStyleSheet(R"(
        QTreeWidget {
            background-color: #2d2d30;
            color: #e0e0e0;
            border: none;
        }
        QTreeWidget::item {
            padding: 5px;
        }
        QTreeWidget::item:hover {
            background-color: #3e3e42;
        }
        QTreeWidget::item:selected {
            background-color: #007acc;
        }
    )");
    
    setWidget(m_treeWidget);
    
    // Connect signals
    connect(m_treeWidget, &QTreeWidget::itemClicked,
            this, &ToolboxPanel::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked,
            this, &ToolboxPanel::onItemDoubleClicked);
}

void ToolboxPanel::createCategoryItems()
{
    // Create category items
    m_categories["RF Systems"] = new QTreeWidgetItem(m_treeWidget, QStringList("RF Systems"));
    m_categories["Processing"] = new QTreeWidgetItem(m_treeWidget, QStringList("Processing"));
    m_categories["Mechanical"] = new QTreeWidgetItem(m_treeWidget, QStringList("Mechanical"));
    m_categories["Power"] = new QTreeWidgetItem(m_treeWidget, QStringList("Power"));
    m_categories["Communication"] = new QTreeWidgetItem(m_treeWidget, QStringList("Communication"));
    m_categories["Thermal"] = new QTreeWidgetItem(m_treeWidget, QStringList("Thermal"));
    m_categories["Control"] = new QTreeWidgetItem(m_treeWidget, QStringList("Control"));
    
    // Expand all categories
    m_treeWidget->expandAll();
}

void ToolboxPanel::populateSubsystems()
{
    m_treeWidget->clear();
    m_categories.clear();
    
    createCategoryItems();
    
    // Manually add subsystems (in real app, this would come from RadarSubsystem registry)
    QMap<QString, QStringList> subsystems;
    subsystems["RF Systems"] = QStringList() << "RF Frontend";
    subsystems["Processing"] = QStringList() << "Signal Processor" << "Tracker" << "Data Fusion";
    subsystems["Mechanical"] = QStringList() << "Antenna Servo";
    subsystems["Power"] = QStringList() << "Power Supply";
    subsystems["Communication"] = QStringList() << "Network Interface";
    subsystems["Thermal"] = QStringList() << "Cooling System";
    subsystems["Control"] = QStringList() << "Embedded Controller";
    
    // Add subsystem items to categories
    for (auto it = subsystems.begin(); it != subsystems.end(); ++it) {
        QString category = it.key();
        if (!m_categories.contains(category)) {
            continue;
        }
        
        QTreeWidgetItem* categoryItem = m_categories[category];
        for (const QString& subsystem : it.value()) {
            QTreeWidgetItem* item = new QTreeWidgetItem(categoryItem, QStringList(subsystem));
            item->setData(0, Qt::UserRole, subsystem);
            
            // Set icon/emoji for visual identification
            QString emoji;
            if (subsystem == "RF Frontend") emoji = "📡";
            else if (subsystem == "Signal Processor") emoji = "⚙️";
            else if (subsystem == "Tracker") emoji = "🎯";
            else if (subsystem == "Antenna Servo") emoji = "📐";
            else if (subsystem == "Data Fusion") emoji = "🔗";
            else if (subsystem == "Power Supply") emoji = "⚡";
            else if (subsystem == "Network Interface") emoji = "🌐";
            else if (subsystem == "Cooling System") emoji = "❄️";
            else if (subsystem == "Embedded Controller") emoji = "🖥️";
            
            item->setText(0, emoji + " " + subsystem);
        }
    }
    
    m_treeWidget->expandAll();
}

void ToolboxPanel::onItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)
    
    if (!item) return;
    
    // Check if it's a subsystem item (not a category)
    if (item->parent() != nullptr) {
        QString subsystemType = item->data(0, Qt::UserRole).toString();
        emit subsystemSelected(subsystemType);
        qDebug() << "Selected subsystem:" << subsystemType;
    }
}

void ToolboxPanel::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)
    
    if (!item) return;
    
    // Check if it's a subsystem item (not a category)
    if (item->parent() != nullptr) {
        QString subsystemType = item->data(0, Qt::UserRole).toString();
        // Double-click creates a node at the center of the canvas
        qDebug() << "Double-clicked subsystem:" << subsystemType;
    }
}
