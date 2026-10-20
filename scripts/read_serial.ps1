param(
    [string]$Port = "COM3",
    [int]$Baud = 115200,
    [int]$Seconds = 25
)

# Reads bootloader/firmware log from COM port. ASCII only: PowerShell 5.1
# mis-parses UTF-8 no-BOM scripts.
$sp = New-Object System.IO.Ports.SerialPort $Port, $Baud, "None", 8, "One"
$sp.ReadTimeout = 500
$sp.DtrEnable = $false
$sp.RtsEnable = $false
$sp.Open()

$sw = [System.Diagnostics.Stopwatch]::StartNew()
while ($sw.Elapsed.Seconds -lt $Seconds) {
    try {
        $line = $sp.ReadLine()
        Write-Output ("[{0,5:N1}s] {1}" -f $sw.Elapsed.TotalSeconds, $line)
    } catch [System.TimeoutException] {
        # no data
    }
}
$sp.Close()
Write-Output "=== done ==="
