### Set links to the ROOT stuff for processing on the IIHE computer cluster ###
### On other systems, just specify the corresponding paths ###
### In addition also a handy "unzipping" facility is provided ###
###
### Nick van Eijndhoven, IIHE-VUB, Brussel, Belgium
#
### Set environment variables to run the ROOT package ###
export ROOTSYS=/ice3/software/root_v5.34.23
export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
#
### Additional libraries like for instance Pythia
export MYLIBS=/ice3/software/pythia6
#
#
### Pointer to the NCFS top directory
export NCFS=/ice3/software/ncfs
#
### Set prompt to ncfs>
export PS1="ncfs>"
#
export PATH=$ROOTSYS/bin:/ice3/software/firefox:$PATH
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
