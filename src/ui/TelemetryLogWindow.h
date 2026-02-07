/**
 * @file TelemetryLogWindow.h
 * @brief Telemetry log window for monitoring packets
 */

#ifndef TELEMETRYLOGWINDOW_H
#define TELEMETRYLOGWINDOW_H

#include <QDockWidget>
#include <QTextEdit>
#include <QPushButton>
#include "../core/TelemetryPacket.h"

/**
 * @class TelemetryLogWindow
 * @brief Log window for telemetry packet monitoring
 * 
 * Displays incoming telemetry packets with timestamps
 * and provides filtering and export capabilities.
 */
class TelemetryLogWindow : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit TelemetryLogWindow(QWidget* parent = nullptr);
    ~TelemetryLogWindow();
    
public slots:
    void logTelemetryPacket(const TelemetryPacket& packet);
    void logMessage(const QString& message);
    void clearLog();
    void exportLog();
    
private:
    void setupUI();
    QString formatPacket(const TelemetryPacket& packet) const;
    
    QTextEdit* m_logTextEdit;
    QPushButton* m_clearButton;
    QPushButton* m_exportButton;
    int m_packetCount;
};

#endif // TELEMETRYLOGWINDOW_H
