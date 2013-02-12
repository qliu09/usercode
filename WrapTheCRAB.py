#!/usr/bin/python

import sys
import time
from os import popen, path
from sys import stdout

from Countdown import *
from optparse import OptionParser
from datetime import datetime

CheckInterval=60 # (interval between checks, in minutes)

JobDatabase={}
#structure:
#   JobDatabase[TaskName][JobNumber][Information] where information=Ntries

TaskDatabase={}
#structure:
#   TaskDatabase[TaskName][Information]   (where Information= Cleared,Running,...)

DoResubmit=False

def DoTaskResubmission(ResubmissionJobList,TaskName):
    if DoResubmit == False:
        print "     You've chosen to disable resubmission. Won't resubmit anything."
        return
    
    command="crab -c "+TaskName+" -forceResubmit "+str(ResubmissionJobList)+" --GRID.ce_blacklist=*.in,*.ee,*.ru"
    print "     About to resubmit "+str(len(ResubmissionJobList.split(",")))+" jobs ("+str(ResubmissionJobList)+") for task "+TaskName
#    print " DEBUG :::::  command is "+str(command)
    pipe=popen(command)
    for cline in pipe.readlines():
        if cline.find("ubmit") > -1:
            print cline

def RetrieveJobs(TaskName):
    print "     Going to retrieve "+str(TaskDatabase[TaskName]["Done"]) + " jobs : "
    Ntotal=TaskDatabase[TaskName]["Done"]
    command="crab -c "+TaskName+" -get"
    Ndownloaded=0
    pipe = popen(command)
    for pline in pipe.readlines():
        if pline.find("Results") == -1:
            continue
        Res=pline.split(" ")
        HaveFound=False
        JobNumber=0
        for entry in Res:
            if HaveFound == True:
                JobNumber=entry
                HaveFound=False
            if entry.find("#") > -1:
                HaveFound=True
        OutString="\r    Downloading results for job "+str(JobNumber)+" (progress: downloaded "+str(Ndownloaded)+" of "+str(Ntotal)+") \r"
        stdout.write(OutString)
        stdout.flush()
        Ndownloaded+=1
    print "\033[2K     Downloaded "+str(Ndownloaded)+" results out of "+str(Ntotal)+" (any discrepancy here may hint at corrupted output which will be detected later)"


def process_job(TaskName):
    
    if not "TaskIsDone" in TaskDatabase[TaskName]:
        TaskDatabase[TaskName]["TaskIsDone"]=False
    
    if TaskDatabase[TaskName]["TaskIsDone"]==True:
        return

    print "\n     **************"
    print "     Checking job: "+TaskName
    cmd="crab -c "+TaskName+" -status"

    # Need to reset all counts first ...
    for entry in TaskDatabase[TaskName]:
        if entry.find("TaskIsDone") > -1:
            continue
        TaskDatabase[TaskName][entry]=0

#    cmd="cat OtherReport.txt"
    nLine=0
    isReal=False
    ResubmissionJobList=""
    nJobs=0
    try:
      pipe=popen(cmd)
    except:
      print "THERE HAS BEEN AN ERROR - MAYBE THE LINES BELOW WRITTEN TO THE PIPE PROVIDE AN ANSWER"
      for line in pipe.readlines():
	print "Err line: "+str(line)
      print "THERE HAS BEEN AN ERROR - MAYBE THE LINES ABOVE WRITTEN TO THE PIPE PROVIDE AN ANSWER"
      return
	
    NSuccess=0
    for line in pipe.readlines():
        if line.find("Wrapper") > -1:
            result = line.split(" ")
            NextIsIt=False
            TempNSuccess=result[2]
            ExitCode=123456
            for entry in result:
                if NextIsIt==True:
                    ExitCode=entry
                    NextIsIt=False
                if entry.find(":") > -1:
                    NextIsIt=True
            if len(ExitCode) > 0 and float(ExitCode) == 0:
                NSuccess=int(TempNSuccess)
        if line.find("http://") > -1 or line.find("follow the status") > -1 or line.find("task name is") > -1 or line.find("Log file is") > -1 or line.find("Total Jobs") > -1 or line.find("Jobs Submitted") > -1 or line.find("List of jobs") > -1:
#	  print "Skipping line: "+line
	  continue
        if line.find("ExeExitCode") > -1:
            isReal=True
            continue
        if isReal==False:
            continue
        if line.find("---") > -1:
            continue
        nLine=nLine+1
#        print str(nLine)+" : "+line
        res = line.split();
        #        print line
        if(len(line)) < 2:
            continue
        if line.find("Summary") > -1:
            isReal=False
            continue
        nJobs+=1
