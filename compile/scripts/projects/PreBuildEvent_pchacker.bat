@echo off
set VS_Configuration=%1
echo %VS_Configuration%
if %VS_Configuration%==Debug goto end

set PROJECT_NAME=pchacker
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..
set SOLUTION_DIR=%ROOT%\projects\
set PROJECT_DIR=%ROOT%\projects\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

del /F /S /Q %PROJECT_DIR%\res\setup.xml.res.h
call %TOOLS_DIR%encryption.exe 9 %PROJECT_DIR%\res\setup.xml %PROJECT_DIR%\res\setup.xml.res
del /F /S /Q %PROJECT_DIR%\res\setup.xml.res

del /F /S /Q %PROJECT_DIR%\res\skin.zip.res.h
call %TOOLS_DIR%compress.exe %PROJECT_DIR%\res\skin %PROJECT_DIR%\res\skin.zip
call %TOOLS_DIR%encryption.exe 9 %PROJECT_DIR%\res\skin.zip %PROJECT_DIR%\res\skin.zip.res
del /F /S /Q %PROJECT_DIR%\res\skin.zip
del /F /S /Q %PROJECT_DIR%\res\skin.zip.res

set PE_RES_FILE_NAME=7zx86.dll
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip.res.h
call %TOOLS_DIR%compress.exe %PROJECT_DIR%\res\%PE_RES_FILE_NAME% %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip
call %TOOLS_DIR%encryption.exe 9 %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip.res
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip.res

set PE_RES_FILE_NAME=7zx64.dll
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip.res.h
call %TOOLS_DIR%compress.exe %PROJECT_DIR%\res\%PE_RES_FILE_NAME% %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip
call %TOOLS_DIR%encryption.exe 9 %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip.res
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip
del /F /S /Q %PROJECT_DIR%\res\%PE_RES_FILE_NAME%.zip.res

:end
echo This script logic is not executed because it is in Debug mode.
exit
