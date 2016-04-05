@echo off
setlocal

:: This script's absolute path.
set SCRIPT=%0

:: The directory containing this script's absolute path.
set SCRIPTPATH=%~dp0
set SCRIPTPATH=%SCRIPTPATH:~0,-1%

:: The directory to generate the visual studio project to.
set OUT_DIR=%SCRIPTPATH%\vs2010

:: Find cmake or die.
where /Q cmake
if %ERRORLEVEL$ EQU 0 (
    set CMAKE_CMD=cmake
) else (
    if exist "C:\Program Files (x86)\CMake\bin\cmake.exe" (
        SET CMAKE_CMD="C:\Program Files (x86)\CMake\bin\cmake.exe"
    )
)
if not defined CMAKE_CMD (
    echo "ERROR: Install cmake. If not installed to default location then add it's location to the path."

    endlocal
    echo Press any key to exit...
    pause > nul :: Pause to allow user to read results.
    exit /B %ERRORLEVEL%
)

:: Generate the visual studio project.
if not exist %OUT_DIR% (
    md "%OUT_DIR%"
)
pushd "%OUT_DIR%"
    :: Important to note that for the "Visual Studio XX XXXX <arch>" generator, leaving <arch>
    :: blank is implicitly Win32, and "Win64" for Win64.
    call %%CMAKE_CMD%% -G "Visual Studio 10 2010" -DBUILD_DEBUG_TARGETS=ON "%SCRIPTPATH%"
popd

endlocal
exit /B %ERRORLEVEL%
