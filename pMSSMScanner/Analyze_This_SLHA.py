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
met=0
lE=0
nj=0
ht=0
NBtags=0
leptons=[]

NGenEvents=10000

NRounds=1

DoIllustration=True## if this is set to true you get nice plots for each signal point (but the script takes longer)

cutoff=0.05 ## cutoff probability for individual compatibility (e.g. 5% = 0.05)

NEvent=0


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
  f.write("\n                                                             SLHAParameters = cms.vstring('SLHAFILE = "+slhapath[slhapath.find("usercode"):]+"'),") # starting at UserCode/
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

Mllhisto = ROOT.TH1F("Mllhisto","",56,20,300);
Mllhisto.SetStats(0)
Mllhisto.Sumw2();
Mllhisto.GetXaxis().SetTitle("m_{ll}")
Mllhisto.GetXaxis().CenterTitle()

HistoCentrality = ROOT.TH1F("HistoCentrality","",12,0.0,2.4);
HistoCentrality.SetStats(0)
HistoCentrality.Sumw2();
HistoCentrality.GetXaxis().SetTitle("min(|#eta_{1}|,|#eta_{2})")
HistoCentrality.GetXaxis().CenterTitle()

HistoBTagMultiplicity = ROOT.TH1F("HistoBTagMultiplicity","",5,-0.5,4.5);
HistoBTagMultiplicity.SetStats(0)
HistoBTagMultiplicity.Sumw2();
HistoBTagMultiplicity.GetXaxis().SetTitle("N_{btags}")
HistoBTagMultiplicity.GetXaxis().CenterTitle()

HistoJetMultiplicity = ROOT.TH1F("HistoJetMultiplicity","",5,1.5,6.5);
HistoJetMultiplicity.SetStats(0)
HistoJetMultiplicity.Sumw2();
HistoJetMultiplicity.GetXaxis().SetTitle("N_{jets}")
HistoJetMultiplicity.GetXaxis().CenterTitle()


Methisto = ROOT.TH1F("Methisto","",20,100,300);
Methisto.SetStats(0)
Methisto.Sumw2();
Methisto.GetXaxis().SetTitle("m_{ll}")
Methisto.GetXaxis().CenterTitle()


def StoreEvent():
    global px, py, lpx,lpy,lpz,lE,nj,leptons,NBtags,met
    global HistoCentrality,HistoBTagMultiplicity,Mllhisto,HistoJetMultiplicity,MllhistoLowMass,MllhistoZmass,MllhistoHighMass
    
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
    JetMultiplicity=nj

    if mll > 0:
#            print "NJ="+str(nj)+" and met="+str(met)
            if nj>=3 and met>100:
#              if mll>20 and mll<70:
#                    HistoCentrality.Fill(max(abs(eta1),abs(eta2)),Weight)
              if centrality=="central" and mll>20 and mll<70:
                    HistoBTagMultiplicity.Fill(BTagMultiplicity,Weight)
                    HistoJetMultiplicity.Fill(JetMultiplicity,Weight)
                    Mllhisto.Fill(mll,Weight)
                    Methisto.Fill(met,Weight)
#              if centrality=="central":
#                    if(mll>=20 and mll<=80) :
#                        MllhistoLowMass.Fill(mll,Weight)
#                    if(mll>=80 and mll<=100) :
#                        MllhistoZmass.Fill(mll,Weight)
#                    if(mll>100) :
#                        MllhistoHighMass.Fill(mll,Weight);
            if nj>=2 and met>150:
#                if mll>20 and mll<70:
#                    HistoCentrality.Fill(max(abs(eta1),abs(eta2)),Weight)
                if centrality=="central" and mll>20 and mll<70:
                    HistoBTagMultiplicity.Fill(BTagMultiplicity,Weight)
                    HistoJetMultiplicity.Fill(JetMultiplicity,Weight)
                    Methisto.Fill(met,Weight)
                    Mllhisto.Fill(mll,Weight)
#                if centrality=="central":
#                    if(mll>=20 and mll<=80) :
#                            MllhistoLowMass.Fill(mll,Weight)
#                    if(mll>=80 and mll<=100) :
#                            MllhistoZmass.Fill(mll,Weight)
#                    if(mll>100) :
#                            MllhistoHighMass.Fill(mll,Weight)
                    

