#!/bin/bash
#################################
# PSI Tier-3 example batch Job  #
#################################

##### SWITCH THIS ON IF YOU WANT TO SCAN THE VICINITY OF A PRE-DEFINED POINT
VicinityScan=0

##### CONFIGURATION ##############################################
# Output files to be copied back to the User Interface
# (the file path must be given relative to the working directory)
OUTFILES=

# Output files to be copied to the SE
# (as above the file path must be given relative to the working directory)
SEOUTFILES="Result_$1.tar"

SOURCEFILES=sourcefile

#
# By default, the files will be copied to $USER_SRM_HOME/$username/$JOBDIR,
# but here you can define the subdirectory under your SE storage path
# to which files will be copied (uncomment line)
#SEUSERSUBDIR="mytestsubdir/somedir"
#
# User's CMS hypernews name (needed for user's SE storage home path
# USER_SRM_HOME below)

HN_NAME=buchmann

# set DBG=1 for additional debug output in the job report files
# DBG=2 will also give detailed output on SRM operations
DBG=0

#### The following configurations you should not need to change
# The SE's user home area (SRMv2 URL)
USER_SRM_HOME="srm://t3se01.psi.ch:8443/srm/managerv2?SFN=/pnfs/psi.ch/cms/trivcat/store/user/"

# Top working directory on worker node's local disk. The batch
# job working directory will be created below this
TOPWORKDIR=/scratch/`whoami`

# Basename of job sandbox (job workdir will be $TOPWORKDIR/$JOBDIR)
JOBDIR=pMSSM_Interpretation_Ready_$1

SEUSERSUBDIR=pMSSM_Interpretation_GlobalScan_Ready

##################################################################

ARGUMENTS=""

RELEASE="/shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher"


############ BATCH QUEUE DIRECTIVES ##############################
# Lines beginning with #$ are used to set options for the SGE
# queueing system (same as specifying these options to the qsub
# command

# Job name (defines name seen in monitoring by qstat and the
#     job script's stderr/stdout names)
#$ -N example_job

# Run time soft and hard limits hh:mm:ss

### Specify the queue on which to run
#$ -q all.q

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
WORKDIR=$TOPWORKDIR/$JOBDIR
RESULTDIR=$STARTDIR/$JOBDIR
if test x"$SEUSERSUBDIR" = x; then
   SERESULTDIR=$USER_SRM_HOME/$HN_NAME/ProcessedTrees/$JOBDIR
else
   SERESULTDIR=$USER_SRM_HOME/$HN_NAME/ProcessedTrees/$SEUSERSUBDIR
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

cd $RELEASE

eval `scramv1 runtime -sh`

cd $WORKDIR


MaxTime=30 ## maximum time for a crab job (in minutes)

function check_time {
  Initial=$1
  Now=$(date +"%s")
  diff=$(($Now-$Initial))
  echo "$(($diff / 60)) minutes and $(($diff % 60)) seconds elapsed."
  MaxRealTime=$((60*MaxTime))
  if [[ $diff -gt $MaxRealTime ]]; then
    echo "Oops, seems like we're above threshold. Need to wrap up, start by packing our bags"
    return 1
  fi
  echo "Timing is still ok. We're currently at $diff of $MaxRealTime seconds ... "
  return 0
}



REFDIR="/shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/usercode/pMSSMScanner/NewGeneration/"



#  _____ _               __ 
# / ____| |             /_ |
#| (___ | |_ ___ _ __    | |
# \___ \| __/ _ \ '_ \   | |
# ____) | ||  __/ |_) |  | |
#|_____/ \__\___| .__/   |_|
#               | |         
#               |_|         
#   SETUP


cd $WORKDIR


echo "Need to do the setup"

