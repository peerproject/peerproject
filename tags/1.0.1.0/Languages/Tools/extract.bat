@echo off
SkinExport.exe ..\..\..\PeerProject\Resource.h "$(VCInstallDir)\atlmfc\include\afxres.h" ..\..\..\PeerProject\PeerProject.rc Skin.xml
pause