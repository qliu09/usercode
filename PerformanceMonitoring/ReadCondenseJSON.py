#!/bin/env python

###whoevver is maintaining this code should put his email address here to be informed of errors!
youremail="marco.andrea.buchmann@cern.ch"

## This script loads the cache provided at /afs/cern.ch/user/s/spinoso/public/overview.cache and makes a JSON file from it, with all variables we need. 
## The advantage is that we only need to process each workflow once, and the user (online) will be presented with results very quickly instead of needing to 
## compare thousands of workflows. Therefore, two files are prepared: 
##  1) Workflow list (list of all workflows with relevant variables to check for matching workflows between releases)
##  2) Performance list (performance of each individual workflow)


try:
    import json
except ImportError:
    import simplejson as json
    print "Using simplejson"
from pprint import pprint

from PerformanceEvaluation import GetPerformance

import sys
import urllib2
import urllib
import httplib
import os
import smtplib
import string

from optparse import OptionParser


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''


def GetJobInformation(workflowname,conn):
    headers = {"Content-type": "application/json"}
    conn.request("GET", "/couchdb/workloadsummary/"+workflowname, None , headers)
    print "Getting JSON from /couchdb/workloadsummary/"+workflowname
    response = conn.getresponse()
    if response.status != 200:
        print "Couldn't contact the workqueue for info on the request"
        print "Reason: %s" % response.reason
        print "Message: %s" % response.msg  
        return ""
    print "JSON retrieved successfully"
    return response.read()

#def GetPerformanceVariables(information):
#  print bcolors.FAIL+" Still need to implement performance variables"+bcolors.ENDC
#  return len(information)


def EstablishConnection():
  url='cmsweb.cern.ch'
  if 'X509_USER_PROXY' in os.environ:
    return httplib.HTTPSConnection(url, cert_file = os.getenv('X509_USER_PROXY'), key_file = os.getenv('X509_USER_PROXY'))
  elif 'X509_USER_CERT' in os.environ and 'X509_USER_KEY' in os.environ:
    return httplib.HTTPSConnection(url, cert_file = os.getenv('X509_USER_CERT'), key_file = os.getenv('X509_USER_KEY'))
  else:
    print "You need a valid proxy or cert/key files"
    sys.exit(-1)


def decode_CMSSW_version_from_request(name):
  substring=name[name.find("CMSSW")+len("CMSSW"):len(name)]
  char_count=0
  last_char=""
  for char in substring:
    if char.isupper():
      if last_char == "_":
	return "CMSSW"+substring[0:char_count-1]
      else :
	return "CMSSW"+substring[0:char_count]
    char_count+=1
    last_char=char
  return name

  
def ExtractWorkFlow(name):
  substring=name[1+name.find("CMSSW")+len("CMSSW"):len(name)]
  char_count=0
  for char in substring:
    if char.isupper():
      substring=substring[char_count:len(substring)]
      break
    char_count+=1
  char_count=0
  underline_counter=0
  for char in reversed(substring):
    if char=="_":
      underline_counter+=1
      if underline_counter == 3:
	return substring[0:len(substring)-char_count-1]
    char_count+=1
  return substring

def ExtractValue(line) :
  return line[line.find(" = ")+3:].replace("'", "").replace("[", "").replace("]", "")
  
  
def AlarmingError(workflow, function, message) :
  SUBJECT = "ReadCondenseJSON :: "+function+" encountered a problem"
  TO = youremail
  FROM = "scaredycat@cern.ch"
  text = "ReadCondenseJSON :: "+function+" encountered a problem : "+message
  BODY = string.join((
        "From: %s" % FROM,
        "To: %s" % TO,
        "Subject: %s" % SUBJECT ,
        "",
        text
        ), "\r\n")
  server = smtplib.SMTP('localhost')
  server.sendmail(FROM, [TO], BODY)
  server.quit()
  
def GetAllTaskInformation(request_name, connection):
#  print "Going to obtain information pertaining to task "+request_name
  headers = {"Content-type": "text/html"}
#  print "/reqmgr/view/showWorkload?requestName="+request_name
  conn.request("GET", "/reqmgr/view/showWorkload?requestName="+request_name, None, headers)
  response = conn.getresponse()
  if response.status != 200:
        print "Couldn't contact the workqueue for info on the request"
        print "Reason: %s" % response.reason
        print "Message: %s" % response.msg  
        return ""
  fullcontent=response.read()
  fullcontent=fullcontent.split("<br/>")
  InputDataset=""
  WhiteList=""
  DashboardActivity=""
  SubTasks=[]
  for line in fullcontent:
    if line.find("request.schema.Task1.InputDataset") > -1:
