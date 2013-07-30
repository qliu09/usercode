#!/usr/bin/python

import os,sys,commands
import sys
import ROOT
import math
import random
px=0
py=0
lpx=0
lpy=0
lpz=0
lE=0
nj=0
ht=0
NBtags=0
leptons=[]

NGenEvents=100000

NRounds=10

DoIllustration=False## if this is set to true you get nice plots for each signal point (but the script takes longer)

cutoff=0.05 ## cutoff probability for individual compatibility (e.g. 5% = 0.05)




def GenerateConfigFile(slhapath):
  
  f = open("This_SLHA_To_LHE.py",'w')

    
  f.write('\nimport FWCore.ParameterSet.Config as cms')
  f.write('\n')
  f.write('\nprocess = cms.Process("PROD")')

  f.write('\nprocess.load("Configuration.StandardSequences.SimulationRandomNumberGeneratorSeeds_cff")')
  f.write('\nprocess.load("PhysicsTools.HepMCCandAlgos.genParticles_cfi")')
  f.write('\nprocess.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")')
  f.write('\n')
  f.write('\nprocess.maxEvents = cms.untracked.PSet(')
  f.write('\n                                       input = cms.untracked.int32('+str(int(float(NGenEvents)/NRounds))+')')
  f.write('\n                                       )')
  f.write('\n')
  f.write('\nprocess.load("FWCore.MessageLogger.MessageLogger_cfi")')
  f.write("\nprocess.MessageLogger.destinations = ['cerr']")
  f.write('\nprocess.MessageLogger.statistics = []')
  f.write('\nprocess.MessageLogger.fwkJobReports = []')
  f.write('\nprocess.MessageLogger.cerr.FwkReport.reportEvery = 100000')
  f.write('\n')
  f.write('\n')
  f.write('\nprocess.source = cms.Source("EmptySource")')
  f.write('\n')
  f.write('\nfrom Configuration.Generator.PythiaUEZ2Settings_cfi import *')
  f.write('\n')
  f.write('\n')
  f.write('\nprocess.generator = cms.EDFilter("Pythia6GeneratorFilter",')
  f.write('\n                                 pythiaHepMCVerbosity = cms.untracked.bool(False),')
  f.write('\n                                 maxEventsToPrint = cms.untracked.int32(0),')
  f.write('\n                                 pythiaPylistVerbosity = cms.untracked.int32(0),')
  f.write('\n                                 comEnergy = cms.double(8000.0),')
  f.write('\n                                 pythiaToLHE = cms.bool(True),')
  f.write('\n                                 PythiaParameters = cms.PSet(')
  f.write('\n                                                             pythiaUESettingsBlock,')
  f.write('\n                                                             processParameters = cms.vstring(')
  f.write("\n                                                                                             'MSEL=39                  ! All SUSY processes ',")
  f.write("\n                                                                                             'IMSS(1) = 11             ! Spectrum from external SLHA file',")
  f.write("\n                                                                                             'IMSS(21) = 33            ! LUN number for SLHA File (must be 33) ',")
  f.write("\n                                                                                             'IMSS(22) = 33            ! Read-in SLHA decay table ',")
  f.write("\n                                                                                             'MSTP(161) = 67',")
  f.write("\n                                                                                             'MSTP(162) = 68',")
  f.write("\n                                                                                             'MSTP(163) = 69'),")
  f.write('\n')
  f.write("\n                                                             SLHAParameters = cms.vstring('SLHAFILE = "+slhapath[slhapath.find("UserCode"):]+"'),") # starting at UserCode/
  f.write("\n                                                             parameterSets = cms.vstring('pythiaUESettings',")
  f.write("\n                                                                                         'processParameters','SLHAParameters')")
  f.write('\n')
  f.write('\n                                                             )')
  f.write('\n                                 )')
  f.write('\n')
  f.write('\nprocess.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",')
  f.write('\n                                                   moduleSeeds = cms.PSet(')
  f.write('\n                                                                          generator = cms.untracked.uint32('+str(int(random.randrange(100,10000000)))+'),')
  f.write('\n                                                                          g4SimHits = cms.untracked.uint32('+str(int(random.randrange(100,10000000)))+'),')
  f.write('\n                                                                          VtxSmeared = cms.untracked.uint32('+str(int(random.randrange(100,10000000)))+')')
  f.write('\n                                                                          ),')
  f.write('\n                                                   )')
  f.write('\n')
  f.write('\nprocess.pp = cms.Path(process.generator)')
  f.write('\n')
  f.write('\nprocess.schedule = cms.Schedule(process.pp)')
  f.write('\n')
  f.write('\n')
  f.close()


