@echo off
echo Installing Palette
echo.
cd "%~dp0AppPackages\PaletteUI.WinUI_1.0.0.0_x64_Test"
powershell -Command "& {Add-AppxPackage -Path '.\PaletteUI.WinUI_1.0.0.0_x64.msix' -DependencyPath '.\Dependencies\x64\Microsoft.WindowsAppRuntime.1.8.msix'}"
echo.
echo Installation complete! You can now launch "PaletteUI" from your Start Menu.
pause
