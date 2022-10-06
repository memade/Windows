@echo off
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