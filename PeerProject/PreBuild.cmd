rem %1 - $(ConfigurationName)
rem %2 - $(PlatformName)

copy /b /y "..\HashLib\%1 %2\HashLib.dll" "%1 %2\"

copy /b /y "..\Services\GeoIP\%1 %2\GeoIP.dll" "%1 %2\"
copy /b /y "..\Data\GeoIP.dat" "%1 %2\"

if exist "..\Services\SQLite\%1 %2\sqlite3.dll" goto sqlite1
copy /b /y "..\Services\SQLite\Release %2\sqlite3.dll" "%1 %2\"
goto sqlite2
:sqlite1
copy /b /y "..\Services\SQLite\%1 %2\sqlite3.dll" "%1 %2\"
:sqlite2

if exist "..\Services\zlib\%1 %2\zlibwapi.dll" goto zlib1
copy /b /y "..\Services\zlib\Release %2\zlibwapi.dll" "%1 %2\"
goto zlib2
:zlib1
copy /b /y "..\Services\zlib\%1 %2\zlibwapi.dll" "%1 %2\"
:zlib2

if exist "..\Services\BugTrap\%1 %2\BugTrap.dll" goto BugTrap1
copy /b /y "..\Services\BugTrap\Release %2\BugTrap.dll" "%1 %2\"
goto BugTrap2
:BugTrap1
copy /b /y "..\Services\BugTrap\%1 %2\BugTrap.dll" "%1 %2\"
:BugTrap2
if "%1" == "x64"   copy /b /y "..\Services\BugTrap\dbghelp-x64.dll" "%1 %2\dbghelp.dll"
if "%1" == "Win32" copy /b /y "..\Services\BugTrap\dbghelp.dll"     "%1 %2\dbghelp.dll"

cscript.exe //E:jscript //nologo Revision.js