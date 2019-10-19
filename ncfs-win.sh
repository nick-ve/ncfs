### Script to initialise the NCFS analysis environment in a Windows (Ubuntu) powershell.
### This will provide a transparent interface to create and use the NCFS libraries 
### in the ROOTSYS directory of the Linux (Ubuntu) environment of the Windows powershell,
### while accessing the NCFS source code from a folder of the standard Windows file system.
### This will allow a single location for the source code (and docs) management.
### In addition also a handy "unzipping" facility (called "extractx") is provided.
###
### Make sure to specify below the correct paths according to the installations on your system.
###
### Note that in case the documentation of the NCFS classes is updated from within the powershell
### via the provided "makedocs.cc" macro, these updates will be performed in the NCFS docs folder 
### of the corresponding standard Windows file system. 
###
### On other (single) systems, please refer to ncfs.bat (Windows) or ncfs.sh (Linux).
###
### Usage example : $source ncfs-win.sh c e
###
### This will initialise the NCFS analysis environment
### for the NCFS package of which the source is installed on drive c.
### Drive e will be marked as external drive, which in general
### contains the data files to be analysed or produced.
###
### Notes :
### -------
### 1) In case the second argument is not provided,
###    the external drive will be set to the same drive
###    as provided in the first argument.
### 2) In case no arguments are provided, both will be set to c.
###
### The environment variables that will be set in the above example are :
### ---------------------------------------------------------------------
### SRCDRIVE will be set to c
### EXTDRIVE will be set to e
### PS1 (= command prompt) will be set to ncfs>
###
### Nick van Eijndhoven, IIHE-VUB, Brussel, Belgium.
#
### Set the Windows drive identifier for the NCFS source area
export SRCDRIVE=/mnt/c
if [ $1 ]
then
 export SRCDRIVE=/mnt/$1
fi 
#
### Set the Windows drive identifier for the external (data) drive
export EXTDRIVE=${SRCDRIVE}
if [ $2 ]
then
 export EXTDRIVE=/mnt/$2
fi 
#
### Set environment variables to run the ROOT package ###
export ROOTSYS=${HOME}/software/root_v5.34.38/build
export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
#
### Additional libraries like for instance Pythia
export MYLIBS=${HOME}/software/pythia-6.4.28
#
### Directory where the CFITSIO header files reside
export CFITSIO=standard ### Use this when the header files are in the standard system include path
###export CFITSIO=/usr/include/cfitsio
#
### Pointer to the NCFS top directory
export NCFS=${SRCDRIVE}/nick/cxx/source/ncfs
#
### Set prompt to ncfs>
export PS1="ncfs>"
#
export PATH=$ROOTSYS/bin:$PATH
#
echo " NCFS environment set :"
echo " ROOTSYS="${ROOTSYS} " NCFS="${NCFS} " CFITSIO="${CFITSIO} " EXTDRIVE="${EXTDRIVE}
#
#
### Define the "extractx" facility to "unzip" various file types
extractx () {
if [ -f $1 ] ; then
       case $1 in
        *.tar.bz2)      tar xvjf $1 ;;
        *.tar.gz)       tar xvzf $1 ;;
        *.tar.xz)       7za x $1 && tar xvf $(basename "$1" .xz) && rm -f $(basename "$1" .xz) ;;
        *.bz2)          bunzip2 $1 ;;
        *.rar)          unrar x $1 ;;
        *.gz)           gunzip $1  ;;
        *.tar)          tar xvf $1 ;;
        *.tbz2)         tar xvjf $1 ;;
        *.tgz)          tar xvzf $1 ;;
        *.zip)          unzip $1 ;;
        *.Z)            uncompress $1 ;;
        *.7z)           7z x $1 ;;
        *)              echo "don't know how to extract '$1'..." ;;
       esac
   else
       echo "'$1' is not a valid file!"
   fi
}
