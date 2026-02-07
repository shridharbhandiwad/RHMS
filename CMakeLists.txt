cmake_minimum_required(VERSION 3.16)

project(RadarHealthMonitoringSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Platform detection
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DPLATFORM_LINUX)
elseif(APPLE)
    add_definitions(-DPLATFORM_MACOS)
endif()

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Gui
)

# Optional QML support
find_package(Qt6 COMPONENTS Quick Qml)
if(Qt6Quick_FOUND AND Qt6Qml_FOUND)
    add_definitions(-DUSE_QML)
    set(QML_LIBS Qt6::Quick Qt6::Qml)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/network
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graph
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nodes
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/nodeeditor/include
)

# Source files
set(CORE_SOURCES
    src/core/SubsystemNode.cpp
    src/core/HealthStatus.cpp
    src/core/TelemetryPacket.cpp
    src/core/RadarSubsystem.cpp
)

set(CORE_HEADERS
    src/core/SubsystemNode.h
    src/core/HealthStatus.h
    src/core/TelemetryPacket.h
    src/core/RadarSubsystem.h
)

set(NETWORK_SOURCES
    src/network/UdpTelemetryReceiver.cpp
    src/network/TelemetryParser.cpp
    src/network/HealthStatusDispatcher.cpp
)

set(NETWORK_HEADERS
    src/network/UdpTelemetryReceiver.h
    src/network/TelemetryParser.h
    src/network/HealthStatusDispatcher.h
)

set(GRAPH_SOURCES
    src/graph/NodeGraphScene.cpp
    src/graph/NodeGraphView.cpp
    src/graph/ConnectionManager.cpp
    src/graph/HierarchicalGraphEngine.cpp
    src/graph/NodeDataModel.cpp
)

set(GRAPH_HEADERS
    src/graph/NodeGraphScene.h
    src/graph/NodeGraphView.h
    src/graph/ConnectionManager.h
    src/graph/HierarchicalGraphEngine.h
    src/graph/NodeDataModel.h
)

set(NODE_SOURCES
    src/nodes/RFFrontendNode.cpp
    src/nodes/SignalProcessorNode.cpp
    src/nodes/TrackerNode.cpp
    src/nodes/AntennaServoNode.cpp
    src/nodes/DataFusionNode.cpp
    src/nodes/PowerSupplyNode.cpp
    src/nodes/NetworkInterfaceNode.cpp
    src/nodes/CoolingSystemNode.cpp
    src/nodes/EmbeddedControllerNode.cpp
)

set(NODE_HEADERS
    src/nodes/RFFrontendNode.h
    src/nodes/SignalProcessorNode.h
    src/nodes/TrackerNode.h
    src/nodes/AntennaServoNode.h
    src/nodes/DataFusionNode.h
    src/nodes/PowerSupplyNode.h
    src/nodes/NetworkInterfaceNode.h
    src/nodes/CoolingSystemNode.h
    src/nodes/EmbeddedControllerNode.h
)

set(UI_SOURCES
    src/ui/MainWindow.cpp
    src/ui/ToolboxPanel.cpp
    src/ui/PropertiesPanel.cpp
    src/ui/HealthDashboard.cpp
    src/ui/TelemetryLogWindow.cpp
    src/ui/NodeWidget.cpp
)

set(UI_HEADERS
    src/ui/MainWindow.h
    src/ui/ToolboxPanel.h
    src/ui/PropertiesPanel.h
    src/ui/HealthDashboard.h
    src/ui/TelemetryLogWindow.h
    src/ui/NodeWidget.h
)

set(UI_FORMS
    src/ui/MainWindow.ui
)

set(RESOURCES
    resources/resources.qrc
)

# Main executable
add_executable(RadarHealthMonitor
    src/main.cpp
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${NETWORK_SOURCES}
    ${NETWORK_HEADERS}
    ${GRAPH_SOURCES}
    ${GRAPH_HEADERS}
    ${NODE_SOURCES}
    ${NODE_HEADERS}
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${UI_FORMS}
    ${RESOURCES}
)

# Link libraries
target_link_libraries(RadarHealthMonitor PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Gui
    ${QML_LIBS}
)

# Compiler warnings
if(MSVC)
    target_compile_options(RadarHealthMonitor PRIVATE /W4)
else()
    target_compile_options(RadarHealthMonitor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation
install(TARGETS RadarHealthMonitor
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Copy resources
install(DIRECTORY resources/
    DESTINATION share/RadarHealthMonitor/resources
)
