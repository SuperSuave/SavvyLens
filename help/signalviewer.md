# Signal Viewer

![Signal Viewer](./images/SignalViewer.png)

Signal Viewer monitors selected DBC signals against the current captured or live frame stream. It updates displayed values when matching frames arrive and can save or restore the selected signal set.

## Typical workflow

1. Load a DBC containing the messages and signals of interest.
2. Open Signal Viewer.
3. Use the signal tree to check one or more signals.
4. Confirm that matching CAN frames are arriving in the main frame collection.
5. Watch the decoded values update.
6. Save the selected signal definition set if you want to restore it later.

## Selecting signals

Signal Viewer uses the loaded DBC definitions to present a tree of available signals.

Select one or more signals from the tree to add them to the monitored set. The viewer then waits for frames matching each selected signal’s parent message.

A selected signal updates only when:

- A matching CAN frame arrives.
- The frame matches the DBC message ID.
- The signal is active for that message.
- Any required multiplexing conditions are satisfied.

If a value does not update as expected, first confirm that the correct DBC is loaded, the expected CAN ID is being captured, and the signal definition has the correct bit layout, byte order, scaling, and multiplexing settings.

## Saving signal sets

Signal Viewer definition files use the `.sdf` extension.

An `.sdf` file stores message and signal references for the selected signal set. You can load a saved definition later or append its contents to the current selection.

This is useful when you routinely monitor the same group of signals across multiple captures or live sessions.

## Signal matching behavior

Signal Viewer uses `DBCHandler` and the currently loaded DBC definitions to resolve selected signals and decode incoming frames.

When loading an `.sdf` definition, SavvyLens first attempts to resolve a saved signal using its CAN ID and associated signal information. If it cannot find the message by CAN ID, it may fall back to matching the message name.

> **Note:** Duplicate DBC message names can make name-based fallback matching less reliable. Prefer unique message names and keep the DBC used by the `.sdf` file available when restoring saved signal sets.

## Troubleshooting

- Confirm that the relevant DBC file is loaded.
- Confirm that the expected CAN ID is present in the main frame collection.
- Confirm that the selected signal belongs to the expected DBC message.
- Confirm the signal’s start bit, length, byte order, signedness, scale, and offset.
- For multiplexed messages, confirm the multiplexor and multiplexing conditions match the incoming payload.
- If a saved `.sdf` file does not restore correctly, verify that the original DBC has not changed IDs or message names.