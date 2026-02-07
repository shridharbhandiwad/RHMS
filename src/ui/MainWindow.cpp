/**
 * @file MainWindow.cpp
 * @brief Implementation of MainWindow
 */

#include "MainWindow.h"
#include "NodeWidget.h"
#include "ToolboxPanel.h"
#include "PropertiesPanel.h"
#include "HealthDashboard.h"
#include "TelemetryLogWindow.h"
#include "../graph/NodeGraphScene.h"
#include "../graph/NodeGraphView.h"
#include "../graph/HierarchicalGraphEngine.h"
#include "../network/UdpTelemetryReceiver.h"
#include "../network/HealthStatusDispatcher.h"
#include "../core/RadarSubsystem.h"
#include "../core/SubsystemNode.h"
#include "../nodes/RFFrontendNode.h"
#include "../nodes/SignalProcessorNode.h"
#include "../nodes/TrackerNode.h"
#include "../nodes/AntennaServoNode.h"
#include "../nodes/DataFusionNode.h"
#include "../nodes/PowerSupplyNode.h"
#include "../nodes/NetworkInterfaceNode.h"
#include "../nodes/CoolingSystemNode.h"
#include "../nodes/EmbeddedControllerNode.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_graphScene(nullptr)
    , m_graphView(nullptr)
    , m_toolboxPanel(nullptr)
    , m_propertiesPanel(nullptr)
    , m_healthDashboard(nullptr)
    , m_telemetryLog(nullptr)
    , m_telemetryReceiver(nullptr)
    , m_healthDispatcher(nullptr)
    , m_hierarchyEngine(nullptr)
    , m_statusLabel(nullptr)
    , m_telemetryStatusLabel(nullptr)
    , m_zoomLabel(nullptr)
    , m_projectModified(false)
    , m_telemetryPort(5000)
{
    registerSubsystemNodes();
    setupUI();
    createMenus();
    createToolbar();
    createDockPanels();
    createStatusBar();
    setupConnections();
    initializeTelemetrySystem();
    
    // Set window properties
    setWindowTitle("Radar Health Monitoring Editor - Defense Grade");
    resize(1600, 900);
    
    qInfo() << "MainWindow initialized successfully";
}

MainWindow::~MainWindow()
{
    if (m_telemetryReceiver) {
        m_telemetryReceiver->stop();
    }
}

void MainWindow::setupUI()
{
    // Create node graph scene and view
    m_graphScene = new NodeGraphScene(this);
    m_graphView = new NodeGraphView(m_graphScene, this);
    
    setCentralWidget(m_graphView);
    
    // Create hierarchical engine
    m_hierarchyEngine = new HierarchicalGraphEngine(this);
    m_hierarchyEngine->setRootScene(m_graphScene);
    
    // Apply dark theme
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2d2d30;
        }
        QMenuBar {
            background-color: #3e3e42;
            color: #e0e0e0;
        }
        QMenuBar::item:selected {
            background-color: #007acc;
        }
        QMenu {
            background-color: #3e3e42;
            color: #e0e0e0;
        }
        QMenu::item:selected {
            background-color: #007acc;
        }
        QToolBar {
            background-color: #3e3e42;
            border: none;
        }
        QStatusBar {
            background-color: #007acc;
            color: #e0e0e0;
        }
    )");
}

void MainWindow::createMenus()
{
    // File menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New", this, &MainWindow::newProject, QKeySequence::New);
    fileMenu->addAction("&Open...", this, &MainWindow::openProject, QKeySequence::Open);
    fileMenu->addAction("&Save", this, &MainWindow::saveProject, QKeySequence::Save);
    fileMenu->addAction("Save &As...", this, &MainWindow::saveProjectAs);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QMainWindow::close, QKeySequence::Quit);
    
    // Edit menu
    QMenu* editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("&Delete", this, &MainWindow::deleteSelected, QKeySequence::Delete);
    editMenu->addAction("Select &All", this, &MainWindow::selectAll, QKeySequence::SelectAll);
    editMenu->addSeparator();
    editMenu->addAction("&Clear Canvas", this, &MainWindow::clearCanvas);
    
    // View menu
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("Zoom &In", this, &MainWindow::zoomIn, QKeySequence::ZoomIn);
    viewMenu->addAction("Zoom &Out", this, &MainWindow::zoomOut, QKeySequence::ZoomOut);
    viewMenu->addAction("&Reset Zoom", this, &MainWindow::zoomReset, QKeySequence("Ctrl+0"));
    viewMenu->addAction("Zoom to &Fit", this, &MainWindow::zoomToFit, QKeySequence("Ctrl+F"));
    
    // Telemetry menu
    QMenu* telemetryMenu = menuBar()->addMenu("&Telemetry");
    telemetryMenu->addAction("&Start Receiver", this, &MainWindow::startTelemetry);
    telemetryMenu->addAction("S&top Receiver", this, &MainWindow::stopTelemetry);
    telemetryMenu->addSeparator();
    telemetryMenu->addAction("&Configure...", this, &MainWindow::configureTelemetry);
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&Help", this, &MainWindow::showHelp, QKeySequence::HelpContents);
    helpMenu->addSeparator();
    helpMenu->addAction("&About", this, &MainWindow::showAbout);
}

