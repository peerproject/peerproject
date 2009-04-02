@echo off
rem    This batch file restores PeerProject settings from previously saved
rem    files PeerProjectSave1.reg and PeerProjectSave2.reg to the registry

if not exist PeerProjectSave?.reg exit

echo REGEDIT4> "%TEMP%\PeerProjectDelOld.reg"
echo [-HKEY_CURRENT_USER\Software\PeerProject]>> "%TEMP%\PeerProjectDelOld.reg"
echo [-HKEY_LOCAL_MACHINE\Software\PeerProject]>> "%TEMP%\PeerProjectDelOld.reg"

start/wait regedit -s "%TEMP%\PeerProjectDelOld.reg"
del "%TEMP%\PeerProjectDelOld.reg" > nul

:import
start/wait regedit -s PeerProjectSave1.reg PeerProjectSave2.reg
