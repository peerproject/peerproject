@echo off
for %%i in ( ..\*.xml ) do (
	SkinTranslate.exe ..\default-en.xml %%i ..\Poedit\#.po
	pause )