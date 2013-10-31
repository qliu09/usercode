#!/usr/bin/python

import os,sys,commands
import sys
import ROOT
import math
import random
import time
px=0
py=0
met=0
lE=0
nj=0
ht=0
NBtags=0
leptons=[]

start=0
done=0
elapsed=0


NGenEvents=1000000

NRounds=1000

DoIllustration=False ## if this is set to true you get nice plots for each signal point (but the script takes longer)

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
  if slhapath.find("usercode") > 0 : 
      f.write("\n                                                             SLHAParameters = cms.vstring('SLHAFILE = "+slhapath[slhapath.find("usercode"):]+"'),") # starting at UserCode/
  if slhapath.find("UserCode") > 0 : 
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
#   for line in lines.splitlines():
#     print " LHE file generation : "+line
   print "Done generating LHE file"
   return "fort.69"


global compat

MllhistoLowMass = ROOT.TH1F("MllhistoLowMass","",12,20,80);
MllhistoLowMass.SetStats(0)
MllhistoLowMass.Sumw2();
MllhistoLowMass.GetXaxis().SetTitle("m_{ll}")
MllhistoLowMass.GetXaxis().CenterTitle()

JetPthisto = ROOT.TH1F("JetPthisto","",200,0,1000);
JetPthisto.SetStats(0)
JetPthisto.Sumw2();
JetPthisto.GetXaxis().SetTitle("JetPt")
JetPthisto.GetXaxis().CenterTitle()

MllhistoZmass = ROOT.TH1F("MllhistoZmass","",4,80,100);
MllhistoZmass.SetStats(0)
MllhistoZmass.Sumw2();
MllhistoZmass.GetXaxis().SetTitle("m_{ll}")
MllhistoZmass.GetXaxis().CenterTitle()

MllhistoHighMass = ROOT.TH1F("MllhistoHighMass","",36,120,300);
MllhistoHighMass.SetStats(0)
MllhistoHighMass.Sumw2();
MllhistoHighMass.GetXaxis().SetTitle("m_{ll}")
MllhistoHighMass.GetXaxis().CenterTitle()

Mllhisto = ROOT.TH1F("Mllhisto","",36,20,200);
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
    global NEvent
#    print "__________________________________________________________________"
#    print "Found two leptons in event "+str(NEvent)
    global px, py, lE, nj, leptons, NBtags, met
    global HistoCentrality,HistoBTagMultiplicity,Mllhisto,HistoJetMultiplicity,MllhistoLowMass,MllhistoZmass,MllhistoHighMass
    
    i1=0
    i2=-1
    id1=leptons[0]["id"]
    id2=0
    
    
    for i in range(1,len(leptons)):
        if id1*leptons[i]["id"]<0:
          id2=leptons[i]["id"]
          i2=i
          break

    if i2<0:
      return
#    print "The two leptons are "+str(id1)+" and "+str(id2)
    p1 = ROOT.TLorentzVector()
    p1.SetPxPyPzE(leptons[0]["px"],leptons[0]["py"],leptons[0]["pz"],leptons[0]["E"])
    eta1=p1.Eta()

    p2 = ROOT.TLorentzVector()
    p2.SetPxPyPzE(leptons[i2]["px"],leptons[i2]["py"],leptons[i2]["pz"],leptons[i2]["E"])
    eta2=p2.Eta()
    
#    print "The leptons have pt "+str(p1.Pt())+" and "+str(p2.Pt())+" and eta "+str(p1.Eta())+" and "+str(p2.Eta())
#    print leptons[0]
#    print leptons[i2]

    flav="SF"
    if abs(id1)==abs(id2):
      flav="SF"
    else:
      flav="OF"

    met=pow(px*px+py*py,0.5)
    mll=(p1+p2).M()
