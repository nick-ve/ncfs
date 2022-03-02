#!bin/sh

####################################################################### 
# Shell script to update the IERS Leap Second and dUT data.
# The data are stored in the directory $NCFS/IERS.
#
# At the IIHE computer cluster I have set up a personal crontab entry
# to automatically execute this script on a daily basis at 01:00h.
#
# The option -N at the wget command ensures to only retrieve the file
# when the timestamp at the server is newer than the local one.
# This will avoid unnecessary data transport.
#
# Nick van Eijndhoven, IIHE-VUB, Brussel March 2, 2022  13:13Z
#######################################################################

### Go to the $NCFS/IERS directory
cd $NCFS/IERS

### Retrieve the data files from the French observatory
wget -N https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat -o iers-update.log
wget -N https://hpiers.obspm.fr/iers/series/opa/eopc04 -a iers-update.log

### Copy them to our standard files leap.txt and dut.txt
cp -f Leap_Second.dat leap.txt
cp -f eopc04 dut.txt