def ProcessEvent():
  global px, py, lpx,lpy,lpz,lE,nj,leptons, NBtags, met, mll
    
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
  nj=0
  met=0
  mll=0
  NBtags=0
  leptons=[]

def ProcessLine(line):
#  print "Processing line" 
  global px, py, lpx, lpy, lpz, lE, nj, leptons, ht, NBtags, NEvent
  
  NEvent+=1

  
  tarray=line.split(" ")
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
      if(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5)>20) :
          lepton={}
          lepton["px"]=ParticlePx
          lepton["py"]=ParticlePy
          lepton["pz"]=ParticlePz
          lepton["E"]=ParticleE
          lepton["id"]=ParticleId
          p1 = ROOT.TLorentzVector()
          p1.SetPxPyPzE(lepton["px"],lepton["py"],lepton["pz"],lepton["E"])
          eta1=p1.Eta()
#          print "Looking at a lepton with eta "+str(eta1)+" in event "+str(NEvent)
          if abs(eta1) < 1.4:    ## CENTRAL REGION ONLY!
	    leptons.append(lepton)

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

    
def FlipOnOverFlowBin():
  global HistoBTagMultiplicity, HistoJetMultiplicity, Mllhisto, Methisto

  HistoBTagMultiplicity.SetBinContent(HistoBTagMultiplicity.GetNbinsX(),HistoBTagMultiplicity.GetBinContent(HistoBTagMultiplicity.GetNbinsX())+HistoBTagMultiplicity.GetBinContent(HistoBTagMultiplicity.GetNbinsX()+1))
  HistoBTagMultiplicity.SetBinContent(HistoBTagMultiplicity.GetNbinsX()+1,0)
  HistoBTagMultiplicity.SetBinError(HistoBTagMultiplicity.GetNbinsX(),math.sqrt(pow(HistoBTagMultiplicity.GetBinError(HistoBTagMultiplicity.GetNbinsX()),2)+pow(HistoBTagMultiplicity.GetBinError(HistoBTagMultiplicity.GetNbinsX()+1),2)))
  HistoBTagMultiplicity.SetBinError(HistoBTagMultiplicity.GetNbinsX()+1,0)

  HistoJetMultiplicity.SetBinContent(HistoJetMultiplicity.GetNbinsX(),HistoJetMultiplicity.GetBinContent(HistoJetMultiplicity.GetNbinsX())+HistoJetMultiplicity.GetBinContent(HistoJetMultiplicity.GetNbinsX()+1))
  HistoJetMultiplicity.SetBinContent(HistoJetMultiplicity.GetNbinsX()+1,0)
  HistoJetMultiplicity.SetBinError(HistoJetMultiplicity.GetNbinsX(),math.sqrt(pow(HistoJetMultiplicity.GetBinError(HistoJetMultiplicity.GetNbinsX()),2)+pow(HistoJetMultiplicity.GetBinError(HistoJetMultiplicity.GetNbinsX()+1),2)))
  HistoJetMultiplicity.SetBinError(HistoJetMultiplicity.GetNbinsX()+1,0)

  Mllhisto.SetBinContent(Mllhisto.GetNbinsX(),Mllhisto.GetBinContent(Mllhisto.GetNbinsX())+Mllhisto.GetBinContent(Mllhisto.GetNbinsX()+1))
  Mllhisto.SetBinContent(Mllhisto.GetNbinsX()+1,0)
  Mllhisto.SetBinError(Mllhisto.GetNbinsX(),math.sqrt(pow(Mllhisto.GetBinError(Mllhisto.GetNbinsX()),2)+pow(Mllhisto.GetBinError(Mllhisto.GetNbinsX()+1),2)))
  Mllhisto.SetBinError(Mllhisto.GetNbinsX()+1,0)

  Methisto.SetBinContent(Methisto.GetNbinsX(),Methisto.GetBinContent(Methisto.GetNbinsX())+Methisto.GetBinContent(Methisto.GetNbinsX()+1))
  Methisto.SetBinContent(Methisto.GetNbinsX()+1,0)
  Methisto.SetBinError(Methisto.GetNbinsX(),math.sqrt(pow(Methisto.GetBinError(Methisto.GetNbinsX()),2)+pow(Methisto.GetBinError(Methisto.GetNbinsX()+1),2)))
  Methisto.SetBinError(Methisto.GetNbinsX()+1,0)



