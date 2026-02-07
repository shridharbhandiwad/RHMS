/**
 * @file TelemetryPacket.h
 * @brief Telemetry packet structure for UDP health monitoring
 */

#ifndef TELEMETRYPACKET_H
#define TELEMETRYPACKET_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QByteArray>
#include "HealthStatus.h"

/**
 * @class TelemetryPacket
 * @brief Encapsulates telemetry data received via UDP
 * 
 * Thread-safe telemetry packet representation for defense-grade
 * real-time radar health monitoring.
 */
class TelemetryPacket
{
public:
    TelemetryPacket();
    TelemetryPacket(const QString& subsystemId, HealthCode healthCode);
    
    // Core identification
    QString subsystemId() const { return m_subsystemId; }
    void setSubsystemId(const QString& id) { m_subsystemId = id; }
    
    // Health information
    HealthCode healthCode() const { return m_healthCode; }
    void setHealthCode(HealthCode code) { m_healthCode = code; }
    
    QString healthMessage() const { return m_healthMessage; }
    void setHealthMessage(const QString& msg) { m_healthMessage = msg; }
    
    // Timestamp
    qint64 timestamp() const { return m_timestamp; }
    void setTimestamp(qint64 ts) { m_timestamp = ts; }
    void updateTimestamp();
    
    // Telemetry parameters (key-value pairs)
    void addParameter(const QString& key, const QVariant& value);
    QVariant parameter(const QString& key) const;
    QMap<QString, QVariant> allParameters() const { return m_parameters; }
    bool hasParameter(const QString& key) const;
    void clearParameters();
    
    // Common telemetry fields
    void setTemperature(double temp) { addParameter("temperature", temp); }
    void setVoltage(double voltage) { addParameter("voltage", voltage); }
    void setCurrent(double current) { addParameter("current", current); }
    void setPower(double power) { addParameter("power", power); }
    void setFrequency(double freq) { addParameter("frequency", freq); }
    void setLatency(int latency) { addParameter("latency", latency); }
    void setErrorCount(int errors) { addParameter("error_count", errors); }
    
    double temperature() const { return parameter("temperature").toDouble(); }
    double voltage() const { return parameter("voltage").toDouble(); }
    double current() const { return parameter("current").toDouble(); }
    double power() const { return parameter("power").toDouble(); }
    double frequency() const { return parameter("frequency").toDouble(); }
    int latency() const { return parameter("latency").toInt(); }
    int errorCount() const { return parameter("error_count").toInt(); }
    
    // Serialization for UDP transmission
    QByteArray serialize() const;
    static TelemetryPacket deserialize(const QByteArray& data);
    
    // JSON serialization
    QString toJson() const;
    static TelemetryPacket fromJson(const QString& json);
    
    // Validation
    bool isValid() const;
    
private:
    QString m_subsystemId;
    HealthCode m_healthCode;
    QString m_healthMessage;
    qint64 m_timestamp;
    QMap<QString, QVariant> m_parameters;
};

#endif // TELEMETRYPACKET_H
