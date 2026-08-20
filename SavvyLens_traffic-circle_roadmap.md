# SavvyLens Modernization Roadmap v2
## Feature/Traffic-Circle branch-based architecture plan

> **Branch reviewed:** `feature/traffic-circle`
>
> **Repository:** `SuperSuave/SavvyLens`
>
> **Purpose:** Provide a detailed, trackable modernization plan based on the reorganized `feature/traffic-circle` tree, including eventual folders, proposed filenames, migration boundaries, dependencies, and acceptance criteria.

---

## 1. Current `feature/traffic-circle` structure

The `feature/traffic-circle` branch has already moved from a mostly flat source tree into a much more useful domain-oriented layout.

```text
SavvyLens/
├── .github/
├── examples/
├── help/
├── icons/
├── images/
├── src/
│   ├── app/
│   ├── bookmarks/
│   ├── bus_protocols/
│   ├── can/
│   ├── common/
│   ├── connections/
│   ├── dbc/
│   ├── docs/
│   ├── frames/
│   ├── io/
│   ├── mcp/
│   ├── mqtt/
│   ├── packaging/
│   ├── playback/
│   ├── re/
│   ├── scripting/
│   ├── sender/
│   ├── themes/
│   ├── third_party/
│   ├── tools/
│   ├── translations/
│   ├── utils/
│   └── widgets/
├── test/
├── translations/
├── ui/
├── SavvyLens.pro
└── README.md
```

The existing structure already gives the modernization effort good boundaries:

- `src/app/` is the application shell and navigation integration point.
- `src/can/` contains CAN primitives and filtering.
- `src/frames/` contains the frame model layer.
- `src/io/`, `src/playback/`, and `src/sender/` are natural traffic-operation boundaries.
- `src/re/` contains the reverse-engineering workspaces that should eventually become coordinated workflows.
- `src/bookmarks/`, `src/dbc/`, `src/connections/`, and `src/scripting/` are already close to the shared services the roadmap needs.

The main architectural recommendation is therefore **not to reorganize everything again**. Add a small number of explicit shared domains, then migrate existing windows toward them incrementally.

---

## 2. Product direction

SavvyLens should feel like a CAN reverse-engineering workbench organized around user intents rather than a menu of independent dialogs.

| User intent | Workspace | Existing/pre-release areas involved |
|---|---|---|
| Connect and collect traffic | Connections / Traffic Studio | `connections`, `io`, `playback`, `sender`, `mqtt`, `mcp` |
| Find and understand behavior | State Explorer | `re/sniffer`, range state, discrete state, frame info, temporal graph |
| Compare evidence | Capture Comparison / Investigation | file comparator, DBC comparator, bookmark event analyzer, bisect |
| Test a hypothesis | Experiment Lab | control analysis, candidates, state detector, fuzzing, sender/playback |
| Decode and reuse semantics | Signal Catalog | `dbc`, frame information, graphing, custom raw-bit definitions |
| Observe and present | Visualizations / Dashboards | graphing, signal viewer, flow view, MQTT |
| Automate repeatable work | Automation | filters, triggers, scripting, playback, sender, MCP |
| Diagnose ECUs | Diagnostics & Transport | ISO-TP, UDS scan, firmware uploader, bus protocols |
| Preserve knowledge | Vehicle Project | bookmarks, captures, DBCs, findings, scripts, layouts |

### Target top-level navigation

```text
Project | Connections | Explore | Compare | Experiment |
Visualize | Automate | Diagnostics | Help / Command Palette
```

This is an information-architecture target, not a requirement to delete existing windows immediately.

---

## 3. Proposed eventual source tree

This is the full eventual layout. It intentionally distinguishes reusable domain services from Qt windows and widgets.

