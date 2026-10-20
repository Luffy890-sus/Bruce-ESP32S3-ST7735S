#!/usr/bin/env bash
# Flashes the esp32s3-n16r8-st7735s build.
#
# Usage: ./scripts/upload.sh [COM-port] [path/to/Bruce]
#   ./scripts/upload.sh /dev/ttyUSB0
#   ./scripts/upload.sh COM3 ../firmware
set -euo pipefail

PORT="${1:-}"
BRUCE="${2:-../firmware}"
ENVIRONMENT="esp32s3-n16r8-st7735s"

cat <<'EOF'
This board has NO auto-reset circuit. Before the flasher starts:

  1. hold BOOT  (GPIO0)
  2. press and release RESET (EN)
  3. release BOOT

EOF

cd "$BRUCE"

if [[ -n "$PORT" ]]; then
    pio run -e "$ENVIRONMENT" -t upload --upload-port "$PORT"
else
    pio run -e "$ENVIRONMENT" -t upload
fi

echo
echo "Logs:  pio device monitor -b 115200 ${PORT:+-p $PORT}"