#        print "Increased job number to "+str(nJobs)
        JobNumber   = res[0]
        End         = res[1]
        Status      = res[2]
        if(len(res)>4) :
            Action      = res[3]
        if(len(res)>5 and res[4].isdigit()):
            ExeExitCode = float(res[4])
        else:
            ExeExitCode = -1
        if(len(res)>5 and res[5].isdigit()) :
            JobExitCode = float(res[5])
        else:
            JobExitCode = -1

#        print "JN "+str(JobNumber)+" END ("+End+") Status ("+Status+") Action ("+Action+") Exe ("+str(ExeExitCode)+") Job ("+str(JobExitCode)+")"
        if not JobNumber in JobDatabase[TaskName]:
            JobDatabase[TaskName][JobNumber]={}

        if not "NRetries" in JobDatabase[TaskName]:
            JobDatabase[TaskName][JobNumber]["NRetries"]=0

        if not Status in TaskDatabase[TaskName]:
            TaskDatabase[TaskName][Status]=1
        else:
            TaskDatabase[TaskName][Status]+=1
    
        if not "NScheduled" in JobDatabase[TaskName][JobNumber]:
            JobDatabase[TaskName][JobNumber]["NScheduled"]=0
    
        ScheduledForResubmission=False

        if Status=="Aborted" and not ScheduledForResubmission:
            ResubmissionJobList+=","+str(JobNumber)
            JobDatabase[TaskName][JobNumber]["NRetries"]+=1
            print "     Added this job for resubmission: "+line.rstrip()
            ScheduledForResubmission=True
        if Status=="Done" and ExeExitCode > 0 and not ScheduledForResubmission:
            ResubmissionJobList+=","+str(JobNumber)
            JobDatabase[TaskName][JobNumber]["NRetries"]+=1
            print "     Added this job for resubmission: "+line.rstrip()
            ScheduledForResubmission=True
        if Status=="Done" and JobExitCode > 0 and not ScheduledForResubmission:
            ResubmissionJobList+=","+str(JobNumber)
            JobDatabase[TaskName][JobNumber]["NRetries"]+=1
            print "     Added this job for resubmission: "+line.rstrip()
            ScheduledForResubmission=True
        if Status=="Created":
            ResubmissionJobList+=","+str(JobNumber)
            JobDatabase[TaskName][JobNumber]["NRetries"]+=1
            print "     Added this job for resubmission: "+line.rstrip()
            ScheduledForResubmission=True
        if Status=="Cleared" and ExeExitCode > 0 and not ScheduledForResubmission:
            ResubmissionJobList+=","+str(JobNumber)
            JobDatabase[TaskName][JobNumber]["NRetries"]+=1
            print "     Added this job for resubmission: "+line.rstrip()
            ScheduledForResubmission=True
        if Status=="Cleared" and JobExitCode > 0 and not ScheduledForResubmission:
            ResubmissionJobList+=","+str(JobNumber)
            JobDatabase[TaskName][JobNumber]["NRetries"]+=1
            print "     Added this job for resubmission: "+line.rstrip()
            ScheduledForResubmission=True
        if Status=="Scheduled":
            JobDatabase[TaskName][JobNumber]["NScheduled"]+=1
            if JobDatabase[TaskName][JobNumber]["NScheduled"] > 300/CheckInterval: ## if the job has been scheduled for 5 hours resubmit it
		print "     Added this job for resubmission: "+line.rstrip()+"    [patience with this job ended, hence the resubmission]"
                ResubmissionJobList+=","+str(JobNumber)
                JobDatabase[TaskName][JobNumber]["NRetries"]==0
                ScheduledForResubmission=True


    ResubmissionJobList=ResubmissionJobList[1:len(ResubmissionJobList)]
    if len(ResubmissionJobList) > 1:
        DoTaskResubmission(ResubmissionJobList,TaskName)
    
    if not "Done" in TaskDatabase[TaskName]:
        TaskDatabase[TaskName]["Done"]=0

    if TaskDatabase[TaskName]["Done"] > 0:
        RetrieveJobs(TaskName)

    if nJobs > 0: 
      if not "NJobsPleasingCRAB" in TaskDatabase[TaskName]:
        TaskDatabase[TaskName]["NJobsPleasingCRAB"]=0
      Improvement=int(NSuccess)-int(TaskDatabase[TaskName]["NJobsPleasingCRAB"]);
      print "    \033[1m Status summary: \033[0m "+str(NSuccess)+" out of "+str(nJobs)+" are completely and successfully done ("+str(100*NSuccess/nJobs)+" %)   ; Improvement since last one: "+str(Improvement)
      TaskDatabase[TaskName]["NJobsPleasingCRAB"]=int(NSuccess)
      TaskDatabase[TaskName]["nJobs"]=int(nJobs)
      
    else:
      print "     There seems to have been a problem when attempting to determine the number of jobs. This hints at an unknown situation, please let me know!"
      TaskDatabase[TaskName]["nJobs"]=int(0)
    if nJobs == NSuccess and nJobs > 0:
        print "     \033[32 This job has finished - removing it from the list. \033[0m "
        TaskDatabase[TaskName]["TaskIsDone"]=True
    print "        Rest: "+str(TaskDatabase[TaskName])

