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
# UTC July 11, 2022  11:15
#############################################################

export JOBNAME=$1
export HOMEDIR=$2
export HOSTDIR=$3
export OUTPUTDIR=$4
export INPUTFILE=$5

export SLAVEDIR=$_CONDOR_SCRATCH_DIR

echo "*** Starting job $JOBNAME ***"
echo " "
echo " Jobname=$JOBNAME"
echo " Homedir=$HOMEDIR"
echo " Hostdir=$HOSTDIR"
echo " Outputdir=$OUTPUTDIR"
echo " Inputfile=$INPUTFILE"
echo " Slavedir=$SLAVEDIR"
echo " "

cd $SLAVEDIR

### Import the necessary input file(s)
cp $HOSTDIR/analyze.cc macro.cc
cp $INPUTFILE data.rnopack

### Load the NCFS environment
source /ice3/software/iihe/ncfs.sh

### Execute the ROOT macro
echo " "
root -b -q macro.cc

### Remove the local copy of the input file
rm -f data.rnopack

### Transfer additionally produced histogram etc. ROOT files
mv -f *.root $OUTPUTDIR

echo " "
echo "*** Job $JOBNAME completed ***"
