#!/usr/bin/env bash
# Builds the esp32s3-n16r8-st7735s environment inside a Bruce Firmware checkout.
#
# Usage: ./scripts/build.sh [path/to/Bruce]
set -euo pipefail

BRUCE="${1:-../firmware}"
ENVIRONMENT="esp32s3-n16r8-st7735s"

cd "$BRUCE"

if [[ ! -f "boards/${ENVIRONMENT}.ini" ]]; then
    echo "error: boards/${ENVIRONMENT}.ini not found -- run scripts/install_to_bruce.sh first" >&2
    exit 1
fi

pio run -e "$ENVIRONMENT"

cat <<EOF

Artifacts in $BRUCE/.pio/build/${ENVIRONMENT}/:
  firmware.bin      -> flash at 0x10000
  bootloader.bin    -> flash at 0x0
  partitions.bin    -> flash at 0x8000

Factory image (flash at 0x0):
  $BRUCE/Bruce-${ENVIRONMENT}.bin
EOF
