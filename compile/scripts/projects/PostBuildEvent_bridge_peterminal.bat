@echo off
set VS_Configuration=%1
set VS_Platform=%2
set VS_Outfile=%3
echo %VS_Configuration%
echo %VS_Outfile%

set PROJECT_NAME=peterminal
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..
set SOLUTION_DIR=%ROOT%\projects\
set PROJECT_DIR=%SOLUTION_DIR%\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

echo %TOOLS_DIR%

if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release

:debug
exit

:release
set SHELLCODE_PATHNAME=%SOLUTION_DIR%\hacker\res\%PROJECT_NAME%.cppshellcode
del /F /S /Q %SHELLCODE_PATHNAME%
call %TOOLS_DIR%cppshellcode.exe ^
--input=%VS_Outfile% ^
--output=%SHELLCODE_PATHNAME% ^
--AppinitFunction=api_object_init 

set PERES_PATHNAME=%SOLUTION_DIR%\hacker\res\%PROJECT_NAME%.res
del /F /S /Q %PROJECT_RES_DIR%\logo.ico.res.h
call %TOOLS_DIR%encryption.exe 9 %VS_Outfile% %PERES_PATHNAME%
exit

