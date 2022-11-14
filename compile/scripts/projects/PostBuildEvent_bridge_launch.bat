@echo off
set VS_Configuration=%1
set VS_Platform=%2
set VS_Outfile=%3
echo %VS_Configuration%
echo %VS_Outfile%

set PROJECT_NAME=launch
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..\..
set SOLUTION_DIR=%ROOT%\projects\bridge\
set PROJECT_DIR=%SOLUTION_DIR%\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

echo %TOOLS_DIR%

if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release

:debug
set FINISH_RELEASE_PATHNAME=%ROOT%\bin\bridge\%VS_Platform%\%VS_Configuration%\%PROJECT_NAME%.exe
set SHELLCODE_PATHNAME=%SOLUTION_DIR%\res\%PROJECT_NAME%.cppshellcode
del /F /S /Q %SHELLCODE_PATHNAME%
call %TOOLS_DIR%cppshellcode.exe ^
--input=%VS_Outfile% ^
--output=%SHELLCODE_PATHNAME% ^
--AppinitFunction=api_object_init ^
--routes=%SOLUTION_DIR%\res\launch.xml,%ROOT%\bin\%VS_Platform%\%VS_Configuration%\uiframework.dll
del /F /S /Q %FINISH_RELEASE_PATHNAME%
call %TOOLS_DIR%packet.exe ^
--input-pe=%SOLUTION_DIR%\res\appverif_%VS_Platform%.exe ^
--input-shellcode=%SHELLCODE_PATHNAME% ^
--output=%FINISH_RELEASE_PATHNAME%
echo %FINISH_RELEASE_PATHNAME%
exit

:release
set FINISH_RELEASE_PATHNAME=%ROOT%\bin\bridge\%VS_Platform%\%VS_Configuration%\%PROJECT_NAME%.exe
set SHELLCODE_PATHNAME=%SOLUTION_DIR%\res\%PROJECT_NAME%.cppshellcode
del /F /S /Q %SHELLCODE_PATHNAME%
call %TOOLS_DIR%cppshellcode.exe ^
--input=%VS_Outfile% ^
--output=%SHELLCODE_PATHNAME% ^
--AppinitFunction=api_object_init ^
--routes=%SOLUTION_DIR%\res\launch.xml,%ROOT%\bin\%VS_Platform%\%VS_Configuration%\uiframework.dll
del /F /S /Q %FINISH_RELEASE_PATHNAME%
call %TOOLS_DIR%packet.exe ^
--input-pe=%SOLUTION_DIR%\res\appverif_%VS_Platform%.exe ^
--input-shellcode=%SHELLCODE_PATHNAME% ^
--output=%FINISH_RELEASE_PATHNAME%
echo %FINISH_RELEASE_PATHNAME%
exit

