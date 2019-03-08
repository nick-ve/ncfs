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
rem * This script creates ncfspack.lib and ncfspack.dll from all .h and .cxx files
rem * in the corresponding source directory.
rem *
rem * In view of the ROOTCINT processing, the following two standard files
rem * are always required :
rem *
rem * NCFSHeaders.h : containing an include of all .h files
rem * NCFSLinkDef.h : containing the #pragma's to define all classes
rem *
rem * --- Nick van Eijndhoven 03-dec-2008 NCFS
rem ****************************************************************************

echo .
echo === Automatic ROOT library production of files ncfspack.lib and ncfspack.dll ===
echo .
rem --- Set the NCFSPack source directory as working directory
cd ..\source

rem --- Prevent linking of the standard ROOT library libNew.lib
if exist %ROOTSYS%\lib\libNew.lib rename %ROOTSYS%\lib\libNew.lib libNew.lix

rem --- The option strings for MSVC++ DLL compilation and linking ---
set mscomp=/nologo /c /Ze /TP /MD /GR /GX /I%ROOTSYS%\include
set msdll=/nologo /Ze /TP /MD /LD /GD /GR /GX /I%ROOTSYS%\include
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
echo *
echo * This script creates ncfspack.lib and ncfspack.dll from all .h and .cxx files
echo * in the corresponding source directory.
echo *
echo * In view of the ROOTCINT processing, the following two standard files
echo * are always required :
echo *
echo * NCFSHeaders.h : containing an include of all .h files
echo * NCFSLinkDef.h : containing the #pragma's to define all classes
echo ****************************************************************************
goto end

:export
echo *** Creation of ROOT loadable export libraries
echo.
rem --- Creation of ROOT dictionary ---
rootcint zzzncfspackdict.cxx -c NCFSHeaders.h NCFSLinkDef.h
rem --- Compilation step ---
cl %mscomp% *.cxx
rem --- Creation of the export LIB ---
bindexplib ncfspack *.obj > ncfspack.def
lib /nologo /machine:IX86 *.obj /def:ncfspack.def /out:ncfspack.lib
rem --- Creation of the DLL ---
link /nologo /machine:IX86 /DLL *.obj ncfspack.exp %mslink% /OUT:ncfspack.dll
rem --- Move the created libs to the corresponding ROOT subdirectory
move ncfspack.lib %ROOTSYS%\lib
move ncfspack.dll %ROOTSYS%\bin
rem --- Delete all intermediate files --- 
del .def
del ncfspack.def
del ncfspack.exp
del zzzncfspackdict.h
del zzzncfspackdict.cxx
del *.obj
echo.
echo *** mklibs done.
goto end

:full
echo *** Creation of ROOT loadable full version libraries
echo.
rem --- Creation of ROOT dictionary ---
rootcint zzzncfspackdict.cxx -c NCFSHeaders.h NCFSLinkDef.h
rem --- Creation of the DLL ---
cl %msdll% *.cxx /link %mslink% /OUT:ncfspack.dll
rem --- Creation of the full version LIB ---
lib /nologo /machine:IX86 *.obj /out:ncfspack.lib
rem --- Move the created libs to the corresponding ROOT subdirectory
move ncfs.lib %ROOTSYS\lib
move ncfs.dll %ROOTSYS\bin
rem --- Delete all intermediate files --- 
del .def
del zzzncfspackdict.h
del zzzncfspackdict.cxx
del *.obj
echo.
echo *** mklibs done.
goto end

:end
rem --- Go back to original directory
cd ..\scripts
rem --- End of script ---