def GenerateLHEFile(slhapath):
   print "Generating LHE file from "+str(slhapath)
   ConfFile=GenerateConfigFile(slhapath)
   LHEPATH="InvalidLHEPath"
   lines = commands.getoutput("cmsRun This_SLHA_To_LHE.py | tee "+slhapath+".log")
   for line in lines.splitlines():
     print " LHE file generation : "+line
   print "Done generating LHE file"
   return "fort.69"


global compat

MllhistoLowMass = ROOT.TH1F("MllhistoLowMass","",12,20,80);
MllhistoLowMass.SetStats(0)
MllhistoLowMass.Sumw2();
MllhistoLowMass.GetXaxis().SetTitle("m_{ll}")
MllhistoLowMass.GetXaxis().CenterTitle()

MllhistoZmass = ROOT.TH1F("MllhistoZmass","",4,80,100);
MllhistoZmass.SetStats(0)
MllhistoZmass.Sumw2();
MllhistoZmass.GetXaxis().SetTitle("m_{ll}")
MllhistoZmass.GetXaxis().CenterTitle()

MllhistoHighMass = ROOT.TH1F("MllhistoHighMass","",40,100,300);
MllhistoHighMass.SetStats(0)
MllhistoHighMass.Sumw2();
MllhistoHighMass.GetXaxis().SetTitle("m_{ll}")
MllhistoHighMass.GetXaxis().CenterTitle()

HistoCentrality = ROOT.TH1F("HistoCentrality","",12,0.0,2.4);
HistoCentrality.SetStats(0)
HistoCentrality.Sumw2();
HistoCentrality.GetXaxis().SetTitle("min(|#eta_{1}|,|#eta_{2})")
HistoCentrality.GetXaxis().CenterTitle()

HistoBTagMultiplicity = ROOT.TH1F("HistoBTagMultiplicity","",7,-0.5,6.5);
HistoBTagMultiplicity.SetStats(0)
HistoBTagMultiplicity.Sumw2();
HistoBTagMultiplicity.GetXaxis().SetTitle("N_{btags}")
HistoBTagMultiplicity.GetXaxis().CenterTitle()

Methisto = ROOT.TH1F("Methisto","",40,100,500);
Methisto.SetStats(0)
Methisto.Sumw2();
Methisto.GetXaxis().SetTitle("m_{ll}")
Methisto.GetXaxis().CenterTitle()


def StoreEvent():
    global px, py, lpx,lpy,lpz,lE,nj,leptons,NBtags
    global HistoCentrality,HistoBTagMultiplicity,MllhistoLowMass,MllhistoZmass,MllhistoHighMass
    
    i1=0
    i2=0
    id1=leptons[0]["id"]
    id2=0
    
    
    for i in range(1,len(leptons)):
        if id1*leptons[i]["id"]<0:
          id2=leptons[i]["id"]
          i2=i
          break

    eta1=0.0
    eta2=0.0

    p1 = ROOT.TLorentzVector()
    p1.SetPxPyPzE(leptons[0]["px"],leptons[0]["py"],leptons[0]["pz"],leptons[0]["E"])
    eta1=p1.Eta()

    p2 = ROOT.TLorentzVector()
    p2.SetPxPyPzE(leptons[i2]["px"],leptons[i2]["py"],leptons[i2]["pz"],leptons[i2]["E"])
    eta2=p2.Eta()

    flav="SF"
    if abs(id1)==abs(id2):
      flav="SF"
    #      print "Same flavor"
    else:
      flav="OF"

    met=pow(px*px+py*py,0.5)
    mll=(p1+p2).M()
    
#    print "Met="+str(met)

    #print "Leptons are at "+str(eta1)+" and at "+str(eta2)
    centrality="invalid"
    if max(abs(eta1),abs(eta2)) > 1.4:
        centrality="forward"
    else:
        centrality="central"

    if centrality!="central" and centrality!="forward":
        print "Weird centrality : "+str(centrality)
        sys.exit(-1)

 
    Weight=1.0;
    if(flav=="OF"):
      Weight=-1.0
    
    BTagMultiplicity=NBtags
    NBtags=0

    if mll > 0:
            if nj>=3 and met>100:
              if mll>20 and mll<70:
                    HistoCentrality.Fill(max(abs(eta1),abs(eta2)),Weight)
              if centrality=="central" and mll>20 and mll<70:
                    HistoBTagMultiplicity.Fill(BTagMultiplicity,Weight)
              if centrality=="central":
                    if(mll>=20 and mll<=80) :
                        MllhistoLowMass.Fill(mll,Weight)
                    if(mll>=80 and mll<=100) :
                        MllhistoZmass.Fill(mll,Weight)
                    if(mll>100) :
                        MllhistoHighMass.Fill(mll,Weight);
            if nj>=2 and met>150:
                if mll>20 and mll<70:
                    HistoCentrality.Fill(max(abs(eta1),abs(eta2)),Weight)
                if centrality=="central" and mll>20 and mll<70:
                    HistoBTagMultiplicity.Fill(BTagMultiplicity,Weight)
                if centrality=="central":
                    if(mll>=20 and mll<=80) :
                            MllhistoLowMass.Fill(mll,Weight)
                    if(mll>=80 and mll<=100) :
                            MllhistoZmass.Fill(mll,Weight)
                    if(mll>100) :
                            MllhistoHighMass.Fill(mll,Weight)

