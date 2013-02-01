#!/bin/python



try:
    import json
except ImportError:
    import simplejson as json
from pprint import pprint
try:
    import math
except ImportError:
    print "Problem while attempting to import math module. Cannot continue without it, sorry"
    sys.exit(0)
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
from math import log10, floor
def round_sig(x, sig=4):
  if x==0: 
    return float(0);
  if x<0:
    return float((round(x, sig-int(floor(log10(-x)))-1)))
  return float((round(x, sig-int(floor(log10(x)))-1)))

def LoadJsonFromFile(Source):
  fullstring=""
  OpenedFile = open(Source)
  
  for line in OpenedFile:
      line=line.encode()
      line=line.replace("u'","'")
      betterline=line.replace("'","\"")
      fullstring+=betterline.replace("None","\"None\"")
  OpenedFile.close()
  return json.loads(fullstring)

def process_histo(histo):
    #information we want: minimum, maximum, sum, mean, rms
    minimum=-1
    maximum=-1
    sum=0
    average=0
    rms=0
    totentries=0;
    for bin in histo:
         if bin["nEvents"] > 0:
             totentries+=bin["nEvents"]
             sum+=bin["average"]*bin["nEvents"]
             if minimum<0 or minimum>bin["average"]:
                minimum=bin["average"]
             if maximum<0 or maximum<bin["average"]:
                maximum=bin["average"]

    if(totentries>0): 
      average=round_sig(sum/totentries)
    else:
      average=0
    for bin in histo:
         if bin["nEvents"] > 0:
             rms+=(bin["average"]-float(average))*(bin["average"]-float(average))*bin["nEvents"]

    if(totentries>0) :
      rms/=totentries
    else:
      rms=0
    rms=pow(rms,0.5)
    rms=round_sig(rms)

#    print "                     Summary: Min:"+str(minimum)+"  , Max:"+str(maximum)+" average:"+str(average)+" mean:"+str(average)+" rms:"+str(rms)+"  sum:"+str(sum)
    HistoInfo = {}
    HistoInfo["min"]=round_sig(minimum)
    HistoInfo["max"]=round_sig(maximum)
    HistoInfo["average"]=round_sig(average)
    HistoInfo["rms"]=round_sig(rms)
    HistoInfo["sum"]=round_sig(sum)
    return HistoInfo

def SHORTCUT_GetPerformance(RAWjson,identifier):
  print bcolors.FAIL + "WATCH OUT - TAKING A SHORTCUT!" + bcolors.ENDC
  return {}

def GetPerformance(RAWjson,identifier):
#  print "Analyzing "+str(identifier)
  PrintWholeJSON=False
  BeVerbose=True
