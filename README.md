<p align="center">
<img width=250 alt="{SavvyLens Logo}" src="https://github.com/SuperSuave/SavvyLens/blob/master/images/LaunchScreen.png?raw=true" />
</p>


# SavvyLens

**A Qt-based, cross-platform CAN bus analysis tool for reverse engineering vehicle messages — built on [SavvyCAN](https://github.com/collin80/SavvyCAN) with a focus on making things easier to find.**

> The work of this was brought on trying to help contribute towards a project that brought a hardware solution for manual pre-conditioning of pre-facelift e-gmp platform vehicles.
> Check out that awesome hardware project at [electroniqbuttons.com](https://www.electroniqbuttons.com).

---

## What's Different from SavvyCAN

SavvyLens is a fork of [SavvyCAN](https://github.com/collin80/SavvyCAN) (C) 2015–2024 Collin Kidder. The sections below describe what has been added on top of the upstream project.

---

### 🕵🏼🔖 Event Correlation & Bookmarking

When reverse engineering CAN traffic, the hardest problem is often correlating a physical event — pressing a button, turning a knob, triggering an action in the vehicle — with an unknown signal somewhere in hundreds of IDs. SavvyLens introduces **event correlation** to help bridge that gap.

- **Manual Bookmarking** lets you mark a point in time during a capture. Open up your bookmarks, jump to them and SavvyLens can then highlight which IDs changed just before or after that bookmark, helping you narrow down which messages are related to the event you triggered.

- **Auto Bookmarking** toggle allows you to start a capture and when you add a bookmark, SavvyLens will then watch for two seconds for IDs that were *new* after the bookmark was placed and will then bookmark those frames for you — IDs already active before capture started are ignored to reduce noise.

- **ID Analysis** goes further by examining the full occurrence history of an ID across the entire log, looking for patterns that suggest what kind of message it carries — for example, whether it looks like a button press (sparse, event-driven), a periodic sensor reading, or a status register. SavvyLens then tries to identify the active byte(s) and shows a typical pattern for that ID. You can analyze a single ID or run analysis across the entire log at once.

---

### 🖥️ Quality of Life Improvements

- **Dark palette** as the default color scheme, give your eyes a break.
- **ID search** — quickly filter and locate a specific frame ID in the main view, both with the search box, or selecting individual frames..
- **Changed byte highlighting** — bytes that differ between successive occurrences of the same ID are visually highlighted, making it easier to spot which part of a payload is actually changing
- **Original Frame Column** - allows you to easily find the frame you were looking at if you were filtering or wanting to split your capture down.
- **Scroll position preserved across filter changes** — applying a filter re-selects the nearest frame to your previous position rather than jumping to the top *(by [dragz](https://github.com/dragz))*

---

## Community Contributions

The following features were contributed by collaborators and merged into SavvyLens.

---

### 🤖 AI Co-pilot — MCP Server *(by [Tichael](https://github.com/Tichael))*

A native **Model Context Protocol (MCP) server** that lets AI assistants (Claude Desktop, Cline, Cursor, etc.) interact directly with your live CAN data, DBC files, and analysis windows over TCP.

**Available MCP tools:**

| Tool | Description |
|---|---|
| `analyze_frame_data` | Statistical analysis of loaded/captured frames |
| `query_can_logs` | Query raw frames by ID, bus, timestamp; supports pagination |
| `query_analysis_tools` | Read data from Sniffer, FlowView, and Bisect windows |
| `query_dbc_signal` | Fetch signal definitions for any CAN ID |
| `parse_can_frame` | Decode a raw payload using loaded DBC definitions |
| `manage_dbc_file` | Create, load, save, or refresh DBC files |
| `manage_dbc_node` / `manage_dbc_message` / `manage_dbc_signal` | Add, edit, or remove DBC entries |
| `open_uds_scanner` / `start_uds_scan` / `stop_uds_scan` | Control the UDS Scanner |
| `open_isotp_interpreter` | Open ISO-TP Interpreter window |
| `add_frame_sender_sequence` | Inject frames via the Frame Sender |
| `open_signal_viewer` / `open_graph` / `open_playback` | Open visualization and playback windows |

Additional MCP features:
- UI toggle and port configuration in Settings
- Status bar indicator for active AI client connections
- Automatic DBC refresh on disk change via `QFileSystemWatcher` + manual refresh button in DBCLoadSaveWindow
- Python bridge script (`mcp_bridge.py`) for non-native MCP clients

See **[MCP_SERVER.md](MCP_SERVER.md)** for setup instructions.

---

### 📊 Signal Viewer Overhaul *(by [Tichael](https://github.com/Tichael))*

The old triple-combobox signal selector has been replaced with a **hierarchical tree widget** (`DbcSignalSelectorTree`) that shows your DBC structure as **Node → Message → Signal**.

- Multi-select with **tri-state cascaded checkboxes** — check a message to select all its signals at once
- Bidirectional sync between the tree and the Signal Viewer table
- `NewGraphDialog` uses the same tree with single-selection and double-click-to-copy, plus programmatic signal highlighting
- Memory leak fixed (unassociated node items are now properly deleted)
- DBC files are no longer mutated by the selector widget

---

### 🛡️ Crash Logging *(by [Tichael](https://github.com/Tichael))*

- Cross-platform crash logging system captures Qt logs and OS-level hard crashes
- Generates timestamped log files with stack traces before the application terminates
- POSIX crash handler fix

---

### 🔬 Frame Info Window Improvements *(by [dragz](https://github.com/dragz))*

- Byte graphs now use **real frame timestamps** on the x-axis (not frame index), with correct sub-second precision via `frameTimestampMicros()`
- X-axis range spans the **entire capture window**, not just where the selected ID appears
- **Per-graph ⌂ reset button** — restores full x-axis and default y-axis range
- **Per-graph ─/● toggle** — switch between line and scatter dot plots; state preserved across frame ID switches
- Header-level **reset-all** and **toggle-all** buttons
- Time style (seconds / millis / clock) now matches the main window setting
- Overlay buttons reposition correctly when the window or splitter is resized

---

## Core Features (from SavvyCAN)

- Real-time CAN frame capture via SocketCAN, Vector, PeakCAN, TinyCAN, and GVRET/CANDue hardware
- DBC file support — load, decode, and visualize signals
- Frame filtering, Frame Sender, Frame Playback
- JavaScript scripting engine for real-time or batch frame processing
- CAN Bridge, Trigger system, Bisect tool
- Multi-monitor support; 1024×768 through 4K

---

## Supported File Formats

| Format | Read | Write |
|---|---|---|
| GVRET native (.csv) | ✅ | ✅ |
| Generic CSV (ID, D0–D7) | ✅ | ✅ |
| BusMaster log | ✅ | ✅ |
| Microchip log | ✅ | ✅ |
| CRTD / OVMS | ✅ | ✅ |
| Vector Trace | ✅ | ✅ |
| IXXAT Minilog | ✅ | ✅ |
| CAN-DO logs | ✅ | ✅ |
| Vehicle Spy logs | ✅ | ✅ |
| CANDump / Kayak | ✅ | ❌ |
| PCAN Viewer | ✅ | ❌ |
| Wireshark SocketCAN PCAP | ✅ | ❌ |

---


- \*Files saved in SavvyLens can still be loaded in SavvyCAN, you will see errors, but nothing will prevent it from loading. If wanted, you can simply delete the added Original Frame Column and the bookmarks located at the end of the file.

## Hardware

A capture device is **not required** — SavvyLens can load and analyze saved log files without any hardware attached.

For live capture, recommended interfaces:

- **[MeatpiHQ WiCAN](https://www.meatpi.com/products/wican)** — the OBD interface used for the pre-conditioning project.
- **[CANDue (EVTV)](http://store.evtv.me/proddetail.php?prod=ArduinoDueCANBUS&cat=23)** — runs GVRET firmware (found in [collin80 repos](https://github.com/collin80))
- Any **Qt SerialBus**-compatible interface: SocketCAN, Vector, PeakCAN, TinyCAN

---

## Requirements

- **Qt 5.14.0 or newer** (Qt SerialBus module required)
- **macOS**: binary builds require macOS 10.15+
- Minimum display resolution: **1024×768**

---

## Building from Source

### Linux / macOS

```sh
git clone https://github.com/SuperSuave/SavvyLens.git
cd SavvyLens

# Adjust path to your Qt installation
~/Qt/5.14/gcc_64/bin/qmake
make

./SavvyLens
```

Create a desktop shortcut on Linux:

```sh
./install.sh
```

### Debug Build

```sh
qmake CONFIG+=debug
make
```

---

## Troubleshooting Builds

**Start here:**
```sh
qmake && make clean && make
```

**`qmake` fails with** `Unknown module(s) in QT: qml serialbus help` **(Ubuntu):**
```sh
sudo apt install libqt5serialbus5-dev libqt5serialport5-dev qtdeclarative5-dev qttools5-dev
```

Make sure you selected **SerialBus support** when installing Qt.

---

## Contributing

Questions and discussion: use the **[Discussions tab](https://github.com/SuperSuave/SavvyLens/discussions)**.  
Bug reports and feature requests: open an **[Issue](https://github.com/SuperSuave/SavvyLens/issues)**.

---

## Attributions

Icons from the Noun Project: *nodes* and *Death* by Adrien Coquet · *message* by Vectorstall · *signal* by shashank singh · *signal* by juli · *signal* by yudi

Plotting powered by [QCustomPlot](http://www.qcustomplot.com/) (integrated into source).

---

## License

See [LICENSE](LICENSE) for details.

