# Palette Demo
MSIX Package Quick Start

# Launch

## Option 1: Launch from Start Menu
Press **Windows Key** or click Start

Type "**PaletteUI**"

Click on the app to launch

## Option 2: Double-click the launcher
`frontend\PaletteUI\Launch-Palette.bat`

# MSIX Package Information

**Package Name** a8f14be3-5f61-455f-9bd2-a32a27e5e2d0

**Display Name** PaletteUI

**Version** 1.0.0.0

**Architecture** x64

**Publisher** CN=Cinnamon

**Package Location** `frontend\PaletteUI\AppPackages\PaletteUI.WinUI_1.0.0.0_x64_Test\`

# Rebuild & Reinstall

From the repository root:

```powershell
cd frontend\PaletteUI
msbuild PaletteUI.WinUI.csproj /t:Build /p:Configuration=Release /p:Platform=x64 /p:AppxPackageSigningEnabled=false /p:GenerateAppxPackageOnBuild=true /restore
```

Then reinstall:

```powershell
.\Install-Palette.bat
```

# Uninstall

To remove the app:

```powershell
Get-AppxPackage -Name "a8f14be3-5f61-455f-9bd2-a32a27e5e2d0" | Remove-AppxPackage
```