void MainWindow::createToolbar()
{
    QToolBar* toolbar = addToolBar("Main Toolbar");
    toolbar->setIconSize(QSize(24, 24));
    
    toolbar->addAction("New", this, &MainWindow::newProject);
    toolbar->addAction("Open", this, &MainWindow::openProject);
    toolbar->addAction("Save", this, &MainWindow::saveProject);
    toolbar->addSeparator();
    toolbar->addAction("Zoom In", this, &MainWindow::zoomIn);
    toolbar->addAction("Zoom Out", this, &MainWindow::zoomOut);
    toolbar->addAction("Fit", this, &MainWindow::zoomToFit);
    toolbar->addSeparator();
    toolbar->addAction("Start Telemetry", this, &MainWindow::startTelemetry);
    toolbar->addAction("Stop Telemetry", this, &MainWindow::stopTelemetry);
}

void MainWindow::createDockPanels()
{
    // Toolbox panel (left)
    m_toolboxPanel = new ToolboxPanel(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolboxPanel);
    
    // Properties panel (right)
    m_propertiesPanel = new PropertiesPanel(this);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesPanel);
    
    // Health dashboard (bottom)
    m_healthDashboard = new HealthDashboard(this);
    m_healthDashboard->setNodeScene(m_graphScene);
    addDockWidget(Qt::BottomDockWidgetArea, m_healthDashboard);
    
    // Telemetry log (bottom, tabified with health dashboard)
    m_telemetryLog = new TelemetryLogWindow(this);
    addDockWidget(Qt::BottomDockWidgetArea, m_telemetryLog);
    tabifyDockWidget(m_healthDashboard, m_telemetryLog);
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setMinimumWidth(200);
    statusBar()->addWidget(m_statusLabel);
    
    m_telemetryStatusLabel = new QLabel("Telemetry: Stopped");
    m_telemetryStatusLabel->setMinimumWidth(150);
    statusBar()->addPermanentWidget(m_telemetryStatusLabel);
    
    m_zoomLabel = new QLabel("Zoom: 100%");
    m_zoomLabel->setMinimumWidth(100);
    statusBar()->addPermanentWidget(m_zoomLabel);
}

void MainWindow::setupConnections()
{
    // Toolbox to node creation
    connect(m_toolboxPanel, &ToolboxPanel::subsystemSelected,
            this, &MainWindow::createNodeFromToolbox);
    
    // Graph view zoom
    connect(m_graphView, &NodeGraphView::zoomChanged,
            this, [this](qreal zoom) {
                m_zoomLabel->setText(QString("Zoom: %1%").arg(qRound(zoom * 100)));
            });
}

void MainWindow::initializeTelemetrySystem()
{
    // Create telemetry receiver
    m_telemetryReceiver = new UdpTelemetryReceiver(m_telemetryPort, this);
    
    // Create health dispatcher
    m_healthDispatcher = new HealthStatusDispatcher(this);
    m_healthDispatcher->setTelemetryReceiver(m_telemetryReceiver);
    
    // Connect telemetry signals
    connect(m_telemetryReceiver, &UdpTelemetryReceiver::telemetryReceived,
            this, &MainWindow::onTelemetryReceived);
    connect(m_telemetryReceiver, &UdpTelemetryReceiver::errorOccurred,
            this, &MainWindow::onTelemetryError);
    connect(m_telemetryReceiver, &UdpTelemetryReceiver::statusChanged,
            this, &MainWindow::onTelemetryStatusChanged);
    
    qInfo() << "Telemetry system initialized on port" << m_telemetryPort;
}

