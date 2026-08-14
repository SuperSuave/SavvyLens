# Log I/O module

This module loads and saves captured CAN traffic.

## Responsibilities

- `FrameFileIO` provides user-facing load/save workflows and format dispatch.
- `formats/` contains low-level format readers and helpers.
- Native format handling can preserve bookmarks.
- Automatic detection chooses a likely input format before invoking a loader.

## Rules for new formats

1. Add format detection only when it is sufficiently discriminating.
2. Keep low-level parser code separate from Qt file-dialog behavior.
3. Add a representative fixture under `test/` when practical.
4. Document whether the new format supports load, save, or both.
5. Avoid modifying the original capture file during import.