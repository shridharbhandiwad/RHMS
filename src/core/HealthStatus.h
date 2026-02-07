/**
 * @file HealthStatus.h
 * @brief Health status definitions and utilities for radar subsystems
 */

#ifndef HEALTHSTATUS_H
#define HEALTHSTATUS_H

#include <QString>
#include <QColor>
#include <QMetaType>

/**
 * @enum HealthCode
 * @brief Standardized health status codes for radar subsystems
 */
enum class HealthCode {
    OK = 0,          ///< Subsystem operating normally
    WARNING = 1,     ///< Subsystem degraded but functional
    ERROR = 2,       ///< Subsystem failed or critical error
    OFFLINE = 3,     ///< Subsystem not responding
    UNKNOWN = 4      ///< Status not yet determined
};

Q_DECLARE_METATYPE(HealthCode)

/**
 * @class HealthStatus
 * @brief Represents the health state of a radar subsystem
 * 
 * Defense-grade health monitoring with timestamp tracking,
 * detailed messages, and visual indicator support.
 */
class HealthStatus
{
public:
    HealthStatus();
    HealthStatus(HealthCode code, const QString& message = "");
    
    // Getters
    HealthCode code() const { return m_code; }
    QString message() const { return m_message; }
    qint64 timestamp() const { return m_timestamp; }
    qint64 lastUpdateTime() const { return m_lastUpdate; }
    
    // Setters
    void setCode(HealthCode code);
    void setMessage(const QString& message);
    void update(HealthCode code, const QString& message = "");
    
    // Visual indicators
    QColor statusColor() const;
    QString statusText() const;
    QString statusIcon() const;
    
    // Status checks
    bool isHealthy() const { return m_code == HealthCode::OK; }
    bool isWarning() const { return m_code == HealthCode::WARNING; }
    bool isError() const { return m_code == HealthCode::ERROR; }
    bool isOffline() const { return m_code == HealthCode::OFFLINE; }
    bool isUnknown() const { return m_code == HealthCode::UNKNOWN; }
    
    // Timeout detection (heartbeat monitoring)
    bool isTimedOut(qint64 timeoutMs = 5000) const;
    
    // Serialization
    QString serialize() const;
    static HealthStatus deserialize(const QString& data);
    
private:
    HealthCode m_code;
    QString m_message;
    qint64 m_timestamp;      ///< Time when status was created
    qint64 m_lastUpdate;     ///< Time of last update
    
    void updateTimestamp();
};

#endif // HEALTHSTATUS_H
