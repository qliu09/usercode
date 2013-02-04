#!/bin/bash
#################################
# PSI Tier-3 example batch Job  #
#################################

SimName="FredericsPoint2_Full"




##if you want to change the SLHA file while a different fast sime generation is running (bad idea ...) you can change this directory. 
SLHAfile=/shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/MagicalPoint/ThisIsIt.slha




##### CONFIGURATION ##############################################
# Output files to be copied back to the User Interface
# (the file path must be given relative to the working directory)
OUTFILES=

# Output files to be copied to the SE
# (as above the file path must be given relative to the working directory)
SEOUTFILES="output_$1.root"

SOURCEFILES=sourcefile

#
# By default, the files will be copied to $USER_SRM_HOME/$username/$JOBDIR,
# but here you can define the subdirectory under your SE storage path
# to which files will be copied (uncomment line)
#SEUSERSUBDIR="mytestsubdir/somedir"
#
# User's CMS hypernews name (needed for user's SE storage home path
# USER_SRM_HOME below)

HN_NAME=`whoami`

# set DBG=1 for additional debug output in the job report files
# DBG=2 will also give detailed output on SRM operations
DBG=0

#### The following configurations you should not need to change
# The SE's user home area (SRMv2 URL)
USER_SRM_HOME="srm://t3se01.psi.ch:8443/srm/managerv2?SFN=/pnfs/psi.ch/cms/trivcat/store/user/"

# Top working directory on worker node's local disk. The batch
# job working directory will be created below this
TOPWORKDIR=/scratch/`whoami`

##################################################################

############ BATCH QUEUE DIRECTIVES ##############################
# Lines beginning with #$ are used to set options for the SGE
# queueing system (same as specifying these options to the qsub
# command

# Job name (defines name seen in monitoring by qstat and the
#     job script's stderr/stdout names)
#$ -N example_job

# Run time soft and hard limits hh:mm:ss

### Specify the queue on which to run
#$ -q short.q

# Change to the current working directory from which the job got
# submitted (will also result in the job report stdout/stderr being
# written to this directory)
#$ -cwd

# here you could change location of the job report stdout/stderr files
#  if you did not want them in the submission directory
#  #$ -o /shome/username/mydir/
#  #$ -e /shome/username/mydir/

##################################################################



##### MONITORING/DEBUG INFORMATION ###############################
DATE_START=`date +%s`
echo "Job started at " `date`
cat <<EOF
################################################################
## QUEUEING SYSTEM SETTINGS:
HOME=$HOME
USER=$USER
JOB_ID=$JOB_ID
JOB_NAME=$JOB_NAME
HOSTNAME=$HOSTNAME
TASK_ID=$TASK_ID
QUEUE=$QUEUE

EOF



echo "################################################################"

if test 0"$DBG" -gt 0; then
   echo "######## Environment Variables ##########"
   env
   echo "################################################################"
fi



##### SET UP WORKDIR AND ENVIRONMENT ######################################
STARTDIR=`pwd`

JOBDIR="Simulation_$1"

WORKDIR=$TOPWORKDIR/$JOBDIR
RESULTDIR=$STARTDIR/$JOBDIR
if test x"$SEUSERSUBDIR" = x; then
   SERESULTDIR=$USER_SRM_HOME/$HN_NAME/Simulation/$SimName/
else
   SERESULTDIR=$USER_SRM_HOME/$HN_NAME/Simulation/$SimName/$SEUSERSUBDIR
fi
if test -e "$WORKDIR"; then
   echo "WARNING: WORKDIR ($WORKDIR) already exists! Removing it..." >&2
   rm -rf $WOKDIR
fi
mkdir -p $WORKDIR
if test ! -d "$WORKDIR"; then
   echo "ERROR: Failed to create workdir ($WORKDIR)! Aborting..." >&2
   exit 1
fi

