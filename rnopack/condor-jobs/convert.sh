#!/bin/sh

####################################################
# Shell script to process the conversion of a RNO-G
# plain ROOT data file into RnoEvent structures.
#
# This shell script is launched by the convert.sub
# HTCondor job description script.
# See the file convert.sub for further details.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC July 15, 2022  16:55
####################################################

export JOBNAME=$1
export HOMEDIR=$2
export HOSTDIR=$3
export OUTPUTDIR=$4
export INPUTFILE=$5

export SLAVEDIR=$_CONDOR_SCRATCH_DIR

### Determine the basename of the input filename
fname=$INPUTFILE
nodir=$(basename $fname)
export FILENAMEBASE=${nodir%.*}

export OUTPUTFILE=$OUTPUTDIR/$FILENAMEBASE.rnopack

echo "*** Starting job $JOBNAME ***"
echo " "
echo " Jobname=$JOBNAME"
echo " Homedir=$HOMEDIR"
echo " Hostdir=$HOSTDIR"
echo " Outputdir=$OUTPUTDIR"
echo " Inputfile=$INPUTFILE"
echo " FilenameBase=$FILENAMEBASE"
echo " Outputfile=$OUTPUTFILE"
echo " Slavedir=$SLAVEDIR"
echo " "

### Create the output directory, if not already present
mkdir -p $OUTPUTDIR

cd $SLAVEDIR

### Import the necessary input file(s)
cp $HOSTDIR/convert.cc macro.cc
cp $INPUTFILE data.root

### Load the NCFS environment
source /ice3/software/iihe/ncfs.sh

### Execute the ROOT macro
echo " "
root -b -q macro.cc

### Remove the local copy of the input file
rm -f data.root

### Rename and transfer the produced output file
mv -f data.rnopack $OUTPUTFILE

### Transfer additionally produced histogram etc. ROOT files
mv -f *.root $OUTPUTDIR

echo " "
echo "*** Job $JOBNAME completed ***"
