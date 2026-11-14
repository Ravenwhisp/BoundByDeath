@echo off
setlocal
cd /d "%~dp0" || exit /b 1

set "game=%CD%\Game"
for %%I in ("%CD%\dist") do set "dist=%%~fI"
for %%I in ("%dist%\BoundByDeath") do set "release=%%~fI"
if /I not "%release%"=="%dist%\BoundByDeath" goto failed

if not exist "%game%\BoundByDeath.exe" goto missing
if not exist "%game%\build.cfg" goto missing
if not exist "%game%\GameScripts.dll" goto missing
if not exist "%game%\Library\" goto missing
if not exist "%game%\Assets\Haptics\game_effects.json" goto missing
if not exist "%game%\*.cso" goto missing
where tar.exe >nul 2>&1 || goto missing

if /I "%~1"=="--validate" (
    echo Release inputs are present in Game.
    exit /b 0
)

if exist "%release%\" rmdir /s /q "%release%" || goto failed
mkdir "%release%\Shaders" || goto failed
mkdir "%release%\Assets\Haptics" || goto failed

copy /y "%game%\BoundByDeath.exe" "%release%\" >nul || goto failed
copy /y "%game%\build.cfg" "%release%\" >nul || goto failed
copy /y "%game%\*.dll" "%release%\" >nul || goto failed
copy /y "%game%\*.cso" "%release%\Shaders\" >nul || goto failed

robocopy "%game%\Library" "%release%\Library" /E /NFL /NDL /NJH /NJS /NP >nul
if errorlevel 8 goto failed
robocopy "%game%\Assets\Haptics" "%release%\Assets\Haptics" /E /NFL /NDL /NJH /NJS /NP >nul
if errorlevel 8 goto failed

set "temporaryZip=%dist%\BoundByDeath.new.zip"
if exist "%temporaryZip%" del /q "%temporaryZip%" || goto failed
tar.exe -a -cf "%temporaryZip%" -C "%dist%" BoundByDeath
if errorlevel 1 goto failed
move /y "%temporaryZip%" "%dist%\BoundByDeath.zip" >nul || goto failed

echo.
echo Release ready: "%dist%\BoundByDeath.zip"
pause
exit /b 0

:missing
echo Missing release input in "%game%" or tar.exe is unavailable.
:failed
echo Packaging failed. See the message above.
pause
exit /b 1
