@echo off
echo Starting Palette...

REM Start backend silently in background using VBScript
echo Set WshShell = CreateObject("WScript.Shell") > "%TEMP%\StartPaletteBackend.vbs"
echo WshShell.Run """%~dp0..\..\out\build\x64-Release\Release\Palette.exe""", 0, False >> "%TEMP%\StartPaletteBackend.vbs"
cscript //nologo "%TEMP%\StartPaletteBackend.vbs"
del "%TEMP%\StartPaletteBackend.vbs"

timeout /t 2 /nobreak > nul

REM Launch frontend UI
start shell:AppsFolder\a8f14be3-5f61-455f-9bd2-a32a27e5e2d0_7e5ceqhj86vwj!App

echo.
echo Palette is now running!
echo - Backend daemon is running silently in the background
echo - Frontend UI should open momentarily
