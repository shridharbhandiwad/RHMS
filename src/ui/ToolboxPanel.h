/**
 * @file ToolboxPanel.h
 * @brief Toolbox panel for dragging subsystem templates
 */

#ifndef TOOLBOXPANEL_H
#define TOOLBOXPANEL_H

#include <QDockWidget>
#include <QTreeWidget>
#include <QMap>

/**
 * @class ToolboxPanel
 * @brief Drag-and-drop toolbox for radar subsystems
 * 
 * Provides categorized list of available subsystem types
 * that can be dragged onto the node graph canvas.
 */
class ToolboxPanel : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ToolboxPanel(QWidget* parent = nullptr);
    ~ToolboxPanel();
    
    // Populate toolbox with available subsystems
    void populateSubsystems();
    
signals:
    void subsystemSelected(const QString& subsystemType);
    void subsystemDragStarted(const QString& subsystemType);
    
private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    
private:
    void setupUI();
    void createCategoryItems();
    
    QTreeWidget* m_treeWidget;
    QMap<QString, QTreeWidgetItem*> m_categories;
};

#endif // TOOLBOXPANEL_H
