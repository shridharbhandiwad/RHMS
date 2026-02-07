# Radar Health Monitoring System (RHMS)

## Defense-Grade Modular Radar Health Monitoring Editor

A production-ready, cross-platform desktop application for radar system designers to visually construct and monitor radar subsystem architectures using a node-based editor paradigm.

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C)
![Qt](https://img.shields.io/badge/Qt-6-41CD52)
![License](https://img.shields.io/badge/license-Proprietary-red)

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [System Requirements](#system-requirements)
- [Building](#building)
- [Usage](#usage)
- [Subsystem Types](#subsystem-types)
- [Telemetry Protocol](#telemetry-protocol)
- [Development](#development)
- [Documentation](#documentation)

---

## Overview

The Radar Health Monitoring System (RHMS) is a defense-grade engineering tool that provides:

- **Visual Node-Based Editor**: Similar to LabVIEW, Unreal Engine Blueprints, or RF design tools
- **Real-Time Health Monitoring**: UDP telemetry integration with thread-safe processing
- **Hierarchical Architecture**: Drill down into subsystem details with nested graphs
- **Cross-Platform**: Windows, Linux, and macOS support via CMake
- **Production Ready**: Designed for deployment in defense radar laboratories

---

## Features

### Core Capabilities

✅ **Node-Based Visual Editor**
- Drag-and-drop subsystem placement
- Visual connection management
- Port-based dataflow modeling
- Infinite canvas with pan/zoom

✅ **Real-Time Health Monitoring**
- UDP telemetry receiver (configurable port)
- Thread-safe packet processing
- Color-coded health indicators (OK / WARNING / ERROR / OFFLINE)
- Live status updates on nodes

✅ **Hierarchical Subsystem Support**
- Create nested subsystem architectures
- Navigate graph hierarchy with breadcrumb navigation
- Expand/collapse node views

✅ **Multiple Subsystem Types**
- RF Frontend
- Signal Processor
- Tracker
- Antenna Servo
- Data Fusion
- Power Supply
- Network Interface
- Cooling System
- Embedded Controller

✅ **Professional UI**
- Dark theme optimized for defense environments
- Dockable panels (Toolbox, Properties, Health Dashboard, Telemetry Log)
- Status indicators and real-time statistics
- Keyboard shortcuts for efficient workflow

✅ **Data Persistence**
- Save/Load radar configurations
- JSON-based serialization
- Project file management

---

## Architecture

### Layered Architecture

```
┌─────────────────────────────────────────────────┐
│              UI Layer (Qt Widgets)              │
│  MainWindow │ Panels │ NodeWidget │ Views       │
├─────────────────────────────────────────────────┤
│              Graph Layer                        │
│  NodeGraphScene │ ConnectionManager │ Layout   │
├─────────────────────────────────────────────────┤
│              Core Layer                         │
│  SubsystemNode │ HealthStatus │ TelemetryPacket│
├─────────────────────────────────────────────────┤
│              Network Layer                      │
│  UdpReceiver │ Parser │ Dispatcher (Thread-Safe)│
└─────────────────────────────────────────────────┘
```

### Key Components

**Core Layer:**
- `SubsystemNode`: Abstract base class for all radar subsystems
- `HealthStatus`: Standardized health monitoring with color-coded states
- `TelemetryPacket`: UDP packet structure with serialization

**Network Layer:**
- `UdpTelemetryReceiver`: Thread-safe UDP listener with worker thread
- `TelemetryParser`: Multi-format packet parser (Binary, JSON, Defense Protocol)
- `HealthStatusDispatcher`: Routes telemetry to appropriate nodes

**Graph Layer:**
- `NodeGraphScene`: Qt Graphics Scene for node visualization
- `NodeGraphView`: Interactive view with pan/zoom/select
- `ConnectionManager`: Manages visual connections with Bezier curves
- `HierarchicalGraphEngine`: Navigation through nested subsystems

**UI Layer:**
- `MainWindow`: Main application window with menu/toolbar
- `ToolboxPanel`: Categorized subsystem library
- `PropertiesPanel`: Node property inspector
- `HealthDashboard`: Real-time health status overview
- `TelemetryLogWindow`: Packet logging with export

---

## System Requirements

### Minimum Requirements

- **OS**: Windows 10+, Ubuntu 20.04+, macOS 10.15+
- **CPU**: Intel Core i5 or AMD equivalent (2.0 GHz)
- **RAM**: 4 GB
- **GPU**: OpenGL 3.3 compatible
- **Disk**: 500 MB free space

### Development Requirements

- **Compiler**: 
  - Windows: MSVC 2019+ or MinGW with C++17
  - Linux: GCC 9+ or Clang 10+
  - macOS: Xcode 12+ (Apple Clang)
- **Qt**: Qt 6.2 or later
- **CMake**: 3.16 or later
- **Build Tools**: Ninja (optional, for faster builds)

---

## Building

### Quick Start

**Linux / macOS:**
```bash
./build.sh
```

**Windows:**
```batch
build.bat
```

### Manual Build

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release -j8

# Run
./RadarHealthMonitor
```

### CMake Options

- `CMAKE_BUILD_TYPE`: `Release`, `Debug`, `RelWithDebInfo`
- `CMAKE_PREFIX_PATH`: Path to Qt installation (if not in PATH)

Example:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/Qt/6.5.0/gcc_64
```

---

## Usage

### Starting the Application

1. **Launch**: Run `RadarHealthMonitor` executable
2. **Start Telemetry**: Menu → Telemetry → Start Receiver
3. **Add Subsystems**: Drag from Toolbox panel onto canvas
4. **Connect Nodes**: Click output port → drag → input port
5. **Monitor Health**: Watch real-time status in Health Dashboard

### Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| New Project | `Ctrl+N` |
| Open Project | `Ctrl+O` |
| Save Project | `Ctrl+S` |
| Zoom In | `Ctrl++` |
| Zoom Out | `Ctrl+-` |
| Reset Zoom | `Ctrl+0` |
| Zoom to Fit | `Ctrl+F` |
| Delete Selected | `Delete` |
| Select All | `Ctrl+A` |

### Mouse Controls

- **Pan**: Middle mouse button or Right mouse button + drag
- **Zoom**: `Ctrl` + Mouse wheel
- **Select**: Left click
- **Multi-Select**: `Ctrl` + Left click
- **Move Node**: Left click + drag
- **Context Menu**: Right click (planned)

---

## Subsystem Types

### RF Systems
- **RF Frontend** 📡: Transmit/receive, frequency control, power monitoring

### Processing
- **Signal Processor** ⚙️: DSP pipeline, latency tracking, buffer management
- **Tracker** 🎯: Multi-target tracking, update rate, track quality
- **Data Fusion** 🔗: Multi-sensor integration, fusion quality

### Mechanical
- **Antenna Servo** 📐: Azimuth/elevation control, position error monitoring

### Power
- **Power Supply** ⚡: Voltage regulation, current monitoring, efficiency

### Communication
- **Network Interface** 🌐: Link status, bandwidth, packet loss, latency

### Thermal
- **Cooling System** ❄️: Fan control, coolant temperature, flow rate

### Control
- **Embedded Controller** 🖥️: System control, watchdog, resource monitoring

---

## Telemetry Protocol

### UDP Packet Format

**Defense Protocol Format:**
```
[Header:4] "RHMS"
[SubsystemID:16] UUID without braces
[HealthCode:1] 0=OK, 1=WARNING, 2=ERROR, 3=OFFLINE
[Timestamp:8] Unix timestamp (milliseconds)
[ParamCount:2] Number of parameters
[Parameters...] Key-value pairs
```

**JSON Format:**
```json
{
  "subsystem_id": "550e8400-e29b-41d4-a716",
  "health_code": 0,
  "health_message": "Operating normally",
  "timestamp": 1704067200000,
  "parameters": {
    "temperature": 45.5,
    "voltage": 28.0,
    "current": 5.2
  }
}
```

### Sending Telemetry (Python Example)

```python
import socket
import json
import time

def send_telemetry(subsystem_id, health_code, params):
    packet = {
        "subsystem_id": subsystem_id,
        "health_code": health_code,
        "health_message": "Normal operation",
        "timestamp": int(time.time() * 1000),
        "parameters": params
    }
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(json.dumps(packet).encode(), ("127.0.0.1", 5000))
    sock.close()

# Example usage
send_telemetry(
    "550e8400-e29b-41d4-a716",
    0,  # OK
    {"temperature": 45.5, "voltage": 28.0, "power": 100.0}
)
```

---

## Development

### Project Structure

```
RadarHealthMonitoringSystem/
├── CMakeLists.txt           # Cross-platform build configuration
├── build.sh                 # Linux/macOS build script
├── build.bat                # Windows build script
├── README.md                # This file
├── src/
│   ├── main.cpp            # Application entry point
│   ├── core/               # Core data structures
│   │   ├── SubsystemNode.*
│   │   ├── HealthStatus.*
│   │   ├── TelemetryPacket.*
│   │   └── RadarSubsystem.*
│   ├── network/            # UDP telemetry system
│   │   ├── UdpTelemetryReceiver.*
│   │   ├── TelemetryParser.*
│   │   └── HealthStatusDispatcher.*
│   ├── graph/              # Node graph engine
│   │   ├── NodeGraphScene.*
│   │   ├── NodeGraphView.*
│   │   ├── NodeDataModel.*
│   │   ├── ConnectionManager.*
│   │   └── HierarchicalGraphEngine.*
│   ├── nodes/              # Specific subsystem implementations
│   │   ├── RFFrontendNode.*
│   │   ├── SignalProcessorNode.*
│   │   ├── TrackerNode.*
│   │   └── ...
│   └── ui/                 # User interface components
│       ├── MainWindow.*
│       ├── ToolboxPanel.*
│       ├── PropertiesPanel.*
│       ├── HealthDashboard.*
│       ├── TelemetryLogWindow.*
│       └── NodeWidget.*
├── resources/              # Icons, stylesheets, etc.
│   ├── resources.qrc
│   └── radar_theme.qss
└── docs/                   # Additional documentation
```

### Adding New Subsystem Types

1. **Create Node Class**: Inherit from `SubsystemNode`
2. **Implement Virtual Methods**: `subsystemType()`, `subsystemCategory()`, etc.
3. **Setup Ports**: Define inputs/outputs in constructor
4. **Register with Factory**: Add to `MainWindow::registerSubsystemNodes()`
5. **Add to Toolbox**: Update `ToolboxPanel::populateSubsystems()`

Example:
```cpp
class CustomNode : public SubsystemNode {
    Q_OBJECT
public:
    explicit CustomNode(QObject* parent = nullptr);
    QString subsystemType() const override { return "CustomType"; }
    QString subsystemCategory() const override { return "Custom"; }
    // ... implement other methods
};
```

---

## Documentation

### Additional Resources

- **Architecture Overview**: `docs/ARCHITECTURE.md`
- **Telemetry Protocol**: `docs/TELEMETRY_PROTOCOL.md`
- **Developer Guide**: `docs/DEVELOPER_GUIDE.md`
- **API Reference**: Generated via Doxygen (run `doxygen Doxyfile`)

### Class Diagrams

Generate class diagrams using Doxygen:
```bash
doxygen Doxyfile
# Output in docs/html/index.html
```

---

## Future Enhancements

### Planned Features

- [ ] Connection validation and type checking
- [ ] Undo/Redo system
- [ ] Auto-layout algorithms (force-directed, hierarchical)
- [ ] Export to PDF/PNG
- [ ] Simulation mode (without hardware)
- [ ] Multi-user collaboration
- [ ] Database integration for historical data
- [ ] Custom scripting support (Lua/Python)
- [ ] Performance profiling tools
- [ ] 3D visualization mode

### Scalability

The system is designed to handle:
- **100+ nodes** in a single graph
- **1000+ telemetry packets/second**
- **Multiple hierarchical levels** (10+ levels deep)
- **Large configurations** (10+ MB project files)

---

## License

**Proprietary - Defense Systems**

This software is developed for defense applications and is not open source.
Unauthorized copying, distribution, or use is strictly prohibited.

© 2026 Defense Systems Engineering Team. All rights reserved.

---

## Support

For technical support or feature requests:
- **Email**: support@defense-systems.mil
- **Issue Tracker**: Internal Jira/GitLab
- **Documentation**: https://wiki.defense-systems.mil/rhms

---

## Contributors

- **Architecture & Core**: Defense Systems Engineering Team
- **UI/UX Design**: Radar Interface Design Group
- **Telemetry Protocol**: Network Systems Division
- **Testing & QA**: Radar Lab Integration Team

---

## Acknowledgments

Built with:
- **Qt Framework** (Qt Company)
- **CMake** (Kitware)
- **C++17 Standard**

Inspired by:
- LabVIEW (National Instruments)
- Unreal Engine Blueprints (Epic Games)
- Node-RED (OpenJS Foundation)
- RF System Design Tools

---

**Built for Defense. Engineered for Reliability.**
