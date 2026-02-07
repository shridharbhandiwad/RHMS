/**
 * @file TelemetryPacket.cpp
 * @brief Implementation of TelemetryPacket class
 */

#include "TelemetryPacket.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDataStream>

TelemetryPacket::TelemetryPacket()
    : m_healthCode(HealthCode::UNKNOWN)
    , m_timestamp(0)
{
    updateTimestamp();
}

TelemetryPacket::TelemetryPacket(const QString& subsystemId, HealthCode healthCode)
    : m_subsystemId(subsystemId)
    , m_healthCode(healthCode)
    , m_timestamp(0)
{
    updateTimestamp();
}

void TelemetryPacket::updateTimestamp()
{
    m_timestamp = QDateTime::currentMSecsSinceEpoch();
}

void TelemetryPacket::addParameter(const QString& key, const QVariant& value)
{
    m_parameters[key] = value;
}

QVariant TelemetryPacket::parameter(const QString& key) const
{
    return m_parameters.value(key, QVariant());
}

bool TelemetryPacket::hasParameter(const QString& key) const
{
    return m_parameters.contains(key);
}

void TelemetryPacket::clearParameters()
{
    m_parameters.clear();
}

QByteArray TelemetryPacket::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    
    stream << m_subsystemId;
    stream << static_cast<int>(m_healthCode);
    stream << m_healthMessage;
    stream << m_timestamp;
    stream << m_parameters;
    
    return data;
}

TelemetryPacket TelemetryPacket::deserialize(const QByteArray& data)
{
    TelemetryPacket packet;
    QDataStream stream(data);
    stream.setVersion(QDataStream::Qt_5_15);
    
    int healthCodeInt;
    stream >> packet.m_subsystemId;
    stream >> healthCodeInt;
    stream >> packet.m_healthMessage;
    stream >> packet.m_timestamp;
    stream >> packet.m_parameters;
    
    packet.m_healthCode = static_cast<HealthCode>(healthCodeInt);
    
    return packet;
}

QString TelemetryPacket::toJson() const
{
    QJsonObject json;
    json["subsystem_id"] = m_subsystemId;
    json["health_code"] = static_cast<int>(m_healthCode);
    json["health_message"] = m_healthMessage;
    json["timestamp"] = m_timestamp;
    
    QJsonObject params;
    for (auto it = m_parameters.begin(); it != m_parameters.end(); ++it) {
        params[it.key()] = QJsonValue::fromVariant(it.value());
    }
    json["parameters"] = params;
    
    QJsonDocument doc(json);
    return doc.toJson(QJsonDocument::Compact);
}

TelemetryPacket TelemetryPacket::fromJson(const QString& json)
{
    TelemetryPacket packet;
    
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        return packet;
    }
    
    QJsonObject root = doc.object();
    packet.m_subsystemId = root["subsystem_id"].toString();
    packet.m_healthCode = static_cast<HealthCode>(root["health_code"].toInt());
    packet.m_healthMessage = root["health_message"].toString();
    packet.m_timestamp = root["timestamp"].toVariant().toLongLong();
    
    QJsonObject params = root["parameters"].toObject();
    for (auto it = params.begin(); it != params.end(); ++it) {
        packet.m_parameters[it.key()] = it.value().toVariant();
    }
    
    return packet;
}

bool TelemetryPacket::isValid() const
{
    return !m_subsystemId.isEmpty() && m_timestamp > 0;
}
