#!/bin/sh

#############################################################
# Shell script to investigate RNO-G data files containing
# RnoEvent structures that have been produced by RnoConvert.
#
# This shell script is launched by the analyze.sub
# HTCondor job description script.
# See the file analyze.sub for further details.
#
# Nick van Eijndhoven, IIHE-VUB, Brussels.
# UTC July 15, 2022  14:50
#############################################################

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
cp $HOSTDIR/analyze-stack.cc macro.cc

### Load the NCFS environment
source /software/icecube/iihe/ncfs.sh

### Execute the ROOT macro
echo " "
root -b -q macro.cc

### Transfer additionally produced histogram etc. ROOT files
mv -f *.root $OUTPUTDIR

echo " "
echo "*** Job $JOBNAME completed ***"
