/**
 * @file TelemetryParser.h
 * @brief Parser for telemetry packet formats
 */

#ifndef TELEMETRYPARSER_H
#define TELEMETRYPARSER_H

#include <QByteArray>
#include "../core/TelemetryPacket.h"

/**
 * @class TelemetryParser
 * @brief Parses binary and JSON telemetry packet formats
 * 
 * Supports multiple packet formats:
 * - Binary (QDataStream format)
 * - JSON (text-based)
 * - Custom defense protocol
 */
class TelemetryParser
{
public:
    enum class Format {
        Auto,           ///< Auto-detect format
        Binary,         ///< QDataStream binary format
        Json,           ///< JSON text format
        DefenseProtocol ///< Custom defense binary protocol
    };
    
    // Parse methods
    static TelemetryPacket parse(const QByteArray& data, Format format = Format::Auto);
    static TelemetryPacket parseBinary(const QByteArray& data);
    static TelemetryPacket parseJson(const QByteArray& data);
    static TelemetryPacket parseDefenseProtocol(const QByteArray& data);
    
    // Format detection
    static Format detectFormat(const QByteArray& data);
    
    // Encoding methods
    static QByteArray encode(const TelemetryPacket& packet, Format format = Format::Binary);
    
private:
    TelemetryParser() = default;
};

#endif // TELEMETRYPARSER_H
