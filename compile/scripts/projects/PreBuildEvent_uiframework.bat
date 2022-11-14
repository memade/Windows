@echo off

set PROJECT_NAME=uiframework
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..
set SOLUTION_DIR=%ROOT%\projects\
set PROJECT_DIR=%ROOT%\projects\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\
set PROJECT_RES_DIR=%ROOT%\projects\%PROJECT_NAME%\res\

del /F /S /Q %PROJECT_RES_DIR%\china.png.res.h
call %TOOLS_DIR%encryption.exe 9 %PROJECT_RES_DIR%china.png %PROJECT_RES_DIR%\china.png.res

del /F /S /Q %PROJECT_RES_DIR%\logo.ico.res.h
call %TOOLS_DIR%encryption.exe 9 %PROJECT_RES_DIR%logo.ico %PROJECT_RES_DIR%\logo.ico.res