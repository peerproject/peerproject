@echo off
rem    This batch file saves PeerProject settings from the registry
rem    to files PeerProjectSave1.reg and PeerProjectSave2.reg

regedit /ea PeerProjectSave1.reg HKEY_CURRENT_USER\Software\PeerProject
regedit /ea PeerProjectSave2.reg HKEY_LOCAL_MACHINE\Software\PeerProject
