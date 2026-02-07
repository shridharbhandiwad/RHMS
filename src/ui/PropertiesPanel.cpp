/**
 * @file PropertiesPanel.cpp
 * @brief Implementation of PropertiesPanel
 */

#include "PropertiesPanel.h"
#include "../core/SubsystemNode.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

PropertiesPanel::PropertiesPanel(QWidget* parent)
    : QDockWidget("Properties", parent)
    , m_tableWidget(nullptr)
    , m_currentNode(nullptr)
{
    setupUI();
}

PropertiesPanel::~PropertiesPanel()
{
}

void PropertiesPanel::setupUI()
{
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(2);
    m_tableWidget->setHorizontalHeaderLabels(QStringList() << "Property" << "Value");
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
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

void PropertiesPanel::displayNodeProperties(SubsystemNode* node)
{
    m_currentNode = node;
    populateProperties(node);
    
    // Connect to property changes for live updates
    if (node) {
        connect(node, &SubsystemNode::propertyChanged,
                this, &PropertiesPanel::updateProperties,
                Qt::UniqueConnection);
        connect(node, &SubsystemNode::healthStatusChanged,
                this, &PropertiesPanel::updateProperties,
                Qt::UniqueConnection);
    }
}

void PropertiesPanel::clearProperties()
{
    m_tableWidget->setRowCount(0);
    m_currentNode = nullptr;
}

void PropertiesPanel::updateProperties()
{
    if (m_currentNode) {
        populateProperties(m_currentNode);
    }
}

void PropertiesPanel::populateProperties(SubsystemNode* node)
{
    if (!node) {
        clearProperties();
        return;
    }
    
    m_tableWidget->setRowCount(0);
    
    // Basic information
    addPropertyRow("Node ID", node->nodeId());
    addPropertyRow("Node Name", node->nodeName());
    addPropertyRow("Subsystem Type", node->subsystemType());
    addPropertyRow("Category", node->subsystemCategory());
    
    // Health status
    addPropertyRow("Health Status", node->healthStatus().statusText());
    addPropertyRow("Health Message", node->healthStatus().message());
    addPropertyRow("Last Update", QString::number(node->healthStatus().lastUpdateTime()));
    
    // Custom properties
    QMap<QString, QVariant> properties = node->allProperties();
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        addPropertyRow(it.key(), it.value());
    }
    
    // Port information
    addPropertyRow("Input Ports", QString::number(node->inputPorts().size()));
    addPropertyRow("Output Ports", QString::number(node->outputPorts().size()));
    
    // Hierarchical info
    addPropertyRow("Has Child Graph", node->hasChildGraph() ? "Yes" : "No");
    addPropertyRow("Is Expanded", node->isExpanded() ? "Yes" : "No");
}

void PropertiesPanel::addPropertyRow(const QString& name, const QVariant& value)
{
    int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);
    
    QTableWidgetItem* nameItem = new QTableWidgetItem(name);
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    
    QTableWidgetItem* valueItem = new QTableWidgetItem(value.toString());
    valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
    
    m_tableWidget->setItem(row, 0, nameItem);
    m_tableWidget->setItem(row, 1, valueItem);
}