#  json = LoadJsonFromFile(identifier)
  JSON=json.loads(RAWjson)
  PerfVarSource=""
  for entry in JSON["performance"]:
    pentry=entry
    entry=entry.replace("/"+identifier+"/","")
    if(PrintWholeJSON) :
        print entry
    if entry.find("/") > -1:
        continue
    else:
        if PerfVarSource=="":
            PerfVarSource=pentry
        else:
            print "THERE ARE TWO POTENTIAL SOURCES FOR PERFORMANCE INFORMATION! There's "+PerfVarSource+" and "+entry+" ! Will use the first one ("+PerfVarSource+")"
        if(PrintWholeJSON):
          for info in JSON["performance"][pentry]["cmsRun1"]:
            print "           "+info
            for element in JSON["performance"][pentry]["cmsRun1"][info]:
                    print "                "+element
                    if element.find("histo") > -1:
                        histinfo=process_histo(JSON["performance"][pentry]["cmsRun1"][info][element]);

  PerformanceSummary={}
  print "Going to load performance information from "+PerfVarSource

  if "errors" in JSON:
    
    for error in JSON["errors"]:
      print "     Error:"+error
    PerformanceSummary["errors"]=len(JSON["errors"])
    if len(JSON["errors"])>0 :
      print "There were "+str(len(JSON["errors"]))+" errors. Cannot use this one"
      return PerformanceSummary
    
  if PerfVarSource in JSON["performance"]:
    if "cmsRun1" in JSON["performance"][PerfVarSource]:
        Performance=JSON["performance"][PerfVarSource]["cmsRun1"]
	
        if "PeakValueVsize" in Performance:
            HistoInformation=process_histo(Performance["PeakValueVsize"]["histogram"])
            PerformanceSummary["PeakValueVsizeAvg"]=HistoInformation["average"]
            PerformanceSummary["PeakValueVsizeRMS"]=HistoInformation["rms"]
        if "PeakValueRss" in Performance:
            HistoInformation=process_histo(Performance["PeakValueRss"]["histogram"])
            PerformanceSummary["PeakValueRssAvg"]=HistoInformation["average"]
            PerformanceSummary["PeakValueRssRMS"]=HistoInformation["rms"]
        if "TotalJobCPU" in Performance:
            PerformanceSummary["TotalJobCPU"]=round_sig(Performance["TotalJobCPU"]["average"])
        if "MinEventCPU" in Performance:
            PerformanceSummary["MinEventCPU"]=round_sig(Performance["MinEventCPU"]["average"])
        if "MaxEventCPU" in Performance:
            PerformanceSummary["MaxEventCPU"]=round_sig(Performance["MaxEventCPU"]["average"])
        if "AvgEventCPU" in Performance:
            PerformanceSummary["AvgEventCPU"]=round_sig(Performance["AvgEventCPU"]["average"])
        if "TotalEventCPU" in Performance:
            PerformanceSummary["TotalEventCPU"]=round_sig(Performance["TotalEventCPU"]["average"])
        if "writeTotalMB" in Performance:
            PerformanceSummary["writeTotalMB"]=round_sig(Performance["writeTotalMB"]["average"])
        if "jobTime" in Performance:
            PerformanceSummary["jobTime"]=round_sig(Performance["jobTime"]["average"])
        if "AvgEventTime" in Performance:
            HistoInformation=process_histo(Performance["AvgEventTime"]["histogram"])
            PerformanceSummary["AvgEventTimeAvg"]=HistoInformation["average"]
            PerformanceSummary["AvgEventTimeRMS"]=HistoInformation["rms"]
        if "MaxEventTime" in Performance:
            PerformanceSummary["MaxEventTime"]=round_sig(Performance["MaxEventTime"]["average"])
        if "MinEventTime" in Performance:
            PerformanceSummary["MinEventTime"]=round_sig(Performance["MinEventTime"]["average"])
        if "TotalJobTime" in Performance:
            HistoInformation=process_histo(Performance["TotalJobTime"]["histogram"])
            PerformanceSummary["TotalJobTimeAvg"]=HistoInformation["average"]
            PerformanceSummary["TotalJobTimeRMS"]=HistoInformation["rms"]
        if "readTotalMB" in Performance:
            PerformanceSummary["readTotalMB"]=round_sig(Performance["readTotalMB"]["average"])


    else:
        "cms run information is not available?! please check this ... "
        sys.exit(0)

  else:
    print "SOMETHING WEIRD HAPPENED HERE ... THE JSON IS NOT AVAILABLE ANYMORE?!"
    sys.exit(0)
  if "TotalJobTimeAvg" in PerformanceSummary and "TotalJobCPU" in PerformanceSummary:
    PerformanceSummary["CPUefficiency"]=PerformanceSummary["TotalJobCPU"]/PerformanceSummary["TotalJobTimeAvg"]
    print " ******* EFFICIENCY : "+str(PerformanceSummary["CPUefficiency"])+" ***********   (not understood yet)"
  if BeVerbose:
    print PerformanceSummary

  if not "errors" in PerformanceSummary:
    print "There are no errors in PerformanceSummary"
    print PerformanceSummary
    if PerformanceSummary["CPUefficiency"] > 1.0:
      print "Too efficient. stopping."
      sys.exit(0)

  return PerformanceSummary
#  print PerformanceSummary
## 1: Memory variables

#histo: PeakValueVsize   ok!
#histo: PeakValueRss     ok!


## 2: CPU variables
#scalar: TotalJobCPU     ok!
#scalar: MinEventCPU     ok!
#scalar: MaxEventCPU     ok!
#scalar: AvgEventCPU     ok!
#scalar: TotalEventCPU   ok!

## 3: General variables
#scalar: writeTotalMB    ok!
#scalar: jobTime         ok!

#histo: AvgEventTime     ok!
#scalar: MaxEventTime    ok!
#scalar: MinEventTime    ok!

#histo: TotalJobTime     ok!
#scalar: readTotalMB     ok!




#                        for bin in json["performance"][pentry]["cmsRun1"][info][element]:
#                            print "                       Bin "+str(bin)+ " contains "+str(json["performance"][pentry]["cmsRun1"][info][element][bin])
                            #                            print "                       "+str(json["performance"][pentry]["cmsRun1"][info][element])
                            #                            print bin
