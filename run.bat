@echo off
cd /d "%~dp0"

g++ -std=c++17 -O2 app\main.cpp -o program.exe

if errorlevel 1 exit /b 1

program.exe %*
