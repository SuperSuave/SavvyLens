#!/usr/bin/env bash
#
# build-debian.sh — build SavvyLens on Debian (including Debian 13 "trixie").
#
# Debian trixie ships Qt 5.15.15 but DROPPED the Qt5 SerialBus module
# (libqt5serialbus5-dev no longer exists; only Qt6 serialbus is packaged).
# SavvyLens hard-requires Qt5 SerialBus, so the system Qt cannot build it.
#
# This script downloads the official Qt 5.15.2 SDK (which bundles
# qtserialbus) into a user-local prefix and builds SavvyLens against it.
# No root/sudo is required.
#
# Usage:
#   ./build-debian.sh [--help]
#
# Options:
#   --help          Show this help and exit
#   --no-download   Skip downloading Qt (assume it is already installed)
#   --qmake <path>  Use a specific qmake instead of the downloaded one
#   --qt-root <dir> Root directory for the Qt SDK (default: $HOME/Qt)
#   --build-dir <dir> Output build directory (default: ./build)
#   --jobs <n>      Number of parallel compile jobs (default: nproc)
#   --prefix <dir>  Install prefix baked into the binary (default: /usr)
#
# Environment variables can also be used: QT_VERSION, QT_RELEASE_DATE,
# QT_ROOT, QT_MIRROR, BUILD_DIR, JOBS, PREFIX.
#
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

QT_VERSION="${QT_VERSION:-5.15.2}"
QT_RELEASE_DATE="${QT_RELEASE_DATE:-202011130601}"
QT_ROOT="${QT_ROOT:-$HOME/Qt}"
QT_MIRROR="${QT_MIRROR:-https://download.qt.io/online/qtsdkrepository}"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"
PREFIX="${PREFIX:-/usr}"

# Qt 5.15.2 splits the SDK into per-module archives. qtbase already bundles
# core/gui/widgets/network/opengl/printsupport/sql. The rest cover the
# remaining modules SavvyLens lists in its .pro (qml, help, serialbus,
# serialport).
QT_MODULES=(qtbase qtdeclarative qttools qtserialbus qtserialport)

VERSION_NUM="${QT_VERSION//./}"                              # 5.15.2 -> 5152
QTDIR="${QT_ROOT}/${QT_VERSION}/gcc_64"
QMAKE="${QTDIR}/bin/qmake"
BASE_URL="${QT_MIRROR}/linux_x64/desktop/qt5_${VERSION_NUM}/qt.qt5.${VERSION_NUM}.gcc_64"

info() { printf '\033[1;34m==>\033[0m %s\n' "$*"; }
die()  { printf '\033[1;31merror:\033[0m %s\n' "$*" >&2; exit 1; }

need_cmd() { command -v "$1" >/dev/null 2>&1 || die "missing required tool: $1"; }

find_7z() {
    for c in 7z 7za 7zr; do
        command -v "$c" >/dev/null 2>&1 && { echo "$c"; return 0; }
    done
    die "missing required tool: 7z (install the 'p7zip-full' package)"
}

usage() {
    sed -n '2,26p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
}

fix_qt() {
    # The official 5.15.2 binaries ship without a usable qt.conf and with an
    # "Enterprise" license check that makes qmake abort with:
    #   "Qt license file was not found!"
    info "Patching qt.conf + license check in ${QTDIR}"
    printf '[Paths]\nPrefix = ..\n' > "${QTDIR}/bin/qt.conf"
    local qconfig="${QTDIR}/mkspecs/qconfig.pri"
    sed -i.bak \
        -e 's/^QT_EDITION = Enterprise$/QT_EDITION = OpenSource/' \
        -e '/^QT_LICHECK/d' \
        "${qconfig}"
    rm -f "${qconfig}.bak"
}

download_qt() {
    need_cmd curl
    local sevenz
    sevenz="$(find_7z)"

    info "Downloading Qt ${QT_VERSION} SDK into ${QT_ROOT}"
    mkdir -p "${QT_ROOT}"
    local dl
    dl="$(mktemp -d)"
    trap 'rm -rf "${dl}"' RETURN

    local m file
    for m in "${QT_MODULES[@]}"; do
        file="${QT_VERSION}-0-${QT_RELEASE_DATE}${m}-Linux-RHEL_7_6-GCC-Linux-RHEL_7_6-X86_64.7z"
        info "Downloading ${m}..."
        curl -fL --retry 3 -o "${dl}/${file}" "${BASE_URL}/${file}"
        "${sevenz}" x -y -o"${QT_ROOT}" "${dl}/${file}" >/dev/null
    done

    file="${QT_VERSION}-0-${QT_RELEASE_DATE}icu-linux-Rhel7.2-x64.7z"
    info "Downloading icu..."
    curl -fL --retry 3 -o "${dl}/${file}" "${BASE_URL}/${file}"
    "${sevenz}" x -y -o"${QT_ROOT}" "${dl}/${file}" >/dev/null

    fix_qt
}

build() {
    need_cmd make
    need_cmd g++

    info "Running qmake (PREFIX=${PREFIX})"
    mkdir -p "${BUILD_DIR}"
    ( cd "${BUILD_DIR}" && "${QMAKE}" CONFIG+=release PREFIX="${PREFIX}" \
        "${ROOT_DIR}/SavvyLens.pro" )

    info "Compiling with ${JOBS} job(s)"
    make -C "${BUILD_DIR}" -j"${JOBS}"

    info "Generating translations"
    "${QTDIR}/bin/lrelease" "${ROOT_DIR}"/translations/*.ts

    info "Build complete: ${BUILD_DIR}/SavvyLens"
}

# ---------------------------------------------------------------- arguments

NO_DOWNLOAD=0
while [[ $# -gt 0 ]]; do
    case "$1" in
        --help|-h) usage; exit 0 ;;
        --no-download) NO_DOWNLOAD=1 ;;
        --qmake) QMAKE="$2"; shift ;;
        --qt-root) QT_ROOT="$2"; QTDIR="${QT_ROOT}/${QT_VERSION}/gcc_64"; QMAKE="${QTDIR}/bin/qmake"; shift ;;
        --build-dir) BUILD_DIR="$2"; shift ;;
        --jobs) JOBS="$2"; shift ;;
        --prefix) PREFIX="$2"; shift ;;
        *) die "unknown option: $1 (try --help)" ;;
    esac
    shift
done

# ---------------------------------------------------------------- main

case "$(uname -m)" in
    x86_64|amd64) ;;
    *) die "this script only downloads the x86_64 Qt SDK; use --qmake with your own Qt" ;;
esac

if [[ "${NO_DOWNLOAD}" -eq 1 ]]; then
    [[ -x "${QMAKE}" ]] || die "qmake not found at ${QMAKE} (and --no-download was given)"
else
    if [[ -x "${QMAKE}" && -f "${QTDIR}/mkspecs/modules/qt_lib_serialbus.pri" ]]; then
        info "Qt ${QT_VERSION} already present in ${QTDIR}; skipping download"
        fix_qt
    else
        download_qt
    fi
fi

build
