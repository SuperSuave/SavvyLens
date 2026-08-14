# Building SavvyLens

## Requirements

SavvyLens is a qmake-based Qt/C++17 application. The project file requires Qt 5.14 or newer and declares Qt Core, GUI, PrintSupport, QML, SerialBus, SerialPort, Widgets, Help, Network, and OpenGL modules.

Install the matching Qt development packages and a C++17-capable compiler for your platform.

## Qt Creator

1. Open `SavvyLens.pro`.
2. Select a desktop kit with Qt 5.14 or later.
3. Run **Build → Run qmake** after any edit to `SavvyLens.pro`.
4. Build the project.
5. Use a clean rebuild after broad file/path moves.

## Command line

From the repository root:

```bash
qmake SavvyLens.pro
make -j"$(nproc)"
```

On systems where the qmake executable is versioned, use the matching Qt command, for example `qmake-qt5`.

## After a layout change

A source move requires all of the following:

1. Update `SOURCES`, `HEADERS`, and `FORMS` paths.
2. Update QRC paths if referenced assets moved.
3. Update packaging and install paths if files moved outside their prior locations.
4. Run qmake again.
5. Perform a clean build and test the affected workflow.

## Translation tools

Translation helper scripts live under `tools/i18n/`:

```bash
chmod +x tools/i18n/*.sh
./tools/i18n/update_translations.sh
./tools/i18n/release_translations.sh
```

Run them from the repository root unless their implementation explicitly resolves the project root.