#    print "Mass : "+str(mll)
    
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
            if nj>=3 and met>100:
              if centrality=="central":
                    Mllhisto.Fill(mll,Weight)
              if centrality=="central" and mll>20 and mll<70:
                    HistoBTagMultiplicity.Fill(BTagMultiplicity,Weight)
                    HistoJetMultiplicity.Fill(JetMultiplicity,Weight)
                    Mllhisto.Fill(mll,Weight)
                    Methisto.Fill(met,Weight)
                    MllhistoLowMass.Fill(mll,Weight)
              if centrality=="central" and mll>120:
                    MllhistoHighMass.Fill(mll,Weight)
            if nj>=2 and met>150:
              if centrality=="central":
                    Mllhisto.Fill(mll,Weight)
              if centrality=="central" and mll>20 and mll<70:
                    HistoBTagMultiplicity.Fill(BTagMultiplicity,Weight)
                    HistoJetMultiplicity.Fill(JetMultiplicity,Weight)
                    Methisto.Fill(met,Weight)
                    MllhistoLowMass.Fill(mll,Weight)
              if centrality=="central" and mll>120:
                    MllhistoHighMass.Fill(mll,Weight)

def ProcessEvent():
  global px, py, nj,leptons, NBtags, met, mll
    
  if nj>=2 and len(leptons)>=2:
      StoreEvent()
  px=0
  py=0
  nl=0
  nj=0
  ht=0
  nj=0
  met=0
  mll=0
  NBtags=0
  leptons=[]

def ProcessLine(line):
  global px, py, nj, leptons, ht, NBtags, NEvent, JetPthisto
  
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
    
  if(abs(ParticleId)==5 and pow(pow(ParticlePx,2)+pow(ParticlePy,2),0.5)>30) :  ## this is a b (we're counting any b's with Pt>30)
      ParticleVector = ROOT.TLorentzVector()
      ParticleVector.SetPxPyPzE(ParticlePx,ParticlePy,ParticlePz,ParticleE)
      particleEta=ParticleVector.Eta()
      if abs(particleEta) < 2.4 :
	NBtags+=1
    

  if(ParticleId==12 or ParticleId==14 or ParticleId==16 ): 
    # this is a neutrino (also need to account for that ... )
    px+=ParticlePx
    py+=ParticlePy

  if(abs(ParticleId)<7 ): ## jet!
      ParticleVector = ROOT.TLorentzVector()
      ParticleVector.SetPxPyPzE(ParticlePx,ParticlePy,ParticlePz,ParticleE)
      if ParticleVector.Pt()>5:
	JetPthisto.Fill(ParticleVector.Pt(),0.69+0.00187*pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5))
      if(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5)>40):
          #print "ADDED A JET *************** Pt="+str(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5))
          if(ParticleStat==1):
              nj+=(0.69+0.00187*pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5))
              ht+=pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5)



  if abs(ParticleId)==11 or abs(ParticleId)==13: #don't consider taus
      if(pow(ParticlePx*ParticlePx+ParticlePy*ParticlePy,0.5)>20) :
#          print "Found a lepton in event "+str(NEvent)+" on line with "+line
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
  global NEvent

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
      ProcessEvent()
      NEvent+=1
      continue
    if not ActionStarted:
      continue
    
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
    wksp2 = ROOT.TLatex(0.61,0.65,"#splitline{#splitline{KSP: "+str(float(int(10000*KSP))/10000)+"}{ }}{Stats: "+str(int(ThisHisto.GetEntries()))+"/"+str((float(iRun)/float(NRounds))*NGenEvents)+"}")
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



def ComputeCompatibility(filename,slhaname,iRun):
  global Methisto,Mllhisto,DoIllustration,HistoBTagMultiplicity,HistoJetMultiplicity,JetPthisto,NRounds,NGenEvents
  
  RefFile = ROOT.TFile("DifferentialDistributions.root")
 
  FlipOnOverFlowBin() 
 
  print "The histograms contain the following number of events   (btag/jet/mll/met):"
  print HistoBTagMultiplicity.Integral()
  print HistoJetMultiplicity.Integral()
  print Mllhisto.Integral()
  print Methisto.Integral()
  
