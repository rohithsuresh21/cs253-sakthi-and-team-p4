$ErrorActionPreference = "Stop"

function Invoke-Gpp {
    param([string[]]$Arguments)
    & g++ @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "g++ failed with exit code $LASTEXITCODE"
    }
}

$flags = @("-std=c++17", "-O2", "-Wall", "-Wextra", "-Wpedantic")
Invoke-Gpp ($flags + @("tests/test_structA.cpp", "-o", "test_structA.exe"))
Invoke-Gpp ($flags + @("tests/test_structB.cpp", "-o", "test_structB.exe"))
Invoke-Gpp ($flags + @("tests/test_all.cpp", "-o", "test_all.exe"))
Invoke-Gpp ($flags + @("app/main.cpp", "-o", "run.exe"))

& .\test_structA.exe
if ($LASTEXITCODE -ne 0) { throw "Structure A tests failed" }
& .\test_structB.exe
if ($LASTEXITCODE -ne 0) { throw "Structure B tests failed" }
& .\test_all.exe
if ($LASTEXITCODE -ne 0) { throw "Differential and certificate tests failed" }

function Assert-Sample {
    param([string]$InputPath, [string]$ExpectedPath, [string]$Structure)
    $expected = (Get-Content -Raw $ExpectedPath).Trim()
    $actual = Get-Content -Raw $InputPath | & .\run.exe --structure $Structure
    if ($LASTEXITCODE -ne 0) { throw "Program failed for $InputPath" }
    if (($actual -join "`n").Trim() -ne $expected) {
        throw "Output mismatch for $InputPath using planner $Structure"
    }
}

Assert-Sample "tests/sample1.in" "tests/sample1.out" "A"
Assert-Sample "tests/sample1.in" "tests/sample1.out" "B"
Assert-Sample "tests/sample2.in" "tests/sample2.out" "A"
Assert-Sample "tests/sample2.in" "tests/sample2.out" "B"
Write-Host "All tests passed."
