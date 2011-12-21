/*

to compile: just run make ... that should take care of it.

*/
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include "Modules/GeneralToolBox.C"
#include "Modules/SampleClass.C"
#include "Modules/Setup.C"
#include "Modules/PeakFinder.C"
#include "Modules/Poisson_Calculator.C"
#include "Modules/setTDRStyle.C"
#include "Modules/ActiveSamples.C"
#include "Modules/UpperLimitsWithShape.C"
#include "Modules/Plotting_Functions.C" //also included for peak finding etc.
#include "Modules/LimitCalculation.C"
#include "Modules/ResultModule.C"
#include "Modules/ExperimentalModule.C"
#include "Modules/StudyModule.C" //also included for peak finding etc.
#include "Modules/Systematics.C"
#include "Modules/CrossSectionReader.C"
#include "Modules/SUSYScan.C"

#include <TCut.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TColor.h>
#include <TPaveText.h>
#include <TRandom.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TSQLResult.h>

#ifndef Verbosity
#define Verbosity 0
#endif
#ifndef HUSH
#define HUSH 1
#endif

using namespace std;

using namespace PlottingSetup;

int main()
{
  gROOT->SetStyle("Plain");
  bool do_fat_line=false; // if you want to have HistLineWidth=1 and FuncWidth=1 as it was before instead of 2
  setTDRStyle(do_fat_line);
  gStyle->SetTextFont(42);
  bool showList=false;
  set_treename("events");//you can set the treename here to be used; options are "events" (for reco) for "PFevents" (for particle flow)
  define_samples(showList,allsamples,signalsamples,scansample,raresample,systsamples);
  setlumi(luminosity);
  do_png(true);
  do_pdf(false);
  do_eps(false);
  do_C(false);
  set_directory(directoryname);//Indicate the directory name where you'd like to save the output files in Setup.C
  setessentialcut(essential);//this sets the essential cut; this one is used in the draw command so it is AUTOMATICALLY applied everywhere. IMPORTANT: Do NOT store weights here!
  stringstream resultsummary;
  
  
  //what to do : 
  bool do_all=false;/// DONE
  bool do_peak_finding=false; /// DONE
  bool calculate_pred_and_observed=false; /// DONE
  bool study_sidebands=false; /// FOR REAL
  bool do_test=false;/// DONE    (just any test you wish)
  bool do_jzb_correction=false; //use this to find out the correction factor; originally this was done automatically but now you should do this separately and update Modules/Setup.C
  bool do_find_sideband_definition=false;
  bool do_pick_up_events=false; ///DONE
  bool do_ttbar_with_shapes=false;
  bool do_upper_limits=false;
  bool do_run_check=false;
  bool do_kinematic_dist_of_pred_and_obs=false;//former do_plot_list
  
  bool do_show_dibosons=false;

  bool do_show_rare_samples=true;
  
  bool overlay_signal=false;
  
  bool do_efficiency_scan_in_susy_space=false; /// NOW OUTSOURCED TO T3
       bool requireZ=true;
       
  bool do_beautiful_ratio_plots=true;
       
  //**** part 1 : peak finding
  float MCPeak=0,MCPeakError=0,DataPeak=0,DataPeakError=0,MCSigma=10,DataSigma=10;
  method=Kostasmethod;//Kostasmethod;//dogaus3sigma;// options: dogaus,doKM,dogaus2sigma,dogaus3sigma
  if(do_peak_finding||do_show_dibosons||do_show_rare_samples||calculate_pred_and_observed||do_all) find_peaks(MCPeak,MCPeakError, DataPeak, DataPeakError,MCSigma,DataSigma,resultsummary);
  
  stringstream datajzb;
  if(DataPeak>0) datajzb<<"("<<jzbvariabledata<<"-"<<TMath::Abs(DataPeak)<<")";
  else datajzb<<"("<<jzbvariabledata<<"+"<<TMath::Abs(DataPeak)<<")";
  stringstream mcjzb;
  if(MCPeak>0) mcjzb<<"("<<jzbvariablemc<<"-"<<TMath::Abs(MCPeak)<<")";
  else mcjzb<<"("<<jzbvariablemc<<"+"<<TMath::Abs(MCPeak)<<")";
  
  dout << "With peak correction, we get : " << endl;
  dout << "    Data : " << datajzb.str() << endl;
  dout << "    MC : " << mcjzb.str() << endl;
  
  if(do_find_sideband_definition) find_sideband_definition();
  
  if(do_jzb_correction) {
    find_correction_factors(jzbvariabledata,jzbvariablemc);
    dout << "Please update Modules/Setup.C to reflect the following values (round them first ... )" <<endl;
    dout << "Corrected JZB variable definition " << endl << "  Data: " << jzbvariabledata << "  and MC: " << jzbvariablemc << endl;
    dout << endl;
    dout << "If you're feeling lazy, copy & paste this : " << endl;
    dout << "  string jzbvariabledata=\""<<jzbvariabledata<<"\";"<<endl;
    dout << "  string jzbvariablemc=\""  <<jzbvariablemc<<"\";"<<endl;
  }
  
  if(study_sidebands) look_at_sidebands(mcjzb.str(),datajzb.str());
  
  vector<float>jzb_limit_bins;
  jzb_limit_bins.push_back(50);
  //jzb_limit_bins.push_back(75);jzb_limit_bins.push_back(100);
  //jzb_limit_bins.push_back(150);jzb_limit_bins.push_back(200);jzb_limit_bins.push_back(500);

  
  if(do_ttbar_with_shapes) prepare_ttbar_limits(mcjzb.str(),datajzb.str(),DataPeakError,MCPeakError,jzb_limit_bins);
  
  if(do_upper_limits) calculate_upper_limits(mcjzb.str(),datajzb.str());
  
  if(do_pick_up_events) {
    
    dout << "Observed: " << endl;
    pick_up_events((const char*)(cutmass&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    dout << "Predicted (JZB<-100) OSSF" << endl;
    pick_up_events((const char*)(cutmass&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
    dout << "Predicted (emu, JZB>100) OSOF" << endl;
    pick_up_events((const char*)(cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    dout << "Predicted (emu, JZB<-100) OSOF" << endl;
    pick_up_events((const char*)(cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
    dout << "Predicted (SB emu, JZB>100) OSOF" << endl;
    pick_up_events((const char*)(sidebandcut&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    dout << "Predicted (SB emu, JZB<-100) OSOF" << endl;
    pick_up_events((const char*)(cutOSOF&&cutnJets&&basiccut&&sidebandcut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
    dout << "Predicted (SB SF, JZB>100) OSSF" << endl;
    pick_up_events((const char*)(sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    dout << "Predicted (SB SF, JZB<-100) OSSF" << endl;
    pick_up_events((const char*)(sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
  }
  
  if(do_run_check) run_check();
  if(do_show_dibosons) show_dibosons(datajzb.str(),mcjzb.str());
  if(do_show_rare_samples) show_rare_samples(datajzb.str(),mcjzb.str());
  
  if(do_kinematic_dist_of_pred_and_obs) kinematic_dist_of_pred_and_obs();
  
  if(do_test) test();
  
  vector<float> ratio_binning; 
  ratio_binning.push_back(0);
  ratio_binning.push_back(5);
  ratio_binning.push_back(10);
  ratio_binning.push_back(20);
  ratio_binning.push_back(50);
  ratio_binning.push_back(100);
  ratio_binning.push_back(200);
  ratio_binning.push_back(350);
  //ratio_binning.push_back(500);
  if(do_beautiful_ratio_plots) Poisson_ratio_plots(mcjzb.str(),datajzb.str(),ratio_binning,MCPeakError,DataPeakError);
  
  write_warning(__FUNCTION__,"Need to remove the next line:");
//  do_new_prediction_plots(mcjzb.str(),datajzb.str(),DataSigma,MCSigma,overlay_signal);
  
//  ttbar_sidebands_comparison(mcjzb.str());

  lm0_illustration();
  return 0;  
}

 