```text
src/
├── app/
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   ├── applicationcontext.cpp
│   ├── applicationcontext.h
│   ├── commandregistry.cpp
│   ├── commandregistry.h
│   ├── commandpalette.cpp
│   ├── commandpalette.h
│   ├── workspacecontroller.cpp
│   ├── workspacecontroller.h
│   ├── navigationmodel.cpp
│   ├── navigationmodel.h
│   ├── mainsettingsdialog.cpp
│   ├── mainsettingsdialog.h
│   ├── settingsscope.cpp
│   ├── settingsscope.h
│   ├── helpwindow.cpp
│   └── helpwindow.h
│
├── analysis/
│   ├── analysissession.cpp
│   ├── analysissession.h
│   ├── selectioncontext.cpp
│   ├── selectioncontext.h
│   ├── marker.cpp
│   ├── marker.h
│   ├── markerstore.cpp
│   ├── markerstore.h
│   ├── frameaggregatestore.cpp
│   ├── frameaggregatestore.h
│   ├── framehistory.cpp
│   ├── framehistory.h
│   ├── framecomparison.cpp
│   ├── framecomparison.h
│   ├── payloaddiff.cpp
│   ├── payloaddiff.h
│   ├── activitystatistics.cpp
│   ├── activitystatistics.h
│   ├── rangestatistics.cpp
│   ├── rangestatistics.h
│   ├── discretestateanalysis.cpp
│   ├── discretestateanalysis.h
│   ├── transitionanalysis.cpp
│   ├── transitionanalysis.h
│   ├── temporalanalysis.cpp
│   ├── temporalanalysis.h
│   ├── candidateanalysis.cpp
│   ├── candidateanalysis.h
│   ├── analysisresult.cpp
│   └── analysisresult.h
│
├── automation/
│   ├── automationrule.cpp
│   ├── automationrule.h
│   ├── triggercondition.cpp
│   ├── triggercondition.h
│   ├── automationaction.cpp
│   ├── automationaction.h
│   ├── automationengine.cpp
│   ├── automationengine.h
│   ├── automationlog.cpp
│   ├── automationlog.h
│   ├── automationcontext.cpp
│   └── automationcontext.h
│
├── bookmarks/
│   ├── bookmark.cpp
│   ├── bookmark.h
│   ├── bookmarkmanager.cpp
│   ├── bookmarkmanager.h
│   ├── bookmarkmanagerdialog.cpp
│   ├── bookmarkmanagerdialog.h
│   ├── bookmarkeventanalyzer.cpp
│   └── bookmarkeventanalyzer.h
│
├── bus_protocols/
│   ├── ... existing protocol implementations ...
│   ├── protocolsession.cpp
│   ├── protocolsession.h
│   ├── isotp/
│   ├── uds/
│   └── j1939/              # only if/when needed
│
├── can/
│   ├── can_structs.h
│   ├── canfilter.cpp
│   ├── canfilter.h
│   ├── canframe.cpp
│   ├── canframe.h
│   ├── canframeflags.h
│   ├── canchannel.cpp
│   ├── canchannel.h
│   ├── canbusdefinition.cpp
│   └── canbusdefinition.h
│
├── common/
│   ├── ... small cross-domain primitives only ...
│   ├── result.h
│   ├── error.h
│   ├── units.h
│   ├── version.h
│   └── buildinfo.h
│
├── connections/
│   ├── connectionmanager.cpp
│   ├── connectionmanager.h
│   ├── connectionprofile.cpp
│   ├── connectionprofile.h
│   ├── connectioncapabilities.cpp
│   ├── connectioncapabilities.h
│   ├── connectionhealth.cpp
│   ├── connectionhealth.h
│   ├── buschannel.cpp
│   ├── buschannel.h
│   ├── connectionregistry.cpp
│   ├── connectionregistry.h
│   ├── connectionmanagerwidget.cpp
│   └── connectionmanagerwidget.h
│
├── dbc/
│   ├── ... existing DBC parser/loader files ...
│   ├── dbcdatabase.cpp
│   ├── dbcdatabase.h
│   ├── dbcmessage.cpp
│   ├── dbcmessage.h
│   ├── dbcsignal.cpp
│   ├── dbcsignal.h
│   ├── dbcsignalcatalog.cpp
│   ├── dbcsignalcatalog.h
│   ├── dbcsignalselector.cpp
│   ├── dbcsignalselector.h
│   ├── dbcresolver.cpp
│   ├── dbcresolver.h
│   └── dbccomparatorwindow.cpp/.h
│
├── docs/
│   ├── architecture.md
│   ├── query-language.md
│   ├── project-format.md
│   ├── scripting-api.md
│   ├── safety-model.md
│   ├── migration-guide.md
│   └── workflows/
│
├── frames/
│   ├── canframemodel.cpp
│   ├── canframemodel.h
│   ├── frameevent.cpp
│   ├── frameevent.h
│   ├── frameeventcodec.cpp
│   ├── frameeventcodec.h
│   ├── framequerymodel.cpp
│   ├── framequerymodel.h
│   ├── frameaggregatemodel.cpp
│   ├── frameaggregatemodel.h
│   ├── framehistorymodel.cpp
│   └── framehistorymodel.h
│
├── io/
│   ├── ... existing import/export handlers ...
│   ├── trafficsource.cpp
│   ├── trafficsource.h
│   ├── trafficsink.cpp
│   ├── trafficsink.h
│   ├── capturemetadata.cpp
│   ├── capturemetadata.h
│   ├── captureindex.cpp
│   ├── captureindex.h
│   ├── captureloader.cpp
│   └── captureloader.h
│
├── playback/
│   ├── ... existing playback implementation ...
│   ├── playbacksession.cpp
│   ├── playbacksession.h
│   ├── playbackscheduler.cpp
│   └── playbackscheduler.h
│
├── project/
│   ├── vehicleproject.cpp
│   ├── vehicleproject.h
│   ├── projectmanifest.cpp
│   ├── projectmanifest.h
│   ├── projectpersistence.cpp
│   ├── projectpersistence.h
│   ├── projectmigration.cpp
│   ├── projectmigration.h
│   ├── projectasset.cpp
│   ├── projectasset.h
│   ├── finding.cpp
│   ├── finding.h
│   ├── findingstore.cpp
│   ├── findingstore.h
│   ├── savedquery.cpp
│   ├── savedquery.h
│   ├── savedlayout.cpp
│   ├── savedlayout.h
│   ├── projectbrowser.cpp
│   └── projectbrowser.h
│
├── query/
│   ├── canquerytoken.h
│   ├── canquerylexer.cpp
│   ├── canquerylexer.h
│   ├── canqueryast.cpp
│   ├── canqueryast.h
│   ├── canqueryparser.cpp
│   ├── canqueryparser.h
│   ├── canqueryevaluator.cpp
│   ├── canqueryevaluator.h
│   ├── querycontext.cpp
│   ├── querycontext.h
│   ├── queryerror.cpp
│   ├── queryerror.h
│   ├── queryeditor.cpp
│   └── queryeditor.h
│
├── re/
│   ├── explore/
│   │   ├── stateexplorerwindow.cpp
│   │   ├── stateexplorerwindow.h
│   │   ├── stateexplorercontroller.cpp
│   │   ├── stateexplorercontroller.h
│   │   ├── stateoverviewmodel.cpp
│   │   ├── stateoverviewmodel.h
│   │   ├── stateinspectorwidget.cpp
│   │   ├── stateinspectorwidget.h
│   │   ├── statehistorywidget.cpp
│   │   ├── statehistorywidget.h
│   │   ├── stateoverviewwidget.cpp
│   │   ├── stateoverviewwidget.h
│   │   ├── rangetabwidget.cpp
│   │   ├── rangetabwidget.h
│   │   ├── discretetabwidget.cpp
│   │   ├── discretetabwidget.h
│   │   ├── temporaltabwidget.cpp
│   │   ├── temporaltabwidget.h
│   │   ├── rawframetabwidget.cpp
│   │   └── rawframetabwidget.h
│   │
│   ├── compare/
│   │   ├── capturecomparisonwindow.cpp
│   │   ├── capturecomparisonwindow.h
│   │   ├── comparisoncontroller.cpp
│   │   ├── comparisoncontroller.h
│   │   ├── comparisonresultmodel.cpp
│   │   ├── comparisonresultmodel.h
│   │   ├── investigationwindow.cpp
│   │   ├── investigationwindow.h
│   │   ├── investigationtimeline.cpp
│   │   ├── investigationtimeline.h
│   │   ├── bisectwindow.cpp/.h
│   │   ├── filecomparatorwindow.cpp/.h
│   │   ├── bookmarkeventanalyzer.cpp/.h
│   │   └── dbccomparatorwindow.cpp/.h
│   │
│   ├── experiment/
│   │   ├── experimentlabwindow.cpp
│   │   ├── experimentlabwindow.h
│   │   ├── experiment.cpp
│   │   ├── experiment.h
│   │   ├── controlanalysisdialog.cpp/.h
│   │   ├── controlcandidatemodel.cpp/.h
│   │   ├── controlstatedetector.cpp/.h
│   │   ├── fuzzingwindow.cpp/.h
│   │   ├── experimentresultmodel.cpp
│   │   └── experimentresultmodel.h
│   │
│   ├── diagnostics/
│   │   ├── diagnosticsworkspace.cpp
│   │   ├── diagnosticsworkspace.h
│   │   ├── isotp_interpreterwindow.cpp/.h
│   │   ├── udsscanwindow.cpp/.h
│   │   └── udsfirmwareuploaderwindow.cpp/.h
│   │
│   ├── legacy/
│   │   ├── rangestatewindow.cpp/.h
│   │   ├── discretestatewindow.cpp/.h
│   │   ├── temporalgraphwindow.cpp/.h
│   │   ├── frameinfowindow.cpp/.h
│   │   ├── graphingwindow.cpp/.h
│   │   ├── flowviewwindow.cpp/.h
│   │   └── sniffer/
│   │       ├── SnifferDelegate.cpp/.h
│   │       ├── snifferitem.cpp/.h
│   │       ├── sniffermodel.cpp/.h
│   │       └── snifferwindow.cpp/.h
│   │
│   └── shared/
│       ├── recontext.cpp
│       ├── recontext.h
│       ├── analysisselectionwidget.cpp
│       ├── analysisselectionwidget.h
│       └── analysisactionmenu.cpp/.h
│
├── scripting/
│   ├── ... existing scripting implementation ...
│   ├── scriptingcontext.cpp
│   ├── scriptingcontext.h
│   ├── scriptapi.cpp
│   ├── scriptapi.h
│   ├── scriptregistry.cpp
│   ├── scriptregistry.h
│   └── scriptlogmodel.cpp/.h
│
├── sender/
│   ├── ... existing sender implementation ...
│   ├── transmissionguard.cpp
│   ├── transmissionguard.h
│   ├── transmissionlog.cpp
│   └── transmissionlog.h
│
├── signals/
│   ├── signaldefinition.cpp
│   ├── signaldefinition.h
│   ├── signalcatalog.cpp
│   ├── signalcatalog.h
│   ├── custombitfield.cpp
│   ├── custombitfield.h
│   ├── derivedsignal.cpp
│   ├── derivedsignal.h
│   ├── signalvalue.cpp
│   ├── signalvalue.h
│   ├── signalresolver.cpp
│   └── signalresolver.h
│
├── traffic/
│   ├── trafficpipeline.cpp
│   ├── trafficpipeline.h
│   ├── traffictransform.cpp
│   ├── traffictransform.h
│   ├── trafficscheduler.cpp
│   ├── trafficscheduler.h
│   ├── trafficlog.cpp
│   ├── trafficlog.h
│   ├── trafficrecipe.cpp
│   └── trafficrecipe.h
│
├── visualization/
│   ├── plotmodel.cpp
│   ├── plotmodel.h
│   ├── timecursor.cpp
│   ├── timecursor.h
│   ├── graphworkspace.cpp
│   ├── graphworkspace.h
│   ├── dashboard.cpp
│   ├── dashboard.h
│   ├── dashboardtile.cpp
│   ├── dashboardtile.h
│   ├── heatmapview.cpp
│   ├── heatmapview.h
│   ├── statetimelineview.cpp
│   ├── statetimelineview.h
│   └── flowview.cpp/.h
│
├── widgets/
│   ├── ... reusable generic widgets only ...
│   ├── hexviewwidget.cpp/.h
│   ├── bitfieldwidget.cpp/.h
│   ├── timelinewidget.cpp/.h
│   ├── markerstripwidget.cpp/.h
│   ├── querybarwidget.cpp/.h
│   ├── emptyworkspacewidget.cpp/.h
│   └── safetyarmwidget.cpp/.h
│
├── tools/
├── utils/
├── themes/
├── third_party/
├── mcp/
├── mqtt/
└── packaging/
```

