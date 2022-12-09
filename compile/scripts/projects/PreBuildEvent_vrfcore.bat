@echo off
set VS_Configuration=%1
echo %VS_Configuration%
if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release

:release
set PROJECT_NAME=vrfcore
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..\..
set SOLUTION_DIR=%ROOT%\projects\
set PROJECT_DIR=%ROOT%\projects\bridge\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

set PE_RES_FILE_NAME=main_logo.ico
set PE_PATHNAME=%PROJECT_DIR%\res\%PE_RES_FILE_NAME%
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=RES_ICO_MAIN --outfilename=main_logo_ico.h

set PE_RES_FILE_NAME=child_hacker_logo.ico
set PE_PATHNAME=%PROJECT_DIR%\res\%PE_RES_FILE_NAME%
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=RES_ICO_CHILD_HACKER --outfilename=child_hacker_logo_ico.h

exit

:debug
echo This script logic is not executed because it is in Debug mode.
exit