cd $WORKDIR
cat <<EOF
################################################################
## JOB SETTINGS:
STARTDIR=$STARTDIR
WORKDIR=$WORKDIR
RESULTDIR=$RESULTDIR
SERESULTDIR=$SERESULTDIR
EOF


###########################################################################
## YOUR FUNCTIONALITY CODE GOES HERE
# set up CMS environment

export SCRAM_ARCH=slc5_amd64_gcc462

source $VO_CMS_SW_DIR/cmsset_default.sh

cd /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/

eval `scramv1 runtime -sh`

cd $WORKDIR

export LD_LIBRARY_PATH=/swshare/glite/d-cache/dcap/lib/:$LD_LIBRARY_PATH
rootoutfiles=""
counter=0

echo "Getting started"

cd /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans
i=$1
ConfigFile=Point2_Configuration${i}.cfg
RSEED=$RANDOM

command="cat MarcoScan20000.cfg | sed 's/RANDOMSEED/"$RSEED"/' | sed 's|LHEPATH|"$WORKDIR"/|g' > $ConfigFile"
echo $command
eval $command

echo "Started with random seed $RSEED"
echo "Now going to create a scan here in `pwd`"
python createScan.py $ConfigFile
cp $SLHAfile Point2_Configuration${i}/SLHA/files/Scenario_iteration${i}_0.slha
cp $SLHAfile Point2_Configuration${i}/SLHA/files/Scenario_iteration${i}_1.slha
cp $SLHAfile Point2_Configuration${i}/SLHA/files/Scenario_iteration${i}_2.slha
cp $SLHAfile Point2_Configuration${i}/SLHA/files/Scenario_iteration${i}_3.slha
cd Point2_Configuration${i}/LHE/
cat createLHEs.py | sed 's/allSlhas.sort/#allSlhas.sort/' > ModifiedcreateLHEs.py
python ModifiedcreateLHEs.py 1
cd ../AODSIM/

AODDir=`pwd`

lheFILE=`ls $WORKDIR | grep lhe`

NEvents=`cat $WORKDIR/$lheFILE | grep "<event>" | wc -l`  #this counts the number of events in the LHE file

command="cat LHEToAODSIM_cfg.py | sed 's/input = cms.untracked.int32(1)/input = cms.untracked.int32($NEvents)/' > LHEToAODSIM_cfg_Adapted.py"
echo "About to issue command $command"
eval $command

cd ../
CONFIGDIR=`pwd`
cd $WORKDIR
command="cmsRun $AODDir/LHEToAODSIM_cfg_Adapted.py lheFile=$WORKDIR/$lheFILE"
echo "About to issue the following command: $command"
eval $command
AODOUT=`pwd`/`ls | grep root`
echo "Produced a file named $AODOUT"
echo "About to clear the config directory $CONFIGDIR"
rm -r $CONFIGDIR
echo "About to clear the lhe file: $WORKDIR/$lheFILE"
rm $WORKDIR/$lheFILE
echo "About to process the output root file $AODOUT"
echo "Now need to use /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/DiLeptonAnalysis/NTupleProducer/test/Zombientuple_cfg.py to produce an ntupleproducer file!"
command="cmsRun /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/DiLeptonAnalysis/NTupleProducer/test/Zombientuple_cfg.py runon=mc FastSim=True files=file:///$AODOUT"
echo "Running command $command"
eval $command

ntuple=`ls | grep NTupleProducer`
SimRootFile=SimSample_$1.root

mv $ntuple $SimRootFile

echo "Now going to upload the output file from NTupleProducer: ($ntuple moved to ) $SimRootFile"
command="lcg-cp -b -D srmv2 /$WORKDIR/$SimRootFile $SERESULTDIR/$SimRootFile"
echo $command
eval $command


echo "Cleaning up $WORKDIR"
rm -rf $WORKDIR

###########################################################################
DATE_END=`date +%s`
RUNTIME=$((DATE_END-DATE_START))
echo "################################################################"
echo "Job finished at " `date`
echo "Wallclock running time: $RUNTIME s"
exit 0
