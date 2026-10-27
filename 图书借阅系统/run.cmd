@echo off
setlocal
cd /d "%~dp0"

if not exist "library_system.exe" (
    echo library_system.exe was not found.
    echo Run the VS Code build task or README build command first.
    pause
    exit /b 1
)

library_system.exe
set "exit_code=%errorlevel%"
pause
endlocal & exit /b %exit_code%