#      print "InputDataset from here: "+line
      InputDataset=ExtractValue(line)
    if line.find("request.schema.dashboardActivity") > -1:
#      print "InputDataset from here: "+line
      DashboardActivity=ExtractValue(line)
    if line.find("input.dataset.runs.whitelist") > -1:
      WhiteList=ExtractValue(line)
#      print "dataset runs whitelist from here: "+line
    if line.find("request.schema.Task") > -1 and line.find("TaskName") > -1 : 
#      print "Find sub tasks from this line: "+line
      SubTasks.append(ExtractValue(line))
  if len(SubTasks) < 1 :
    print "SubTasks is empty ?!?!?!?!"
    AlarmingError(request_name,"GetAllTaskInformation","SubTasks is empty?!?!");

#  print "Read a really long document with "+str(len(fullcontent))+" lines"
#  print "Input Dataset: "+InputDataset
#  print "WhiteList : "+WhiteList
#  print "SubTasks : "+str(SubTasks)
  
  AdditionalInformation={};
  AdditionalInformation["InputDataset"]=InputDataset
  AdditionalInformation["WhiteList"]=WhiteList
  AdditionalInformation["SubTasks"]=SubTasks
  AdditionalInformation["DashboardActivity"]=DashboardActivity
#  print AdditionalInformation
  return AdditionalInformation
  
print "Hello."

try:
    from socket import ssl
except ImportError:
    print "error: no ssl support"

def LoadJsonFromFile(Source):
  fullstring=""
  try:
    OpenedFile = open(Source)
    for line in OpenedFile:
      line=line.encode()
      line=line.replace("u'","'")
      betterline=line.replace("'","\"")
      betterline=betterline.replace("None","\"None\"")
      fullstring+=betterline.replace("\"\"None\"\"","\"None\"")
  finally:
    OpenedFile.close()
  if Source=="/afs/cern.ch/user/b/buchmann/public/WorkFlowList.json":
    print fullstring
  try:
    content=json.loads(fullstring)
    return json.loads(fullstring)
  except :
    print "PROBLEM WHILE READING JSON FROM "+Source
    AlarmingError("","LoadJsonFromFile","Problem while reading from "+Source)
    sys.exit(0)

def CreateReleaseSkeleton():
  print "No previous local database found - creating an empty skeleton"
  fempty = open('/afs/cern.ch/user/b/buchmann/public/WorkFlowList.json', 'w')
  EmptyJSON={}
  EmptyJSON["content"]="None"
  json.dump(EmptyJSON, fempty)
  fempty.close() 

print "Welcome to the json condenser ... will load all tasks, analyze them and spit out a compact file containing all the information we need"

## establish a connection to cmsweb.cern.ch first (for all JSONs and workflow information)
conn = EstablishConnection()


## parse options: maybe we want to rebuild the whole index (don't do this too often though ... )
DoCompleteReload=False
parser = OptionParser()
parser.add_option("-r", "--rebuild",action="store_true", dest="DoCompleteReload", default=False,help="Rebuild the whole database (don't use known data)")
(options, args) = parser.parse_args()
if options.DoCompleteReload : 
  DoCompleteReload=True


## load any previous databases we may have and load the list of workflows
json_object   = LoadJsonFromFile("/afs/cern.ch/user/s/spinoso/public/overview.cache");
if not os.path.exists("/afs/cern.ch/user/b/buchmann/public/WorkFlowList.json"):
  print "There is no /afs/cern.ch/user/b/buchmann/public/WorkFlowList.json - need to create it first"
  CreateReleaseSkeleton()
  
old_jobsinfo  = LoadJsonFromFile("/afs/cern.ch/user/b/buchmann/public/WorkFlowList.json");




NTasks=0
NAcceptedTasks=0
TaskCollection={}
SimpleTaskCollection={}
newTasks=0
maxTasks=10 # maximum number of tasks to be loaded (for testing; for production set this to -1)

## and now let's look at all those workflows and retrieve the necessary information

for task in json_object: 
  if maxTasks > 0 and newTasks >= maxTasks: 
      print "Putting an end to this ... "
      break

  NTasks+=1
  if not "type" in task or task["type"] != "TaskChain": 
    continue
  CMSSW_version=""
  campaign=task["campaign"]
  status=task["status"]
  request_name=task["request_name"]
  if request_name.find("CMSSW") > -1: #if we can't find a CMSSW version, we can't compare it.
    CMSSW_version=decode_CMSSW_version_from_request(request_name)
  workflowname=ExtractWorkFlow(request_name)
  if not status == "announced":  #only interested in finished tasks
    continue
  
  #and now exclude any tests
  if request_name.find("test") > -1: 
    continue
  if request_name.find("Test") > -1: 
    continue
  if request_name.find("TEST") > -1: 
    continue
  