### Important folder rule

Do not place domain logic in `src/common/`, `src/utils/`, or `src/widgets/` merely because it is shared by two windows. Those folders should remain intentionally boring:

- `common`: tiny cross-domain primitives.
- `utils`: generic helpers with no product-domain meaning.
- `widgets`: reusable visual components with no analysis/business logic.

If code knows about CAN IDs, timestamps, DBC signals, markers, findings, capture sources, or transmission safety, it belongs in an explicit domain folder.

---

## 4. Migration rules

### 4.1 Do not move files just to make the tree look finished

A folder move should happen when one of these is true:

- The file is being actively modified for the new architecture.
- The old folder creates a misleading dependency boundary.
- The move enables a testable reusable service.
- The move is part of a completed workspace migration.

### 4.2 Extract algorithms before replacing windows

For example:

```text
Old Range State window
  → extract RangeStatistics service
  → add tests
  → use service in old window
  → use service in State Explorer
  → deprecate old window
```

Do the same for discrete states, temporal analysis, payload diffs, comparisons, and candidate scoring.

### 4.3 Preserve compatibility

- Keep old settings readable.
- Keep old capture formats readable.
- Import existing bookmarks into the new marker/finding model.
- Keep old windows available during at least one migration cycle.
- Add schema versions to project and analysis data.
- Avoid changing transmit behavior while refactoring passive analysis.

### 4.4 Keep active operations separate

Shared infrastructure is encouraged; shared visual semantics are not always appropriate.

- Passive capture: safe by default.
- Replay: clearly scheduled and inspectable.
- Sender: explicitly armed.
- Bridge: source/destination and transformations visible.
- Fuzzing: separate safety boundary.
- UDS writes/firmware: separate confirmation and audit flow.

---

## 5. Phased roadmap

## Phase 0 — Baseline and architecture decisions

### Goal

Turn the `feature/traffic-circle` organization into an explicit set of stable boundaries before implementing major workflow changes.

### Tasks

- [ ] Add GitHub epics and labels.
- [ ] Record the `feature/traffic-circle` tree as the architectural baseline.
- [ ] Audit existing `src/common`, `src/utils`, `src/widgets`, and `src/tools` for domain leakage.
- [ ] Record current build commands and supported Qt/compiler/platform combinations.
- [ ] Establish representative capture files for tests and performance.
- [ ] Measure CPU, memory, UI latency, and frame loss under idle/medium/high bus loads.
- [ ] Identify current CAN frame type and timestamp ownership.
- [ ] Identify all active-transmission entry points and existing safety checks.
- [ ] Decide project persistence format.
- [ ] Decide whether large captures need indexing immediately or after MVP.

### Files to add

```text
src/docs/architecture.md
src/docs/roadmap.md
src/docs/safety-model.md
src/docs/project-format.md
src/docs/migration-guide.md
```

### Exit criteria

- [ ] Build and benchmark process is documented.
- [ ] The canonical frame/timestamp/session decisions are recorded.
- [ ] At least one test capture is checked in or reliably generated.
- [ ] Existing active-write paths are listed and protected from accidental refactoring.

---

## Phase 1 — Canonical frame and session foundations

### Goal

Give every future workspace a common data context without changing the visible UI yet.

### 1.1 Canonical frame event

Potential files:

```text
src/frames/frameevent.h
src/frames/frameevent.cpp
src/frames/frameeventcodec.h
src/frames/frameeventcodec.cpp
src/can/canframe.h
src/can/canframe.cpp
src/can/canframeflags.h
```

Tasks:

- [ ] Audit `src/can/can_structs.h` and `src/frames/canframemodel.*`.
- [ ] Define immutable `FrameEvent` with sequence, monotonic timestamp, wall timestamp, channel, ID, flags, DLC, and up to 64 data bytes.
- [ ] Preserve classic CAN, CAN-FD, extended, RTR, and error-frame state.
- [ ] Define conversion adapters from existing frame objects.
- [ ] Add unit tests for payload lengths, flags, timestamps, and serialization.

### 1.2 Analysis session

Potential files:

```text
src/analysis/analysissession.h
src/analysis/analysissession.cpp
src/analysis/selectioncontext.h
src/analysis/selectioncontext.cpp
src/analysis/analysissource.h
src/analysis/analysissource.cpp
```

`SelectionContext` should include:

