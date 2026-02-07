/**
 * @file UdpTelemetryReceiver.h
 * @brief Thread-safe UDP receiver for radar telemetry packets
 */

#ifndef UDPTELEMETRYRECEIVER_H
#define UDPTELEMETRYRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <memory>
#include "../core/TelemetryPacket.h"

/**
 * @class UdpReceiverWorker
 * @brief Worker thread for UDP telemetry reception
 * 
 * Runs in separate thread to avoid blocking the UI.
 * Receives and parses telemetry packets in real-time.
 */
class UdpReceiverWorker : public QObject
{
    Q_OBJECT
    
public:
    explicit UdpReceiverWorker(quint16 port, QObject* parent = nullptr);
    ~UdpReceiverWorker();
    
public slots:
    void start();
    void stop();
    void setPort(quint16 port);
    
signals:
    void packetReceived(const TelemetryPacket& packet);
    void errorOccurred(const QString& error);
    void statusChanged(const QString& status);
    void started();
    void stopped();
    
private slots:
    void processPendingDatagrams();
    
private:
    bool bindSocket();
    void unbindSocket();
    
    QUdpSocket* m_socket;
    quint16 m_port;
    bool m_running;
    mutable QMutex m_mutex;
};

/**
 * @class UdpTelemetryReceiver
 * @brief Thread-safe UDP telemetry receiver for defense-grade monitoring
 * 
 * Manages a worker thread that continuously receives telemetry packets
 * and emits thread-safe signals for UI updates.
 */
class UdpTelemetryReceiver : public QObject
{
    Q_OBJECT
    
public:
    explicit UdpTelemetryReceiver(quint16 port = 5000, QObject* parent = nullptr);
    ~UdpTelemetryReceiver();
    
    // Control methods
    void start();
    void stop();
    bool isRunning() const;
    
    // Configuration
    void setPort(quint16 port);
    quint16 port() const { return m_port; }
    
    // Statistics
    quint64 packetsReceived() const { return m_packetsReceived; }
    quint64 packetsDropped() const { return m_packetsDropped; }
    void resetStatistics();
    
signals:
    void telemetryReceived(const TelemetryPacket& packet);
    void errorOccurred(const QString& error);
    void statusChanged(const QString& status);
    void receiverStarted();
    void receiverStopped();
    
private slots:
    void handlePacketReceived(const TelemetryPacket& packet);
    void handleWorkerStarted();
    void handleWorkerStopped();
    
private:
    quint16 m_port;
    bool m_running;
    quint64 m_packetsReceived;
    quint64 m_packetsDropped;
    
    QThread* m_workerThread;
    UdpReceiverWorker* m_worker;
    mutable QMutex m_mutex;
};

#endif // UDPTELEMETRYRECEIVER_H
