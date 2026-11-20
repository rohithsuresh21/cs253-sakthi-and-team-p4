$ErrorActionPreference = "Stop"

function Invoke-Gpp {
    param([string[]]$Arguments)
    & g++ @Arguments
    if ($LASTEXITCODE -ne 0) { throw "g++ failed with exit code $LASTEXITCODE" }
}

$flags = @("-std=c++17", "-O2", "-Wall", "-Wextra", "-Wpedantic")
Invoke-Gpp ($flags + @("app/main.cpp", "-o", "run.exe"))
Invoke-Gpp ($flags + @("bench/benchmark.cpp", "-o", "benchmark.exe"))
Invoke-Gpp ($flags + @("bench/adversarial_cases.cpp", "bench/test_adversarial.cpp", "-o", "test_adversarial.exe"))
Invoke-Gpp ($flags + @("bench/adversarial_cases.cpp", "bench/crossover_analysis.cpp", "-o", "crossover.exe"))
Invoke-Gpp ($flags + @("bench/crossover_report.cpp", "-o", "crossover_report.exe"))

& .\test_adversarial.exe
if ($LASTEXITCODE -ne 0) { throw "Adversarial tests failed" }
& .\benchmark.exe
if ($LASTEXITCODE -ne 0) { throw "Benchmark failed" }
& .\crossover.exe 5 results/crossover.csv
if ($LASTEXITCODE -ne 0) { throw "Crossover analysis failed" }
& .\crossover_report.exe results/crossover.csv
if ($LASTEXITCODE -ne 0) { throw "Crossover report failed" }
python bench/plot.py
if ($LASTEXITCODE -ne 0) { throw "Plot generation failed" }