- Source/capture identifier.
- Bus/channel.
- CAN ID set or range.
- Selected frame(s).
- Byte/bit spans.
- Signal identifiers.
- Time range.
- Baseline/comparison range.
- Marker references.

Tasks:

- [ ] Define ownership/lifetime rules.
- [ ] Add context change notifications.
- [ ] Add adapters for legacy windows.
- [ ] Ensure live, file, and comparison sources can be represented.

### 1.3 Markers

Potential files:

```text
src/analysis/marker.h
src/analysis/marker.cpp
src/analysis/markerstore.h
src/analysis/markerstore.cpp
```

Tasks:

- [ ] Define manual, bookmark, trigger, diagnostic, transmit, and analysis marker types.
- [ ] Store timestamp/range, label, tags, color, origin, and optional selection context.
- [ ] Add marker creation to existing sniffer/frame views.
- [ ] Keep compatibility with `src/bookmarks/` during transition.

### Exit criteria

- [ ] Existing frame tools can receive a shared selection.
- [ ] A marker can be created from a frame/time selection.
- [ ] New code can consume `FrameEvent` without depending on a QWidget model.

---

## Phase 2 — Query and selection system

### Goal

Replace incompatible ad hoc filters with one reusable expression system.

### Proposed files

```text
src/query/canquerytoken.h
src/query/canquerylexer.h
src/query/canquerylexer.cpp
src/query/canqueryast.h
src/query/canqueryast.cpp
src/query/canqueryparser.h
src/query/canqueryparser.cpp
src/query/canqueryevaluator.h
src/query/canqueryevaluator.cpp
src/query/querycontext.h
src/query/querycontext.cpp
src/query/queryerror.h
src/query/queryerror.cpp
src/query/queryeditor.h
src/query/queryeditor.cpp
src/project/savedquery.h
src/project/savedquery.cpp
```

### Initial grammar

```text
id:0x123
id:0x700..0x7FF
bus:1
extended:true
fd:true
rtr:false
byte[2] == 0x80
changed
changed(byte[3])
rate > 10Hz
age < 100ms
```

### Later grammar

```text
signal:VehicleSpeed > 20
within:2s after:marker("unlock")
dbc:unknown
entropy(byte[4]) > 2.5
```

### Tasks

- [ ] Define grammar and error messages.
- [ ] Implement parser/evaluator tests independent of widgets.
- [ ] Add legacy adapter for `src/can/canfilter.*`.
- [ ] Add query editor with syntax feedback.
- [ ] Add saved query persistence.
- [ ] Support query evaluation against raw events, aggregates, and analysis results.

### Exit criteria

- [ ] One query can filter at least the main frame view and one RE view.
- [ ] Query errors are actionable.
- [ ] Saved queries can be reopened without being tied to a specific window.

---

## Phase 3 — Discoverability and application shell

### Goal

Expose existing capability before undertaking deep window consolidation.

### Existing integration point

```text
src/app/mainwindow.cpp
src/app/mainwindow.h
```

Because the main window is already substantial, introduce services around it instead of rewriting it in one pass.

### Proposed files

```text
src/app/commandregistry.h
src/app/commandregistry.cpp
src/app/commandpalette.h
src/app/commandpalette.cpp
src/app/workspacecontroller.h
src/app/workspacecontroller.cpp
src/app/navigationmodel.h
src/app/navigationmodel.cpp
src/re/shared/analysisactionmenu.h
src/re/shared/analysisactionmenu.cpp
src/re/shared/analysisselectionwidget.h
src/re/shared/analysisselectionwidget.cpp
```

### Command palette seed commands

- [ ] Open Sniffer.
- [ ] Open Range State.
- [ ] Open Discrete State.
- [ ] Open Frame Info.
- [ ] Open Graphing.
- [ ] Open ISO-TP.
- [ ] Open UDS Scan.
- [ ] Start/stop recording.
- [ ] Create marker/bookmark.
- [ ] Graph selection.
- [ ] Compare selection.
- [ ] Add trigger.
- [ ] Open settings.

### Contextual handoff actions

For an ID/frame/byte/time selection:

- [ ] Inspect.
- [ ] State analysis.
- [ ] Graph.
- [ ] Compare.
- [ ] Create marker.
- [ ] Create finding.
- [ ] Define signal.
- [ ] Add filter.
- [ ] Add trigger.
- [ ] Export.
- [ ] Replay.
- [ ] Compose/transmit, separately gated.

### Exit criteria

- [ ] Every major existing feature is searchable.
- [ ] Selected context follows at least four handoffs.
- [ ] Workspace creation is centralized enough to avoid duplicate setup logic.

---

## Phase 4 — State Explorer MVP

### Goal

Unify the user workflow around discovering changing IDs, ranges, discrete states, and temporal behavior.

### Existing source scope

```text
src/re/sniffer/
src/re/rangestatewindow.cpp
src/re/rangestatewindow.h
src/re/discretestatewindow.cpp
src/re/discretestatewindow.h
src/re/temporalgraphwindow.cpp
src/re/temporalgraphwindow.h
src/re/frameinfowindow.cpp
src/re/frameinfowindow.h
```

### Proposed destination

```text
src/re/explore/stateexplorerwindow.h
src/re/explore/stateexplorerwindow.cpp
src/re/explore/stateexplorercontroller.h
src/re/explore/stateexplorercontroller.cpp
src/re/explore/stateoverviewmodel.h
src/re/explore/stateoverviewmodel.cpp
src/re/explore/stateoverviewwidget.h
src/re/explore/stateoverviewwidget.cpp
src/re/explore/stateinspectorwidget.h
src/re/explore/stateinspectorwidget.cpp
src/re/explore/statehistorywidget.h
src/re/explore/statehistorywidget.cpp
src/re/explore/rangetabwidget.h
src/re/explore/rangetabwidget.cpp
src/re/explore/discretetabwidget.h
src/re/explore/discretetabwidget.cpp
src/re/explore/temporaltabwidget.h
src/re/explore/temporaltabwidget.cpp
src/re/explore/rawframetabwidget.h
src/re/explore/rawframetabwidget.cpp
```

### First vertical slice: Live Change Explorer

In scope:

- [ ] One aggregate row per ID/channel.
- [ ] Count, rate, last-seen age, current payload.
- [ ] Prior-payload XOR/change highlighting.
- [ ] Initial query support.
- [ ] Marker creation.
- [ ] Handoffs to legacy Frame Info and Graphing.
- [ ] Old sniffer remains available.

Not in scope:

- Full project persistence.
- Active transmission.
- Automated counter/checksum inference.
- Full candidate scoring.

### State Explorer tabs

#### Overview

- [ ] ID/channel/flags/DLC.
- [ ] Rate and timing statistics.
- [ ] Current/previous payload.
- [ ] Changed-byte mask.
- [ ] DBC coverage.

#### Ranges

- [ ] Min/max per byte/field.
- [ ] Unique values.
- [ ] Signed/unsigned representations.
- [ ] Optional value histograms.

