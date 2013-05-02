
@echo off
@setLocal EnableExtensions
@echo off

set "oldline1=    <PlatformToolset>v110_xp</PlatformToolset>"
set "oldline2=    <PlatformToolset>v110</PlatformToolset>"
set "oldline3=    <PlatformToolset>v90</PlatformToolset>"

set counter=0

cd ..\

for /r %%n in (*.vcxproj) do (
  echo %%n
  set update=0

  for /f "delims=" %%l in (%%n) do (
           if "%%l"=="%oldline1%" (set /a update+=1 && set /a counter+=1 && echo     ^<PlatformToolset^>v100^</PlatformToolset^>>>%%n.temp
    ) else if "%%l"=="%oldline2%" (set /a update+=1 && set /a counter+=1 && echo     ^<PlatformToolset^>v100^</PlatformToolset^>>>%%n.temp
    ) else if "%%l"=="%oldline3%" (set /a update+=1 && set /a counter+=1 && echo     ^<PlatformToolset^>v100^</PlatformToolset^>>>%%n.temp
    ) else (echo %%l>>%%n.temp)
  )

@setLocal EnableExtensions EnableDelayedExpansion
  if !update! neq 0 (move /y %%n.temp %%n
  ) else (del %%n.temp && echo         No change.)
endlocal
)

echo.
echo %counter% changes to PlatformToolset v100.
echo.
endlocal
pause
