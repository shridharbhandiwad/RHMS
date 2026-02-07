/**
 * @file TelemetryLogWindow.cpp
 * @brief Implementation of TelemetryLogWindow
 */

#include "TelemetryLogWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

TelemetryLogWindow::TelemetryLogWindow(QWidget* parent)
    : QDockWidget("Telemetry Log", parent)
    , m_logTextEdit(nullptr)
    , m_clearButton(nullptr)
    , m_exportButton(nullptr)
    , m_packetCount(0)
{
    setupUI();
}

TelemetryLogWindow::~TelemetryLogWindow()
{
}

void TelemetryLogWindow::setupUI()
{
    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    
    // Log text area
    m_logTextEdit = new QTextEdit(this);
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setLineWrapMode(QTextEdit::NoWrap);
    m_logTextEdit->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e1e;
            color: #d4d4d4;
            font-family: 'Courier New', monospace;
            font-size: 9pt;
            border: none;
        }
    )");
    
    // Button bar
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_clearButton = new QPushButton("Clear", this);
    m_exportButton = new QPushButton("Export", this);
    
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addStretch();
    
    // Assemble layout
    mainLayout->addWidget(m_logTextEdit);
    mainLayout->addLayout(buttonLayout);
    
    setWidget(mainWidget);
    
    // Connect signals
    connect(m_clearButton, &QPushButton::clicked, this, &TelemetryLogWindow::clearLog);
    connect(m_exportButton, &QPushButton::clicked, this, &TelemetryLogWindow::exportLog);
}

void TelemetryLogWindow::logTelemetryPacket(const TelemetryPacket& packet)
{
    m_packetCount++;
    QString formattedPacket = formatPacket(packet);
    
    // Add timestamp
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString logLine = QString("[%1] #%2 %3")
                         .arg(timestamp)
                         .arg(m_packetCount, 6, 10, QChar('0'))
                         .arg(formattedPacket);
    
    // Append to log
    m_logTextEdit->append(logLine);
    
    // Auto-scroll to bottom
    QTextCursor cursor = m_logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logTextEdit->setTextCursor(cursor);
    
    // Limit log size to prevent memory issues
    if (m_packetCount > 10000) {
        QTextCursor limitCursor = m_logTextEdit->textCursor();
        limitCursor.movePosition(QTextCursor::Start);
        limitCursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 100);
        limitCursor.removeSelectedText();
    }
}

void TelemetryLogWindow::logMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString logLine = QString("[%1] %2").arg(timestamp).arg(message);
    m_logTextEdit->append(logLine);
}

void TelemetryLogWindow::clearLog()
{
    m_logTextEdit->clear();
    m_packetCount = 0;
    logMessage("Log cleared");
}

void TelemetryLogWindow::exportLog()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Telemetry Log",
        QString("telemetry_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "Text Files (*.txt);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error",
                            QString("Could not open file for writing:\n%1").arg(fileName));
        return;
    }
    
    QTextStream out(&file);
    out << m_logTextEdit->toPlainText();
    file.close();
    
    logMessage(QString("Log exported to: %1").arg(fileName));
}

QString TelemetryLogWindow::formatPacket(const TelemetryPacket& packet) const
{
    QString formatted;
    
    // Subsystem ID and health
    formatted += QString("ID:%1 ").arg(packet.subsystemId().left(8));
    
    // Health code
    QString healthStr;
    switch (packet.healthCode()) {
        case HealthCode::OK:      healthStr = "OK  "; break;
        case HealthCode::WARNING: healthStr = "WARN"; break;
        case HealthCode::ERROR:   healthStr = "ERR "; break;
        case HealthCode::OFFLINE: healthStr = "OFF "; break;
        default:                  healthStr = "UNK "; break;
    }
    formatted += QString("[%1] ").arg(healthStr);
    
    // Message
    if (!packet.healthMessage().isEmpty()) {
        formatted += packet.healthMessage() + " ";
    }
    
    // Parameters
    QMap<QString, QVariant> params = packet.allParameters();
    if (!params.isEmpty()) {
        formatted += "{ ";
        for (auto it = params.begin(); it != params.end(); ++it) {
            formatted += QString("%1:%2 ").arg(it.key()).arg(it.value().toString());
        }
        formatted += "}";
    }
    
    return formatted;
}
