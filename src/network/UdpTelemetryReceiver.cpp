/**
 * @file UdpTelemetryReceiver.cpp
 * @brief Implementation of UDP telemetry receiver
 */

#include "UdpTelemetryReceiver.h"
#include "TelemetryParser.h"
#include <QDebug>
#include <QHostAddress>
#include <QMutexLocker>

// ============================================================================
// UdpReceiverWorker Implementation
// ============================================================================

UdpReceiverWorker::UdpReceiverWorker(quint16 port, QObject* parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_port(port)
    , m_running(false)
{
}

UdpReceiverWorker::~UdpReceiverWorker()
{
    stop();
}

void UdpReceiverWorker::start()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_running) {
        qWarning() << "UDP receiver already running";
        return;
    }
    
    if (bindSocket()) {
        m_running = true;
        emit statusChanged(QString("UDP receiver started on port %1").arg(m_port));
        emit started();
        qInfo() << "UDP telemetry receiver started on port" << m_port;
    } else {
        emit errorOccurred(QString("Failed to bind to port %1").arg(m_port));
    }
}

void UdpReceiverWorker::stop()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_running) {
        return;
    }
    
    unbindSocket();
    m_running = false;
    emit statusChanged("UDP receiver stopped");
    emit stopped();
    qInfo() << "UDP telemetry receiver stopped";
}

void UdpReceiverWorker::setPort(quint16 port)
{
    QMutexLocker locker(&m_mutex);
    
    bool wasRunning = m_running;
    if (wasRunning) {
        locker.unlock();
        stop();
        locker.relock();
    }
    
    m_port = port;
    
    if (wasRunning) {
        locker.unlock();
        start();
    }
}

bool UdpReceiverWorker::bindSocket()
{
    if (m_socket) {
        delete m_socket;
        m_socket = nullptr;
    }
    
    m_socket = new QUdpSocket(this);
    
    // Connect socket signals
    connect(m_socket, &QUdpSocket::readyRead,
            this, &UdpReceiverWorker::processPendingDatagrams);
    
    // Bind to port for receiving datagrams
    if (!m_socket->bind(QHostAddress::Any, m_port)) {
        qCritical() << "Failed to bind UDP socket to port" << m_port 
                    << ":" << m_socket->errorString();
        delete m_socket;
        m_socket = nullptr;
        return false;
    }
    
    qInfo() << "UDP socket bound to port" << m_port;
    return true;
}

void UdpReceiverWorker::unbindSocket()
{
    if (m_socket) {
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }
}

void UdpReceiverWorker::processPendingDatagrams()
{
    while (m_socket && m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        
        QHostAddress sender;
        quint16 senderPort;
        
        qint64 bytesRead = m_socket->readDatagram(datagram.data(), datagram.size(),
                                                   &sender, &senderPort);
        
        if (bytesRead > 0) {
            // Parse the telemetry packet
            TelemetryPacket packet = TelemetryParser::parse(datagram);
            
            if (packet.isValid()) {
                emit packetReceived(packet);
            } else {
                qWarning() << "Received invalid telemetry packet from" 
                          << sender.toString() << ":" << senderPort;
            }
        }
    }
}

// ============================================================================
// UdpTelemetryReceiver Implementation
// ============================================================================

UdpTelemetryReceiver::UdpTelemetryReceiver(quint16 port, QObject* parent)
    : QObject(parent)
    , m_port(port)
    , m_running(false)
    , m_packetsReceived(0)
    , m_packetsDropped(0)
    , m_workerThread(nullptr)
    , m_worker(nullptr)
{
    // Create worker thread
    m_workerThread = new QThread(this);
    m_worker = new UdpReceiverWorker(m_port);
    m_worker->moveToThread(m_workerThread);
    
    // Connect signals
    connect(m_workerThread, &QThread::started,
            m_worker, &UdpReceiverWorker::start);
    
    connect(m_worker, &UdpReceiverWorker::packetReceived,
            this, &UdpTelemetryReceiver::handlePacketReceived);
    
    connect(m_worker, &UdpReceiverWorker::errorOccurred,
            this, &UdpTelemetryReceiver::errorOccurred);
    
    connect(m_worker, &UdpReceiverWorker::statusChanged,
            this, &UdpTelemetryReceiver::statusChanged);
    
    connect(m_worker, &UdpReceiverWorker::started,
            this, &UdpTelemetryReceiver::handleWorkerStarted);
    
    connect(m_worker, &UdpReceiverWorker::stopped,
            this, &UdpTelemetryReceiver::handleWorkerStopped);
    
    // Cleanup on thread finish
    connect(m_workerThread, &QThread::finished,
            m_worker, &QObject::deleteLater);
}

UdpTelemetryReceiver::~UdpTelemetryReceiver()
{
    stop();
    
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait(3000);
        if (m_workerThread->isRunning()) {
            m_workerThread->terminate();
            m_workerThread->wait();
        }
    }
}

void UdpTelemetryReceiver::start()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_running) {
        qWarning() << "Telemetry receiver already running";
        return;
    }
    
    qInfo() << "Starting UDP telemetry receiver on port" << m_port;
    m_workerThread->start();
}

void UdpTelemetryReceiver::stop()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_running) {
        return;
    }
    
    qInfo() << "Stopping UDP telemetry receiver";
    QMetaObject::invokeMethod(m_worker, "stop", Qt::QueuedConnection);
    
    m_workerThread->quit();
    m_workerThread->wait(3000);
}

bool UdpTelemetryReceiver::isRunning() const
{
    QMutexLocker locker(&m_mutex);
    return m_running;
}

void UdpTelemetryReceiver::setPort(quint16 port)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_port != port) {
        m_port = port;
        if (m_worker) {
            QMetaObject::invokeMethod(m_worker, [this, port]() {
                m_worker->setPort(port);
            }, Qt::QueuedConnection);
        }
    }
}

void UdpTelemetryReceiver::resetStatistics()
{
    QMutexLocker locker(&m_mutex);
    m_packetsReceived = 0;
    m_packetsDropped = 0;
}

void UdpTelemetryReceiver::handlePacketReceived(const TelemetryPacket& packet)
{
    {
        QMutexLocker locker(&m_mutex);
        m_packetsReceived++;
    }
    
    // Forward to subscribers (thread-safe signal)
    emit telemetryReceived(packet);
}

void UdpTelemetryReceiver::handleWorkerStarted()
{
    {
        QMutexLocker locker(&m_mutex);
        m_running = true;
    }
    emit receiverStarted();
}

void UdpTelemetryReceiver::handleWorkerStopped()
{
    {
        QMutexLocker locker(&m_mutex);
        m_running = false;
    }
    emit receiverStopped();
}
