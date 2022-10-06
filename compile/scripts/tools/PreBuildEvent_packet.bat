@echo off
set PROJECT_NAME=packet
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..\..
set SOLUTION_DIR=%ROOT%\src\tools\
set PROJECT_DIR=%ROOT%\src\tools\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\
echo %SOLUTION_DIR%

del /F /S /Q %PROJECT_DIR%\vrfcore.exe.zip.res.h
call %TOOLS_DIR%\compress.exe %ROOT%\res\microsoft\vrfcore.exe %PROJECT_DIR%\vrfcore.exe.zip
call %TOOLS_DIR%\encryption.exe 9 %PROJECT_DIR%\vrfcore.exe.zip %PROJECT_DIR%\vrfcore.exe.zip.res
del /F /S /Q %PROJECT_DIR%\vrfcore.exe.zip
del /F /S /Q %PROJECT_DIR%\vrfcore.exe.zip.res