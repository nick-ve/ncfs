########################################################################################
### Script to initialize the ROOT and NCFS environment on the IIHE computer cluster. ###
### On other systems, just specify the corresponding paths.                          ###
### In addition also a handy "unzipping" facility is provided.                       ###
###                                                                                  ###
### Nick van Eijndhoven, IIHE-VUB, Brussel, Belgium                                  ###
########################################################################################
#
### Pointer to the NCFS top directory
export NCFS=/software/icecube/nick/ncfs
#
########################################################################
### Set environment variables to run ROOT and the NCFS-Pack packages ###
########################################################################
#
### Use the line below in case you use a cvmfs distribution of ROOT
source /cvmfs/sft.cern.ch/lcg/views/LCG_107/x86_64-el9-gcc11-opt/setup.sh
#
### Use the line below in case you use a local (pre)compiled copy of ROOT
### export ROOTSYS=/software/icecube/root_v6.24.08
#
### Additional libraries like for instance HDF5
### export XTRALIBS=/usr/lib/x86_64-linux-gnu/hdf5/serial
#
### Set the library path
export LD_LIBRARY_PATH=$NCFS/libs:$NCFS/Pythia6:$ROOTSYS/lib:$XTRALIBS:$LD_LIBRARY_PATH
#
### Suppress warnings from Cling
export CLING_STANDARD_PCH=none
#
### Directory where the CFITSIO header files reside
### export CFITSIO=standard ### Use this when the header files are in the standard system include path
export CFITSIO=/usr/include/cfitsio
#
### Directory where the HDF5 header files reside
export HDF5=standard ### Use this when the header files are in the standard system include path
### export HDF5=/usr/include/hdf5/serial
#
### Set the regular bash prompt to ncfs>
export PS1="ncfs>"
#
### Set the prompt for any SLx environment to ncfs>
export APPTAINERENV_PS1="ncfs>"
#
export PATH=$ROOTSYS/bin:/software/icecube/firefox:$PATH
#
echo " NCFS environment set :"
echo " ROOTSYS="${ROOTSYS} 
echo " NCFS="${NCFS} 
echo " CFITSIO="${CFITSIO}
echo " HDF5="${HDF5} 
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
