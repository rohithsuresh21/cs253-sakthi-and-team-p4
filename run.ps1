$ErrorActionPreference = "Stop"

Set-Location $PSScriptRoot

g++ -std=c++17 -O2 app/main.cpp -o program.exe

if ($LASTEXITCODE -ne 0) {
    Write-Error "Compilation failed."
    exit 1
}

& ".\program.exe" @args

exit $LASTEXITCODE
