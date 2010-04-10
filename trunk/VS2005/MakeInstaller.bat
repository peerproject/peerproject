set OLDDIR=%CD%
if exist "..\Installer\InnoSetup\ISCC.exe" (
	cd "..\Installer"
	"InnoSetup\ISCC.exe" Scripts\Main.VS2005.iss /dConfigurationName=%1 /dPlatformName=%2 /o"%OLDDIR%\Installer\"
) else if exist "%ProgramFiles%\Inno Setup 5\ISCC.exe" (
	cd "..\Installer"
	"%ProgramFiles%\Inno Setup 5\ISCC.exe" Scripts\Main.VS2005.iss /dConfigurationName=%1 /dPlatformName=%2 /o"%OLDDIR%\Installer\"
) else (
	echo Inno Setup is missing. Install Inno Setup 5 Quickstart Pack before creating the installer.
)
