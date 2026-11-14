@echo off
setlocal
cd /d "%~dp0" || exit /b 1

set "game=%CD%\Game"
for %%I in ("%CD%\dist") do set "dist=%%~fI"
for %%I in ("%dist%\BoundByDeath") do set "release=%%~fI"
for %%I in ("%dist%\.building") do set "stagingRoot=%%~fI"
for %%I in ("%stagingRoot%\BoundByDeath") do set "staging=%%~fI"
if /I not "%release%"=="%dist%\BoundByDeath" goto failed
if /I not "%staging%"=="%dist%\.building\BoundByDeath" goto failed

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

echo Copying the existing Game files into a temporary release folder.
echo No Library assets are being generated or rebuilt.
echo The previous release stays available during the copy.
echo Wait for "Release ready" before opening the new release.
if exist "%stagingRoot%\" rmdir /s /q "%stagingRoot%" || goto failed
mkdir "%staging%\Shaders" || goto failed
mkdir "%staging%\Assets\Haptics" || goto failed

copy /y "%game%\BoundByDeath.exe" "%staging%\" >nul || goto failed
copy /y "%game%\build.cfg" "%staging%\" >nul || goto failed
for /f "delims=" %%F in ('dir /b /a-d "%game%\*.dll" ^| findstr /V /B /I /C:"GameScripts_runtime_"') do (
    copy /y "%game%\%%F" "%staging%\" >nul || goto failed
)
copy /y "%game%\*.cso" "%staging%\Shaders\" >nul || goto failed

echo Copying Library. This can take several minutes.
robocopy "%game%\Library" "%staging%\Library" /E /R:2 /W:2 /NFL /NDL /NJH /NP
if errorlevel 8 goto failed
robocopy "%game%\Assets\Haptics" "%staging%\Assets\Haptics" /E /R:2 /W:2 /NFL /NDL /NJH /NP
if errorlevel 8 goto failed

set "temporaryZip=%dist%\BoundByDeath.new.zip"
if exist "%temporaryZip%" del /q "%temporaryZip%" || goto failed
echo Compressing the release. This can take several minutes.
tar.exe -a -cf "%temporaryZip%" -C "%stagingRoot%" BoundByDeath
if errorlevel 1 goto failed
if exist "%release%\" rmdir /s /q "%release%" || goto failed
move "%staging%" "%release%" >nul || goto failed
rd "%stagingRoot%" >nul 2>&1
move /y "%temporaryZip%" "%dist%\BoundByDeath.zip" >nul || goto failed

echo.
echo Release ready: "%dist%\BoundByDeath.zip"
pause
exit /b 0

:missing
echo Missing release input in "%game%" or tar.exe is unavailable.
:failed
echo Packaging failed. See the message above. The new release was not completed.
pause
exit /b 1
