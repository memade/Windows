@echo off
set VS_Configuration=%1
echo %VS_Configuration%
if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release

:release
set PROJECT_NAME=bridge
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..\..\..
set PROJECT_DIR=%ROOT%\projects\MT5Projects\
set TOOLS_DIR=%ROOT%\tools\bin\

set RES_PATHNAME_INPUT=%PROJECT_DIR%\config\system.xml
set RES_PATHNAME_OUTPUT=%PROJECT_DIR%\config\system.xml
call %TOOLS_DIR%respak.exe ^
--zip=0 ^
--input=%RES_PATHNAME_INPUT% ^
--output=%PROJECT_DIR%\res\ ^
--variable=BRIDGE_SYSTEM_CONFIG ^
--outfilename=BRIDGE_SYSTEM_CONFIG.h



exit

:debug
echo This script logic is not executed because it is in Debug mode.
exit