#### States

- [ ] Discrete values.
- [ ] Frequency and dwell time.
- [ ] Transition matrix.
- [ ] Marker-relative transitions.

#### Timeline

- [ ] Payload/value plot.
- [ ] Marker overlays.
- [ ] Time cursor shared with future graph workspace.

#### Raw

- [ ] Bounded frame history.
- [ ] Prior-frame XOR.
- [ ] Frozen baseline comparison.
- [ ] Changed bits highlighted.

### Algorithm files

```text
src/analysis/frameaggregatestore.h/.cpp
src/analysis/framehistory.h/.cpp
src/analysis/payloaddiff.h/.cpp
src/analysis/activitystatistics.h/.cpp
src/analysis/rangestatistics.h/.cpp
src/analysis/discretestateanalysis.h/.cpp
src/analysis/transitionanalysis.h/.cpp
src/analysis/temporalanalysis.h/.cpp
```

### Migration order

1. [ ] Extract aggregate/rate logic.
2. [ ] Add tests.
3. [ ] Use it from the legacy sniffer.
4. [ ] Add Live Change Explorer.
5. [ ] Extract range/discrete algorithms.
6. [ ] Use algorithms in old windows and new tabs.
7. [ ] Migrate frame-info details into inspector widgets.
8. [ ] Retire old windows only after parity.

### Acceptance tests

- [ ] Identify the highest-change ID in a live capture.
- [ ] Select a byte and see range, state, transition, and history information.
- [ ] Freeze a baseline and perform an action.
- [ ] Rank/filter changes after a marker.
- [ ] Preserve selection when opening graph or frame details.

---

## Phase 5 — Vehicle Project, markers, and findings

### Goal

Prevent reverse-engineering discoveries from disappearing when a window closes.

### Proposed files

```text
src/project/vehicleproject.h/.cpp
src/project/projectmanifest.h/.cpp
src/project/projectpersistence.h/.cpp
src/project/projectmigration.h/.cpp
src/project/projectasset.h/.cpp
src/project/finding.h/.cpp
src/project/findingstore.h/.cpp
src/project/savedquery.h/.cpp
src/project/savedlayout.h/.cpp
src/project/projectbrowser.h/.cpp
src/project/projectbrowser.cpp
```

### Recommended project layout

```text
MyVehicle.savvylens/
├── project.json
├── captures/
├── dbc/
├── scripts/
├── findings.json
├── markers.json
├── queries.json
├── layouts/
└── exports/
```

### Finding model

```text
Finding
  ├─ title/type
  ├─ CAN ID/channel
  ├─ byte/bit span or signal reference
  ├─ interpretation
  ├─ confidence: candidate/tested/verified/rejected
  ├─ tags
  ├─ supporting captures/markers/comparisons/plots
  ├─ author/timestamps
  └─ optional DBC/custom signal mapping
```

### Tasks

- [ ] Define schema version.
- [ ] Decide embedded versus linked captures.
- [ ] Import existing bookmarks without loss.
- [ ] Add finding creation from State Explorer.
- [ ] Add evidence links.
- [ ] Add project search/browser.
- [ ] Persist layouts and saved queries.
- [ ] Keep secrets outside normal project exports.

### Exit criteria

A project can be reopened with its captures, DBC associations, markers, findings, layouts, queries, and scripts intact.

---

## Phase 6 — Capture Comparison and Investigation

### Goal

Make file comparison, event comparison, DBC comparison, and bisect part of one evidence workflow.

### Existing source scope

```text
src/re/filecomparatorwindow.cpp/.h
src/re/bookmarkeventanalyzer.cpp/.h
src/re/dbccomparatorwindow.cpp/.h
src/re/bisectwindow.cpp/.h
```

### Proposed destination

```text
src/re/compare/capturecomparisonwindow.h/.cpp
src/re/compare/comparisoncontroller.h/.cpp
src/re/compare/comparisonresultmodel.h/.cpp
src/re/compare/investigationwindow.h/.cpp
src/re/compare/investigationtimeline.h/.cpp
src/re/compare/comparisoninput.h/.cpp
src/re/compare/comparisonmetrics.h/.cpp
```

Legacy windows can initially remain in:

```text
src/re/legacy/
```

### Comparison inputs

- [ ] Capture A versus capture B.
- [ ] Baseline versus action time range.
- [ ] Repeated marker/bookmark occurrences.
- [ ] DBC-known versus raw/unknown traffic.
- [ ] Before/after an event.

### Result ranking

- [ ] New/missing IDs.
- [ ] Changed-byte frequency.
- [ ] Changed-bit frequency.
- [ ] Value delta/range.
- [ ] Timing relative to marker.
- [ ] Repeatability.
- [ ] DBC coverage.

### Bisect integration

- [ ] Represent bisect inputs as selected ranges/commits/capture segments.
- [ ] Show narrowing steps in Investigation timeline.
- [ ] Preserve each step as an evidence marker.
- [ ] Promote final result to a finding.

### Exit criteria

A user can compare two captures or event windows, inspect ranked candidates, open them in State Explorer/graphs, and save a finding with evidence.

---

## Phase 7 — Signal Catalog and visualization

### Goal

Define a raw bitfield or DBC signal once and reuse it everywhere.

### Proposed signal files

```text
src/signals/signaldefinition.h/.cpp
src/signals/signalcatalog.h/.cpp
src/signals/custombitfield.h/.cpp
src/signals/derivedsignal.h/.cpp
src/signals/signalvalue.h/.cpp
src/signals/signalresolver.h/.cpp
```

### Signal sources

- [ ] DBC signal.
- [ ] User-defined raw bitfield.
- [ ] Inferred candidate.
- [ ] Derived expression.
- [ ] Frame rate/age/activity metric.

### Signal definition fields

```text
stable ID
CAN ID/channel
start bit/length
byte order
signedness
scale/offset
units
enum labels
validity/update policy
origin/confidence
project association
```

### Existing modules to integrate

```text
src/dbc/
src/re/frameinfowindow.cpp/.h
src/re/graphingwindow.cpp/.h
src/re/newgraphdialog.cpp/.h
src/widgets/
```

### Proposed visualization files

```text
src/visualization/plotmodel.h/.cpp
src/visualization/timecursor.h/.cpp
src/visualization/graphworkspace.h/.cpp
src/visualization/dashboard.h/.cpp
src/visualization/dashboardtile.h/.cpp
src/visualization/heatmapview.h/.cpp
src/visualization/statetimelineview.h/.cpp
src/visualization/flowview.h/.cpp
```

### Visualization tasks

- [ ] Shared time cursor.
- [ ] Shared marker overlays.
- [ ] Drag/drop signals and findings into graphs.
- [ ] Save layouts to project.
- [ ] Add line, step/state, histogram, activity heatmap, and flow views.
- [ ] Reuse graphs in live and playback sessions.
- [ ] Add dashboard tiles for testing/monitoring.

### Exit criteria

A custom signal created in the inspector can be plotted, triggered, scripted, exported, and displayed in a dashboard without redefining it.

