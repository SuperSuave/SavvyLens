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

## Traffic Circle Boundaries

| Directory          | Owns                                                                                              | Must not own                                           |
| ------------------ | ------------------------------------------------------------------------------------------------- | ------------------------------------------------------ |
| src/app            | Application lifecycle, main window, workspace registration, global navigation, app-level settings | CAN decoding, capture processing, RE algorithms        |
| src/can            | Fundamental CAN value types, flags, channel/ID semantics, low-level filter compatibility          | Qt widgets, workspace behavior, project persistence    |
| src/frames         | Frame collections, Qt frame models, raw frame presentation adapters                               | Analysis algorithms tied to a particular RE workflow   |
| src/connections    | Adapter discovery/configuration, channels, capability and health state                            | Per-window connection copies or business logic         |
| src/io             | File readers/writers, capture metadata, import/export normalization                               | Application navigation or live UI ownership            |
| src/dbc            | DBC parsing, lookup, messages/signals and DBC-specific UI                                         | Capture/session ownership                              |
| src/bookmarks      | Existing bookmark model/manager and compatibility                                                 | Future cross-workspace analysis calculations           |
| src/playback       | Capture playback scheduling and replay UI                                                         | Generic active-send safety policy                      |
| src/sender         | Manual frame transmission and sender UI                                                           | Passive capture or analysis                            |
| src/scripting      | Script runtime, templates, stable integration APIs                                                | Direct access to private Qt widgets                    |
| src/mcp / src/mqtt | External integration/transport adapters                                                           | Core frame ownership or analysis logic                 |
| src/re             | RE workspace UI/controllers and transitional legacy tools                                         | Reusable cross-workspace services                      |
| src/widgets        | Generic reusable visual components                                                                | Vehicle/CAN-specific algorithms or persistence         |
| src/common         | Tiny cross-domain types and error/result helpers                                                  | New CAN/analysis/project “miscellaneous” code          |
| src/utils          | Stateless generic helpers                                                                         | Domain models, QObject service registries, UI behavior |
| src/tools          | Developer/tooling utilities                                                                       | Runtime application logic                              |
| src/packaging      | Install/release assets and platform packaging scripts                                             | Application runtime behavior                           |