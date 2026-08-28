# Performance Baseline

Traffic Circle should not make normal capture, filtering, recording,
or frame inspection less responsive.

Before a substantial live-data, model, or UI update, test a Release build with:

- A representative live CAN session or capture.
- Main frame view and the related workspace open.
- A filter applied, including selected-CAN-ID filtering.
- Recording enabled where the change affects capture/recording.

Record:
- Platform, Qt version, adapter/source, and commit.
- Approximate frame rate and capture duration.
- Whether the UI stayed responsive.
- Whether frame/recording counts look correct.
- CPU/memory observations or obvious regressions.

Do not commit private vehicle captures unless intentionally sanitized.