---

## Phase 8 — Traffic Studio and automation

### Goal

Unify capture, playback, composing, transforms, and bridging through a shared pipeline.

### Existing source scope

```text
src/io/
src/playback/
src/sender/
src/mqtt/
src/mcp/
src/scripting/
src/app/triggerdialog...
```

The current feature/traffic-circle root still contains `triggerdialog.ui`; decide whether its final ownership belongs under `src/automation/` or a UI-specific directory during migration.

### Proposed traffic files

```text
src/traffic/trafficpipeline.h/.cpp
src/traffic/trafficsource.h/.cpp
src/traffic/trafficsink.h/.cpp
src/traffic/traffictransform.h/.cpp
src/traffic/trafficscheduler.h/.cpp
src/traffic/trafficlog.h/.cpp
src/traffic/trafficrecipe.h/.cpp
src/io/capturemetadata.h/.cpp
src/io/captureindex.h/.cpp
src/io/captureloader.h/.cpp
src/playback/playbacksession.h/.cpp
src/playback/playbackscheduler.h/.cpp
src/sender/transmissionguard.h/.cpp
src/sender/transmissionlog.h/.cpp
```

### Pipeline

```text
Source → Query → Transform → Scheduler → Sink
```

Sources:

- [ ] Live bus.
- [ ] Capture file.
- [ ] Selected session range.
- [ ] Generated frames.
- [ ] Script.

Sinks:

- [ ] UI/session.
- [ ] Capture recorder.
- [ ] Physical CAN bus.
- [ ] Bridge destination.
- [ ] Export.
- [ ] MQTT/MCP.

### Traffic Studio modes

- [ ] Capture.
- [ ] Replay.
- [ ] Compose.
- [ ] Transform.
- [ ] Bridge.

### Automation files

```text
src/automation/automationrule.h/.cpp
src/automation/triggercondition.h/.cpp
src/automation/automationaction.h/.cpp
src/automation/automationengine.h/.cpp
src/automation/automationcontext.h/.cpp
src/automation/automationlog.h/.cpp
```

### Automation examples

```text
When id:0x123 and changed(byte[2])
Then createMarker("state changed") and startRecording("event")
```

```text
When signal:VehicleSpeed == 0 for 10s
Then stopPlayback() and exportLast(30s)
```

### Safety tasks

- [ ] Make receive-only the default.
- [ ] Centralize arm/disarm state.
- [ ] Show target connection, bus, rate, count, and transforms.
- [ ] Add stop control.
- [ ] Log every outbound frame.
- [ ] Require explicit confirmation for destructive operations.

---

## Phase 9 — Experiment Lab

### Goal

Connect passive evidence to controlled hypothesis testing.

### Existing source scope

```text
src/re/controlanalysisdialog.cpp/.h
src/re/controlcandidatemodel.cpp/.h
src/re/controlstatedetector.cpp/.h
src/re/fuzzingwindow.cpp/.h
src/sender/
src/playback/
```

### Proposed destination

```text
src/re/experiment/experimentlabwindow.h/.cpp
src/re/experiment/experimentlabcontroller.h/.cpp
src/re/experiment/experiment.h/.cpp
src/re/experiment/experimentresultmodel.h/.cpp
src/re/experiment/candidatehypothesis.h/.cpp
src/re/experiment/safetypolicy.h/.cpp
```

### Candidate lifecycle

```text
Observed → Candidate → Tested → Verified
                    ↘ Rejected
```

Candidate fields:

- [ ] ID/channel and byte/bit span.
- [ ] Candidate type.
- [ ] Evidence score/explanation.
- [ ] Value/transition behavior.
- [ ] Supporting markers/comparisons.
- [ ] Test history/outcome.
- [ ] Safety classification.

### Workflow

1. [ ] Collect capture.
2. [ ] Define action markers.
3. [ ] Compare repeated events.
4. [ ] Rank candidate fields.
5. [ ] Define hypothesis.
6. [ ] Simulate/replay offline.
7. [ ] Optionally perform explicitly armed live test.
8. [ ] Record result.
9. [ ] Promote/reject finding.

### Exit criteria

A candidate can be traced from evidence through testing to a project finding without manually copying IDs, ranges, or notes between windows.

---

## Phase 10 — Connections, settings, and diagnostics

### Goal

Make bus/interface context consistent throughout the application while retaining safe separation for diagnostics and programming.

### Proposed connection files

```text
src/connections/connectionmanager.h/.cpp
src/connections/connectionprofile.h/.cpp
src/connections/connectioncapabilities.h/.cpp
src/connections/connectionhealth.h/.cpp
src/connections/connectionregistry.h/.cpp
src/connections/buschannel.h/.cpp
src/connections/connectionmanagerwidget.h/.cpp
```

### Connection capabilities

- [ ] Receive.
- [ ] Transmit.
- [ ] Record.
- [ ] Replay.
- [ ] Bridge.
- [ ] Diagnose.
- [ ] Stream.

### Settings files

```text
src/app/settingsscope.h/.cpp
src/app/settingsprofile.h/.cpp
src/app/settingsmigration.h/.cpp
```

Define scopes:

- [ ] Application.
- [ ] User profile.
- [ ] Vehicle project.
- [ ] Current session.
- [ ] Connection-specific.

### Diagnostics destination

```text
src/re/diagnostics/diagnosticsworkspace.h/.cpp
src/re/diagnostics/isotp_interpreterwindow.h/.cpp
src/re/diagnostics/udsscanwindow.h/.cpp
src/re/diagnostics/udsfirmwareuploaderwindow.h/.cpp
src/bus_protocols/protocolsession.h/.cpp
```

Tasks:

- [ ] Shared connection/address context.
- [ ] Decoded transaction history.
- [ ] Diagnostic markers/findings.
- [ ] Explicitly separate firmware upload from generic replay.
- [ ] Preserve audit log and confirmation behavior.

---

## Phase 11 — Legacy migration and cleanup

### Goal

Remove duplication only after replacement workflows have parity.

### Legacy staging folder

During migration, use:

```text
src/re/legacy/
```

Candidate files to stage later:

```text
src/re/legacy/sniffer/
src/re/legacy/rangestatewindow.*
src/re/legacy/discretestatewindow.*
src/re/legacy/temporalgraphwindow.*
src/re/legacy/frameinfowindow.*
src/re/legacy/graphingwindow.*
src/re/legacy/flowviewwindow.*
src/re/legacy/filecomparatorwindow.*
src/re/legacy/bisectwindow.*
```

Do not move these until the new workspace has parity and the move produces a real dependency improvement.

### Per-window migration checklist

- [ ] Inventory current features.
- [ ] Identify reusable logic.
- [ ] Extract service/algorithm.
- [ ] Add unit tests.
- [ ] Make old window use extracted service.
- [ ] Add new workspace view.
- [ ] Add context handoff.
- [ ] Validate against legacy behavior.
- [ ] Add migration note.
- [ ] Deprecate old entry point.
- [ ] Remove only after a defined release window.

