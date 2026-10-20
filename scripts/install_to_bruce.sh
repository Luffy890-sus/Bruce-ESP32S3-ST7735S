#!/usr/bin/env bash
# Copies the ESP32-S3-N16R8 + ST7735S board port into a Bruce Firmware checkout.
#
# Usage: ./scripts/install_to_bruce.sh [path/to/Bruce]
set -euo pipefail

BRUCE="${1:-../firmware}"
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

for probe in platformio.ini boards; do
    if [[ ! -e "$BRUCE/$probe" ]]; then
        echo "error: '$BRUCE' does not look like a Bruce Firmware checkout (missing $probe)" >&2
        exit 1
    fi
done

copy_into() {
    local src="$REPO_ROOT/$1"
    local dst_dir="$BRUCE/$2"
    mkdir -p "$dst_dir"
    cp -R "$src" "$dst_dir/"
    printf '  %s\n' "$dst_dir/$(basename "$src")"
}

echo "Installing board port into $BRUCE"
copy_into "boards/esp32s3-n16r8-st7735s.ini"               "boards"
copy_into "boards/_boards_json/esp32s3-n16r8-st7735s.json" "boards/_boards_json"
copy_into "boards/esp32s3-n16r8-st7735s"                   "boards"
copy_into "boards/esp32s3-n16r8-st7735s_board"             "boards"

cat <<EOF

Port installed. Next:
  cd $BRUCE
  pio run -e esp32s3-n16r8-st7735s
EOF
