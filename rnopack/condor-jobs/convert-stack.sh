#!/bin/sh

####################################################
# Shell script to process the conversion of a RNO-G
# plain ROOT data file into RnoEvent structures.
#
# This shell script is launched by the HTCondor
# job description script convert-stack.sub.
# See the file convert-stack.sub for further details.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC July 14, 2022  16:52
####################################################

export JOBNAME=$1
export HOMEDIR=$2
export HOSTDIR=$3
export OUTPUTDIR=$4

export SLAVEDIR=$_CONDOR_SCRATCH_DIR

echo "*** Starting job $JOBNAME ***"
echo " "
echo " Jobname=$JOBNAME"
echo " Homedir=$HOMEDIR"
echo " Hostdir=$HOSTDIR"
echo " Outputdir=$OUTPUTDIR"
echo " Slavedir=$SLAVEDIR"
echo " "

### Create the output directory, if not already present
mkdir -p $OUTPUTDIR

cd $SLAVEDIR

### Import the necessary input file(s)
cp $HOSTDIR/convert-stack.cc macro.cc

### Load the NCFS environment
source /ice3/software/iihe/ncfs.sh

### Execute the ROOT macro
echo " "
root -b -q macro.cc

### Transfer the produced output file with RnoEvent structures
mv -f *.rnopack $OUTPUTDIR

### Transfer additionally produced histogram etc. ROOT files
mv -f *.root $OUTPUTDIR

echo " "
echo "*** Job $JOBNAME completed ***"