### Exit criteria

- [ ] No core logic exists only inside a window class.
- [ ] New workspaces use shared session/query/signal/project services.
- [ ] Legacy windows are either removed or explicitly retained as specialized views.
- [ ] README and docs describe the new workflows.

---

## 6. Detailed module consolidation matrix

| Current feature/traffic-circle module | First action | Eventual location | Replacement/workspace |
|---|---|---|---|
| `src/re/sniffer/` | Extract aggregates/history; preserve UI | `src/re/explore/` + `src/analysis/` | State Explorer |
| `src/re/rangestatewindow.*` | Extract range algorithm | `src/analysis/` + `src/re/explore/` | State Explorer Ranges tab |
| `src/re/discretestatewindow.*` | Extract state/transition algorithm | `src/analysis/` + `src/re/explore/` | State Explorer States tab |
| `src/re/temporalgraphwindow.*` | Extract temporal queries | `src/analysis/` + `src/visualization/` | State Explorer Timeline |
| `src/re/frameinfowindow.*` | Extract raw/bit decoding | `src/signals/`, `src/widgets/`, `src/re/explore/` | Inspector |
| `src/re/filecomparatorwindow.*` | Extract comparison engine | `src/re/compare/` | Capture Comparison |
| `src/re/bookmarkeventanalyzer.*` | Use marker/finding model | `src/re/compare/`, `src/bookmarks/` | Investigation |
| `src/re/dbccomparatorwindow.*` | Share signal/DBC catalog | `src/re/compare/`, `src/dbc/` | Capture Comparison |
| `src/re/bisectwindow.*` | Make range narrowing a mode | `src/re/compare/` | Investigation |
| `src/re/controlstatedetector.*` | Extract candidate analysis | `src/analysis/`, `src/re/experiment/` | Experiment Lab |
| `src/re/controlcandidatemodel.*` | Define candidate lifecycle | `src/re/experiment/` | Experiment Lab |
| `src/re/controlanalysisdialog.*` | Compose candidate evidence UI | `src/re/experiment/` | Experiment Lab |
| `src/re/fuzzingwindow.*` | Add safety policy and experiment link | `src/re/experiment/` | Experiment Lab |
| `src/re/graphingwindow.*` | Share signal/time/marker context | `src/visualization/` | Visualizations |
| `src/re/newgraphdialog.*` | Replace with graph workspace setup | `src/visualization/` | Visualizations |
| `src/re/flowviewwindow.*` | Retain as specialized visualization | `src/visualization/` | Visualizations |
| `src/dbc/` | Add catalog/resolver layer | `src/dbc/`, `src/signals/` | Signal Catalog |
| `src/bookmarks/` | Generalize bookmarks into markers/findings | `src/bookmarks/`, `src/analysis/`, `src/project/` | Vehicle Project |
| `src/can/canfilter.*` | Adapt to query engine | `src/query/` + compatibility adapter | Query system |
| `src/frames/canframemodel.*` | Preserve model; add aggregate/history models | `src/frames/` | Explorer/Traffic Studio |
| `src/io/` | Add common source/sink/metadata interfaces | `src/io/`, `src/traffic/` | Traffic Studio |
| `src/playback/` | Add shared scheduler/session | `src/playback/`, `src/traffic/` | Traffic Studio |
| `src/sender/` | Add transmission guard/log | `src/sender/`, `src/traffic/` | Traffic Studio/Experiment |
| `src/scripting/` | Bind to session/query/signals/markers | `src/scripting/`, `src/automation/` | Automation |
| `src/mcp/`, `src/mqtt/` | Integrate as automation/stream sinks | Existing areas + `src/traffic/` | Automation/Traffic Studio |
| `src/connections/` | Centralize profile/capability/health | `src/connections/` | Connections |
| `src/app/mainwindow.*` | Add command/workspace controllers | `src/app/` | Application shell |

---

## 7. Dependency direction

Target dependency direction:

```text
common / utils
       ↓
can / frames / query
       ↓
analysis / signals / project / traffic / connections
       ↓
re services and workspace controllers
       ↓
widgets and windows
       ↓
app shell / navigation
```

Preferred rules:

- `analysis` should not depend on a window.
- `query` should not depend on a model-view class.
- `project` should not depend on a specific workspace window.
- `signals` should be usable by analysis, graphing, triggers, and scripts.
- `traffic` may depend on CAN/frame/session abstractions but not on the main window.
- Workspace windows may depend on services, but services must not call back into windows.
- The app shell coordinates workspaces; it should not contain analysis algorithms.

### Dependency checks to add later

- [ ] Document allowed include directions.
- [ ] Add a lightweight include/dependency audit script under `src/tools/` or repository tooling.
- [ ] Reject new domain logic in `src/common`/`src/utils` during review.

---

## 8. Testing strategy

### Unit tests

```text
test/
├── analysis/
│   ├── frameaggregatestore_test.cpp
│   ├── payloaddiff_test.cpp
│   ├── rangestatistics_test.cpp
│   ├── discretestateanalysis_test.cpp
│   └── transitionanalysis_test.cpp
├── query/
│   ├── canquerylexer_test.cpp
│   ├── canqueryparser_test.cpp
│   └── canqueryevaluator_test.cpp
├── project/
│   ├── projectpersistence_test.cpp
│   ├── projectmigration_test.cpp
│   └── findingstore_test.cpp
├── signals/
│   ├── signalresolver_test.cpp
│   └── custombitfield_test.cpp
├── io/
│   ├── capturemetadata_test.cpp
│   └── frameeventcodec_test.cpp
└── fixtures/
```

### Integration tests

- [ ] Live frame source to aggregate model.
- [ ] Selection handoff from explorer to graph/frame info.
- [ ] Marker creation and rendering.
- [ ] Project save/reopen.
- [ ] Comparison result to finding.
- [ ] Signal catalog to graph/trigger.
- [ ] Traffic pipeline source/filter/sink.

### Manual safety tests

- [ ] Sender disarmed by default.
- [ ] Correct target bus shown.
- [ ] Rate/message limits enforced.
- [ ] Stop control works.
- [ ] Bridge source/destination are unambiguous.
- [ ] Fuzzing cannot silently use a normal replay profile.
- [ ] UDS/firmware operations retain confirmation and audit logging.

---

## 9. GitHub tracking structure

Create these epics on the `feature/traffic-circle` branch roadmap:

1. `[Epic] Foundation: frame events, analysis session, selections, markers`
2. `[Epic] Query: unified CAN query/filter system`
3. `[Epic] Discoverability: command palette and contextual actions`
4. `[Epic] Explore: State Explorer consolidation`
5. `[Epic] Project: vehicle projects, findings, and evidence`
6. `[Epic] Compare: capture comparison and investigation`
7. `[Epic] Signals: signal catalog and custom bitfields`
8. `[Epic] Visualize: graph workspace, timelines, and dashboards`
9. `[Epic] Traffic: capture, replay, sender, transforms, and bridge`
10. `[Epic] Automation: triggers, scripts, and action engine`
11. `[Epic] Experiment: control discovery and safe validation`
12. `[Epic] Connections: profiles, capabilities, health, and settings`
13. `[Epic] Diagnostics: ISO-TP, UDS, and transaction history`
14. `[Epic] Migration: legacy windows, documentation, benchmarks, cleanup`

