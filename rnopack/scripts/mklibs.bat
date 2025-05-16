@echo off
rem ****************************************************************************
rem *   Script to create an MSVC++ LIB and relocatable DLL from *.cxx files
rem *
rem * Usage :
rem * -------
rem * mklibs export : ROOT loadable DLL and export LIB are created
rem * mklibs full   : ROOT loadable DLL and full LIB version are created
rem * 
rem * Notes :
rem * -------
rem * 1) "mklibs export" is the default, enabling ROOT loadable library creation
rem *    via 'double clicking'.
rem *
rem * 2) Providing unsupported options results in displaying the help info.  
rem *
rem * 3) An environment variable NCFS has to be set, pointing to the
rem *    location where the NCFS top level directory is residing.  
rem *
rem * This script creates icepack.lib and icepack.dll from all .h and .cxx files
rem * in the current directory.
rem *
rem * In view of the ROOTCINT processing, the following two standard files
rem * are always required :
rem *
rem * ICEHeaders.h : containing an include of all .h files
rem * ICELinkDef.h : containing the #pragma's to define all classes
rem *
rem * --- Nick van Eijndhoven, IIHE-VUB, Brussel, May 3, 2021  10:11 UTC
rem ****************************************************************************

echo .
echo === Automatic ROOT library production of files ice.lib and ice.dll ===
echo .
rem --- Set the IcePack source directory as working directory
cd ..\source

rem --- The option strings for MSVC++ DLL compilation and linking ---
set mscomp=/nologo /c /TP /Ze /MD /GR /GX /I%NCFS%\ncfspack\source /I%ROOTSYS%\include
set msdll=/nologo /TP /Ze /MD /LD /GD /GR /GX /I%NCFS%\ncfspack\source /I%ROOTSYS%\include
set mslink=/ENTRY:_DllMainCRTStartup@12 %ROOTSYS%\lib\*.lib

if "%1" == "" goto export
if "%1" == "export" goto export
if "%1" == "full" goto full

rem --- Displaying of the help info ---
echo ****************************************************************************
echo *   Script to create an MSVC++ LIB and relocatable DLL from *.cxx files
echo *
echo * Usage :
echo * -------
echo * mklibs export : ROOT loadable DLL and export LIB are created
echo * mklibs full   : ROOT loadable DLL and full LIB version are created
echo * 
echo * Notes :
echo * -------
echo * 1) "mklibs export" is the default, enabling ROOT loadable library creation
echo *    via 'double clicking'.
echo * 2) Providing unsupported options results in displaying the help info.  
echo * 3) An environment variable NCFS has to be set, pointing to the
echo *    location where the ralice source code is residing.  
echo *
echo * This script creates rnopack.lib and rnopack.dll from all .h and .cxx files
echo * in the current directory.
echo *
echo * In view of the ROOTCINT processing, the following two standard files
echo * are always required :
echo *
echo * ICEHeaders.h : containing an include of all .h files
echo * ICELinkDef.h : containing the #pragma's to define all classes
echo ****************************************************************************
goto end

:export
echo *** Creation of ROOT loadable export libraries
echo.
rem --- Creation of ROOT dictionary ---
rootcint rnopackdict.cxx -c -I%NCFS%\ncfspack\source ICEHeaders.h ICELinkDef.h
rem --- Compilation step ---
cl %mscomp% *.cxx
rem --- Creation of the export LIB ---
bindexplib rnopack *.obj > rnopack.def
lib /nologo /machine:IX86 *.obj /def:rnopack.def /out:rnopack.lib
rem --- Creation of the DLL ---
link /nologo /machine:IX86 /DLL *.obj rnopack.exp %mslink% /OUT:rnopack.dll
rem --- Move the created libs to the corresponding ROOT subdirectories
move rnopack.lib %NCFS%\libs
move rnopack.dll %NCFS%\libs
move rnopackdict* %NCFS%\libs
rem --- Delete intermediate files
del rnopack.def
del rnopack.exp
goto root_clean

:full
echo *** Creation of ROOT loadable full version libraries
echo.
rem --- Creation of ROOT dictionary ---
rootcint rnopackdict.cxx -c -I%NCFS%\ncfspack\source RNOHeaders.h RNOLinkDef.h
rem --- Creation of the DLL ---
cl %msdll% *.cxx /link %mslink% /OUT:rnopack.dll
rem --- Creation of the full version LIB ---
lib /nologo /machine:IX86 *.obj /out:rnopack.lib
rem --- Move the created libs to the corresponding ROOT subdirectories
move rnopack.lib %NCFS%\libs
move rnopack.dll %NCFS%\libs
move rnopackdict* %NCFS%\libs
rem --- Delete intermediate files
goto root_clean

:root_clean
rem --- Delete all intermediate files --- 
del .def
del *.obj
echo.
echo *** mklibs done.
goto end

:end
rem --- Go back to original directory
cd ..\scripts
rem --- End of script ---