def ProcessEvent():
  global px, py, lpx,lpy,lpz,lE,nj,leptons
    
  if nj>=2 and len(leptons)>=2:
      StoreEvent()
  px=0
  py=0
  lE=0
  lpx=0
  lpy=0
  lpz=0
  nl=0
  nj=0
  ht=0
  NBtags=0
  leptons=[]

def ProcessLine(line):
#  print "Processing line" 
  global px, py, lpx, lpy, lpz, lE, nj, leptons, ht, NBtags

  
  tarray=line.split(" ")
#  print line
#  print tarray
  array=[]
  for entry in tarray:
    if len(entry) == 0:
      continue
    array.append(entry)
  if len(array)<7:
    return
  
  ParticleId  = int(array[0])
  ParticleStat= int(array[1])
  ParticleDad = int(array[2])
  ParticleMom = int(array[3])
  ParticlePx  = float(array[6])
  ParticlePy  = float(array[7])
  ParticlePz  = float(array[8])
  ParticleE   = float(array[9])

  if(ParticleId==1000022):
    # this is an LSP !
    px+=ParticlePx
    py+=ParticlePy
    #print "LSP !!!!!!!!        "+str(line)
    
  if(abs(ParticleId)==5 and pow(pow(ParticlePx,2)+pow(ParticlePy,2),0.5)>40 ) :  ## this is a b (we're counting any b's with Pt>40)
      NBtags+=1
    

  if(ParticleId==12 or ParticleId==14 or ParticleId==16 ): 
    # this is a neutrino (also need to account for that ... )
    px+=ParticlePx
    py+=ParticlePy

  if(abs(ParticleId)<7 ): ## jet!
      if(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5)>40):
          #print "ADDED A JET *************** Pt="+str(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5))
          if(ParticleStat==1):
              nj+=1
              ht+=pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5)



  if abs(ParticleId)==11 or abs(ParticleId)==13: #don't consider taus
      if(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5)>20):
          lepton={}
          lepton["px"]=ParticlePx
          lepton["py"]=ParticlePy
          lepton["pz"]=ParticlePz
          lepton["E"]=ParticleE
          lepton["id"]=ParticleId
          leptons.append(lepton)

          #print "ACCEPTED LEPTON (Pt="+str(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5))+"!!!!!!!!        "+str(line)
       #else:
          #print "REJECTED LEPTON (Pt="+str(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5))+"!!!!!!!!        "+str(line)

def FillHistogram(filename):
  print "Filling histogram for "+filename
  lhefile = open(filename)
  ActionStarted=False
  
  global px,py,pz 
  
  for sline in lhefile:
    line=sline.rstrip()
    if line.find("<event>") > -1 :
      ActionStarted=True
      continue
    if line.find("</event>") > -1 :
        #print " ******************* EVENT ENDED "
      ProcessEvent()
        #print "Time to clean event"
      continue
    if not ActionStarted:
      continue
    
    #print line
    ProcessLine(line)
  
def Illustrate(ThisHisto,RefHisto,filename,KSP) :
    RefHisto.SetLineColor(ROOT.TColor.GetColor("#FF0000"))
    RefHisto.Scale(ThisHisto.Integral()/RefHisto.Integral())
    RefHisto.SetMaximum(ThisHisto.GetMaximum()*1.3)
    
    leg = ROOT.TLegend(0.6,0.75,0.77,0.89);
    leg.AddEntry(RefHisto,"reference (data!)","l")
    leg.AddEntry(ThisHisto,"this point","lp")
    leg.SetFillColor(ROOT.kWhite)
    leg.SetLineColor(ROOT.kWhite)
    
    Decoration = ROOT.TLatex(0.5,0.94,"LHE-Based Signal Bouncer")
    Decoration.SetNDC()
    Decoration.SetTextAlign(22)
    Decoration.SetTextSize(0.05)
    
    can = ROOT.TCanvas("can","can",500,500)
    ThisHisto.Draw("e1")
    RefHisto.Draw("histo,same")
    ThisHisto.Draw("e1,same")
    wksp2 = ROOT.TLatex(0.61,0.65,"#splitline{#splitline{KSP: "+str(float(int(10000*KSP))/10000)+"}{ }}{Stats: "+str(int(ThisHisto.GetEntries()))+"/"+str(NGenEvents)+"}")
    wksp2.SetNDC()
    wksp2.SetTextSize(0.03)
    wksp2.Draw()
    leg.Draw()
    Decoration.Draw()
    can.SaveAs(filename.replace(".","_")+"_"+str(ThisHisto.GetName())+".png")