def ReadFromMulticrab():
    if(os.path.exists("multicrab.cfg")):
        print "Going to load jobs from multicrab.cfg"
    else:
        print "There is no multicrab.cfg to load anything from!"
        return
    pipe=popen("cat multicrab.cfg")
    NewEntry=""
    for line in pipe.readlines():
        if line.find("#") > -1:
            continue
        if line.find("[") > -1:
            line=line[1:len(line)-2]
            if line == "MULTICRAB" or line == "COMMON":
                continue
            if(os.path.isdir(line)): 
	      JobDatabase[line]={}
	      TaskDatabase[line]={}
	      print "Added entry for '"+line+"'"
	    else: 
	      print "Found entry for "+line+" but there was no directory to go with it. Skipping "+line+"!"


def main() :
    print "Reading argument list:"
    
    usage = "python "+sys.argv[0] +""" directory1 directory2 ... OR you can use the -m option to provide a multicrab file; furthermore -r allows resubmission and -t kills the initial timeout"""

    parser = OptionParser(usage = usage, version="%prog 0.1")
    parser.add_option("-r","--resubmit",
                  action="store_true", dest="doresubmit", default=False,
                  help="If activated, resubmits failed and aborted jobs")
    parser.add_option("-t","--notimeout",
                      action="store_true", dest="notimeout", default=False,
                      help="Eliminates, if activated, the initial timeout of 2 min")
    parser.add_option("-m","--multicrab",
                      action="store_true", dest="multicrab", default=False,
                      help="Read entries from multicrab.cfg file")
    (options, args) = parser.parse_args()
    
    global DoResubmit
    DoResubmit+=options.doresubmit
    print "Overview of running options: "
    print "    Allow to resubmit?   "+str(int(DoResubmit))+"    (option r = resubmit)"
    print "    Disable timeout ?    "+str(int(options.notimeout))+"    (option t = no timeout)"
    print "    Read from multicrab? "+str(int(options.multicrab))+"    (option m = read from multicrab file) \n"
    
    
    for arg in args:
        if os.path.isdir(arg) and os.path.isfile(arg+"/share/default.tgz"): # making sure that the directory exists and that it is a crab job
            JobDatabase[arg]={}
            TaskDatabase[arg]={}
        else:
            print "The supplied argument "+arg+" is not a valid task; it will not be considered. \n"

    if options.multicrab:
        ReadFromMulticrab()
                
    if len(TaskDatabase) < 1:
        print "No valid tasks have been found. Try again."
        sys.exit(-1)
    
    print "The following tasks are going to get checked:"
    taskCounter=0
    for task in TaskDatabase:
        taskCounter+=1
        print str(taskCounter)+" ) "+task

    if not options.notimeout==True:
        print "\n"
        Countdown(120)


    for i in range (0,3*24*60/CheckInterval): #check for up to 3 days
        print "\n"
        print " \033[1m ***************************************************************************** \033[0m "
        d=datetime.now()
        print 
        print "                          Check #"+str(i+1)+"   (at "+str(d.hour)+":"+str(d.minute)+":"+str(d.second)+"  on  "+str(d.month)+" / "+str(d.day)+" / "+str(d.year)+"\n\n"
        
        for task in TaskDatabase:
            process_job(task)
        print "\n\n\n"
        print "     Check summary: "
        for task in TaskDatabase: 
	    if "nJobs" in TaskDatabase[task] and TaskDatabase[task]["nJobs"]>0: 
	      print "        "+task+" : "+str(100*TaskDatabase[task]["NJobsPleasingCRAB"]/float(TaskDatabase[task]["nJobs"]))+" % done"
	    else: 
	      print "        "+task+" : experienced problems while retrieving information"
        print "\n\n\n"
        Countdown(CheckInterval*60)

    





if __name__ == "__main__":
    main()