#  RefHistoBTagMultiplicity = RefFile.Get("dBTag")
#  KSP_BTag = RefHistoBTagMultiplicity.KolmogorovTest(HistoBTagMultiplicity)
        
#  RefHistoJetMultiplicity = RefFile.Get("dJet")
#  KSP_Jet = RefHistoJetMultiplicity.KolmogorovTest(HistoJetMultiplicity)
        
  RefHistoMllLow = RefFile.Get("dMll_Low")
  KSP_LM = RefHistoMllLow.KolmogorovTest(MllhistoLowMass)
        
  RefHistoMllHigh = RefFile.Get("dMll_High")
  KSP_HM = RefHistoMllHigh.KolmogorovTest(MllhistoHighMass)
        
        #  RefMllhisto = RefFile.Get("dMll")
        #  KSP_Mass = RefMllhisto.KolmogorovTest(Mllhisto)
  
  RefMEThisto = RefFile.Get("dMET")
  KSP_MET  = RefMEThisto.KolmogorovTest(Methisto)
  
  BTag_Ratio=-1000
  if HistoBTagMultiplicity.GetBinContent(1) > 0:
      BTag_Ratio=(HistoBTagMultiplicity.Integral()-HistoBTagMultiplicity.GetBinContent(1))/(HistoBTagMultiplicity.GetBinContent(1)) #    N(>=1 btag) / N(0 btag)
  if HistoBTagMultiplicity.Integral()-HistoBTagMultiplicity.GetBinContent(1) > 0 and HistoBTagMultiplicity.GetBinContent(1)==0:
      BTag_Ratio=100 ## pretty interesting case, we only have btagged jets!
  
  print "Integral : "+str(HistoBTagMultiplicity.Integral())
  print "In 0 bin : "+str(HistoBTagMultiplicity.GetBinContent(1))
  BTagGauss = ROOT.TF1("BTagGaus","gaus",-1000,1000)
  BTagGauss.SetParameter(0,1)
  BTagGauss.SetParameter(1,27.25)  # ratio of >=1 btags / 0 btags
  BTagGauss.SetParameter(2,10)
  KSP_BTag = BTagGauss.Eval(BTag_Ratio)

#  for nbt in range(1,HistoBTagMultiplicity.GetNbinsX()+1):
#    print str(nbt)+" : "+str(HistoBTagMultiplicity.GetBinCenter(nbt))+" has "+str(HistoBTagMultiplicity.GetBinContent(nbt))+" entries"

  print "Compatibilities: "
  #  print "Mll: "+str(KSP_Mass)
  print "low mass: "+str(KSP_LM)
  print "high mass: "+str(KSP_HM)
  print "BTag: "+str(KSP_BTag)
#  print "BTag: "+str(KSP_BTag)
#  print "JET: "+str(KSP_Jet)
  print "MET: "+str(KSP_MET)
        
  KSP_Final = KSP_LM + KSP_HM + KSP_MET + KSP_BTag
  
  PResult=str(("{0:.10f}".format(KSP_Final)))
  PResult="0p"+PResult[2:]
  
  mswrite = open("Marker.txt",'w')
  mswrite.write(PResult)
  mswrite.close()
  
  kaswrite = open("KS.txt",'w')
  kaswrite.write(str(KSP_Final))
  kaswrite.close()
  
  kspwrite = open("KS_Summary.txt",'w')
  kspwrite.write(str(KSP_LM)+' ')
  kspwrite.write(str(KSP_HM)+' ')
  kspwrite.write(str(KSP_MET)+' ')
  kspwrite.write(str(KSP_BTag)+' ')
  kspwrite.write(str(float(Methisto.Integral()/    ((float(iRun)/float(NRounds))*NGenEvents))   )+' ')
  
  
