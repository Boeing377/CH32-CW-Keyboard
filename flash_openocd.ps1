$ErrorActionPreference = "Stop"

$OpenOcdExe = "C:\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\OpenOCD\OpenOCD\bin\openocd.exe"
$OpenOcdCfg = "C:\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\OpenOCD\OpenOCD\bin\wch-riscv.cfg"
$Firmware = Join-Path $PSScriptRoot "build-cmake\CH32V203USBKEY.elf"

if (-not (Test-Path $OpenOcdExe)) {
    throw "OpenOCD not found: $OpenOcdExe"
}

if (-not (Test-Path $OpenOcdCfg)) {
    throw "OpenOCD config not found: $OpenOcdCfg"
}

if (-not (Test-Path $Firmware)) {
    throw "Firmware not found: $Firmware. Build the project first with CMake."
}

& $OpenOcdExe -f $OpenOcdCfg -c "program `"$Firmware`" verify reset exit"
