@echo off
md ..\New
for %%i in ( ..\*.xml ) do SkinTranslate.exe ..\default-en.xml %%i ..\New\#.po
pause