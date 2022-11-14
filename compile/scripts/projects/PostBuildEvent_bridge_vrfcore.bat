@echo off
set VS_Configuration=%1
set VS_Outfile=%2
echo %VS_Configuration%
echo %VS_Outfile%

set PROJECT_NAME=vrfcore
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..
set SOLUTION_DIR=%ROOT%\projects\bridge\
set PROJECT_DIR=%SOLUTION_DIR%\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release

:debug
del /F /S /Q %PROJECT_DIR%\res\peloadd.res.h
call %TOOLS_DIR%encryption.exe 8 %VS_Outfile% %PROJECT_DIR%\res\peloadd.res
exit

:release
del /F /S /Q %PROJECT_DIR%\res\peload.res.h
call %TOOLS_DIR%encryption.exe 8 %VS_Outfile% %PROJECT_DIR%\res\peload.res
exit