void MainWindow::registerSubsystemNodes()
{
    RadarSubsystem& registry = RadarSubsystem::instance();
    
    // Register all subsystem types with factory functions
    registry.registerSubsystem(
        SubsystemDefinition{"RFFrontend", "RF Systems", "RF Frontend", 
                          "RF signal transmission and reception", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new RFFrontendNode(parent); }
    );
    
    registry.registerSubsystem(
        SubsystemDefinition{"SignalProcessor", "Processing", "Signal Processor",
                          "Radar signal processing", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new SignalProcessorNode(parent); }
    );
    
    registry.registerSubsystem(
        SubsystemDefinition{"Tracker", "Processing", "Tracker",
                          "Target tracking", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new TrackerNode(parent); }
    );
    
    registry.registerSubsystem(
        SubsystemDefinition{"AntennaServo", "Mechanical", "Antenna Servo",
                          "Antenna positioning", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new AntennaServoNode(parent); }
    );
    
    registry.registerSubsystem(
        SubsystemDefinition{"DataFusion", "Processing", "Data Fusion",
                          "Multi-sensor data fusion", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new DataFusionNode(parent); }
    );
    
    registry.registerSubsystem(
        SubsystemDefinition{"PowerSupply", "Power", "Power Supply",
                          "Power distribution", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new PowerSupplyNode(parent); }
    );
    
    registry.registerSubsystem(
        SubsystemDefinition{"NetworkInterface", "Communication", "Network Interface",
                          "Network communication", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new NetworkInterfaceNode(parent); }
    );
    
    registry.registerSubsystem(
        SubsystemDefinition{"CoolingSystem", "Thermal", "Cooling System",
                          "Thermal management", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new CoolingSystemNode(parent); }
    );
    
    registry.registerSubsystem(
        SubsystemDefinition{"EmbeddedController", "Control", "Embedded Controller",
                          "System controller", "", {}, {}, false},
        [](QObject* parent) -> SubsystemNode* { return new EmbeddedControllerNode(parent); }
    );
    
    qInfo() << "Registered" << registry.availableTypes().size() << "subsystem types";
}

void MainWindow::newProject()
{
    // TODO: Ask to save if modified
    clearCanvas();
    m_currentProjectFile.clear();
    m_projectModified = false;
    m_statusLabel->setText("New project created");
}

void MainWindow::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Open Project", "", "Radar Project Files (*.rhms);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // TODO: Implement project loading
    m_currentProjectFile = fileName;
    m_statusLabel->setText("Project opened: " + fileName);
}

void MainWindow::saveProject()
{
    if (m_currentProjectFile.isEmpty()) {
        saveProjectAs();
        return;
    }
    
    // TODO: Implement project saving
    m_projectModified = false;
    m_statusLabel->setText("Project saved: " + m_currentProjectFile);
}

void MainWindow::saveProjectAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save Project As", "", "Radar Project Files (*.rhms);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    m_currentProjectFile = fileName;
    saveProject();
}

void MainWindow::deleteSelected()
{
    // TODO: Implement node deletion
    m_statusLabel->setText("Delete selected");
}

void MainWindow::selectAll()
{
    if (m_graphScene) {
        QPainterPath path;
        path.addRect(m_graphScene->sceneRect());
        m_graphScene->setSelectionArea(path);
    }
}

void MainWindow::clearCanvas()
{
    if (m_graphScene) {
        m_graphScene->clearScene();
        m_statusLabel->setText("Canvas cleared");
    }
}

void MainWindow::zoomIn()
{
    if (m_graphView) {
        m_graphView->zoomIn();
    }
}

void MainWindow::zoomOut()
{
    if (m_graphView) {
        m_graphView->zoomOut();
    }
}

void MainWindow::zoomReset()
{
    if (m_graphView) {
        m_graphView->zoomReset();
    }
}

void MainWindow::zoomToFit()
{
    if (m_graphView) {
        m_graphView->zoomToFit();
    }
}

void MainWindow::startTelemetry()
{
    if (m_telemetryReceiver && !m_telemetryReceiver->isRunning()) {
        m_telemetryReceiver->start();
        m_statusLabel->setText("Telemetry receiver started");
    }
}

