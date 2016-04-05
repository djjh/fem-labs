@echo off
setlocal

:: This script's absolute path.
set SCRIPT=%0

:: The directory containing this script's absolute path.
set SCRIPTPATH=%~dp0
set SCRIPTPATH=%SCRIPTPATH:~0,-1%

:: The directory to generate the visual studio project to.
set OUT_DIR=%SCRIPTPATH%\vs2010

:: Remove the output, dep, repo, and build directories.
rd /s /q %OUT_DIR%
for /d %%G in ("%SCRIPTPATH%\dep\*") do for /d %%H in ("%%~G\*.git", "%%~G\*.build") do rd /s /q "%%~H"

endlocal
exit /B %ERRORLEVEL%