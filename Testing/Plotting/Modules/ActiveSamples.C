/****

Off peak status (RestrictToMassPeak) : 

x  Necessary adaptations identified
x  Started working on necessary adaptations
x  Necessary adaptations implemented
x  Necessary adaptations tested

DONE!


****/

#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

#include <TMath.h>
#include <TColor.h>
#include <TPaveText.h>
#include <TRandom.h>
#include <TF1.h>

#ifndef SampleClassLoaded
#include "ActiveSamples.C"
#endif

#ifndef Verbosity
#define Verbosity 0
#endif
#ifndef HUSH
#define HUSH 1
#endif 

void define_SMS_sample(bool showList, samplecollection &allsamples, samplecollection &signalsamples, samplecollection &scansample, bool loadall=false)
{
    char hostname[1023];
    gethostname(hostname,1023);

    /*
    
    CRUCIAL: The NAME needs to contain either "SMS" or "mSUGRA" for the SUSY scan algorithm (to be able to distinguish the two)
    
    */

    string prefix="/shome/";
    if((int)((string)hostname).find("falda")>-1) prefix="/scratch/";
    
      /// NEW: Only indicate the very first file - please place scans in ntuples/SMS/ and adapt the name here and in SUSYScan.C (the files are loaded when needed avoiding high memory usage and startup times)
	scansample.AddSample(prefix+"buchmann/ntuples/mSUGRA/mSUGRA_clean_splitup_0_0.root","mSUGRA",1,1,false,true,0,kRed); // there is only one sample in the scan; we give each event weight "1"
//	scansample.AddSample(prefix+"buchmann/ntuples/SMS/SMS_clean_splitup_0_0.root","SMS",1,1,false,true,0,kRed); // there is only one sample in the scan; we give each event weight "1"

    if(loadall) {
      for(int ix=0;ix<PlottingSetup::ScanXzones;ix++) {
	for(int iy=1;iy<PlottingSetup::ScanYzones;iy++) {//note: the "zero-zero" sample is already loaded above!
	  scansample.AddSample((prefix+"buchmann/ntuples/SMS/SMS_clean_splitup_"+any2string(ix)+"_"+any2string(iy)+".root"),"SMS",1,1,false,true,ix*PlottingSetup::ScanXzones+iy,kRed); // there is only one sample in the scan; we give each event weight "1"
	}
      }
    }
	

    flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
    if(!PlottingSetup::RestrictToMassPeak) {
      PlottingSetup::cutmass=PlottingSetup::openmasscut;
//      PlottingSetup::sidebandcut=TCut("mll<2&&mll>3");//impossible cut - because we don't want any sidebands!
      PlottingSetup::sidebandcut=TCut("SideBandsHaveBeenCalledForOffPeakAnalysis__ERROR==1");//this will trigger errors whenever sidebands are called - perfect for checking any remaining uses (checked, and no uses were found).
      PlottingSetup::genMassCut=PlottingSetup::openGenmasscut;//impossible cut - because we don't want any sidebands!
    }
}

