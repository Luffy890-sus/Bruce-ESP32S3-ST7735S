param(
    [string]$Port = "COM3",
    [int]$Baud = 460800,
    [string]$Bin = "Bruce-esp32s3-n16r8-st7735s.bin",
    [int]$LogSeconds = 40
)

# Flash + capture boot log. ASCII only: PowerShell 5.1 mis-parses UTF-8 no-BOM.
$env:PYTHONIOENCODING = "utf-8"
$py = "$env:USERPROFILE\.platformio\penv\Scripts\python.exe"

Write-Output ">>> waiting for download mode on $Port ..."
& $py -m esptool --chip esp32s3 --port $Port --baud $Baud --connect-attempts -1 `
    write_flash 0x0 $Bin 2>&1 | ForEach-Object { "$_" }

if ($LASTEXITCODE -ne 0) {
    Write-Output ">>> FLASH FAILED (exit $LASTEXITCODE)"
    exit 1
}

Write-Output ">>> flashed OK, reading boot log for $LogSeconds s ..."
Start-Sleep -Seconds 3

for ($try = 0; $try -lt 8; $try++) {
    try {
        $sp = New-Object System.IO.Ports.SerialPort $Port, 115200, "None", 8, "One"
        $sp.ReadTimeout = 1000
        $sp.Open()
        $sw = [System.Diagnostics.Stopwatch]::StartNew()
        while ($sw.Elapsed.Seconds -lt $LogSeconds) {
            try { Write-Output $sp.ReadLine() } catch [System.TimeoutException] {}
        }
        $sp.Close()
        break
    } catch {
        Write-Output ">>> port busy, retry $($try+1)"
        Start-Sleep -Seconds 3
    }
}
Write-Output "=== done ==="