def ComputeCompatibility(filename,slhaname):
  global Methisto,Mllhisto,DoIllustration,HistoBTagMultiplicity,HistoJetMultiplicity
  
  RefFile = ROOT.TFile("DifferentialDistributions.root")
    
#  RefHistoCentrality = RefFile.Get("HistoCentrality")
#  KSP_Centrality = RefHistoCentrality.KolmogorovTest(HistoCentrality)
    
 
  FlipOnOverFlowBin() 
 
  print "The histograms contain the following number of events   (btag/jet/mll/met):"
  print HistoBTagMultiplicity.Integral()
  print HistoJetMultiplicity.Integral()
  print Mllhisto.Integral()
  print Methisto.Integral()
  
  print "Looking at btag"
  RefHistoBTagMultiplicity = RefFile.Get("dBTag")
  KSP_BTag = RefHistoBTagMultiplicity.KolmogorovTest(HistoBTagMultiplicity)
        
  print "Looking at jets"
  RefHistoJetMultiplicity = RefFile.Get("dJet")
  KSP_Jet = RefHistoJetMultiplicity.KolmogorovTest(HistoJetMultiplicity)
        
  print "Looking at mll"
  RefMllhisto = RefFile.Get("dMll")
  KSP_Mass = RefMllhisto.KolmogorovTest(Mllhisto)
  
  print "Looking at met"
  RefMEThisto = RefFile.Get("dMET")
  KSP_MET  = RefMEThisto.KolmogorovTest(Methisto)
  
  print "Compatibilities found: "
  print "Mll: "+str(KSP_Mass)
  print "BTag: "+str(KSP_BTag)
  print "JET: "+str(KSP_Jet)
  print "MET: "+str(KSP_MET)
        
#  KSP_Mass = pow(    pow(KSP_LowMass,3) * KSP_ZMass * KSP_HighMass , 1/5.)
  
#  KSP_Final = pow ( pow(KSP_Mass,3) * KSP_Centrality * KSP_BTag , 1/5.0)
  
  KSP_Final = KSP_BTag + KSP_Jet + KSP_Mass + KSP_MET
  
  PResult=str(("{0:.10f}".format(KSP_Final)))
  PResult="0p"+PResult[2:]
  
  mswrite = open("Marker.txt",'w')
  mswrite.write(PResult)
  mswrite.close()
  
  kaswrite = open("KS.txt",'w')
  kaswrite.write(str(KSP_Final))
  kaswrite.close()
  
#  print "Compatibility in low mass: "+str(KSP_LowMass)
#  print "Compatibility in Z mass: "+str(KSP_ZMass)
#  print "Compatibility in high mass: "+str(KSP_HighMass)
  print "Compatibility in mass range: "+str(KSP_Mass)
  print "Compatibility in NJets: "+str(KSP_Jet)
#  print "Compatibility in centrality: "+str(KSP_Centrality)
  print "Compatibility in BTag: "+str(KSP_BTag)
  print "Compatibility in MET: "+str(KSP_MET)
  
  print "Combined Kolmogorov-Smirnov probability is "+str(KSP_Final)
  
  if not DoIllustration:
    return KSP_Final
  
  # now draw this!
  print "NEED TO UPDATE PLOTTING FOR FOUR REGIONS"

#  Illustrate(HistoCentrality,RefHistoCentrality,slhaname,KSP_Centrality)
  Illustrate(HistoBTagMultiplicity,RefHistoBTagMultiplicity,slhaname,KSP_BTag)
  Illustrate(Mllhisto,RefMllhisto,slhaname,KSP_Mass)
  Illustrate(Methisto,RefMEThisto,slhaname,KSP_MET)
  Illustrate(HistoJetMultiplicity,RefHistoJetMultiplicity,slhaname,KSP_Jet)
#  Illustrate(MllhistoZmass,RefMllhistoZmass,slhaname,KSP_ZMass)
#  Illustrate(MllhistoHighMass,RefMllhistoHighMass,slhaname,KSP_HighMass)

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