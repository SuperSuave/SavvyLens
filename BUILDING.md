# Building on Debian Trixie

Debian 13 ("trixie") ships Qt **5.15.15**, but it **removed the Qt5 SerialBus
module**. The package `libqt5serialbus5-dev` no longer exists in trixie (only the
Qt6 `qt6-serialbus-dev` is packaged). SavvyLens hard-requires Qt5 SerialBus
(`QT += serialbus`, used by `QCanBusFrame` / `QCanBusDevice` throughout), so the
system Qt **cannot** build it and a plain `qmake && make` fails with:

```
Project ERROR: Unknown module(s) in QT: serialbus
```

The fix is to build against the official **Qt 5.15.2 SDK**, which bundles
`qtserialbus` (and its canbus plugins: socketcan, virtualcan, peakcan, …). No
root access is required.

## Quick start

```sh
./build-debian.sh
```

This downloads Qt 5.15.2 into `~/Qt`, patches it, and builds SavvyLens into
`./build/`. The resulting binary is `build/SavvyLens`.

```sh
./build/SavvyLens
```

### Options

| Option | Default | Description |
|---|---|---|
| `--qt-root <dir>` | `~/Qt` | Where to install the Qt SDK |
| `--build-dir <dir>` | `./build` | Output directory |
| `--jobs <n>` | `nproc` | Parallel compile jobs |
| `--prefix <dir>` | `/usr` | Install prefix baked into the binary |
| `--no-download` | off | Skip downloading Qt (use an existing SDK) |
| `--qmake <path>` | `~/Qt/5.15.2/gcc_64/bin/qmake` | Use a specific qmake |

Everything can also be set via environment variables (`QT_VERSION`,
`QT_RELEASE_DATE`, `QT_ROOT`, `QT_MIRROR`, `BUILD_DIR`, `JOBS`, `PREFIX`).

## What the script does

1. Downloads the Qt 5.15.2 per-module archives from `download.qt.io`:
   `qtbase`, `qtdeclarative` (QML), `qttools` (Help), `qtserialbus`,
   `qtserialport`, and `icu`.
2. Writes `bin/qt.conf` and patches `mkspecs/qconfig.pri` so qmake runs
   (the official binaries ship with an Enterprise license check that aborts
   with `Qt license file was not found!`).
3. Runs `qmake CONFIG+=release` + `make`.
4. Runs `lrelease` to generate the translation `.qm` files.

## Manual build (without the script)

```sh
BASE="https://download.qt.io/online/qtsdkrepository/linux_x64/desktop/qt5_5152/qt.qt5.5152.gcc_64"
mkdir -p ~/Qt

for m in qtbase qtdeclarative qttools qtserialbus qtserialport; do
  curl -sL -o "/tmp/${m}.7z" "${BASE}/5.15.2-0-202011130601${m}-Linux-RHEL_7_6-GCC-Linux-RHEL_7_6-X86_64.7z"
  7z x -y -o"$HOME/Qt" "/tmp/${m}.7z" >/dev/null
done
curl -sL -o /tmp/icu.7z "${BASE}/5.15.2-0-202011130601icu-linux-Rhel7.2-x64.7z"
7z x -y -o"$HOME/Qt" /tmp/icu.7z >/dev/null

# qmake from the official SDK aborts without this
Q="$HOME/Qt/5.15.2/gcc_64"
printf '[Paths]\nPrefix = ..\n' > "$Q/bin/qt.conf"
sed -i 's/Enterprise/OpenSource/; /^QT_LICHECK/d' "$Q/mkspecs/qconfig.pri"

# build
mkdir -p build && cd build
"$Q/bin/qmake" CONFIG+=release PREFIX=/usr ../SavvyLens.pro
make -j"$(nproc)"
"$Q/bin/lrelease" ../translations/*.ts
```

## Installing

For a system-wide install (requires `sudo`):

```sh
cd build
sudo make install          # installs to /usr/local by default (see --prefix)
```

Or use the desktop-shortcut helper (user-level, no sudo):

```sh
cd build
../src/packaging/linux/install.sh
```

## Requirements

- `curl`, `p7zip` (or `p7zip-full`), `g++`, `make` (a C++17 compiler).
- ~1 GB of free disk space for the Qt SDK.
- An x86_64 machine (the script downloads the `linux_x64` SDK).

## Troubleshooting

- **`Unknown module(s) in QT: serialbus`** — you used the system Qt, which has
  no Qt5 SerialBus on trixie. Use the script's downloaded Qt, or pass your own
  with `--qmake`.
- **`Qt license file was not found!`** — re-run the `qconfig.pri` patch above.
- **`7z: command not found`** — `sudo apt install p7zip-full`.
