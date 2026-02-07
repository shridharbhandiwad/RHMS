# Quick Start Guide - Radar Health Monitoring System

Get up and running in 5 minutes!

---

## Prerequisites

Before you begin, ensure you have:

- **Qt 6.2+** installed
- **CMake 3.16+** installed
- **C++17 compatible compiler**:
  - Windows: MSVC 2019+
  - Linux: GCC 9+ or Clang 10+
  - macOS: Xcode 12+

---

## Step 1: Clone the Repository

```bash
git clone https://github.com/shridharbhandiwad/RHMS
cd RHMS
git checkout cursor/radar-health-monitoring-editor-ded5
```

---

## Step 2: Build the Application

### Linux / macOS:
```bash
./build.sh
```

### Windows:
```batch
build.bat
```

### Manual Build:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j8
```

---

## Step 3: Run the Application

### Linux / macOS:
```bash
./build/RadarHealthMonitor
```

### Windows:
```batch
build\Release\RadarHealthMonitor.exe
```

---

## Step 4: Start Telemetry Receiver

1. Click **Telemetry** menu → **Start Receiver**
2. Status bar should show: "Telemetry: Started"
3. Default UDP port: **5000** (configurable via Telemetry → Configure)

---

## Step 5: Add Subsystem Nodes

### Method 1: Drag and Drop
1. Open **Toolbox** panel (left side)
2. Expand a category (e.g., "RF Systems")
3. **Drag** "📡 RF Frontend" onto the canvas
4. Release to place the node

### Method 2: Double-Click
1. **Double-click** a subsystem in the Toolbox
2. Node appears at canvas center

---

## Step 6: Send Test Telemetry

Create a test script `send_telemetry.py`:

```python
#!/usr/bin/env python3
import socket
import json
import time

def send_telemetry(subsystem_id, health_code, params):
    packet = {
        "subsystem_id": subsystem_id,
        "health_code": health_code,
        "health_message": "Operating normally",
        "timestamp": int(time.time() * 1000),
        "parameters": params
    }
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(json.dumps(packet).encode(), ("127.0.0.1", 5000))
    sock.close()
    print(f"Sent telemetry for {subsystem_id}")

# Get node ID from Properties panel
# Example: Replace with actual node ID from your application
node_id = "550e8400-e29b-41d4-a716-446655440000"

# Send healthy status
send_telemetry(node_id, 0, {
    "temperature": 45.5,
    "voltage": 28.0,
    "power": 100.0,
    "frequency": 9.5e9
})

# Continuous sending
while True:
    send_telemetry(node_id, 0, {
        "temperature": 45.5 + (time.time() % 10),
        "voltage": 28.0,
        "power": 100.0
    })
    time.sleep(1)
```

Run the script:
```bash
python3 send_telemetry.py
```

---

## Step 7: Monitor Health Status

Watch the **Health Dashboard** panel (bottom) to see:
- ✓ Green indicator: Healthy
- ⚠ Orange indicator: Warning
- ✗ Red indicator: Error
- ○ Gray indicator: Offline

---

## Step 8: Inspect Node Properties

1. Click a node to select it
2. **Properties** panel (right) shows:
   - Node ID and name
   - Health status
   - Telemetry parameters
   - Port information

---

## Step 9: View Telemetry Log

1. Click **Telemetry Log** tab (bottom panel)
2. See real-time packet stream
3. Click **Export** to save log to file

---

## Common Operations

### Pan the Canvas
- **Middle mouse button** + drag
- **Right mouse button** + drag

### Zoom
- **Ctrl + Mouse wheel** to zoom
- **Ctrl + +** to zoom in
- **Ctrl + -** to zoom out
- **Ctrl + 0** to reset zoom
- **Ctrl + F** to fit all nodes

### Select Nodes
- **Left click** to select
- **Ctrl + Left click** for multi-select
- **Ctrl + A** to select all

### Delete Nodes
- Select node(s)
- Press **Delete** key

### Save Project
- **Ctrl + S** or File → Save
- Saves as `.rhms` file

---

## Keyboard Shortcuts Cheat Sheet

| Action | Shortcut |
|--------|----------|
| New Project | `Ctrl+N` |
| Open Project | `Ctrl+O` |
| Save Project | `Ctrl+S` |
| Zoom In | `Ctrl++` |
| Zoom Out | `Ctrl+-` |
| Reset Zoom | `Ctrl+0` |
| Fit to View | `Ctrl+F` |
| Delete | `Delete` |
| Select All | `Ctrl+A` |
| Help | `F1` |
| Quit | `Ctrl+Q` |

---

## Telemetry Health Codes

| Code | Status | Color |
|------|--------|-------|
| 0 | OK | Green |
| 1 | WARNING | Orange |
| 2 | ERROR | Red |
| 3 | OFFLINE | Gray |
| 4 | UNKNOWN | Yellow |

---

## Example Test Scenario

### Create a Simple Radar Chain

1. **Add nodes:**
   - Power Supply
   - RF Frontend
   - Signal Processor
   - Tracker

2. **Connect them:**
   - Power Supply → Power In (all nodes)
   - RF Frontend → Signal Processor
   - Signal Processor → Tracker

3. **Send telemetry** for each node ID

4. **Observe:**
   - Health status updates
   - Properties panel updates
   - Dashboard statistics
   - Telemetry log entries

---

## Troubleshooting

### Build Fails
- **Qt not found**: Set `CMAKE_PREFIX_PATH` to Qt installation
  ```bash
  cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.5.0/gcc_64
  ```
- **Compiler too old**: Upgrade to GCC 9+ / MSVC 2019+ / Xcode 12+

### No Telemetry Received
- Check firewall (allow UDP port 5000)
- Verify telemetry receiver is started
- Check node ID matches in script and application
- Look for errors in Telemetry Log

### Node Won't Appear
- Check console output for errors
- Verify node type is registered
- Try restarting application

### Connection Won't Create
- Ensure output port → input port direction
- Check port types are compatible
- Verify nodes are not the same

---

## Next Steps

- Read **README.md** for full documentation
- See **docs/ARCHITECTURE.md** for system design
- Review **docs/IMPLEMENTATION_SUMMARY.md** for details
- Customize subsystem types
- Extend with custom nodes
- Integrate with real radar hardware

---

## Support

- **GitHub Issues**: https://github.com/shridharbhandiwad/RHMS/issues
- **Documentation**: `/docs` folder
- **API Docs**: Generate with Doxygen

---

## Example Screenshots

### Main Window
```
┌─────────────────────────────────────────────────────────┐
│ File  Edit  View  Telemetry  Help                      │
├────────┬─────────────────────────────────────┬──────────┤
│        │                                     │          │
│Toolbox │         Node Canvas                 │Properties│
│        │                                     │          │
│  📡 RF │     [Node] ──→ [Node] ──→ [Node]   │ Name: RF │
│  ⚙️  SP│                                     │ Type: .. │
│  🎯 TR │                                     │ Health:✓│
│        │                                     │          │
├────────┴─────────────────────────────────────┴──────────┤
│ Health Dashboard                                        │
│ ✓ RF Frontend     │ OK │ Normal operation │ 10:23:45  │
│ ⚠ Signal Proc     │WARN│ High CPU load    │ 10:23:46  │
└─────────────────────────────────────────────────────────┘
```

---

**You're now ready to design and monitor radar systems!** 🎯

Happy monitoring! 📡
