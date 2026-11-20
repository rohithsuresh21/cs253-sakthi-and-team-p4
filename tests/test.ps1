.\test_structA
.\test_structB
.\test_all
.\hard_cases
Get-Content tests\sample1.in | .\run --structure A
Get-Content tests\sample1.in | .\run --structure B
Get-Content tests\sample2.in | .\run --structure A
Get-Content tests\sample2.in | .\run --structure B
