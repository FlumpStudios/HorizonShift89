@echo off
echo Cleaning build files...
del /Q build\*.gb 2>nul
del /Q build\*.map 2>nul
del /Q build\*.noi 2>nul
del /Q build\*.sym 2>nul
del /Q src\*.o 2>nul
del /Q res\*.o 2>nul
echo Done.
pause
