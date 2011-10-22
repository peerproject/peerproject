@echo off
md ..\New
SkinTranslate.exe ..\default-en.xml ..\New\default-en.pot
for %%i in ( ..\*.xml ) do SkinTranslate.exe ..\default-en.xml %%i ..\New\#.po
pause
for %%i in (..\New\*.po) do msgmerge.exe --no-escape --quiet --no-wrap --update --backup=off "%%i" ..\New\default-en.pot
for %%i in (..\New\*.po) do SkinTranslate.exe default-en.xml "%%i" ..\New\#.xml
:: Set Visual Studio path:
:: SkinExport.exe ..\..\..\PeerProject\Resource.h "$(VCInstallDir)\atlmfc\include\afxres.h" ..\..\..\PeerProject\PeerProject.rc ..\New\Skin.xml
pause