#  print status
#  print " CMSSW Version: "+str(CMSSW_version)
#  print " WorkFlow     : "+str(workflowname)
#  print " Type         : "+str(task["type"])
#  print "    #         : "+str(NAcceptedTasks)
#  print task
#  print " Type         : "+str(task["status"])
  
  if CMSSW_version == "": #making sure that we have a CMSSW version
    #print "******* SKIPPING ********"
    continue
  
  TaskIsNew=False
  
  if not CMSSW_version in old_jobsinfo:
    TaskIsNew=True
    print bcolors.WARNING + "*********** CMSSW version   "+str(CMSSW_version)+" is not in our old database yet" +bcolors.ENDC
    
  if CMSSW_version in old_jobsinfo:
    print bcolors.OKBLUE + "CMSSW version is in the old database" + bcolors.ENDC
    if not request_name in old_jobsinfo[CMSSW_version]:
      TaskIsNew=True
      print bcolors.FAIL + "Request name ("+request_name+") is not in old job info for version "+CMSSW_version+ bcolors.ENDC
    else:
      print bcolors.OKGREEN + "Request name ("+request_name+") is in old job info for version "+CMSSW_version+ bcolors.ENDC
  else:
    print bcolors.FAIL + "CMSSW version ("+str(CMSSW_version)+") was not in the old database" + bcolors.ENDC
  if DoCompleteReload:
    print "Rebuilding ... "
    TaskIsNew=True
  
  if not CMSSW_version in TaskCollection:
    TaskCollection[CMSSW_version]={}
    SimpleTaskCollection[CMSSW_version]={}
    
  TaskCollection[CMSSW_version][request_name]={}
  SimpleTaskCollection[CMSSW_version][request_name]={}
  TaskCollection[CMSSW_version][request_name]["RelValWorkFlow"]=workflowname
  SimpleTaskCollection[CMSSW_version][request_name]["RelValWorkFlow"]=workflowname
  
  if TaskIsNew:
    # load stuff from JSON (online)
    print "new: "+request_name
    newTasks+=1
    TaskCollection[CMSSW_version][request_name]["InDepthInformation"]=GetAllTaskInformation(request_name,conn)
    SimpleTaskCollection[CMSSW_version][request_name]["InDepthInformation"]=TaskCollection[CMSSW_version][request_name]["InDepthInformation"]
    TaskCollection[CMSSW_version][request_name]["Performance"]=GetPerformance(GetJobInformation(request_name,conn),request_name)
    #Note that SimpleTaskCollection does NOT contain performance information - as the name implies, it is a smaller version of the more complete TaskCollection :-)
  else:
#    load stuff directly from old file (i.e. all json stuff)
    TaskCollection[CMSSW_version][request_name]["InDepthInformation"]=old_jobsinfo[CMSSW_version][request_name]["InDepthInformation"]
    TaskCollection[CMSSW_version][request_name]["Performance"]=old_jobsinfo[CMSSW_version][request_name]["Performance"]
    SimpleTaskCollection[CMSSW_version][request_name]["InDepthInformation"]=TaskCollection[CMSSW_version][request_name]["InDepthInformation"]
    # note that the performance information is no longer stored in this json directly in order to save space
  SimpleTaskCollection[CMSSW_version][request_name]["InDepthInformation"]["errors"]=TaskCollection[CMSSW_version][request_name]["Performance"]["errors"];
    
  print "******************************************************************************************\n\n\n"
  
  
  NAcceptedTasks+=1
  
print "Processed "+str(NTasks)+" tasks (only "+str(NAcceptedTasks)+" were accepted)"
print "Had to load data for "+str(newTasks)+" new tasks"
print "remember that the old database doesn't really have any information so this needs to be switched back on  (look for \"TODO456\""

print "REFERENCE : "
print old_jobsinfo

fp = open('/afs/cern.ch/user/b/buchmann/public/WorkFlowList.json', 'w')
json.dump(SimpleTaskCollection, fp)

fp = open('/afs/cern.ch/user/b/buchmann/public/WorkFlowListIncludingPerformance.json', 'w')
json.dump(TaskCollection, fp)

print " ****** "
print " new " 
print SimpleTaskCollection

print " And now, to flood your screen, the complete version:"
print TaskCollection
