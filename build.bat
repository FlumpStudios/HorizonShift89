@echo off
echo Building Horizon Shift 89...
echo.

set GBDK=C:\Projects\GB\gbdk
set PROJECT=C:\Projects\GB\HorizonShift89

"%GBDK%\bin\lcc" -Wa-l -Wl-m -Wl-j -Wm-yoA -Wm-yt0x1B -Wm-yn"HorizonShift89" -Iinclude -o build\HorizonShift89.gb src\main.c src\player.c src\enemy.c src\bullet.c src\collision.c src\utils.c

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful!
    echo ROM: %PROJECT%\build\HorizonShift89.gb
) else (
    echo.
    echo Build failed!
)

echo.
pause
