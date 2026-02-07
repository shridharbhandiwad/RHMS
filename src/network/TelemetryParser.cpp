/**
 * @file TelemetryParser.cpp
 * @brief Implementation of telemetry parser
 */

#include "TelemetryParser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

TelemetryPacket TelemetryParser::parse(const QByteArray& data, Format format)
{
    if (data.isEmpty()) {
        qWarning() << "Cannot parse empty telemetry data";
        return TelemetryPacket();
    }
    
    // Auto-detect format if needed
    if (format == Format::Auto) {
        format = detectFormat(data);
    }
    
    // Parse based on format
    switch (format) {
        case Format::Binary:
            return parseBinary(data);
        case Format::Json:
            return parseJson(data);
        case Format::DefenseProtocol:
            return parseDefenseProtocol(data);
        default:
            qWarning() << "Unknown telemetry format";
            return TelemetryPacket();
    }
}

TelemetryPacket TelemetryParser::parseBinary(const QByteArray& data)
{
    // Use TelemetryPacket's built-in deserialization
    return TelemetryPacket::deserialize(data);
}

TelemetryPacket TelemetryParser::parseJson(const QByteArray& data)
{
    QString jsonString = QString::fromUtf8(data);
    return TelemetryPacket::fromJson(jsonString);
}

TelemetryPacket TelemetryParser::parseDefenseProtocol(const QByteArray& data)
{
    // Custom defense protocol parser
    // Format: [Header:4][SubsystemID:16][HealthCode:1][Timestamp:8][ParamCount:2][Params...]
    
    if (data.size() < 31) {
        qWarning() << "Defense protocol packet too small";
        return TelemetryPacket();
    }
    
    TelemetryPacket packet;
    
    // Parse header (magic number: "RHMS")
    QByteArray header = data.mid(0, 4);
    if (header != "RHMS") {
        qWarning() << "Invalid defense protocol header";
        return TelemetryPacket();
    }
    
    // Parse subsystem ID (16 bytes, UUID without braces)
    QString subsystemId = QString::fromUtf8(data.mid(4, 16)).trimmed();
    packet.setSubsystemId(subsystemId);
    
    // Parse health code (1 byte)
    quint8 healthCode = static_cast<quint8>(data.at(20));
    packet.setHealthCode(static_cast<HealthCode>(healthCode));
    
    // Parse timestamp (8 bytes, big-endian qint64)
    qint64 timestamp = 0;
    for (int i = 0; i < 8; ++i) {
        timestamp = (timestamp << 8) | static_cast<quint8>(data.at(21 + i));
    }
    packet.setTimestamp(timestamp);
    
    // Parse parameter count (2 bytes, big-endian quint16)
    quint16 paramCount = (static_cast<quint8>(data.at(29)) << 8) | 
                         static_cast<quint8>(data.at(30));
    
    // Parse parameters
    int offset = 31;
    for (quint16 i = 0; i < paramCount && offset < data.size(); ++i) {
        // Each parameter: [KeyLen:1][Key:KeyLen][ValueType:1][Value:varies]
        if (offset >= data.size()) break;
        
        quint8 keyLen = static_cast<quint8>(data.at(offset++));
        if (offset + keyLen >= data.size()) break;
        
        QString key = QString::fromUtf8(data.mid(offset, keyLen));
        offset += keyLen;
        
        if (offset >= data.size()) break;
        quint8 valueType = static_cast<quint8>(data.at(offset++));
        
        // Parse value based on type
        QVariant value;
        switch (valueType) {
            case 0: { // Double
                if (offset + 8 > data.size()) break;
                double val = 0;
                memcpy(&val, data.constData() + offset, 8);
                value = val;
                offset += 8;
                break;
            }
            case 1: { // Integer
                if (offset + 4 > data.size()) break;
                qint32 val = 0;
                memcpy(&val, data.constData() + offset, 4);
                value = val;
                offset += 4;
                break;
            }
            case 2: { // String
                if (offset >= data.size()) break;
                quint8 strLen = static_cast<quint8>(data.at(offset++));
                if (offset + strLen > data.size()) break;
                value = QString::fromUtf8(data.mid(offset, strLen));
                offset += strLen;
                break;
            }
        }
        
        packet.addParameter(key, value);
    }
    
    return packet;
}

TelemetryParser::Format TelemetryParser::detectFormat(const QByteArray& data)
{
    if (data.isEmpty()) {
        return Format::Binary;
    }
    
    // Check for defense protocol header
    if (data.size() >= 4 && data.left(4) == "RHMS") {
        return Format::DefenseProtocol;
    }
    
    // Check for JSON (starts with '{')
    if (data.trimmed().startsWith('{')) {
        return Format::Json;
    }
    
    // Default to binary
    return Format::Binary;
}

QByteArray TelemetryParser::encode(const TelemetryPacket& packet, Format format)
{
    switch (format) {
        case Format::Json:
            return packet.toJson().toUtf8();
        case Format::Binary:
        default:
            return packet.serialize();
    }
}
