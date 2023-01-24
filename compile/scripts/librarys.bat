@echo off
set x86debug="Debug|x86"
set x86release="Release|x86"
set win32debug="Debug|Win32"
set win32release="Release|Win32"
set x64debug="Debug|x64"
set x64release="Release|x64"
set devent_dir="C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.com"

echo build 3rdparty.librarys.
set build_target="..\msvc\librarys.sln"
set build_configure=%x86debug%
%devent_dir% %build_target% /ReBuild %build_configure%
set build_configure=%x86release%
%devent_dir% %build_target% /ReBuild %build_configure%
set build_configure=%x64debug%
%devent_dir% %build_target% /ReBuild %build_configure%
set build_configure=%x64release%
%devent_dir% %build_target% /ReBuild %build_configure%

set CURRENT=%cd%\
set copyto_include_wxwidgets=%CURRENT%..\..\include\wxwidgets
set copyto_librarys_wxwidgets=%CURRENT%..\..\librarys\wxwidgets
set source_directory_x86=%CURRENT%..\..\3rdparty\wxWidgets-3.2.1\lib\vc_lib
set source_directory_x64=%CURRENT%..\..\3rdparty\wxWidgets-3.2.1\lib\vc_x64_lib

echo build 3rdparty.wxwidgets.3.2.1.
set build_target="..\..\3rdparty\wxWidgets-3.2.1\build\msw\wx_vc17.sln"
set build_configure=%win32debug%
%devent_dir% %build_target% /ReBuild %build_configure%
set build_configure=%win32release%
%devent_dir% %build_target% /ReBuild %build_configure%
set build_configure=%x64debug%
%devent_dir% %build_target% /ReBuild %build_configure%
set build_configure=%x64release%
%devent_dir% %build_target% /ReBuild %build_configure%

rem mkdir %copyto_include_wxwidgets%\x86
rem mkdir %copyto_include_wxwidgets%\x64
mkdir %copyto_librarys_wxwidgets%\x86
mkdir %copyto_librarys_wxwidgets%\x64
rem xcopy %source_directory_x86%\*.h %copyto_include_wxwidgets%\x86 /S /A /Y
rem xcopy %source_directory_x64%\*.h %copyto_include_wxwidgets%\x64 /S /A /Y
xcopy %source_directory_x86%\*.lib %copyto_librarys_wxwidgets%\x86 /S /A /Y
xcopy %source_directory_x64%\*.lib %copyto_librarys_wxwidgets%\x64 /S /A /Y
rem rmdir /S/Q %source_directory_x86%
rem rmdir /S/Q %source_directory_x64%
del /S %source_directory_x86%\*.lib
del /S %source_directory_x64%\*.lib

cd %CURRENT%..\..\3rdparty\wxWidgets-3.2.1\
call clean.bat

exit