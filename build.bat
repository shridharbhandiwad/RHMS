@echo off
REM Build script for Radar Health Monitoring System (Windows)

setlocal

set BUILD_DIR=build
set BUILD_TYPE=Release
set CMAKE_GENERATOR=

:parse_args
if "%1"=="--debug" (
    set BUILD_TYPE=Debug
    shift
    goto parse_args
)
if "%1"=="--clean" (
    if exist %BUILD_DIR% rd /s /q %BUILD_DIR%
    echo Cleaned build directory
    shift
    goto parse_args
)

echo Building for Windows...

REM Create build directory
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Run CMake
echo Configuring with CMake...
cmake .. ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    exit /b 1
)

REM Build
echo Building...
cmake --build . --config %BUILD_TYPE% --parallel

if %ERRORLEVEL% neq 0 (
    echo Build failed
    exit /b 1
)

echo.
echo Build completed successfully!
echo Build type: %BUILD_TYPE%
echo Executable: %BUILD_DIR%\%BUILD_TYPE%\RadarHealthMonitor.exe

endlocal
