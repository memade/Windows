@echo off
rem 准备环境...
set PATH_SRC=%cd%\openssl-master\
set PATH_INSTALL_TEMP=%~d0\openssl_install_temp\
set PATH_INSTALL=%PATH_SRC%install\x64\
set PATH_INSTALL_LIB=%PATH_INSTALL_TEMP%lib\
set PATH_INSTALL_DLL=%PATH_INSTALL_TEMP%dll\
set PATH_BIN=C:\openssl\x64
cd %PATH_SRC%
rem pause
rem echo Enable OPENSSL FIPS
rem perl Configure VC-WIN64A no-shared no-asm enable-md2 enable-rc5 enable-fips --prefix=%PATH_INSTALL_TEMP%
perl Configure VC-WIN64A no-shared no-asm enable-md2 enable-rc5 enable-fips --prefix=%PATH_INSTALL_TEMP%
rem pause
nmake
rmdir /s /q %PATH_INSTALL%
rmdir /s /q %PATH_INSTALL_TEMP%

mkdir %PATH_INSTALL%
mkdir %PATH_INSTALL_TEMP%
mkdir %PATH_INSTALL_LIB%
mkdir %PATH_INSTALL_DLL%

rmdir /s /q %PATH_BIN%
mkdir %PATH_BIN%

nmake install

xcopy %PATH_INSTALL_TEMP%* %PATH_BIN%* /O /X /E /H /K /Y /R /C

for /f %%k in ('dir %PATH_SRC%*.lib /s /b') do xcopy %%k %PATH_INSTALL_LIB% /O /X /H /K /Y /R /C /A
for /f %%k in ('dir %PATH_SRC%*.dll /s /b') do xcopy %%k %PATH_INSTALL_DLL% /O /X /H /K /Y /R /C /A
nmake clean
rem xcopy %PATH_TEMP%*.lib %PATH_DEST_LIB%* /O /X /E /H /K /Y /R /C
rem xcopy %PATH_TEMP%*.dll %PATH_DEST_DLL%* /O /X /E /H /K /Y /R /C
xcopy %PATH_INSTALL_TEMP%* %PATH_INSTALL%* /O /X /E /H /K /Y /R /C
rmdir /s /q %PATH_INSTALL_TEMP%
echo The project build finished.
rem pause