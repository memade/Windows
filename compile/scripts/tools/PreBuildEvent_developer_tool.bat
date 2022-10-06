@echo off
set PROJECT_NAME=developer_tool
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..\..
set SOLUTION_DIR=%ROOT%\src\tools\
set PROJECT_DIR=%ROOT%\src\tools\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\
echo %SOLUTION_DIR%

del /F /S /Q %PROJECT_DIR%\res\help.txt.res.h
call %TOOLS_DIR%\encryption.exe 9 %PROJECT_DIR%\res\help.txt %PROJECT_DIR%\res\help.txt.res
del /F /S /Q %PROJECT_DIR%\res\help.txt.res