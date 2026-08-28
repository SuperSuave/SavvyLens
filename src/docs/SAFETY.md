# Active CAN Safety

SavvyLens is passive by default. Sending, replaying, bridging, fuzzing,
UDS requests, and firmware upload are active operations.

For any active operation:
- Show the target adapter and bus/channel.
- Keep it visually distinct from passive capture and analysis.
- Provide an accessible stop control.
- Avoid allowing a passive selection or analysis action to transmit
  without an explicit user action.

Firmware upload remains separate from normal replay and experimentation.

During Traffic Circle, shared session/filter/pipeline work must not bypass
the existing sender, playback, fuzzing, or diagnostic safety checks.