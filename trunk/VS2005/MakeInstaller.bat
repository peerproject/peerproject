set OLDDIR=%CD%
if exist "%ProgramFiles%\Inno Setup 5\ISCC.exe" (
	cd "..\Setup"
	"%ProgramFiles%\Inno Setup 5\ISCC.exe" Scripts\Main_VS2005.iss /dConfigurationName=%1 /dPlatformName=%2 /o"%OLDDIR%\Installer\"
) else (
	echo You must have Inno Setup 5 Quickstart Pack installed and located before creating the installer.
)
