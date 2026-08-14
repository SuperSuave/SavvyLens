# Development workflow

## Branches

Use this simple branch relationship:

```text
master
└── pre-release
    ├── feature/<focused-change>
    ├── fix/<focused-bug>
    └── chore/<maintenance-task>
```

- `master` contains stable, release-ready work.
- `pre-release` is the integration branch for the next release.
- `feature/` branches contain a user-facing capability.
- `fix/` branches contain a focused correction.
- `chore/` branches contain maintenance such as source layout, documentation, or build cleanup.

For source organization work:

```bash
git switch pre-release
git pull --ff-only origin pre-release
git switch -c chore/repo-layout
```

## Commit guidance

- Keep source moves and behavior changes separate where practical.
- Make module-level moves in buildable commits.
- Use `git mv` for file moves.
- Review `git diff --summary` to ensure Git recognizes renames.
- Do not mix a large refactor into a path-only move unless it is required to restore compilation.

## Required checks

After modifying `SavvyLens.pro`:

```bash
qmake SavvyLens.pro
make -j"$(nproc)"
```

Before committing a broad move:

```bash
git diff --check
git status --short
git diff --summary
```

## Smoke tests

Test the modules touched by a change. For broad layout changes, verify at least:

- Application starts and the main frame table receives traffic.
- A CAN connection can be configured.
- A DBC can be loaded and Signal Viewer can decode a selected signal.
- A log can be loaded and saved.
- Playback opens and can prepare a sequence.
- Frame Sender opens without enabling unintended traffic.
- Scripting opens, loads a template, validates, and safely runs a non-transmitting script.
- MCP settings and server startup still work.
- F1 opens runtime help from the expected installed/development location.