<#
.SYNOPSIS
  Flashes the factory image, waiting as long as needed for the board to enter
  download mode. This board has no auto-reset circuit, so esptool must keep
  sending sync bytes while you toggle BOOT/RESET by hand.

.EXAMPLE
  .\scripts\flash_wait.ps1                       # COM3, firmware\*.factory.bin
  .\scripts\flash_wait.ps1 -Port COM3 -Baud 115200
#>
[CmdletBinding()]
param(
    [string] $Port = "COM3",
    [string] $Baud = "460800",
    [string] $Bin  = ""
)

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot

if (-not $Bin) {
    $Bin = (Get-ChildItem (Join-Path $repoRoot "firmware") -Filter "*.factory.bin" |
            Select-Object -First 1).FullName
}
if (-not $Bin -or -not (Test-Path $Bin)) { throw "factory image not found (firmware\*.factory.bin)" }

Write-Host "Port : $Port"
Write-Host "Image: $Bin"
Write-Host ""
Write-Host "Now put the board into download mode:" -ForegroundColor Yellow
Write-Host "  1. press and HOLD BOOT"
Write-Host "  2. press and release RESET"
Write-Host "  3. release BOOT"
Write-Host ""
Write-Host "esptool keeps retrying, so the timing does not matter." -ForegroundColor Yellow
Write-Host ""

# --connect-attempts -1 => send sync forever, so you can toggle the buttons
# at your own pace. esptool picks the chip up the moment it is in the ROM
# bootloader and then flashes.
& "$env:USERPROFILE\.platformio\penv\Scripts\python.exe" -m esptool `
    --chip esp32s3 --port $Port --baud $Baud --connect-attempts -1 `
    write_flash 0x0 $Bin

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "Flashed. The board resets automatically." -ForegroundColor Green
    Write-Host "Logs:  pio device monitor -p $Port -b 115200"
} else {
    Write-Host "Flash failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
