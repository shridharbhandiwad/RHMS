/**
 * @file HealthStatus.cpp
 * @brief Implementation of HealthStatus class
 */

#include "HealthStatus.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

HealthStatus::HealthStatus()
    : m_code(HealthCode::UNKNOWN)
    , m_message("Initializing")
    , m_timestamp(0)
    , m_lastUpdate(0)
{
    updateTimestamp();
}

HealthStatus::HealthStatus(HealthCode code, const QString& message)
    : m_code(code)
    , m_message(message)
    , m_timestamp(0)
    , m_lastUpdate(0)
{
    updateTimestamp();
}

void HealthStatus::setCode(HealthCode code)
{
    m_code = code;
    updateTimestamp();
}

void HealthStatus::setMessage(const QString& message)
{
    m_message = message;
    updateTimestamp();
}

void HealthStatus::update(HealthCode code, const QString& message)
{
    m_code = code;
    if (!message.isEmpty()) {
        m_message = message;
    }
    updateTimestamp();
}

void HealthStatus::updateTimestamp()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (m_timestamp == 0) {
        m_timestamp = now;
    }
    m_lastUpdate = now;
}

QColor HealthStatus::statusColor() const
{
    switch (m_code) {
        case HealthCode::OK:
            return QColor(0, 255, 0);      // Green
        case HealthCode::WARNING:
            return QColor(255, 165, 0);    // Orange
        case HealthCode::ERROR:
            return QColor(255, 0, 0);      // Red
        case HealthCode::OFFLINE:
            return QColor(128, 128, 128);  // Gray
        case HealthCode::UNKNOWN:
        default:
            return QColor(200, 200, 0);    // Yellow
    }
}

QString HealthStatus::statusText() const
{
    switch (m_code) {
        case HealthCode::OK:
            return "OK";
        case HealthCode::WARNING:
            return "WARNING";
        case HealthCode::ERROR:
            return "ERROR";
        case HealthCode::OFFLINE:
            return "OFFLINE";
        case HealthCode::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

QString HealthStatus::statusIcon() const
{
    switch (m_code) {
        case HealthCode::OK:
            return "✓";
        case HealthCode::WARNING:
            return "⚠";
        case HealthCode::ERROR:
            return "✗";
        case HealthCode::OFFLINE:
            return "○";
        case HealthCode::UNKNOWN:
        default:
            return "?";
    }
}

bool HealthStatus::isTimedOut(qint64 timeoutMs) const
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    return (now - m_lastUpdate) > timeoutMs;
}

QString HealthStatus::serialize() const
{
    QJsonObject json;
    json["code"] = static_cast<int>(m_code);
    json["message"] = m_message;
    json["timestamp"] = m_timestamp;
    json["lastUpdate"] = m_lastUpdate;
    
    QJsonDocument doc(json);
    return doc.toJson(QJsonDocument::Compact);
}

HealthStatus HealthStatus::deserialize(const QString& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject json = doc.object();
    
    HealthStatus status;
    status.m_code = static_cast<HealthCode>(json["code"].toInt());
    status.m_message = json["message"].toString();
    status.m_timestamp = static_cast<qint64>(json["timestamp"].toDouble());
    status.m_lastUpdate = static_cast<qint64>(json["lastUpdate"].toDouble());
    
    return status;
}
