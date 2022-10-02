
rem vcvarsall x64
E:
cd E:\www.skstu.com\Project.Gpl\openssl\openssl-fips-2.0.16
Set FPSDIR=E:\www.skstu.com\Project.Gpl\openssl\openssl-fips-2.0.16
Set PROCESSOR_ARCHITECTURE=WIN32

xcopy inc32\* include\* /O /X /E /H /K /Y /R /C

call ms\do_fips.bat
mkdir lib
copy out32dll\* lib\*
mkdir bin
copy util\* bin\*
copy out32dll\fips_standalone_sha1.exe bin
xcopy inc32\* include\* /O /X /E /H /K /Y /R /C

cd E:\www.skstu.com\Project.Gpl\openssl\openssl-1.0.2u
rmdir /S/Q tmp32dll
perl Configure VC-WIN32 fips --openssldir=E:\www.skstu.com\Project.Gpl\openssl\openssl-1.0.2u threads no-ssl2 no-ssl3 no-idea no-mdc2 no-rc5 no-ec2m no-shared --with-fipsdir=E:\www.skstu.com\Project.Gpl\openssl\openssl-fips-2.0.16
perl -pi".bak" -e "s/-WX//g" Makefile
call ms\do_nasm.bat
nmake -f ms\nt.mak
cd ..
