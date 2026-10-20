<#
.SYNOPSIS
  Copies the ESP32-S3-N16R8 + ST7735S board port into a Bruce Firmware checkout.

.EXAMPLE
  .\scripts\install_to_bruce.ps1 -BrucePath ..\firmware
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [string] $BrucePath = "..\firmware"
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$bruce = (Resolve-Path -Path $BrucePath).Path

foreach ($probe in @("platformio.ini", "boards")) {
    if (-not (Test-Path (Join-Path $bruce $probe))) {
        throw "'$bruce' does not look like a Bruce Firmware checkout (missing $probe)."
    }
}

$items = @(
    @{ From = "boards/esp32s3-n16r8-st7735s.ini";                 To = "boards" },
    @{ From = "boards/_boards_json/esp32s3-n16r8-st7735s.json";   To = "boards/_boards_json" },
    @{ From = "boards/esp32s3-n16r8-st7735s";                     To = "boards" },
    @{ From = "boards/esp32s3-n16r8-st7735s_board";               To = "boards" }
)

foreach ($item in $items) {
    $source = Join-Path $repoRoot ($item.From -replace '/', '\')
    $destDir = Join-Path $bruce ($item.To -replace '/', '\')
    $dest = Join-Path $destDir (Split-Path -Leaf $source)

    if (-not (Test-Path $destDir)) {
        New-Item -ItemType Directory -Force -Path $destDir | Out-Null
    }

    Copy-Item -Path $source -Destination $destDir -Recurse -Force
    Write-Host ("  {0}" -f $dest)
}

Write-Host ""
Write-Host "Port installed. Next:" -ForegroundColor Green
Write-Host "  cd $bruce"
Write-Host "  pio run -e esp32s3-n16r8-st7735s"
