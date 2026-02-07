######################################################################
# Radar Health Monitoring System (RHMS)
# Qt Project File for qmake build system
# Version 1.0.0
######################################################################

# Qt Configuration
QT += core gui widgets network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Optional QML support
qtHaveModule(quick): {
    QT += quick qml
    DEFINES += USE_QML
}

# Application Configuration
TARGET = RadarHealthMonitor
TEMPLATE = app
VERSION = 1.0.0

# C++ Standard
CONFIG += c++17
CONFIG -= app_bundle

# Require C++17
QMAKE_CXXFLAGS += -std=c++17

# Platform-specific definitions
win32: DEFINES += PLATFORM_WINDOWS
unix:!macx: DEFINES += PLATFORM_LINUX
macx: DEFINES += PLATFORM_MACOS

# Compiler warnings
unix {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
}
win32-msvc* {
    QMAKE_CXXFLAGS += /W4
}

# Include directories
INCLUDEPATH += \
    $$PWD/src \
    $$PWD/src/core \
    $$PWD/src/network \
    $$PWD/src/graph \
    $$PWD/src/ui \
    $$PWD/src/nodes

# Main source file
SOURCES += src/main.cpp

# Core sources
SOURCES += \
    src/core/SubsystemNode.cpp \
    src/core/HealthStatus.cpp \
    src/core/TelemetryPacket.cpp \
    src/core/RadarSubsystem.cpp

HEADERS += \
    src/core/SubsystemNode.h \
    src/core/HealthStatus.h \
    src/core/TelemetryPacket.h \
    src/core/RadarSubsystem.h

# Network sources
SOURCES += \
    src/network/UdpTelemetryReceiver.cpp \
    src/network/TelemetryParser.cpp \
    src/network/HealthStatusDispatcher.cpp

HEADERS += \
    src/network/UdpTelemetryReceiver.h \
    src/network/TelemetryParser.h \
    src/network/HealthStatusDispatcher.h

# Graph sources
SOURCES += \
    src/graph/NodeGraphScene.cpp \
    src/graph/NodeGraphView.cpp \
    src/graph/ConnectionManager.cpp \
    src/graph/HierarchicalGraphEngine.cpp \
    src/graph/NodeDataModel.cpp

HEADERS += \
    src/graph/NodeGraphScene.h \
    src/graph/NodeGraphView.h \
    src/graph/ConnectionManager.h \
    src/graph/HierarchicalGraphEngine.h \
    src/graph/NodeDataModel.h

# Node sources
SOURCES += \
    src/nodes/RFFrontendNode.cpp \
    src/nodes/SignalProcessorNode.cpp \
    src/nodes/TrackerNode.cpp \
    src/nodes/AntennaServoNode.cpp \
    src/nodes/DataFusionNode.cpp \
    src/nodes/PowerSupplyNode.cpp \
    src/nodes/NetworkInterfaceNode.cpp \
    src/nodes/CoolingSystemNode.cpp \
    src/nodes/EmbeddedControllerNode.cpp

HEADERS += \
    src/nodes/RFFrontendNode.h \
    src/nodes/SignalProcessorNode.h \
    src/nodes/TrackerNode.h \
    src/nodes/AntennaServoNode.h \
    src/nodes/DataFusionNode.h \
    src/nodes/PowerSupplyNode.h \
    src/nodes/NetworkInterfaceNode.h \
    src/nodes/CoolingSystemNode.h \
    src/nodes/EmbeddedControllerNode.h

# UI sources
SOURCES += \
    src/ui/MainWindow.cpp \
    src/ui/ToolboxPanel.cpp \
    src/ui/PropertiesPanel.cpp \
    src/ui/HealthDashboard.cpp \
    src/ui/TelemetryLogWindow.cpp \
    src/ui/NodeWidget.cpp

HEADERS += \
    src/ui/MainWindow.h \
    src/ui/ToolboxPanel.h \
    src/ui/PropertiesPanel.h \
    src/ui/HealthDashboard.h \
    src/ui/TelemetryLogWindow.h \
    src/ui/NodeWidget.h

# UI Forms
FORMS += \
    src/ui/MainWindow.ui

# Resources
RESOURCES += \
    resources/resources.qrc

# Build directories
MOC_DIR = build/moc
OBJECTS_DIR = build/obj
UI_DIR = build/ui
RCC_DIR = build/rcc

# Output directory
DESTDIR = build/bin

# Default rules for deployment
unix {
    target.path = /usr/local/bin
    INSTALLS += target
    
    resources.path = /usr/local/share/RadarHealthMonitor/resources
    resources.files = resources/*
    INSTALLS += resources
}

win32 {
    target.path = C:/Program Files/RadarHealthMonitor
    INSTALLS += target
    
    resources.path = C:/Program Files/RadarHealthMonitor/resources
    resources.files = resources/*
    INSTALLS += resources
}

macx {
    target.path = /Applications
    INSTALLS += target
    
    QMAKE_INFO_PLIST = resources/Info.plist
    ICON = resources/app_icon.icns
}

# Additional configuration
QMAKE_TARGET_COMPANY = "Defense Systems Engineering Team"
QMAKE_TARGET_PRODUCT = "Radar Health Monitoring System"
QMAKE_TARGET_DESCRIPTION = "Defense-Grade Modular Radar Health Monitoring Editor"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2026 Defense Systems Engineering Team"

# Debug/Release configurations
CONFIG(debug, debug|release) {
    DEFINES += DEBUG_BUILD
    message("Building in DEBUG mode")
}

CONFIG(release, debug|release) {
    DEFINES += RELEASE_BUILD
    message("Building in RELEASE mode")
}

# Print configuration summary
message("==============================================")
message("Radar Health Monitoring System")
message("Version: $$VERSION")
message("Target: $$TARGET")
message("Qt Version: $$QT_VERSION")
message("C++ Standard: C++17")
message("==============================================")
