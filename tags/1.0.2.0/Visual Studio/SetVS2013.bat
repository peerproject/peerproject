﻿@echo off
@setLocal EnableExtensions EnableDelayedExpansion
@echo off

set "header1=Microsoft Visual Studio Solution File, Format Version 12.00"
set "header2=# Visual Studio 2013"
set "version=v120_xp"

echo %header1%>>PeerProject.sln.temp
echo %header2%>>PeerProject.sln.temp

for /f "skip=2 delims=*" %%a in (PeerProject.sln) do (
echo %%a>>PeerProject.sln.temp
)
xcopy PeerProject.sln.temp PeerProject.sln /y
del PeerProject.sln.temp /f /q

cd ..\

set counter=0

for /r %%n in (*.vcxproj) do (
  echo %%n
  set update=0

  for /f "delims=" %%l in (%%n) do (
    set "linetest=%%l"
    if "!linetest:~5,15!"=="PlatformToolset" (
      if not "!linetest:~21,7!"=="%version%" (set /a update+=1 && set /a counter+=1) && echo     ^<PlatformToolset^>%version%^</PlatformToolset^>>>%%n.temp
    ) else (setLocal DisableDelayedExpansion && echo %%l>>%%n.temp&& endlocal)
  )

  if !update! neq 0 (move /y %%n.temp %%n
  ) else (del %%n.temp && echo         No change.)
)

echo.
echo %counter% changes to PlatformToolset %version%.
echo.
endlocal
pause
