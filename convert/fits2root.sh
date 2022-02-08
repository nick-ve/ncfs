#!/bin/sh

####################################################
# Shell script to process the conversion of
# a FITS data file into a ROOT file.
#
# This shell script is launched by the fits2root.sub
# HTCondor job description script.
# See the file fits2root.sub for further details.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC February 7, 2022  09:05
####################################################

export JOBNAME=$1
export HOMEDIR=$2
export HOSTDIR=$3
export OUTPUTDIR=$4
export INPUTFILE=$5

export SLAVEDIR=$_CONDOR_SCRATCH_DIR

export FILENAMEBASE=`basename -s .fits $INPUTFILE`

export OUTPUTFILE=$OUTPUTDIR/$FILENAMEBASE.root

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

cd $SLAVEDIR

### Import the necessary input file(s)
cp $HOSTDIR/fits2root.cc macro.cc
cp $INPUTFILE data.fits

### Load the NCFS environment
source /ice3/software/iihe/ncfs.sh

### Execute the ROOT macro
echo " "
root -b -q macro.cc

### Rename and transfer the produced output file
rm -f $OUTPUTFILE
mv -f data.root $OUTPUTFILE

echo " "
echo "*** Job $JOBNAME completed ***"
