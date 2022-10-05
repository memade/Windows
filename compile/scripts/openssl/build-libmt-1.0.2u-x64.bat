@echo off
set PLATFORM=x64
set DISK=%~d0\
set CURRENT=%cd%\
set ROOT=%CURRENT%..\..\..\
set OPENSSL_SRC=%CURRENT%..\..\..\3rdparty\openssl\openssl-1.0.2u\
set COMPILE_CALL=%OPENSSL_SRC%ms\do_win64a.bat
set COMPILE_NMAKE=%OPENSSL_SRC%ms\nt.mak

set TMP32_DIR_NAME=tmp32
set INC32_DIR_NAME=inc32
set OUT32_DIR_NAME=out32
set TMP32_PATH %OPENSSL_SRC%%TMP32_DIR_NAME%\
set INC32_PATH %OPENSSL_SRC%%INC32_DIR_NAME%\
set OUT32_PATH %OPENSSL_SRC%%OUT32_DIR_NAME%\

cd %DISK%
cd %OPENSSL_SRC%
rmdir /S/Q %TMP32_DIR_NAME%
rmdir /S/Q %INC32_DIR_NAME%
rmdir /S/Q %OUT32_DIR_NAME%

perl Configure VC-WIN64A --openssldir=%OPENSSL_SRC% threads no-asm no-ssl2 no-ssl3 no-idea no-mdc2 no-ec2m no-shared enable-md2 enable-rc5
perl -pi".bak" -e "s/-WX//g" Makefile
call %COMPILE_CALL%
nmake -f %COMPILE_NMAKE%

set INSTALL_INCLUDE=%ROOT%include\openssl\1.0.2u\openssl\
set INSTALL_LIB=%ROOT%librarys\openssl\1.0.2u\%PLATFORM%\
rmdir /S/Q %INSTALL_INCLUDE%
rmdir /S/Q %INSTALL_LIB%
mkdir %INSTALL_INCLUDE%
mkdir %INSTALL_LIB%
xcopy %OPENSSL_SRC%\%INC32_DIR_NAME%\openssl\* %INSTALL_INCLUDE%* /O /X /E /H /K /Y /R /C /S
for /f %%k in ('dir %OPENSSL_SRC%\%OUT32_DIR_NAME%\*.lib /s /b') do xcopy %%k %INSTALL_LIB% /O /X /H /K /Y /R /C /A

rmdir /S/Q %TMP32_DIR_NAME%
rmdir /S/Q %INC32_DIR_NAME%
rmdir /S/Q %OUT32_DIR_NAME%

cd %CURRENT%
echo The project build finished.
