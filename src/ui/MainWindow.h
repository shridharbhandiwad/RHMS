/**
 * @file MainWindow.h
 * @brief Main application window for Radar Health Monitoring System
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

class NodeGraphScene;
class NodeGraphView;
class ToolboxPanel;
class PropertiesPanel;
class HealthDashboard;
class TelemetryLogWindow;
class UdpTelemetryReceiver;
class HealthStatusDispatcher;
class HierarchicalGraphEngine;

/**
 * @class MainWindow
 * @brief Main application window
 * 
 * Integrates all UI components and manages the application workflow:
 * - Node graph canvas
 * - Toolbox panel
 * - Properties inspector
 * - Health dashboard
 * - Telemetry log
 * - UDP telemetry receiver
 * - Menu bar and toolbar
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent* event) override;
    
private slots:
    // File menu actions
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    
    // Edit menu actions
    void deleteSelected();
    void selectAll();
    void clearCanvas();
    
    // View menu actions
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void zoomToFit();
    
    // Telemetry menu actions
    void startTelemetry();
    void stopTelemetry();
    void configureTelemetry();
    
    // Help menu actions
    void showAbout();
    void showHelp();
    
    // Node operations
    void createNodeFromToolbox(const QString& subsystemType);
    void onNodeSelected(SubsystemNode* node);
    void onSelectionCleared();
    
    // Telemetry
    void onTelemetryReceived(const TelemetryPacket& packet);
    void onTelemetryError(const QString& error);
    void onTelemetryStatusChanged(const QString& status);
    
private:
    void setupUI();
    void createMenus();
    void createToolbar();
    void createDockPanels();
    void createStatusBar();
    void setupConnections();
    void initializeTelemetrySystem();
    void registerSubsystemNodes();
    
    // UI components
    NodeGraphScene* m_graphScene;
    NodeGraphView* m_graphView;
    ToolboxPanel* m_toolboxPanel;
    PropertiesPanel* m_propertiesPanel;
    HealthDashboard* m_healthDashboard;
    TelemetryLogWindow* m_telemetryLog;
    
    // Telemetry system
    UdpTelemetryReceiver* m_telemetryReceiver;
    HealthStatusDispatcher* m_healthDispatcher;
    
    // Hierarchical navigation
    HierarchicalGraphEngine* m_hierarchyEngine;
    
    // Status bar widgets
    QLabel* m_statusLabel;
    QLabel* m_telemetryStatusLabel;
    QLabel* m_zoomLabel;
    
    // State
    QString m_currentProjectFile;
    bool m_projectModified;
    quint16 m_telemetryPort;
};

#endif // MAINWINDOW_H
