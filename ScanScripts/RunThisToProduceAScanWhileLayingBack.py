#!/usr/bin/python

import os
import commands
from os import popen
import time
from sys import stdout

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




slha_path_valid=False
while not slha_path_valid:
  slha_path=raw_input('Please provide the slha file path:')
  if os.path.exists(slha_path):
    slha_path_valid=True
  else:
    print "Sorry, the provided path does not exist"

print "You have provided the path "+slha_path


NEvents_valid=False
while not NEvents_valid:
  NEvents=float(raw_input('How many events would you like to generate?'))
  if NEvents>0 and NEvents<1000000:
    NEvents_valid=True
  else:
    print "Please specify a number in the 0<1000000 range ... anything larger takes too long, anything smaller doesn't make sense"

    
NJobsToSubmit=NEvents/200

print "Will submit "+str(NJobsToSubmit)+" which will each generate 200 events"

CopyCommand="cp "+slha_path+" MagicalPoint/ThisIsIt.slha"
commands.getoutput(CopyCommand)

for ijob in range(0, int(NJobsToSubmit)):
  cmd="qsub -q short.q -e /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/Logs/stdout_"+str(ijob)+".txt -o /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/Logs/stdout_"+str(ijob)+".txt -N Aora_"+str(ijob)+" /shome/buchmann/Year_Of_Discovery/ScanProduction/CMSSW_5_3_2_patch4/src/UserCode/Scans/WrapItAll.sh "+ str(ijob)
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
while(len(jobnumbercollection)>0 and counter<300) :
  time.sleep(60)
  counter+=1
  currlist=[]
  pipe=popen("qstat | grep `whoami` | awk '{print $1}'")
  for line in pipe.readlines():
    currlist.append(int(line))
  checklist(jobnumbercollection,currlist)
  
  
print "All done - you can now let your analyzer loose on these ntuples :-)"