void define_samples(bool showList, samplecollection &allsamples, samplecollection &signalsamples, samplecollection &scansample, samplecollection &raresample)
{
    flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
    if(!PlottingSetup::RestrictToMassPeak) {
      PlottingSetup::cutmass=PlottingSetup::openmasscut;
      PlottingSetup::sidebandcut=TCut("mll<2&&mll>3");//impossible cut - because we don't want any sidebands!
      PlottingSetup::genMassCut=PlottingSetup::openGenmasscut;//impossible cut - because we don't want any sidebands!
    }
  
    float ZJetsCrossSection         = 3048.0; //NNLO----------------------------------
    float TTbarCrossSection         = 165.0;//(NLO) ---- 165.0; // approx. NNLO-----
    float WJetsCrossSection         = 31314.0;//NNLO-------3.131e4; //NNLO------------
    float ZnunuCrossSection         = 5760.0;//NNLO -------4.5e+3; //(LO);------------
//Single top x-sections from here: 
//https://twiki.cern.ch/twiki/bin/view/CMS/SingleTopSigma
    float SingleTopSCrossSection     = 4.63; // NLO;----------------------------------
    float SingleTopTCrossSection     = 64.57; // NLO;--------------------------------
    float SingleTopUCrossSection     = 15.74; // NLO;----------------------------------
    float VVJetsCrossSection        = 4.8; // LO;-------------------------------------
    float LM4CrossSection           = 1.879*1.35; // k*LO
    float LM8CrossSection           = 0.7300*1.41 ; // k*LO  

    
    //the following numbers are from the MadGraphStandardModel210Summary as linked on the GeneratorProduction2011 page    
    //    float QCD50to100CrossSection=30000000; // not used
    float QCD100to250CrossSection=7000000;
    float QCD250to500CrossSection=171000;
    float QCD500to1000CrossSection=5200;
    float QCD1000toInfCrossSection=83.0;
    
    //    float totEventsQCD50to100=207418.0; // not used
    long totEventsQCD100to250   = 638792;
    long totEventsQCD250to500   = 344454; 
    long totEventsQCD500to1000  = 10e10;  //unknown but now suppressed. (no event passes anyway)
    long totEventsQCD1000toInf  = 163185;

    
    long totEventsZjets         = 32005720;//summer11
    //spring:2313911.0;//2743142.0; // DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola
    long totEventsTTbar         =  3701872;//summer11
    //1161621.0;//Spring//1144028.0; // PabloV13/TTJets_TuneZ2_7TeV-madgraph-tauola
    long totEventsWJets         = 56674902;//summer11
    //15010237.0;//spring//14350756.0; /// WJetsToLNu_TuneZ2_7TeV-madgraph-tauola.root
    long totEventsZnunu         =  2106977;//2167964.0; //ZinvisibleJets_7TeV-madgraph.root
    long totEventsVVJets        =   959076;//509072.0;
    long totEventsSingleTopS    =   493868;//489472.0;
    long totEventsSingleTopT    =   475460;//477610.0;
    long totEventsSingleTopU    =   489417;//477599.0;
    long totEventsLM4           =   218380;                                                                                                                                                                                                                                  
    long totEventsLM8           =   180960; 

    
    Int_t nice_blue  = TColor::GetColor("#2E9AFE");
    Int_t nice_green = TColor::GetColor("#81f781");
    Int_t nice_red   = TColor::GetColor("#F78181");
    Int_t nice_pink  = TColor::GetColor("#F781BE");
    Int_t nice_orange= TColor::GetColor("#F7BE81");//#FA9624
    Int_t nice_black = TColor::GetColor("#2A0A1B");//black with a bit of purple :-)    
    
    
    Int_t dy_color        = nice_blue;
    Int_t wjets_color     = kGray;
    Int_t ttbar_color     = nice_green;
    Int_t singletop_color = nice_orange;
    Int_t qcd_color       = nice_black;
    Int_t diboson_color   = nice_pink;
    Int_t lm_color        = kRed;
    
    dy_color 		= kYellow;
    wjets_color 	= kGray;
    ttbar_color 	= kMagenta+2;
    singletop_color	= kBlue;
    qcd_color		= kPink;
    diboson_color	= kGreen;
    lm_color		= kViolet+7;

    Int_t rare_color		= nice_blue;
  
    char hostname[1023];
    gethostname(hostname,1023);
    
    string prefix="/shome/";
    if((int)((string)hostname).find("falda")>-1) prefix="/scratch/";
    
//    allsamples.AddSample(prefix+"buchmann/ntuples/Data/AllData_2809pb.root","Data",1,1,true,false,0,kBlack);
    allsamples.AddSample(prefix+"buchmann/ntuples/Data/NewSelectionForPaper/AllData_3p5_newselection__updated.root ","Data",1,1,true,false,0,kBlack);

    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TTJets_TuneZ2_7TeV-madgraph-tauola_Summer11_2.root","TTbar",totEventsTTbar,TTbarCrossSection,false,false,1,ttbar_color);//summer11
    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola_Summer11_2.root","WJets",totEventsWJets,WJetsCrossSection,false,false,3,wjets_color);//summer
    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/VVJetsTo4L_TuneD6T_7TeV-madgraph-tauola.root","DiBosons",totEventsVVJets,VVJetsCrossSection,false,false,4,diboson_color);
    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TToBLNu_TuneZ2_s-channel_7TeV-madgraph_2.root","SingleTop",totEventsSingleTopS,SingleTopSCrossSection,false,false,2,singletop_color);
    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TToBLNu_TuneZ2_t-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopT,SingleTopTCrossSection,false,false,2,singletop_color);
    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TToBLNu_TuneZ2_tW-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopU,SingleTopUCrossSection,false,false,2,singletop_color);
    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/ZinvisibleJets_7TeV-madgraph.root","Z nunu",totEventsZnunu,ZnunuCrossSection,false,false,4,diboson_color);
    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola_Summer11.root","ZJets",totEventsZjets,ZJetsCrossSection,false,false,7,dy_color);//summer11
    allsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM4_SUSY_sftsht_7TeV-pythia6.root","LM4",totEventsLM4,LM4CrossSection,false,true,8,lm_color);

    /*signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM0_SUSY_sftsht_7TeV-pythia6.root","LM0",219796,38.93*1.41 ,false,true,3,lm_color);
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM1_SUSY_sftsht_7TeV-pythia6.root","LM1",218176,4.888*1.34,false,true,4,lm_color);
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM2_SUSY_sftsht_7TeV-pythia6.root","LM2",206336,0.6027*1.33,false,true,5,lm_color);
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM3_SUSY_sftsht_7TeV-pythia6.root","LM3",215120,3.438*1.40,false,true,6,lm_color);*/
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM4_SUSY_sftsht_7TeV-pythia6.root","LM4",totEventsLM4,LM4CrossSection,false,true,1,lm_color);/*
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM5_SUSY_sftsht_7TeV-pythia6.root","LM5",223992,0.473*1.34,false,true,7,lm_color);*/
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM8_SUSY_sftsht_7TeV-pythia6.root","LM8",totEventsLM8,LM8CrossSection,false,true,2,lm_color);/*
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM9_SUSY_sftsht_7TeV-pythia6.root","LM9",227808,7.134*1.48,false,true,8,lm_color);
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM11_SUSY_sftsht_7TeV-pythia6.root","LM11",218703,0.8236*1.35,false,true,9,lm_color);
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM12_SUSY_sftsht_7TeV-pythia6.root","LM12",223491,4.414*1.34,false,true,10,lm_color);
    signalsamples.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/LM13_SUSY_sftsht_7TeV-pythia6.root","LM13",173240,6.899*1.42,false,true,11,lm_color);*/

    // We do not use the following (rare) samples; nonetheless we include them in a separate category to prove that they are negligible
    write_warning(__FUNCTION__,"Rare samples not yet reprocessed with new selectioN");/*
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TTWWTo2Lplus2Nu_NTP.root","Rare SS",39791,4.76E-5,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/WWminusTo2L2Nu_NTP.root","Rare SS",51063,0.001559,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TTWWTo2Lminus2Nu_NTP.root","Rare SS",39496 ,4.719E-5,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TTWTo2Lplus2Nu_NTP.root","Rare SS",48764,0.006841,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/WWplusTo2L2Nu_NTP.root","Rare SS",48244 ,0.004216,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TTZTo2Lplus2Nu_NTP.root","Rare SS",36740,0.002024,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/WWWTo2Lplus2Nu_NTP.root","Rare SS",43322,0.008957,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TTWTo2Lminus2Nu_NTP.root","Rare SS",50435,0.002705,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/WWWTo2Lminus2Nu_NTP.root","Rare SS",17147,0.004109,false,false,6,rare_color);//summer11
    raresample.AddSample(prefix+"buchmann/ntuples/MC/NewSelectionForPaper/TTZTo2Lminus2Nu_NTP.root","Rare SS",36781,0.001946,false,false,6,rare_color);//summer11
    */
    //scan files are only defined when necessary!

    if(showList) allsamples.ListSamples();

}
