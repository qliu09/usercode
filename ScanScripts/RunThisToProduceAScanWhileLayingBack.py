#!/usr/bin/python

import os
import commands
from os import popen
import time
from sys import stdout
import sys 
jobnumbercollection=[]


def checklist(joblist,currlist):
  jobfinished=[]
  for i in joblist:
    if i in currlist: ## if the job is still active
      currlist.remove(int(i))## we don't need this anymore on the list of currently running jobs (we already know it's running)
    else:#job is finished
      jobfinished.append(int(i)) 
  for job in jobfinished:
    joblist.remove(int(job))
  stdout.write("\r Jobs left: "+str(len(joblist))+" / " +str(totaljobnumber))
  stdout.flush()




#slha_path_valid=False
#while not slha_path_valid:
#  slha_path=raw_input('Please provide the slha file path:')
#  if os.path.exists(slha_path):
#    slha_path_valid=True
#  else:
#    print "Sorry, the provided path does not exist"
#
#print "You have provided the path "+slha_path

print "You have to provide three arguments: mglu mchi mlsp"

mglu=int(sys.argv[1])
mchi=int(sys.argv[2])
mlsp=int(sys.argv[3])

if mglu<mchi or mchi<mlsp or mglu<mlsp:
  print "The order needs to be mglu > mchi > mlsp"
  sys.exit(0)
  
if mchi-mlsp<100:
  print "Please allow for at least 100 GeV between the chi and the lsp ... "
  sys.exit(0)



NEvents=20000
    
NJobsToSubmit=NEvents/200

print "Will submit "+str(NJobsToSubmit)+" which will each generate 200 events"

T5zzname="T5zz_"+str(mglu)+"_"+str(mchi)+"_"+str(mlsp)

ExecutableName="WrapItAll_"+T5zzname+".sh"


command = "cat /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/WrapItAll.sh_TEMPLATE | sed 's/POINTNAME/"+str(T5zzname)+"/' | sed 's/DEFINEMGLU/"+str(mglu)+"/' | sed 's/DEFINEMCHI/"+str(mchi)+"/' | sed 's/DEFINEMLSP/"+str(mlsp)+"/' > "+ExecutableName
#print command
commands.getoutput(command)

commands.getoutput("mkdir -p /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/Logs/"+T5zzname);

for ijob in range(0, int(NJobsToSubmit)):
  cmd="qsub -q all.q -e /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/Logs/"+T5zzname+"/stderr_"+str(ijob)+".txt -o /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/Logs/"+T5zzname+"/stdout_"+str(ijob)+".txt -N "+T5zzname+"_J"+str(ijob)+" /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/"+ExecutableName+" "+ str(ijob)
  print cmd
  pipe=popen(cmd)
  for l in pipe.readlines():
    if l.find("Your job")>-1:
      thisjobnumber=int(l[l.index('job ')+4:l.index(' (')])
      print "Submitted job "+str(ijob)+" with job number: "+str(thisjobnumber)
      jobnumbercollection.append(thisjobnumber)
        
counter=0
totaljobnumber=len(jobnumbercollection)
print 'Total number of jobs:',totaljobnumber
#while(len(jobnumbercollection)>0 and counter<3000) :
#  time.sleep(60)
#  counter+=1
#  currlist=[]
#  pipe=popen("qstat | grep `whoami` | awk '{print $1}'")
#  for line in pipe.readlines():
#    currlist.append(int(line))
#  checklist(jobnumbercollection,currlist)
##  
#  
#print "All done - you can now let your analyzer loose on these ntuples :-)"
