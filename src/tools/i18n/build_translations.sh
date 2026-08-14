#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"

# To add this tool in Qt Creator:
# Category: Linguist
# Name: Build Translations (lrelease)
# Executable: tools/i18n/build_translations.sh
# Working Directory: %{ActiveProject:Path}

echo "Generating .qm files..."
lrelease -verbose "$PROJECT_ROOT/SavvyLens.pro"
echo ".qm files generated."