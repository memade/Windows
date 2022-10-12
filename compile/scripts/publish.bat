@echo off
set PROJECT_NAME=publish
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..
set PROJECT_DIR=%ROOT%\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

set INCLUDE_DIR_ORIGIN=%ROOT%\include
set LIBRARYS_DIR_ORIGIN=%ROOT%\librarys
set INCLUDE_DIR_MOVETO=%PROJECT_DIR%\windows\include
set LIBRARYS_DIR_MOVETO=%PROJECT_DIR%\windows\librarys

cd %DISK%
cd %PROJECT_DIR%

mkdir %INCLUDE_DIR_MOVETO%
mkdir %LIBRARYS_DIR_MOVETO%

xcopy %INCLUDE_DIR_ORIGIN%\* %INCLUDE_DIR_MOVETO%\* /O /X /E /H /K /Y /R /C /S
xcopy %LIBRARYS_DIR_ORIGIN%\* %LIBRARYS_DIR_MOVETO%\* /O /X /E /H /K /Y /R /C /S
rem for /f %%k in ('dir %LIBRARYS_DIR_ORIGIN%\*.lib /s /b') do xcopy %%k %LIBRARYS_DIR_MOVETO% /O /X /H /K /Y /R /C /A

cd %DISK%
cd %CURRENT%



