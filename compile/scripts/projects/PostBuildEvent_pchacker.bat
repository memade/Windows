@echo off
set VS_Configuration=%1
set VS_Outfile=%2
echo %VS_Configuration%
echo %VS_Outfile%

set PROJECT_NAME=pchacker
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..
set SOLUTION_DIR=%ROOT%\projects\
set PROJECT_DIR=%ROOT%\projects\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

del /F /S /Q %PROJECT_DIR%\res\%PROJECT_NAME%.memadepe
call %TOOLS_DIR%cppshellcode.exe --zip=1 --no-shellcode=1 --input=%VS_Outfile% --output=%PROJECT_DIR%\res\%PROJECT_NAME%.memadepe

if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release


:debug
del /F /S /Q %PROJECT_DIR%\res\msvcr100d.dll
call %TOOLS_DIR%packet.exe ^
--input-pe=%PROJECT_DIR%\res\origin_msvcr100d.dll ^
--input-shellcode=%PROJECT_DIR%\res\%PROJECT_NAME%.memadepe ^
--output=%PROJECT_DIR%\res\msvcr100d.dll
exit

:release
del /F /S /Q %PROJECT_DIR%\res\msvcr100.dll
call %TOOLS_DIR%packet.exe ^
--input-pe=%PROJECT_DIR%\res\origin_msvcr100.dll ^
--input-shellcode=%PROJECT_DIR%\res\%PROJECT_NAME%.memadepe ^
--output=%PROJECT_DIR%\res\msvcr100.dll
exit


