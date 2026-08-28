# Frame Data Baseline

## Scope

This document records the existing `feature/traffic-circle` frame-data baseline
before introducing `FrameAggregateStore`, `FrameHistory`, or `PayloadDiff`.

It is an inventory of current ownership and behavior. It does not change
existing frame, Sniffer, bookmark, playback, or analysis workflows.

## Current frame representation

- Primary frame type: `CANFrame`
- Definition: `src/can/can_structs.h`
- Representation: `CANFrame` publicly inherits from Qt's `QCanBusFrame`.
- Main frame-view model: `CANFrameModel`
- Sniffer aggregation model: `SnifferModel`

### Frame identity and transport fields

- CAN ID: inherited `QCanBusFrame::frameId()`.
- Bus/channel: `int CANFrame::bus`.
- Payload: inherited `QCanBusFrame::payload()`.
- Frame type: inherited `QCanBusFrame::frameType()`.
- Extended-frame state: inherited
  `QCanBusFrame::hasExtendedFrameFormat()`.
- The exact handling of RTR, error, CAN-FD, and other Qt frame-format
  distinctions must be confirmed at each consuming/model layer before defining
  the first aggregate-key implementation.

### SavvyLens metadata fields

- Direction: `bool CANFrame::isReceived`.
- Local time-delta metadata: `uint64_t CANFrame::timedelta`.
- Overwrite-mode count: `uint32_t CANFrame::frameCount`.
- Original source/frame position: `int CANFrame::originalIndex`.
  The default value `-1` represents an unassigned original index.

### Timestamp representation

- Timestamp accessor: inherited `QCanBusFrame::timeStamp()`.
- Existing ordering and model logic converts the Qt timestamp to microseconds:

  ```cpp
  timeStamp().seconds() * 1000000 + timeStamp().microSeconds()
  ```

- Timestamp units for existing ordering and presentation logic are
  microseconds.
- `CANFrameModel` may normalize frame timestamps by subtracting a model-level
  `timeOffset`, then storing the adjusted value back into copied frames.
- This document does not establish that timestamps are monotonic, session-wide,
  or equivalent across live capture, file load, and playback sources.

## Ownership and lifetime

### Main frame model

`CANFrameModel` is a `QAbstractTableModel` that owns its current frame data by
value:

```cpp
QVector<CANFrame> frames;
QVector<CANFrame> filteredFrames;
```

- `frames` is the model's primary frame collection.
- `filteredFrames` is a separate by-value collection used for the current
  filtered presentation.
- The model receives frames through the `addFrame(...)`, `addFrames(...)`, and
  `insertFrames(...)` APIs.
- The model supports clearing, filtering, sorting, timestamp normalization, and
  an overwrite mode.
- When frames are added or inserted, the model assigns:

  ```cpp
  tempFrame.originalIndex = frames.count();
  ```

  Therefore, `originalIndex` is a raw-frame coordinate within this model's
  `frames` vector. It is not yet a durable capture/session-wide identity.

### Consumer access

The model exposes read-only raw pointers for legacy consumers:

```cpp
const CANFrame *getFilteredFrameRef(int row) const;
const QVector<CANFrame> *getListReference() const;
const QVector<CANFrame> *getFilteredListReference() const;
```

It also provides filtered-row lookup by frame ID and original source index:

```cpp
int findFirstFilteredRowByFrameId(uint32_t frameId) const;
int findFilteredRowByOriginalIndex(int originalIndex) const;
```

These accessors do not transfer ownership. Callers must not modify the returned
frames or containers.

### Lifetime constraint

Pointers obtained from `CANFrameModel` must be treated as short-lived views.
They may become invalid when the model inserts, clears, filters, sorts, or
rebuilds its vectors.

A future `FrameAggregateStore`, `FrameHistory`, or `PayloadDiff` service must
therefore consume `CANFrame` values/copies or explicitly owned snapshots. It
must not retain `CANFrameModel` pointers, `QVector` pointers, or row indexes as
durable analysis references.

## Existing main-model behavior

`CANFrameModel` currently provides presentation-oriented behavior:

- Filtering by CAN ID through `setFilterState(...)` and `setActiveFilterIds(...)`.
- Filtering by bus through `setBusFilterState(...)`.
- Sorting through `sortByColumn(...)`.
- A distinct overwrite mode through `setOverwriteMode(...)` and
  `recalcOverwrite()`.
- Filtered-row lookup by CAN ID and `originalIndex`.

These behaviors remain owned by the existing table model. A future aggregate
store must not replace or silently redefine them in its first slice.

## Existing aggregation behavior

### Sniffer model

`SnifferModel` is an existing Qt item model that maintains derived aggregation
state through dynamically allocated `SnifferItem` instances:

```cpp
QMap<quint32, SnifferItem*> mMap;
QMap<quint32, SnifferItem*> mFilters;
```

`SnifferModel::update(CANConnection*, QVector<CANFrame>&)` groups frames by
the numeric Qt CAN ID returned from `QCanBusFrame::frameId()`:

```cpp
if (!mMap.contains(frame.frameId()))
{
    mMap[frame.frameId()] = new SnifferItem(frame, mTimeSequence);
}
else
{
    mMap[frame.frameId()]->update(frame, mTimeSequence, mMuteNotched);
}
```

The current Sniffer aggregation key is therefore:

```text
frameId
```

This legacy grouping does not distinguish bus/channel, direction, standard
versus extended format, frame type, source/capture identity, timestamp, or
payload.

`SnifferItem` owns Sniffer-specific derived state and remains part of the
working Sniffer workflow.

### Compatibility boundary

A future `FrameAggregateStore` must not replace `SnifferModel` in its first
slice. It should initially be a separate UI-neutral service that can later be
adapted into the Sniffer and future explorer workflows.

The existing Sniffer key is only numeric CAN ID. A future aggregate key must
make its treatment of bus/channel, frame format, direction, and source identity
explicit rather than inheriting this implicit legacy behavior.

## Initial aggregate-key recommendation

Proposed starting key:

```text
(bus, frameId, extended-frame-format state, frame type, direction)
```

Reason:

- The same numeric CAN ID can exist on more than one bus/channel.
- The main frame view exposes received/transmitted direction separately.
- Frame-format distinctions must not be silently discarded.
- Payload, timestamp, `timedelta`, `frameCount`, `originalIndex`, UI rows, and
  raw pointers describe frame instances or presentation state, not aggregate
  identity.

Deferred decisions:

- Exact treatment of error frames.
- Exact treatment of RTR frames.
- Whether DLC belongs in the key.
- Whether CAN-FD mode belongs in the key.
- How a future source/capture identifier participates in the key.
- Whether a later compatibility adapter intentionally reproduces the legacy
  Sniffer `frameId`-only grouping.

## Consequences for future Phase 1 work

- `FrameAggregateStore` should consume copied or value-like frame data, not
  depend on `MainWindow` or a QWidget model.
- `FrameHistory` should not be introduced until retention and ownership rules
  are explicit.
- `PayloadDiff` should operate on stable value data or explicit snapshots.
- Existing `CANFrameModel` and `SnifferModel` remain unchanged during this
  inventory step.

## Open decisions

- Canonical immutable frame-event representation.
- Timestamp source, units, and monotonicity contract for future analysis data.
- Live, file, and playback source equivalence.
- Analysis-session ownership and lifetime.
- Whether large captures require indexing before the first explorer MVP.