mkdir -p Candidates
rm -f Candidates/*
rm -f MyParameters.txt
rm -f KS.txt
cp ${REFDIR}/SuggestAPoint.exec .
cp /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/superiso_v3.3/slha.x .
cp ${REFDIR}/IsPointAcceptable.exec .
cp /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/usercode/pMSSMScanner/DifferentialDistributions.root .
cp /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/usercode/pMSSMScanner/NewGeneration/RecentPointStorage.txt .

#  _____ _               ___  
# / ____| |             |__ \ 
#| (___ | |_ ___ _ __      ) |
# \___ \| __/ _ \ '_ \    / / 
# ____) | ||  __/ |_) |  / /_ 
#|_____/ \__\___| .__/  |____|
#               | |           
#               |_|           
#
# INITIALIZE CLOCK, GENERATE POINTS


Initialtime=$(date +"%s")
FullInitialtime=`date +%H:%M:%S`
echo "Initial time : $Initialtime which actually happens to be $FullInitialtime"

for model in {1..10000}; do
  if [[ $VicinityScan -lt 1 ]]; then 
    rm RecentPointStorage.txt
    echo "Removed recent point storage file as we're not doing a vicinity scan"
  fi
  echo "************************************************************************************************************************************"
  echo "************************************************************************************************************************************"
  echo "************************************************************************************************************************************"
  echo "Checking model $model"
  echo "************************************************************************************************************************************"
  echo "************************************************************************************************************************************"
  echo "************************************************************************************************************************************"
  check_time $Initialtime
  CanContinue=$?
  if [[ $CanContinue -gt 0 ]]; then 
    break
  fi
  
  ./SuggestAPoint.exec `pwd` > MicroLog.txt
  
  echo "Will now check constraints on model (using SuperISO)"
  ./slha.x susyhit_slha.out | tee Constraints.txt
  ./IsPointAcceptable.exec `pwd`/Constraints.txt
  Response=`cat IsAcceptable.txt`
  rm IsAcceptable.txt
  
  if [[ $Response -lt 1 ]]; then
    echo "Boundaried not respected, rejecting this point (response was $Response)"
  else
    SLHAname="`date +%H_%M_%S_%N`__${RANDOM}.slha"
    mv ${WORKDIR}/susyhit_slha.out /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/$SLHAname
    echo "The output (susyhit_slha_out) is now called $SLHAname and is at /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/$SLHAname : "
    ls -ltrh /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/$SLHAname
    python ${REFDIR}/Analyze_This_SLHA.py /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/$SLHAname
    Marker=`cat Marker.txt`
    echo "Have gotten Marker with information $Marker"
    ${REFDIR}/StoreThisPoint.exec `pwd`/KS_Summary.txt
    NOW=`date +%Y%m%d_%H%M%S`
    mv /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/$SLHAname $WORKDIR/Candidates/${Marker}_${1}_${NOW}
  fi
  
done

echo "Ok, time is up. now need to pack up and leave"

#  _____ _               ____  
# / ____| |             |___ \ 
#| (___ | |_ ___ _ __     __) |
# \___ \| __/ _ \ '_ \   |__ < 
# ____) | ||  __/ |_) |  ___) |
#|_____/ \__\___| .__/  |____/ 
#               | |            
#               |_|            
# TAR & GO

ls -ltrh 
${REFDIR}/LogToRoot.exec
cp pointlog.txt Candidates/pointlog.txt
mv pMSSMpoints.root Candidates/Overview.root

mv ConstraintLog.txt Candidates/
#mv HiggsPoints.root Candidates/
tar cvf $SEOUTFILES Candidates/
echo "Candidates have been packed. ready to leave."

TARname="`date +%H_%M_%S_%N`__${RANDOM}.tar"
mkdir -p /shome/buchmann/Candidates/${JOBDIR}/
cp $SEOUTFILES /shome/buchmann/Candidates/${JOBDIR}/${TARname}

#### RETRIEVAL OF OUTPUT FILES AND CLEANING UP ############################
cd $WORKDIR
if test 0"$DBG" -gt 0; then
    echo "########################################################"
    echo "############# Working directory contents ###############"
    echo "pwd: " `pwd`
    ls -Rl
    echo "########################################################"
    echo "YOUR OUTPUT WILL BE MOVED TO $RESULTDIR"
    echo "########################################################"
fi

if test -e $SEOUTFILES; then
   if test 0"$DBG" -ge 2; then
      srmdebug="-v"
   fi
   for n in $SEOUTFILES; do
       if test ! -e $WORKDIR/$n; then
          echo "WARNING: Cannot find output file $WORKDIR/$n. Ignoring it" >&2
       else
          lcg-cp -b -D srmv2 $srmdebug file:////$WORKDIR/$n $SERESULTDIR/$n
          if test $? -ne 0; then
             echo "ERROR: Failed to copy $WORKDIR/$n to $SERESULTDIR/Tars/$n" >&2
          fi
          lcg-cp -b -D srmv2 $srmdebug file:////$WORKDIR/Candidates/Overview.root $SERESULTDIR/Overview/pMSSMpoints_${1}.root
          if test $? -ne 0; then
             echo "ERROR: Failed to copy $WORKDIR/$n to $SERESULTDIR/$n" >&2
          fi
   fi
   done
fi

echo "Cleaning up $WORKDIR"
rm -rf $WORKDIR

###########################################################################
DATE_END=`date +%s`
RUNTIME=$((DATE_END-DATE_START))
echo "################################################################"
echo "Job finished at " `date`
echo "Wallclock running time: $RUNTIME s"
exit 0
