/**
 * @file HealthDashboard.h
 * @brief Health monitoring dashboard panel
 */

#ifndef HEALTHDASHBOARD_H
#define HEALTHDASHBOARD_H

#include <QDockWidget>
#include <QTableWidget>
#include <QTimer>
#include "../core/HealthStatus.h"

class SubsystemNode;
class NodeGraphScene;

/**
 * @class HealthDashboard
 * @brief Overview dashboard for subsystem health monitoring
 * 
 * Displays real-time health status of all subsystems
 * with color-coded indicators and alerts.
 */
class HealthDashboard : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit HealthDashboard(QWidget* parent = nullptr);
    ~HealthDashboard();
    
    void setNodeScene(NodeGraphScene* scene);
    
public slots:
    void updateDashboard();
    void clearDashboard();
    
private slots:
    void onRefreshTimer();
    
private:
    void setupUI();
    void populateHealthTable();
    void addHealthRow(SubsystemNode* node);
    QColor getStatusColor(HealthCode code) const;
    
    QTableWidget* m_tableWidget;
    NodeGraphScene* m_scene;
    QTimer* m_refreshTimer;
};

#endif // HEALTHDASHBOARD_H
