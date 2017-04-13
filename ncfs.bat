@ECHO OFF
rem ***********************************************************
rem *
rem * Script to initialise the NCFS analysis environment.
rem *
rem * Usage example : ncfs D E
rem *
rem * This will initialise the NCFS analysis environment
rem * for the NCFS package which is installed on drive D.
rem * Drive E will be marked as external drive, which in general
rem * contains the data files to be analysed or produced.
rem *
rem * Note : In case the second argument is not provided,
rem *        the external drive will be set to to the same drive
rem *        as provided in the first argument.
rem *
rem * The environment variables that will be set in this example are :
rem * ----------------------------------------------------------------
rem * PROMPT will be set to ncfs>
rem * EXTDRIVE will be set to E:
rem * HOME will be set to D:\nick
rem * TEMP will be set to D:\temp
rem * TMPDIR will be set to D:\temp
rem * ROOTSYS will be set to D:\root
rem * NCFS will be set to HOME\cxx\source\ncfs
rem * MSVCDIR will be set to D:\msvc
rem * INCLUDE will get addition of MSVCDIR\include
rem * LIB will get addition of MSVCDIR\lib
rem * PATH will get addition of ROOTSYS\bin, MSVCDIR\bin and MSVCDIR\dll
rem *
rem * --- Nick van Eijndhoven 25-mar-2009 NCFS
rem ***********************************************************

if "%1" == "" goto help

set EXTDRIVE=%2:
if "%EXTDRIVE%" == ":" set EXTDRIVE=%1:
 
set PROMPT=ncfs$g
set HOME=%1:\nick
set TEMP=%1:\temp
set TMPDIR=%1:\temp
set ROOTSYS=%1:\root
set NCFS=%HOME%\cxx\source\ncfs
set MSVCDIR=%1:\msvc
set INCLUDE=%MSVCDIR%\include;%INCLUDE%
set LIB=%MSVCDIR%\lib;%LIB%
set PATH=%PATH%;%ROOTSYS%\bin;%MSVCDIR%\bin;%MSVCDIR%\dll

echo *** NCFSPack on drive %1   EXTDRIVE is %EXTDRIVE% ***
echo .

goto end

rem --- Display of the help info --- 
:help
echo .
echo --- Drive letter of NCFS installation is missing.
echo .
echo ***********************************************************
echo *
echo * Script to initialise the NCFS analysis environment.
echo *
echo * Usage example : ncfs D E
echo *
echo * This will initialise the NCFS analysis environment
echo * for the NCFS package which is installed on drive D.
echo * Drive E will be marked as external drive, which in general
echo * contains the data files to be analysed or produced.
echo *
echo * Note : In case the second argument is not provided,
echo *        the external drive will be set to the same drive
echo *        as provided in the first argument.
echo *
echo * The environment variables that will be set in this example are :
echo * ----------------------------------------------------------------
echo * PROMPT will be set to ncfs$g
echo * EXTDRIVE will be set to E:
echo * HOME will be set to D:\nick
echo * TEMP will be set to D:\temp
echo * TMPDIR will be set to D:\temp
echo * ROOTSYS will be set to D:\root
echo * NCFS will be set to HOME\cxx\source\ncfs
echo * MSVCDIR will be set to D:\msvc
echo * INCLUDE will get addition of MSVCDIR\include
echo * LIB will get addition of MSVCDIR\lib
echo * PATH will get addition of ROOTSYS\bin, MSVCDIR\bin and MSVCDIR\dll
echo *
echo * --- Nick van Eijndhoven 25-mar-2009 NCFS
echo ***********************************************************

:end
rem --- End of script ---
