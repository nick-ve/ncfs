################################################################################
### Set links to the ROOT stuff for processing on the IIHE computer cluster. ###
### On other systems, just specify the corresponding paths.                  ###
### In addition also a handy "unzipping" facility is provided.               ###
###                                                                          ###
### Nick van Eijndhoven, IIHE-VUB, Brussel, Belgium                          ###
################################################################################
#
### Set environment variables to run the ROOT package ###
export ROOTSYS=/software/icecube/root_v5.34.38
export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
#
### My private root loadable library area
export MYLIBS=/software/icecube/pythia-6.4.28
#
### Directory where the CFITSIO header files reside (if not in the standard system include path)
export CFITSIO=/usr/include/cfitsio
#
### Pointer to the NCFS top directory
export NCFS=/software/icecube/nick/ncfs
#
### Set the regular bash prompt to ncfs>
export PS1="ncfs>"
#
### Set the prompt for any SLx environment to ncfs>
export APPTAINERENV_PS1="ncfs>"
#
export PATH=$ROOTSYS/bin:/software/icecube/firefox:$PATH
#
echo " NCFS environment set: ROOTSYS="${ROOTSYS} " NCFS="${NCFS} " CFITSIO="${CFITSIO}
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
