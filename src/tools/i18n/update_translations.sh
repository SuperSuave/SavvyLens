#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

# To add this tool in Qt Creator:
# Category: Linguist
# Name: Update Translations (lupdate)
# Executable: lupdate
# Arguments: -project SavvyLens.pro
# Working Directory: %{ActiveProject:Path}

echo "Updating translation files..."

lupdate -verbose "${PROJECT_ROOT}/SavvyLens.pro"

echo "Translation files updated."