#                            print "bin low:"+str(bin["lowerEdge"])+" , bin high:"+str(bin["upperEdge"])+"   average: "+str(bin["average"])+"    Entries:"+str(bin["nEvents"])




IsTest=True
print "This file should be loaded as a part of ReadCondenseJSON.py"
#GetPerformance('vlimant_RVCMSSW_6_1_0_pre3JpsiMM_121009_142211_5090')
#GetPerformance('etorassa_EXO-Fall11_R4-01333_T1_IT_CNAF_MSS_batch21_v1__120310_144739')

if not IsTest:
    GetPerformance('etorassa_EXO-Fall11_R4-01320_T1_ES_PIC_MSS_batch20_v1__120306_011150')
    GetPerformance('etorassa_EXO-Fall11_R4-01304_T1_DE_KIT_MSS_batch19_v1__120301_222509')
    GetPerformance('etorassa_EXO-Fall11_R4-01282_T1_DE_KIT_MSS_batch19_v1__120301_222349')
    GetPerformance('etorassa_EXO-Fall11_R4-01197_T1_ES_PIC_MSS_batch15_v1__120218_124324')
    GetPerformance('etorassa_EXO-Fall11_R4-01163_T1_FR_CCIN2P3_MSSbatch12_v_1_120209_191156_2904')
    GetPerformance('etorassa_EXO-Fall11_R4-01131_T1_DE_KIT_MSS_v1_120207_173839_4401')
    GetPerformance('etorassa_EXO-Fall11_R4-01114_T1_UK_RAL_MSS_v1_120207_173731_1770')
    GetPerformance('etorassa_EXO-Fall11_R4-00985_T1_UK_RAL_MSS_v1_120108_224126')
    GetPerformance('etorassa_EXO-Fall11_R4-00975_T1_DE_KIT_MSS_v1_120108_231811')
    GetPerformance('etorassa_EXO-Fall11_R4-00964_T1_IT_CNAF_MSS_v1_120108_231731')
    GetPerformance('etorassa_EXO-Fall11_R4-00922_T1_IT_CNAF_MSS_v1_111229_155328')
    GetPerformance('etorassa_EXO-Fall11_R4-00903_T1_FR_CCIN2P3_MSS_v1_111229_155203')
    GetPerformance('etorassa_EXO-Fall11_R4-00809_T1_US_FNAL_MSS_v1_111229_144719')
    GetPerformance('etorassa_EXO-Fall11_R4-00790_T1_US_FNAL_MSS_v1_111229_154914')
    GetPerformance('etorassa_EXO-Fall11_R4-00690_T1_ES_PIC_MSS_v1_120108_231416')
    GetPerformance('etorassa_EXO-Fall11_R4-00673_T1_US_FNAL_MSS_v1_120108_231344')
    GetPerformance('etorassa_EXO-Fall11_R4-00337_T1_TW_ASGC_MSS_v1_111229_153518')
    GetPerformance('etorassa_EXO-Fall11_R4-00329_T1_TW_ASGC_MSS_v1_111229_153441')
    GetPerformance('etorassa_EXO-Fall11_R4-00122_T1_TW_ASGC_MSS_v1_111229_144053')
    GetPerformance('etorassa_EXO-Fall11_R4-00105_T1_TW_ASGC_MSS_v1_111229_143938')
    GetPerformance('etorassa_EXO-Fall11_R4-00019_T1_ES_PIC_MSS_v1_111215_182715')
    GetPerformance('etorassa_EXO-Fall11_R4-00017_T1_ES_PIC_MSS_v1_111215_182706')
    GetPerformance('etorassa_EXO-Fall11_R2-00737_T1_FR_CCIN2P3_MSS_batch69_v1__120919_165742_3959')
    GetPerformance('etorassa_EXO-Fall11_R2-00729_T1_FR_CCIN2P3_MSS_batch66_v1__120908_233100_6257')
    GetPerformance('etorassa_EXO-Fall11_R2-00728_T1_FR_CCIN2P3_MSS_batch66_v1__120908_233055_9861')
    GetPerformance('etorassa_EXO-Fall11_R2-00727_T1_FR_CCIN2P3_MSS_batch66_v1__120908_233051_144')
    GetPerformance('etorassa_EXO-Fall11_R2-00726_T1_FR_CCIN2P3_MSS_batch66_v1__120908_233046_945')
    GetPerformance('etorassa_EXO-Fall11_R2-00725_T1_FR_CCIN2P3_MSS_batch66_v1__120908_233041_8641')
    GetPerformance('etorassa_EXO-Fall11_R2-00724_T1_FR_CCIN2P3_MSS_batch66_v1__120908_233036_6455')
    GetPerformance('etorassa_EXO-Fall11_R2-00723_T1_FR_CCIN2P3_MSS_batch66_v1__120908_233031_7844')
    GetPerformance('etorassa_EXO-Fall11_R2-00722_T1_FR_CCIN2P3_MSS_batch66_v1__120908_233025_5888')
    GetPerformance('etorassa_EXO-Fall11_R2-00712_T1_US_FNAL_MSS_batch57_v1__120705_152622_5997')
    GetPerformance('etorassa_EXO-Fall11_R2-00707_T1_ES_PIC_MSS_batch42_v1__120423_110108_4026')
    GetPerformance('etorassa_EXO-Fall11_R2-00668_T1_IT_CNAF_MSS_batch35_v1__120310_095835')
    GetPerformance('etorassa_EXO-Fall11_R2-00657_T1_ES_PIC_MSS_batch34_v1__120306_005218')
    GetPerformance('etorassa_EXO-Fall11_R2-00568_T1_ES_PIC_MSS_batch31_v1__120223_192546')
    GetPerformance('etorassa_EXO-Fall11_R2-00481_T1_IT_CNAF_MSS__v1__120209_212212_4286')
    GetPerformance('etorassa_EXO-Fall11_R2-00392_T1_DE_KIT_MSS_v1_120128_191459')
    GetPerformance('etorassa_EXO-Fall11_R2-00382_T1_IT_CNAF_MSS_v1_120128_191454')
    GetPerformance('etorassa_EXO-Fall11_R2-00249_T1_US_FNAL_MSS_v1_111215_190816')
    GetPerformance('etorassa_EXO-Fall11_R1-01447_T1_US_FNAL_MSS_batch83_v1__120924_174605_7793')
    GetPerformance('etorassa_EXO-Fall11_R1-01446_T1_US_FNAL_MSS_batch80_v1__120919_154317_8622')
    GetPerformance('etorassa_EXO-Fall11_R1-01445_T1_TW_ASGC_MSS_batch80_v1__120919_154313_6976')
    GetPerformance('etorassa_EXO-Fall11_R1-01444_T1_TW_ASGC_MSS_batch80_v1__120919_154309_256')
    GetPerformance('etorassa_EXO-Fall11_R1-01443_T1_TW_ASGC_MSS_batch80_v1__120919_154303_1479')
    GetPerformance('etorassa_EXO-Fall11_R1-01442_T1_TW_ASGC_MSS_batch80_v1__120919_154257_8484')
    GetPerformance('etorassa_EXO-Fall11_R1-01441_T1_TW_ASGC_MSS_batch80_v1__120919_154250_1990')
    GetPerformance('etorassa_EXO-Fall11_R1-01438_T1_FR_CCIN2P3_MSS_batch75_v1__120830_185601_6889')
    GetPerformance('etorassa_EXO-Fall11_R1-01270_T1_ES_PIC_MSS_batch32_v1__120218_100522')
    GetPerformance('etorassa_EXO-Fall11_R1-01245_T1_ES_PIC_MSS_batch30_v1__120215_132352')
    GetPerformance('etorassa_EXO-Fall11_R1-01240_T1_ES_PIC_MSS_batch30_v1__120215_132336')
    GetPerformance('etorassa_EXO-Fall11_R1-01097_T1_DE_KIT_MSS_v1_120121_113856')
    GetPerformance('etorassa_EXO-Fall11_R1-01065_T1_IT_CNAF_MSS_v1_120113_185625')
    GetPerformance('etorassa_EXO-Fall11_R1-01057_T1_DE_KIT_MSS_v1_111223_152753')
    GetPerformance('etorassa_EXO-Fall11_R1-01046_T1_UK_RAL_MSS_v1_111223_152702')
    GetPerformance('etorassa_EXO-Fall11_R1-00985_T1_IT_CNAF_MSS_v1_111207_152035')
    GetPerformance('etorassa_EXO-Fall11_R1-00953_T1_FR_CCIN2P3_MSS_v1_111207_151822')
    GetPerformance('etorassa_BPH-Summer12_DR52X-00001_T1_TW_ASGC_MSS_batch1_v1__120322_185648')


