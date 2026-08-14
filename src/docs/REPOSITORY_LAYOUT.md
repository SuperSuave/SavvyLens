# Repository layout

SavvyLens keeps implementation source under `src/`, Qt Designer forms under `ui/`, runtime data in top-level asset directories, and developer documentation under `docs/`.

## Top-level directories

| Path | Purpose |
|---|---|
| `src/` | Qt/C++ application source and bundled C++ dependencies |
| `ui/` | Qt Designer `.ui` forms |
| `help/` | Runtime Markdown help opened by feature windows through F1 |
| `translations/` | Qt Linguist `.ts`, `.qm`, and phrase-book files |
| `icons/`, `images/` | Runtime image assets referenced by QRC or packaging |
| `themes/` | Runtime theme data; theme-management source belongs in `src/themes/` |
| `tools/` | Developer and external integration utilities |
| `packaging/` | Platform installer metadata and installation scripts |
| `docs/` | Developer-facing architecture, build, and workflow documentation |
| `examples/` | Example files distributed with the application |
| `test/` | Tests and test fixtures |

## Source modules

| New code responsibility | Location |
|---|---|
| Application startup, main window, preferences, help launcher | `src/app/` |
| Frame type and simple shared CAN filter | `src/can/` |
| Cross-cutting formatting/parsing/window helpers | `src/common/` |
| Logger and lock-free primitives | `src/utils/` |
| Device, serial, SocketCANd, GVRET, MQTT, and connection management | `src/connections/` |
| ISO-TP, UDS, and J1939 protocol implementation | `src/bus_protocols/` |
| DBC model, handler, editors, DBC-specific controls | `src/dbc/` |
| Canonical captured/live frame model | `src/frames/` |
| Log import/export facade and capture-format readers | `src/io/` |
| Bookmark storage and bookmark UI | `src/bookmarks/` |
| Frame replay | `src/playback/` |
| Trigger/modifier frame transmission | `src/sender/` |
| JavaScript runtime, editor, and built-in templates | `src/scripting/` |
| Reverse-engineering tools | `src/re/` |
| In-app MCP server | `src/mcp/` |
| C++ MQTT implementation | `src/mqtt/` |
| Reusable Qt widgets and delegates | `src/widgets/` |
| SavvyLens-owned plotting extensions | `src/plotting/` |
| Theme-management implementation | `src/themes/` |
| Vendored source | `src/third_party/` |

## Include policy

Keep source/header pairs in the same module. Use a source-root include base and module-qualified includes for dependencies outside the current directory:

```qmake
INCLUDEPATH += $$PWD/src
```

```cpp
#include "can/can_structs.h"
#include "connections/canconmanager.h"
#include "io/framefileio.h"
#include "sender/can_trigger_structs.h"
```

For a header in the same directory, use the short local include:

```cpp
#include "scriptcontainer.h"
```

Do not use `../` include paths. Do not add every module directory separately to `INCLUDEPATH`.

## Adding new files

1. Place the new source/header pair in the module that owns its behavior.
2. Add the new source path to `SOURCES` and header path to `HEADERS` in `SavvyLens.pro` or its relevant included `.pri` file.
3. Add a `.ui` form under `ui/` and list it in `FORMS` if the feature uses Qt Designer.
4. Add or update F1 help in `help/` for substantial user-facing features.
5. Run qmake and perform a clean build after changing project paths.
