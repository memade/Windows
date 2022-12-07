@echo off
set VS_Configuration=%1
echo %VS_Configuration%
if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release

:release
set PROJECT_NAME=hacker
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..
set SOLUTION_DIR=%ROOT%\projects\
set PROJECT_DIR=%ROOT%\projects\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

set PE_RES_FILE_NAME=EventAggregation.dll
set PE_PATHNAME=%ROOT%\bin\x64\Release\%PE_RES_FILE_NAME%
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res.h
call %TOOLS_DIR%encryption.exe 9 %PE_PATHNAME% %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res

set PE_RES_FILE_NAME=RTCore32.sys
set PE_PATHNAME=%PROJECT_DIR%\devices\MicroStarRTCore\res\%PE_RES_FILE_NAME%
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res.h
call %TOOLS_DIR%encryption.exe 9 %PE_PATHNAME% %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res

set PE_RES_FILE_NAME=RTCore64.sys
set PE_PATHNAME=%PROJECT_DIR%\devices\MicroStarRTCore\res\%PE_RES_FILE_NAME%
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res.h
call %TOOLS_DIR%encryption.exe 9 %PE_PATHNAME% %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res

set PE_RES_FILE_NAME=PROCEXP152.SYS
set PE_PATHNAME=%PROJECT_DIR%\devices\MicrosoftProcexp152\res\%PE_RES_FILE_NAME%
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res.h
call %TOOLS_DIR%encryption.exe 9 %PE_PATHNAME% %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.res


exit

:debug
echo This script logic is not executed because it is in Debug mode.
exit
