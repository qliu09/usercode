#include <iostream>
#include <fstream>
#include <TCut.h>
#include <TColor.h>
#include <TStyle.h>

#ifndef SampleClassLoaded
#include "SampleClass.C"
#endif
#define SetupLoaded

using namespace std;


namespace PlottingSetup {

  string directoryname="SNAF_tester";
  
  bool RestrictToMassPeak=true; //if you want to switch between offpeak ("false") and onpeak ("true") analysis please use this switch; the masscut below will be adapted automatically when adding samples :-)
  
  float luminosity=3523.18;//1936;//751.0;//486.0;//468.0//336.;//pb^{-1}
  float lumiuncert=0.045;// to be indicated in [0,1] range, e.g. for 4% write 0.04

  string jzbvariabledata="jzb[1]+0.06*pt";
  string jzbvariablemc="jzb[1]+0.04*pt";
  float jzbHigh = 350.; // Range for JZB plots

  samplecollection allsamples("completesamplecollection");
  samplecollection signalsamples("signalsamplecollection");
  samplecollection scansample("scansamplecollection");
  samplecollection raresample("raresamplecollection");
  int data=1;
  int mc=0;
  int mcwithsignal=2;
  TCut passtrig("(passed_triggers||!is_data)");
  TCut cutmass("abs(mll-91.2)<20");
  TCut genMassCut("abs(genMll-91.2)<20");
  TCut openmasscut("mll>40"); // this is the mass cut used in the off peak analysis!
  TCut openGenmasscut("genMll>40");
  //TCut cutmass("mll>2");
  TCut basiccut("mll>2");//basically nothing.
  TCut basicqualitycut("(pfJetGoodNum>=2&&pfJetGoodID[0]!=0)&&(pfJetGoodNum>=2&&pfJetGoodID[1]!=0)");//don't use this for the "essential cut", because we want to plot nJets as well as mll in the inclusive case; we thus use it as an addition nJets cut.
  //TCut jetqualitycut("(pfJetGoodNum>=2&&pfJetGoodID[0])&&(pfJetGoodNum>=2&&pfJetGoodID[1])");//now part of the basiccut
  //TCut jetqualitycut("mll>0");
  
  TCut cutnJets("pfJetGoodNum>=3"&&basicqualitycut);
  TCut cutnJetsJESdown("pfJetGoodNum25>=3"&&basicqualitycut);
  TCut cutnJetsJESup("pfJetGoodNum35>=3"&&basicqualitycut);
  TCut cutOSOF("(id1!=id2)&&(ch1*ch2<0)");
  TCut cutOSSF("(id1==id2)&&(ch1*ch2<0)");
  TCut sidebandcut("(mll>55&&mll<70)||(mll>112&&mll<160)");

  //TCut sidebandcut("(mll>61&&mll<70)||(mll>112&&mll<190)");
  //TCut basiccut("(passed_triggers||!is_data)");
  
  // SUSY scan parameters
  float mglustart=25;float mgluend=1200;float mglustep=25;
  float mLSPstart=25;float mLSPend=1200;float mLSPstep=25;
  
  float m0start=20;  float m0end=2000;  float m0step=20;
  float m12start=20; float m12end=760;  float m12step=20;
  
  int ScanXzones=10; // number of zones in x for (mSUGRA) scans
  int ScanYzones=10; // number of zones in y for (mSUGRA) scans

  TCut essential(passtrig);//add here any cuts you ALWAYS want
  int dogaus=0;
  int doKM=1;
  int dogaus2sigma=2;
  int dogaus3sigma=3;
  int Kostasmethod=-99;
  
  float fitresultconstdata=0;//this is the result when fitting in the 0-30 GeV range
  float fitresultconstmc=0;//this is the result when fitting in the 0-30 GeV range
  
  int method=-1;//Fitting method
  
  //now some style issues:
  float DataMarkerSize=1.2;
  
  //here we save our number of predicted and observed events (with errors)
  vector<float> Nobs;
  vector<float> Npred;
  vector<float> Nprederr;
  
  int noJES=0;
  int JESdown=1;
  int JESup=2;
  
  //some refinement: nicer color gradient
  Double_t stops[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[5]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[5] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[5]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  int fi=TColor::CreateGradientColorTable(5, stops, red, green,blue, 255);
  
  // LIMITS
  int nlimittoys=1000; // how many toys for setting limits
  string limitmethod="cls";//what method to use to set limits
  int limitpatience=50; // for how many minutes should the limit calculation (for one configuration!) be allowed to run before being aborted? this only has an effect when when running on the grid.
  bool ConsiderSignalContaminationForLimits=true; //whether or not to consider signal contamination when computing limits (standard:true)
  int nuisancemodel=1;
  
  float JZBPeakPositionData=-999;
  float JZBPeakPositionMC=-999;
  float JZBPeakWidthData=-999;
  float JZBPeakWidthMC=-999;

  // two possible future systematics that ATM only take up CPU time in SUSY scans
  bool computeJZBefficiency=false;
  bool computeJZBresponse=false;

  //watch out, the cbafbasedir string is in GeneralToolBox
}
