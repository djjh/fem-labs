@echo off
setlocal

:: This script's absolute path.
set SCRIPT=%0

:: The directory containing this script's absolute path.
set SCRIPTPATH=%~dp0
set SCRIPTPATH=%SCRIPTPATH:~0,-1%

:: The directory to generate the visual studio project to.
set OUT_DIR=%SCRIPTPATH%\vs2015

:: Find msbuild or die.
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"

:: Build it.
pushd "%OUT_DIR%"
    call MSBuild ALL_BUILD.vcxproj
popd

:: Run it.
call %OUT_DIR%\Debug\arc.exe

endlocal
exit /B %ERRORLEVEL%