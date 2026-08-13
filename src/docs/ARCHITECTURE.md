# Architecture

SavvyLens is a Qt desktop application centered on captured/live CAN frames. Connection backends deliver traffic, the frame model stores and filters it, and feature modules provide analysis, protocol, transmission, DBC, import/export, and automation workflows.

## Dependency direction

```text
connections/ ─┐
bus_protocols/├────► can/ ───► frames/
io/          ─┘                  │
                                 ├──► playback/
                                 ├──► sender/
                                 ├──► scripting/
                                 ├──► dbc/
                                 └──► re/

common/, utils/, widgets/ ───────► broadly shared support
app/ ────────────────────────────► application composition and user entry points
```

Feature modules may depend on shared modules. Shared modules should not depend on feature-window classes.

## Key modules

### `src/can/`

`CANFrame` extends Qt's `QCanBusFrame` with SavvyLens-specific bus, direction, timing, frame-count, and original-index information. `CANFilter` is a small ID/mask/bus matching predicate used by subscribers such as the scripting system.

### `src/connections/`

Connection management owns CAN bus creation, serial/network/device backends, server/log-server facilities, and the `CANConManager` interface used by transmit-capable features. `CANBridgeWindow` belongs here because it forwards selected traffic between live buses.

### `src/frames/`

`CANFrameModel` is the canonical captured/live frame model. It stores full and filtered collections, supports filtering, sorting, duplicate overwrite mode, timestamp normalization, and optional DBC interpretation/display behavior.

### `src/io/`

`FrameFileIO` is the high-level format facade for user-visible loading and saving. It supports many third-party CAN log formats, auto-detection, native CSV handling, and optional bookmark persistence. `formats/` contains lower-level format readers such as BLF and PCAP/PCAPNG support.

### `src/dbc/`

DBC handling provides DBC files, messages, signals, editors, load/save workflows, and DBC-specific controls. `DbcSignalSelectorTree` is DBC-owned because it presents DBC file/node/message/signal structure; `SignalViewerWindow` decodes selected live signals from captured frames.

### `src/sender/`

Frame Sender is a reactive CAN transmission engine. It supports timed sends, bus/ID/signal/value triggers, maximum trigger counts, payload modifiers, and DBC-aware signal conditions. Keep its sender-specific trigger structures here.

### `src/scripting/`

The scripting module provides a JavaScript editor and runtime containers. Scripts can subscribe to CAN, ISO-TP, and UDS traffic; send CAN/ISO-TP/UDS messages; and receive callbacks. Built-in script templates are source-controlled under this module and installed as runtime data.

### `src/re/`

Reverse-engineering tools include traffic comparison, graphing, state detection, fuzzing, protocol interpretation, UDS utilities, sniffer views, and frame bisection. They are user-facing investigative tools rather than shared core layers.

### `src/widgets/`

Reusable widgets include the CAN bit grid, byte-change item delegate, and filter-list helpers. A widget belongs here only when it is not owned by a specific feature/domain such as DBC.