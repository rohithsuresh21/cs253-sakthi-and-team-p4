$ErrorActionPreference = "Stop"

$source = Join-Path $PSScriptRoot "app/main.cpp"
$program = Join-Path $PSScriptRoot "program.exe"
& g++ -std=c++17 -O2 $source -o $program | ForEach-Object { [Console]::Error.WriteLine($_) }
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

if ($MyInvocation.ExpectingInput) {
    $input | & $program @args
} else {
    & $program @args
}

exit $LASTEXITCODE
