/**
 * @file PropertiesPanel.h
 * @brief Properties inspector panel for selected nodes
 */

#ifndef PROPERTIESPANEL_H
#define PROPERTIESPANEL_H

#include <QDockWidget>
#include <QTableWidget>

class SubsystemNode;

/**
 * @class PropertiesPanel
 * @brief Displays and edits properties of selected nodes
 * 
 * Shows node properties, telemetry data, and configuration
 * in a table format with live updates.
 */
class PropertiesPanel : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit PropertiesPanel(QWidget* parent = nullptr);
    ~PropertiesPanel();
    
public slots:
    void displayNodeProperties(SubsystemNode* node);
    void clearProperties();
    void updateProperties();
    
private:
    void setupUI();
    void populateProperties(SubsystemNode* node);
    void addPropertyRow(const QString& name, const QVariant& value);
    
    QTableWidget* m_tableWidget;
    SubsystemNode* m_currentNode;
};

#endif // PROPERTIESPANEL_H
