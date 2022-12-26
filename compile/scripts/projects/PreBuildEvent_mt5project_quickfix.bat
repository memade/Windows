@echo off
set VS_Configuration=%1
echo %VS_Configuration%
if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release

:release
set PROJECT_NAME=quickfix
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..\..\..
set PROJECT_DIR=%ROOT%\projects\MT5Projects\src\shared\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

echo %TOOLS_DIR%

set PE_RES_FILE_NAME=FIX40
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

set PE_RES_FILE_NAME=FIX41
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

set PE_RES_FILE_NAME=FIX42
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

set PE_RES_FILE_NAME=FIX43
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

set PE_RES_FILE_NAME=FIX44
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

set PE_RES_FILE_NAME=FIX50
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

set PE_RES_FILE_NAME=FIX50SP1
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

set PE_RES_FILE_NAME=FIX50SP2
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

set PE_RES_FILE_NAME=FIXT11
set PE_PATHNAME=%PROJECT_DIR%\spec\%PE_RES_FILE_NAME%.xml
call %TOOLS_DIR%respak.exe --zip=1 --input=%PE_PATHNAME% --output=%PROJECT_DIR%\res\ --variable=%PE_RES_FILE_NAME%_RES --outfilename=%PE_RES_FILE_NAME%.h

exit

:debug
echo This script logic is not executed because it is in Debug mode.
exit
