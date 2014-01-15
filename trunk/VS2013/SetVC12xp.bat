@echo off
@setLocal EnableExtensions EnableDelayedExpansion
@echo off

set "version=v120_xp"

set counter=0

cd ..\

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
