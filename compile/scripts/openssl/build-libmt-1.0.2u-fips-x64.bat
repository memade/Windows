@echo off
set PLATFORM=x64
set DISK=%~d0
set CURRENT=%cd%
set ROOT=%CURRENT%..\..\..
set OPENSSL_SRC=%CURRENT%\..\..\..\3rdparty\openssl\openssl-1.0.2u
set FIPS_SRC=%CURRENT%\..\..\..\3rdparty\openssl\openssl-fips-2.0.16
set COMPILE_FIPS_CALL=%FIPS_SRC%\ms\do_fips.bat
set COMPILE_OPENSSL_CALL=%OPENSSL_SRC%\ms\do_win64a.bat

echo %FIPS_SRC%

rem compile openssl fips
cd %DISK%
cd %FIPS_SRC%
set PROCESSOR_ARCHITECTURE=AMD64
set FIPSINSTALLDIR=%FIPS_SRC%\install
rem mkdir %INC32_DIR_NAME%
rem xcopy %FIPS_SRC%include\* %FIPS_SRC%inc32\* /O /X /E /H /K /Y /R /C
call %COMPILE_FIPS_CALL%
mkdir lib
copy out32dll\* lib\*
mkdir bin
copy util\* bin\*
copy out32dll\fips_standalone_sha1.exe bin
xcopy inc32\* include\* /O /X /E /H /K /Y /R /C

pause

cd %OPENSSL_SRC%
rem rmdir /S/Q tmp32dll
rem threads no-asm no-ssl2 no-ssl3 no-idea no-mdc2 no-ec2m no-shared enable-md2 enable-rc5
rem threads no-ssl2 no-ssl3 no-idea no-mdc2 no-rc5 no-ec2m no-shared
perl Configure VC-WIN64A fips --openssldir=%OPENSSL_SRC% threads no-asm no-ssl2 no-ssl3 no-idea no-mdc2 no-ec2m no-shared enable-md2 enable-rc5 --with-fipsdir=%FIPS_SRC%
perl -pi".bak" -e "s/-WX//g" Makefile
call %COMPILE_OPENSSL_CALL%
nmake -f %OPENSSL_SRC%\ms\nt.mak


set INSTALL_INCLUDE=%ROOT%\..\include\openssl\1.0.2u-fips\openssl\
set INSTALL_LIB=%ROOT%\..\librarys\openssl\1.0.2u-fips\%PLATFORM%\

xcopy %FIPSINSTALLDIR%\include\openssl\* %INSTALL_INCLUDE%* /O /X /E /H /K /Y /R /C /S
for /f %%k in ('dir %FIPSINSTALLDIR%\lib\*.lib /s /b') do xcopy %%k %INSTALL_LIB% /O /X /H /K /Y /R /C /A

xcopy %OPENSSL_SRC%\inc32\openssl\* %INSTALL_INCLUDE%* /O /X /E /H /K /Y /R /C /S
for /f %%k in ('dir %OPENSSL_SRC%\out32\*.lib /s /b') do xcopy %%k %INSTALL_LIB% /O /X /H /K /Y /R /C /A

cd %FIPS_SRC%
rmdir /S/Q bin
rmdir /S/Q lib
rmdir /S/Q inc32
rmdir /S/Q out32dll
rmdir /S/Q tmp32dll
rmdir /S/Q install

cd %OPENSSL_SRC%
rmdir /S/Q inc32
rmdir /S/Q out32
rmdir /S/Q tmp32

cd %CURRENT%
echo The project build finished.
