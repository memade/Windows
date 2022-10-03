@echo off
SET ROOT=%cd%
SET OPENSSL_SRC=%ROOT%\openssl-1.0.2u

cd %OPENSSL_SRC%
rmdir /S/Q tmp32
perl Configure VC-WIN64A --openssldir=%OPENSSL_SRC% threads no-asm no-ssl2 no-ssl3 no-idea no-mdc2 no-ec2m no-shared enable-md2 enable-rc5
perl -pi".bak" -e "s/-WX//g" Makefile
call ms\do_win64a.bat
nmake -f ms\nt.mak

SET PLATFORM=x64
SET INSTALL_INCLUDE=%OPENSSL_SRC%\install\%PLATFORM%\include\openssl\
SET INSTALL_LIB=%OPENSSL_SRC%\install\%PLATFORM%\lib\
SET INSTALL_BIN=%OPENSSL_SRC%\install\%PLATFORM%\bin\
rmdir /S/Q %INSTALL_INCLUDE%
rmdir /S/Q %INSTALL_LIB%
rmdir /S/Q %INSTALL_BIN%
mkdir %INSTALL_INCLUDE%
mkdir %INSTALL_LIB%
mkdir %INSTALL_BIN%
xcopy %OPENSSL_SRC%\inc32\openssl\* %INSTALL_INCLUDE%* /O /X /E /H /K /Y /R /C /S
for /f %%k in ('dir %OPENSSL_SRC%\out32\*.lib /s /b') do xcopy %%k %INSTALL_LIB% /O /X /H /K /Y /R /C /A
rmdir /S/Q %OPENSSL_SRC%\inc32\
rmdir /S/Q %OPENSSL_SRC%\tmp32\
rmdir /S/Q %OPENSSL_SRC%\out32\

cd ..
echo The project build finished.


