#!bin/sh
###################################################################
# Script to provide a Python 3 or Python 2 environment via CVMFS. #
# Nick van Eijndhoven, IIHE-VUB, Brussel.                         #
###################################################################
#
### Load Python3
eval $(/cvmfs/icecube.opensciencegrid.org/py3-v4.1.1/setup.sh)
#
### Set prompt to python>
export PS1="python>"
#
### Get the Python versions
echo " Python environment set : Use python to invoke $(python --version) or python2 to invoke the old version2."