def ComputeCompatibility(filename,slhaname):
  global METhisto,Mllhisto,DoIllustration
  
  RefFile = ROOT.TFile("NewReferenceDiffs.root")
    
  RefHistoCentrality = RefFile.Get("HistoCentrality")
  KSP_Centrality = RefHistoCentrality.KolmogorovTest(HistoCentrality)
    
  RefHistoBTagMultiplicity = RefFile.Get("HistoBTagMultiplicity")
  KSP_BTag = RefHistoBTagMultiplicity.KolmogorovTest(HistoBTagMultiplicity)
        
  RefMllhistoLowMass = RefFile.Get("MllhistoLowMass")
  KSP_LowMass = RefMllhistoLowMass.KolmogorovTest(MllhistoLowMass)
        
  RefMllhistoZmass = RefFile.Get("MllhistoZmass")
  KSP_ZMass = RefMllhistoZmass.KolmogorovTest(MllhistoZmass)
      
  RefMllhistoHighMass = RefFile.Get("MllhistoHighMass")
  KSP_HighMass = RefMllhistoHighMass.KolmogorovTest(MllhistoHighMass)
    
  KSP_Mass = pow(    pow(KSP_LowMass,3) * KSP_ZMass * KSP_HighMass , 1/5.)
  
  KSP_Final = pow ( pow(KSP_Mass,3) * KSP_Centrality * KSP_BTag , 1/5.0)
  
  PResult=str(("{0:.10f}".format(KSP_Final)))
  PResult="0p"+PResult[2:]
  
  mswrite = open("Marker.txt",'w')
  mswrite.write(PResult)
  mswrite.close()
  
  kaswrite = open("KS.txt",'w')
  kaswrite.write(str(KSP_Final))
  kaswrite.close()
  
  print "Compatibility in low mass: "+str(KSP_LowMass)
  print "Compatibility in Z mass: "+str(KSP_ZMass)
  print "Compatibility in high mass: "+str(KSP_HighMass)
  print "Compatibility in centrality: "+str(KSP_Centrality)
  print "Compatibility in BTag: "+str(KSP_BTag)
  
  print "Combined Kolmogorov-Smirnov probability is "+str(KSP_Final)
  
  if not DoIllustration:
    return KSP_Final
  
  # now draw this!
  print "NEED TO UPDATE PLOTTING FOR FOUR REGIONS"

  Illustrate(HistoCentrality,RefHistoCentrality,slhaname,KSP_Centrality)
  Illustrate(HistoBTagMultiplicity,RefHistoBTagMultiplicity,slhaname,KSP_BTag)
  Illustrate(MllhistoLowMass,RefMllhistoLowMass,slhaname,KSP_LowMass)
  Illustrate(MllhistoZmass,RefMllhistoZmass,slhaname,KSP_ZMass)
  Illustrate(MllhistoHighMass,RefMllhistoHighMass,slhaname,KSP_HighMass)

  RefFile.Close()
  
  return KSP_Final
  

def ProcessFile(filename,slhaname) :
  
  FillHistogram(filename)
  
  #sprint "not storing histogram"
  #StoreHistogram(filename)
  
  global compat
  compat = ComputeCompatibility(filename,slhaname)
  
  global cutoff
  if(compat<cutoff):
    print "====> Rejecting this point (c KSP = "+str(compat)+" )"
    print "         (note that cutoff is "+str(cutoff)+")"
    if compat>0: 
      print "         we're a factor of cutoff/cKSP  = " + str(cutoff/compat) + " below threshold"
    return 0
  else:
    print "====> Accepting this point (c KSP = "+str(compat)+" )"
    print "         Note that cutoff is "+str(cutoff)+";"
    print "         we're a factor of cKSP /cutoff = " + str(compat/cutoff) + " above threshold"
    return 1


def CrunchNumbersForLHEFile(filename,slhaname):
  global compat
  return [ProcessFile(filename,slhaname),compat]
  
def AnalyzeThisSLHA(slhapath):
  retval=0.0
  for i in range(0,NRounds):
    print "Now doing round "+str(i)+" of generating an LHE file (doing it in "+str(NRounds)+" rounds to reduce sandbox usage)"
    LHEFile = GenerateLHEFile(slhapath)
    retval=CrunchNumbersForLHEFile(LHEFile,slhapath)[0]
    commands.getoutput("rm "+LHEFile)
  
  return retval

if len(sys.argv)<2:
  print "You need to indicate an SLHA file (including its FULL PATH) !"
  sys.exit(-1)

SLHA_PATH=sys.argv[1]

result = AnalyzeThisSLHA(SLHA_PATH)

sys.exit(result)