void MainWindow::stopTelemetry()
{
    if (m_telemetryReceiver && m_telemetryReceiver->isRunning()) {
        m_telemetryReceiver->stop();
        m_statusLabel->setText("Telemetry receiver stopped");
    }
}

void MainWindow::configureTelemetry()
{
    bool ok;
    int port = QInputDialog::getInt(
        this, "Configure Telemetry", "UDP Port:", 
        m_telemetryPort, 1024, 65535, 1, &ok
    );
    
    if (ok && port != m_telemetryPort) {
        bool wasRunning = m_telemetryReceiver->isRunning();
        if (wasRunning) {
            stopTelemetry();
        }
        
        m_telemetryPort = port;
        m_telemetryReceiver->setPort(port);
        
        if (wasRunning) {
            startTelemetry();
        }
        
        m_statusLabel->setText(QString("Telemetry port changed to %1").arg(port));
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About Radar Health Monitoring System",
        "<h2>Radar Health Monitoring Editor</h2>"
        "<p><b>Defense-Grade Modular System</b></p>"
        "<p>Version 1.0.0</p>"
        "<p>A node-based editor for radar system design and health monitoring.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Real-time UDP telemetry</li>"
        "<li>Hierarchical subsystem architecture</li>"
        "<li>Visual node-based editing</li>"
        "<li>Defense-grade reliability</li>"
        "</ul>"
    );
}

void MainWindow::showHelp()
{
    QMessageBox::information(this, "Help",
        "<h3>Radar Health Monitoring Editor - Quick Start</h3>"
        "<p><b>Creating Nodes:</b><br>"
        "Drag subsystems from the Toolbox panel onto the canvas.</p>"
        "<p><b>Connecting Nodes:</b><br>"
        "Click and drag from an output port to an input port.</p>"
        "<p><b>Navigation:</b><br>"
        "- Pan: Middle mouse button or Right mouse button<br>"
        "- Zoom: Ctrl + Mouse wheel or View menu<br>"
        "- Select: Left click<br>"
        "- Multi-select: Ctrl + Left click</p>"
        "<p><b>Telemetry:</b><br>"
        "Start the telemetry receiver to monitor subsystem health via UDP.</p>"
    );
}

void MainWindow::createNodeFromToolbox(const QString& subsystemType)
{
    // Convert display name to type
    QString nodeType = subsystemType.remove(' ');
    
    // Create node using factory
    SubsystemNode* node = RadarSubsystem::instance().createSubsystem(nodeType, this);
    if (!node) {
        qWarning() << "Failed to create node of type:" << nodeType;
        return;
    }
    
    // Add to scene at center
    QPointF centerPos = m_graphView->mapToScene(m_graphView->viewport()->rect().center());
    m_graphScene->addNode(node, centerPos);
    
    // Register with health dispatcher
    m_healthDispatcher->registerNode(node);
    
    m_projectModified = true;
    m_statusLabel->setText(QString("Created node: %1").arg(node->nodeName()));
}

void MainWindow::onNodeSelected(SubsystemNode* node)
{
    if (m_propertiesPanel) {
        m_propertiesPanel->displayNodeProperties(node);
    }
}

void MainWindow::onSelectionCleared()
{
    if (m_propertiesPanel) {
        m_propertiesPanel->clearProperties();
    }
}

void MainWindow::onTelemetryReceived(const TelemetryPacket& packet)
{
    // Log to telemetry window
    if (m_telemetryLog) {
        m_telemetryLog->logTelemetryPacket(packet);
    }
}

void MainWindow::onTelemetryError(const QString& error)
{
    m_statusLabel->setText("Telemetry Error: " + error);
    if (m_telemetryLog) {
        m_telemetryLog->logMessage("ERROR: " + error);
    }
}

void MainWindow::onTelemetryStatusChanged(const QString& status)
{
    m_telemetryStatusLabel->setText("Telemetry: " + status);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_projectModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Unsaved Changes",
            "Do you want to save changes before closing?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );
        
        if (reply == QMessageBox::Save) {
            saveProject();
            event->accept();
        } else if (reply == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }
    
    // Stop telemetry
    if (m_telemetryReceiver) {
        m_telemetryReceiver->stop();
    }
    
    event->accept();
}
