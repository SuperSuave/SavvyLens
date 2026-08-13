# Bundled dependencies

This directory contains third-party source retained in the SavvyLens tree for build convenience.

## Current dependencies

| Dependency | Purpose |
|---|---|
| QCustomPlot | Plotting library used by graphical analysis features |
| SimpleCrypt | Lightweight obfuscation helper used for stored configuration values |

## Rules

- Preserve upstream license headers and attribution.
- Do not mix SavvyLens-specific changes directly into vendored files when an extension class can be used instead.
- Keep SavvyLens-owned QCustomPlot extensions in `src/plotting/`.
- Record the upstream source/version and local changes when updating a bundled dependency.