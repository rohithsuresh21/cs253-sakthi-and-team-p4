@echo off
g++ -std=c++17 -O2 "%~dp0app\main.cpp" -o "%~dp0program.exe" 1>&2
if errorlevel 1 exit /b %errorlevel%

"%~dp0program.exe" %*
exit /b %errorlevel%
