@echo off
set VS_Configuration=%1
echo %VS_Configuration%

set PROJECT_NAME=pchacker
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%\..\..
set SOLUTION_DIR=%ROOT%\projects\
set PROJECT_DIR=%ROOT%\projects\%PROJECT_NAME%\
set TOOLS_DIR=%ROOT%\tools\bin\

xcopy /Y %ROOT%\include\stdc++.h D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\include\
xcopy /Y %ROOT%\include\pchacker.hpp D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\include\
xcopy /Y %ROOT%\include\container.hpp D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\include\

xcopy /Y %ROOT%\include\stdc++.h D:\__Code.Aliyun.Com__\CardPackage\CardPackage\include\
xcopy /Y %ROOT%\include\pchacker.hpp D:\__Code.Aliyun.Com__\CardPackage\CardPackage\include\
xcopy /Y %ROOT%\include\container.hpp D:\__Code.Aliyun.Com__\CardPackage\CardPackage\include\

if %VS_Configuration%==Debug goto debug
if %VS_Configuration%==Release goto release


:debug
echo F|xcopy /S /I /Q /Y /F %PROJECT_DIR%\res\msvcr100d.dll D:\__Code.Aliyun.Com__\CardPackage\bin\win32\debug\msvcr100.dll
xcopy /Y %PROJECT_DIR%\res\peloadd.res.h D:\__Code.Aliyun.Com__\CardPackage\CardPackage\include\
echo F|xcopy /S /I /Q /Y /F %PROJECT_DIR%\res\msvcr100d.dll D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\bin\Debug\msvcr100.dll
xcopy /Y %PROJECT_DIR%\res\peloadd.res.h D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\include\
echo F|xcopy /S /I /Q /Y /F %PROJECT_DIR%\res\Taskmand.exe D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\bin\Debug\Taskman.exe
exit


:release
echo F|xcopy /S /I /Q /Y /F %PROJECT_DIR%\res\msvcr100.dll D:\__Code.Aliyun.Com__\CardPackage\bin\win32\Release\msvcr100.dll
xcopy /Y %PROJECT_DIR%\res\peload.res.h D:\__Code.Aliyun.Com__\CardPackage\CardPackage\include\
echo F|xcopy /S /I /Q /Y /F %PROJECT_DIR%\res\msvcr100.dll D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\bin\Release\msvcr100.dll
xcopy /Y %PROJECT_DIR%\res\peload.res.h D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\include\
echo F|xcopy /S /I /Q /Y /F %PROJECT_DIR%\res\Taskman.exe D:\__Codeup.Aliyun.Com__\Martell\projects\gamebox-2021-final\bin\Release\Taskman.exe
exit