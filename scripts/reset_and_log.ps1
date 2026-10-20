param(
    [string]$Port = "COM3",
    [int]$Seconds = 20
)

# Reset the board over RTS and capture the boot log. ASCII only.
$sp = New-Object System.IO.Ports.SerialPort $Port, 115200, "None", 8, "One"
$sp.ReadTimeout = 500
$sp.WriteTimeout = 500
$sp.Open()

$sp.RtsEnable = $true
$sp.DtrEnable = $false
Start-Sleep -Milliseconds 200
$sp.RtsEnable = $false
$sp.DtrEnable = $true

$sw = [System.Diagnostics.Stopwatch]::StartNew()
while ($sw.Elapsed.Seconds -lt $Seconds) {
    try { Write-Output ("[{0,5:N1}] {1}" -f $sw.Elapsed.TotalSeconds, $sp.ReadLine()) }
    catch [System.TimeoutException] {}
}
$sp.Close()
Write-Output "=== done ==="
