@echo off
set PLATFORM=%1
set PROJECT_NAME=shellcode_boot
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..\..
set SOLUTION_DIR=%ROOT%\src\tools\
set PROJECT_DIR=%ROOT%\src\tools\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

echo Extract position independent shellcode

powershell.exe -NoProfile -ExecutionPolicy Bypass -File ^
%PROJECT_DIR%\res\PowerShell\Out-Shellcode.ps1 ^
%PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.exe ^
%PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.map ^
%PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.bin

del /F /S /Q %PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.bin.res.h
call %TOOLS_DIR%\encryption.exe 9 %PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.bin %PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.bin.res
del /F /S /Q %PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.bin
del /F /S /Q %PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.map
del /F /S /Q %PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.exe
del /F /S /Q %PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.bin.res

xcopy %PROJECT_DIR%\bin\%PROJECT_NAME%_%PLATFORM%.bin.res.h %ROOT%\include\ /S/Y/E/Q 