#  print "Compatibility in low mass: "+str(KSP_LowMass)
#  print "Compatibility in Z mass: "+str(KSP_ZMass)
#  print "Compatibility in high mass: "+str(KSP_HighMass)
#  print "Compatibility in mass range: "+str(KSP_Mass)
  print "Compatibility in low mass range: "+str(KSP_LM)
  print "Compatibility in high mass range: "+str(KSP_HM)
#  print "Compatibility in NJets: "+str(KSP_Jet)
#  print "Compatibility in centrality: "+str(KSP_Centrality)
#  print "Compatibility in BTag: "+str(KSP_BTag)
  print "Compatibility in MET: "+str(KSP_MET)
  print "Compatibility in N(>=1b)/N(0b): "+str(KSP_BTag)+"   (ratio is "+str(BTag_Ratio)+")"
  
  print "Combined Kolmogorov-Smirnov probability is "+str(KSP_Final)
  
  print "The MET histogram contains "+str(Methisto.Integral())
  
  OutFile = ROOT.TFile("GeneratedDistributions_v2_JetPtThreshold30.root","RECREATE")
  OutFile.cd()
  HistoBTagMultiplicity.Write()
  Mllhisto.Write()
  Methisto.Write()
  HistoJetMultiplicity.Write()
  JetPthisto.Write()
  OutFile.Close()
  
  if not DoIllustration:
      return KSP_Final
  
  # now draw this!
  print "NEED TO UPDATE PLOTTING FOR FOUR REGIONS"

#  Illustrate(HistoCentrality,RefHistoCentrality,slhaname,KSP_Centrality)
#  Illustrate(HistoBTagMultiplicity,RefHistoBTagMultiplicity,slhaname,KSP_BTag)
#  Illustrate(Mllhisto,RefMllhisto,slhaname,KSP_Mass)
  Illustrate(MllhistoLowMass,RefHistoMllLow,slhaname,KSP_LM)
  Illustrate(MllhistoHighMass,RefHistoMllHigh,slhaname,KSP_HM)
  Illustrate(Methisto,RefMEThisto,slhaname,KSP_MET)
#  Illustrate(HistoJetMultiplicity,RefHistoJetMultiplicity,slhaname,KSP_Jet)
#  Illustrate(MllhistoZmass,RefMllhistoZmass,slhaname,KSP_ZMass)
#  Illustrate(MllhistoHighMass,RefMllhistoHighMass,slhaname,KSP_HighMass)

  RefFile.Close()
  
  return KSP_Final
  

def ProcessFile(filename,slhaname,iRun) :
  
  FillHistogram(filename)
  
  #sprint "not storing histogram"
  #StoreHistogram(filename)
  
  global compat
  compat = ComputeCompatibility(filename,slhaname,iRun)
  
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


def CrunchNumbersForLHEFile(filename,slhaname,iRun):
  global compat
  return [ProcessFile(filename,slhaname,iRun),compat]
  
def AnalyzeThisSLHA(slhapath):
  retval=0.0
  for iRun in range(1,NRounds+1):
    rightnow=time.time()
    elapsed = rightnow-start
    if elapsed>30*60: # more than half an hour has passed
       print "This SLHA is taking too long to process - will stick with the statistics we currently have and be happy with that"
       break
    else: 
      print "So far only "+str(elapsed)+" seconds have passed; this means it's ok to add another LHE round (which we wanted anyway)"
      print "Now doing round "+str(iRun)+" of generating an LHE file (doing it in "+str(NRounds)+" rounds to reduce sandbox usage)"
      LHEFile = GenerateLHEFile(slhapath)
      retval=CrunchNumbersForLHEFile(LHEFile,slhapath,iRun)[0]
      commands.getoutput("rm "+LHEFile)
  
  return retval

if len(sys.argv)<2:
  print "You need to indicate an SLHA file (including its FULL PATH) !"
  sys.exit(-1)

SLHA_PATH=sys.argv[1]
start = time.time()

result = AnalyzeThisSLHA(SLHA_PATH)

sys.exit(result)
