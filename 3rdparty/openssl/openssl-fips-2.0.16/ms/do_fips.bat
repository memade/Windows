rem @echo off

rem SET FIPSINSTALLDIR=C:\777
SET ASM=%1
SET EXARG=
SET MFILE=ntdll.mak

if NOT X%OSVERSION% == X goto wince

if NOT X%PROCESSOR_ARCHITECTURE% == X goto defined 

echo Processor Architecture Undefined: defaulting to X86

goto X86

:defined

if %PROCESSOR_ARCHITECTURE% == x86 goto X86

if %PROCESSOR_ARCHITECTURE% == IA64 goto IA64

if %PROCESSOR_ARCHITECTURE% == AMD64 goto AMD64

echo Processor Architecture Unrecognized: defaulting to X86

:X86
echo Auto Configuring for X86

SET TARGET=VC-WIN32
if x%ASM% == xno-asm goto compile
SET ASM=nasm

goto compile

:IA64

echo Auto Configuring for IA64
SET TARGET=VC-WIN64I

goto compile

:AMD64

echo Auto Configuring for AMD64
SET TARGET=VC-WIN64A
if x%ASM% == xno-asm goto compile
SET ASM=nasm

goto compile

:wince

echo Auto Configuring for WinCE
SET TARGET=VC-CE
SET MFILE=cedll.mak

:compile

if x%ASM% == xno-asm SET EXARG=no-asm
perl Configure %TARGET% %EXARG% fipscheck --prefix=%FIPSINSTALLDIR%
pause

echo on

perl util\mkfiles.pl >MINFO
@if ERRORLEVEL 1 goto error
perl util\mk1mf.pl dll %ASM% %TARGET% >ms\%MFILE%
@if ERRORLEVEL 1 goto error

nmake -f ms\%MFILE% clean
nmake -f ms\%MFILE%
@if ERRORLEVEL 1 goto error
nmake -f ms\%MFILE% install
@if ERRORLEVEL 1 goto error

@echo.
@echo.
@echo.
@echo ***************************
@echo ****FIPS BUILD SUCCESS*****
@echo ***************************

@goto end

:error

@echo.
@echo.
@echo.
@echo ***************************
@echo ****FIPS BUILD FAILURE*****
@echo ***************************

:end