### Suggested labels

```text
area:app
area:analysis
area:automation
area:bookmarks
area:can
area:connections
area:dbc
area:frames
area:io
area:project
area:query
area:re
area:signals
area:traffic
area:visualization
kind:architecture
kind:algorithm
kind:ui
kind:migration
kind:documentation
kind:performance
risk:active-can
priority:p0
priority:p1
priority:p2
```

### Issue template fields

```text
Problem / user workflow
Target workspace or folder
Current files/modules
Proposed files/modules
Scope
Non-goals
Dependencies
API/data model changes
UI notes
Safety considerations
Acceptance criteria
Performance criteria
Migration/compatibility notes
Test captures/scenarios
```

---

## 10. Recommended first implementation slice

### Live Change Explorer

This is the smallest slice that validates the architecture and immediately improves the current sniffer experience.

### Files to add first

```text
src/analysis/selectioncontext.h
src/analysis/selectioncontext.cpp
src/analysis/frameaggregatestore.h
src/analysis/frameaggregatestore.cpp
src/analysis/framehistory.h
src/analysis/framehistory.cpp
src/analysis/payloaddiff.h
src/analysis/payloaddiff.cpp
src/re/explore/stateexplorerwindow.h
src/re/explore/stateexplorerwindow.cpp
src/re/explore/stateexplorercontroller.h
src/re/explore/stateexplorercontroller.cpp
src/re/explore/stateoverviewmodel.h
src/re/explore/stateoverviewmodel.cpp
```

### Files to integrate initially

```text
src/app/mainwindow.cpp
src/app/mainwindow.h
src/frames/canframemodel.cpp
src/frames/canframemodel.h
src/re/sniffer/sniffermodel.cpp
src/re/sniffer/sniffermodel.h
src/re/frameinfowindow.cpp
src/re/frameinfowindow.h
src/re/graphingwindow.cpp
src/re/graphingwindow.h
```

### MVP behavior

- [ ] Aggregate incoming frames by ID/channel.
- [ ] Show count, rate, age, current payload, previous payload.
- [ ] Highlight changed bytes and bits.
- [ ] Select an ID and preserve it as `SelectionContext`.
- [ ] Create a marker from current traffic.
- [ ] Open existing Frame Info/Graphing with the selected context.
- [ ] Keep legacy Sniffer available.

### Definition of done

- [ ] Builds on supported platforms.
- [ ] Handles benchmark capture without UI-induced drops.
- [ ] Selection handoffs work.
- [ ] Legacy behavior remains available.
- [ ] At least one integration test and one live/manual capture test pass.

---

## 11. Progress ledger

Update this table after each implementation thread or pull request.

| Phase | Status | Current task | Branch/PR | Last update | Blockers/notes |
|---|---|---|---|---|---|
| 0. Baseline | `[ ]` | Record architecture decisions and benchmarks |  | 2026-08-13 | Based on `feature/traffic-circle` |
| 1. Frame/session | `[ ]` | Define `FrameEvent`, `SelectionContext`, markers |  | 2026-08-13 |  |
| 2. Query | `[ ]` | Define initial grammar and evaluator |  | 2026-08-13 |  |
| 3. Discoverability | `[ ]` | Command palette/context handoffs |  | 2026-08-13 |  |
| 4. State Explorer | `[ ]` | Live Change Explorer |  | 2026-08-13 |  |
| 5. Project/findings | `[ ]` | Project schema and finding model |  | 2026-08-13 |  |
| 6. Comparison | `[ ]` | Unified comparison model |  | 2026-08-13 |  |
| 7. Signals/visualization | `[ ]` | Signal Catalog design |  | 2026-08-13 |  |
| 8. Traffic/automation | `[ ]` | Pipeline and rule interfaces |  | 2026-08-13 |  |
| 9. Experiment | `[ ]` | Candidate lifecycle/safety model |  | 2026-08-13 |  |
| 10. Connections/diagnostics | `[ ]` | Shared connection identity |  | 2026-08-13 |  |
| 11. Migration | `[ ]` | Legacy capability inventory |  | 2026-08-13 |  |

---

## 12. Future-thread update template

Use this at the top of future SavvyLens threads:

```markdown
## SavvyLens roadmap update

**Branch:**
**Roadmap phase:**
**Epic:**
**Issue / PR:**
**Status:** not started | in progress | blocked | ready for review | complete

### What changed
- 

### Files added or changed
- 

### Decisions made
- 

### Current API/data model
- 

### Validation
- Build platforms:
- Test captures/scenarios:
- Performance:
- Safety behavior:

### Problems/open questions
- 

### Next step
- 
```

When starting a new thread, include the branch and the relevant phase. If the repository layout changes again, include the new commit SHA so the plan can be rebased against the correct tree.

---

## 13. Decision log

| Date | Decision | Reason | Consequences |
|---|---|---|---|
| 2026-08-13 | Base roadmap on `feature/traffic-circle`, not `master` | `feature/traffic-circle` contains the major source re-layout | All future paths in this document use the reorganized tree |
| 2026-08-13 | Add explicit `analysis`, `query`, `project`, `signals`, `traffic`, `automation`, and `visualization` domains eventually | Prevent shared logic from disappearing into `common`, `utils`, or window classes | New folders should be added incrementally as services are extracted |
| 2026-08-13 | Keep legacy RE windows during migration | Avoid a rewrite and preserve behavior during parity work | Use adapters and `src/re/legacy/` only when useful |
|  |  |  |  |

---

## 14. Practical order of work

If implementation needs to stay focused, follow this order:

1. [ ] Review and accept the proposed folder/domain boundaries.
2. [ ] Add tests/fixtures and record performance baselines.
3. [ ] Extract `SelectionContext` and `FrameEvent` without changing the UI.
4. [ ] Extract marker primitives and connect them to an existing view.
5. [ ] Build the first aggregate store and use it in the old sniffer.
6. [ ] Build Live Change Explorer under `src/re/explore/`.
7. [ ] Add command palette and contextual handoffs.
8. [ ] Extract range/discrete/temporal analysis services.
9. [ ] Add the State Explorer tabs.
10. [ ] Add project/findings persistence.
11. [ ] Add comparison/investigation workflow.
12. [ ] Add Signal Catalog and shared visualization context.
13. [ ] Add Traffic Studio pipeline and automation.
14. [ ] Add Experiment Lab.
15. [ ] Integrate connections and diagnostics.
16. [ ] Migrate/deprecate legacy windows after parity.

The first major user-visible milestone should be **State Explorer MVP**, but the first coding milestone should be the smaller **Live Change Explorer** slice that validates the shared data and selection architecture.
