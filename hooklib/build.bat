@echo off
setlocal enabledelayedexpansion

REM Navigate to the directory where the script is located
cd /d "%~dp0"

REM Set the path to ndk-build using LocalAppData
set NDK_BUILD_CMD=%LocalAppData%\Android\Sdk\ndk\23.2.8568313\ndk-build.cmd

REM Step 1: Execute ndk-build command with -j8
echo Running ndk-build...
call "%NDK_BUILD_CMD%" -j8
if errorlevel 1 (
    echo ndk-build failed!
    exit /b 1
)

echo Build script executed successfully.
endlocal
exit /b 0
