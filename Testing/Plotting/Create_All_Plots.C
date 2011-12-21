#include <iostream>
#include <vector>
#include <sys/stat.h>
#include "Modules/GeneralToolBox.C"
#include "Modules/SampleClass.C"
#include "Modules/setTDRStyle.C"
#include "Modules/Setup.C"
#include "Modules/Poisson_Calculator.C"
#include "Modules/ActiveSamples.C"
#include "Modules/PeakFinder.C"
#include "Modules/UpperLimitsWithShape.C"
//#include "Modules/external/cl95cms.c"
//#include "Modules/external/roostats_cl95.C"
#include "Modules/Plotting_Functions.C"
#include "Modules/ResultModule.C"
#include "Modules/LimitCalculation.C"
//#include "Modules/ExperimentalModule.C"
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

//using namespace std;

using namespace PlottingSetup;

int main(int narg, char *argv[])
{
  gROOT->SetStyle("Plain");
  bool do_fat_line=false; // if you want to have HistLineWidth=1 and FuncWidth=1 as it was before instead of 2
  setTDRStyle(do_fat_line);
  gStyle->SetTextFont(42);
  bool showList=true;
  set_directory(directoryname);//Indicate the directory name where you'd like to save the output files in Setup.C
  set_treename("events");//you can set the treename here to be used; options are "events" (for reco) for "PFevents" (for particle flow)
//  define_samples(showList,allsamples);
  define_samples(showList,allsamples,signalsamples,scansample,raresample,systsamples);
  setlumi(luminosity);
  do_png(true);
  do_pdf(true);
  do_eps(false);
  do_C(false);
  do_root(true);
  setessentialcut(essential);//this sets the essential cut; this one is used in the draw command so it is AUTOMATICALLY applied everywhere. IMPORTANT: Do NOT store weights here!
  stringstream resultsummary;

  
  //what to do : 
  bool do_all           = false;/// DONE
  bool do_peak_finding              = true; /// DONE
  bool do_kinematic_variables       = false; ///DONE
  bool do_kinematic_PF_variables    = false; ///DONE
  bool do_region_comparison         = false; /// Comparison between signal and control regions
  bool do_lepton_comparison         = false; ///DONE
  bool do_jzb_plots                 = false; /// DONE
  bool do_pred                      = false; /// DONE
  bool overlay_signal               = false; /// Overlay LM4 signal to predictions
  bool do_ratio                     = false; /// DONE ------------------------------------------- still need to adapt the legend!
  bool do_signal_bg_comparison_plot = false; /// DONE
  bool do_jzb_efficiency_curve      = false;
  bool do_ttbar_comparison          = false; /// DONE
  bool do_zjets_comparison          = true; /// DONE
  bool do_JES                       = false; /// DONE
////  bool calculate_pred_and_observed  = false; /// DONE -- now rerouted to the new way of computing results
//  bool calculate_yields             = false; ///DONE /// superfluous (replaced by get_new_results)
  bool get_new_results              = false; ///DONE
  bool dopoisson                    = false;//should we calculate stat err with poisson? ///DONE
  bool verbose                      = false;//prints out a lot more information ... ///DONE
  //--------------------------------------------
  // Systematics and limits
  bool do_compute_systematics=false; /// DONE (might want to add something related to the peak?)
  bool requireZ=true; // should we require a Z for MC efficiency?
  bool do_compute_efficiency=false; /// This is the cumulative efficiency that we quoted in our first AN note. Not fully finished.
  bool do_prepare_limits_using_shapes=false; /// ALMOST DONE **************************************
  bool do_compute_upper_limits_from_counting_experiment=false; /// DONE
  bool doobserved=false; // want to get the observed limit as well?
  bool doquick=true; // this will cause the number of predicted/observed events to hurry up (no MC!)
  
  //--------------------------------------------
  // More not-so-standard stuff
  bool do_model_scan=false; // this can take days - use the grid version!
  bool do_zjet_ttbar_shapes=false; /// DONE
  bool do_test=false;/// DONE    (just any test you wish)
  bool do_pick_up_events=false; ///DONE
  
  //--------------------------------------------
  // To be run manually !
  bool do_response_correction=true; ///DONE 
               //use this to find out the correction factor; originally this was done automatically but now you should do this separately and update Modules/Setup.C
  
  
  //--------------------------------------------
  // getting this from input!
  bool do_save_template=false; // the configuration (i.e. peak positions, observed, predicted, jzb bins) is saved to a file, to be used with DistributedModelCalculations etc.
     if(narg==2&&argv[1]!="") do_save_template=true;
  if(narg>2) {
    print_usage();
    return 0;
  }
  
  
  todo();
  //**** part 1 : peak finding
  float MCPeak=0,MCPeakError=0,DataPeak=0,DataPeakError=0,MCSigma=10,DataSigma=10;
  method=Kostasmethod;//Kostasmethod;//dogaus3sigma;// options: dogaus,doKM,dogaus2sigma,dogaus3sigma
  if(do_peak_finding||do_zjet_ttbar_shapes||do_region_comparison||do_jzb_plots||do_lepton_comparison||do_signal_bg_comparison_plot||do_pred||do_ratio||do_save_template||do_compute_upper_limits_from_counting_experiment||do_compute_systematics||do_compute_efficiency||do_model_scan||do_prepare_limits_using_shapes||do_all||get_new_results||do_ttbar_comparison||do_zjets_comparison) find_peaks(MCPeak,MCPeakError, DataPeak, DataPeakError,MCSigma,DataSigma,resultsummary);
  
  stringstream datajzb;
  if(DataPeak>0) datajzb<<"("<<jzbvariabledata<<"-"<<TMath::Abs(DataPeak)<<")";
  else datajzb<<"("<<jzbvariabledata<<"+"<<TMath::Abs(DataPeak)<<")";
  stringstream mcjzb;
  if(MCPeak>0) mcjzb<<"("<<jzbvariablemc<<"-"<<TMath::Abs(MCPeak)<<")";
  else mcjzb<<"("<<jzbvariablemc<<"+"<<TMath::Abs(MCPeak)<<")";
  
  dout << "With peak correction, we get : " << endl;
  dout << "    Data : " << datajzb.str() << endl;
  dout << "    MC : " << mcjzb.str() << endl;
  
  //This is the binning we'll use for ratio plots
  global_ratio_binning.push_back(0);
  global_ratio_binning.push_back(5);
  global_ratio_binning.push_back(10);
  global_ratio_binning.push_back(20);
  global_ratio_binning.push_back(50);
  global_ratio_binning.push_back(100);
  global_ratio_binning.push_back(200);
  global_ratio_binning.push_back(350);
  //global_ratio_binning.push_back(500);

    //these are the JZB cuts defining our search regions
  vector<float> jzb_cut;
  jzb_cut.push_back(50);
//  jzb_cut.push_back(75);
  jzb_cut.push_back(100);
//  jzb_cut.push_back(125);
  jzb_cut.push_back(150);
//  jzb_cut.push_back(175);
  jzb_cut.push_back(200);
//  jzb_cut.push_back(225);
  jzb_cut.push_back(250);


  
  //**** part 2 : kinematic plots
  if(do_kinematic_variables||do_all) do_kinematic_plots();
  if(do_kinematic_PF_variables) do_kinematic_PF_plots();
  
  //**** part 3: Leptonic comparison (ee vs mm, eemm vs emu)
  if(do_lepton_comparison||do_all) lepton_comparison_plots();

  //**** part 3b: comparison between control regions (SFZPJZBPOS,SFZPJZBNEG,...)
  if (do_region_comparison||do_all) region_comparison_plots(mcjzb.str(),datajzb.str(),jzb_cut);

  //**** part 4: JZB plots (OSOF, OSSF) 
  if(do_jzb_plots||do_all) jzb_plots(mcjzb.str(),datajzb.str(),global_ratio_binning);
  //**** part 5 : Prediction plots
  if(do_pred||do_all) do_prediction_plots(mcjzb.str(),datajzb.str(),DataSigma,MCSigma,overlay_signal);
  
  //**** part 6: Ratio plots
  if(do_ratio||do_all) do_ratio_plots(mcjzb.str(),datajzb.str(),global_ratio_binning);
  
  //**** part 7: some decoration: the old 
  if(do_signal_bg_comparison_plot||do_all) signal_bg_comparison();
  if(do_ttbar_comparison||do_all) ttbar_sidebands_comparison(mcjzb.str(),global_ratio_binning);
  if(do_zjets_comparison||do_all) zjets_prediction_comparison();
  if(do_JES||do_all) make_JES_plot();

  if (do_jzb_efficiency_curve ) plot_jzb_sel_eff(mcjzb.str(),signalsamples,jzb_cut);


  //**** part 8: observed and predicted!
  if(do_all||get_new_results) doquick=false;
  if(do_all||do_save_template||do_compute_upper_limits_from_counting_experiment||do_model_scan||get_new_results) get_result(mcjzb.str(),datajzb.str(),DataPeakError,MCPeakError,jzb_cut,verbose,dopoisson,doquick);
  
  vector<float>jzb_shape_limit_bins;
  jzb_shape_limit_bins.push_back(50);jzb_shape_limit_bins.push_back(75);jzb_shape_limit_bins.push_back(100);
  jzb_shape_limit_bins.push_back(150);jzb_shape_limit_bins.push_back(200);jzb_shape_limit_bins.push_back(500);
  if(do_prepare_limits_using_shapes) prepare_limits(mcjzb.str(),datajzb.str(),DataPeakError,MCPeakError,jzb_shape_limit_bins);
  
  //------------------------ end of standard functions
  
  vector<vector<float> > all_systematics;
  vector<vector<float> > all_efficiency;

  if(do_compute_systematics||do_compute_upper_limits_from_counting_experiment||do_all) all_systematics=compute_systematics(mcjzb.str(),MCPeakError,alwaysflip,datajzb.str(),signalsamples,jzb_cut,requireZ);
//  if(do_compute_efficiency) compute_efficiency(mcjzb.str(),allsamples, 50, -200, 300);
  if(do_compute_efficiency) write_warning(__FUNCTION__,"efficiency computation deactivated");

  if(do_compute_upper_limits_from_counting_experiment||do_all) compute_upper_limits_from_counting_experiment(all_systematics,jzb_cut,mcjzb.str(),doobserved,alwaysflip);
  
  //------------------------ end of analysis parts - below: special applications
  
  if(do_model_scan) scan_SUSY_parameter_space(mcjzb.str(),datajzb.str(),jzb_cut,requireZ,MCPeakError);
  
  if(do_zjet_ttbar_shapes) draw_ttbar_and_zjets_shape(mcjzb.str(),datajzb.str());
    
  //now do predicted and observed. 
  //to that end, we need to consider systematics! 
  // systematics come from: 
  // Poisson error on pred. contributions (easy)
  // +20%/-40% from MC closure test
  // from peak position --> whenever we want to investigate systematics, peak position must have been set! 
  
  //jzb response correction
  if(do_response_correction) {
    find_correction_factors(jzbvariabledata,jzbvariablemc);
    dout << "Please update Modules/Setup.C to reflect the following values (round them first ... )" <<endl;
    dout << "Corrected JZB variable definition " << endl << "  Data: " << jzbvariabledata << "  and MC: " << jzbvariablemc << endl;
    dout << endl;
    dout << "If you're feeling lazy, copy & paste this : " << endl;
    dout << "  string jzbvariabledata=\""<<jzbvariabledata<<"\";"<<endl;
    dout << "  string jzbvariablemc=\""  <<jzbvariablemc<<"\";"<<endl;
  }
  
   if(do_pick_up_events) {
    
    dout << "Observed: " << endl;
    pick_up_events((const char*)(cutmass&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    dout << "Predicted (JZB<-100) OSSF" << endl;
    pick_up_events((const char*)(cutmass&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
    dout << "Predicted (emu, JZB>100) OSOF" << endl;
    pick_up_events((const char*)(cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    dout << "Predicted (emu, JZB<-100) OSOF" << endl;
    
    if(PlottingSetup::RestrictToMassPeak) {
      pick_up_events((const char*)(cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
      dout << "Predicted (SB SF, JZB>100) OSSF" << endl;
      pick_up_events((const char*)(sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
      dout << "Predicted (SB SF, JZB<-100) OSSF" << endl;
      pick_up_events((const char*)(sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
      dout << "Predicted (SB emu, JZB>100) OSOF" << endl;
      pick_up_events((const char*)(sidebandcut&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
      dout << "Predicted (SB emu, JZB<-100) OSOF" << endl;
      pick_up_events((const char*)(cutOSOF&&cutnJets&&basiccut&&sidebandcut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
    }
  }

  if(do_save_template) save_template(mcjzb.str(),datajzb.str(),jzb_cut,MCPeakError);
  
  if(do_test) test();
  
  return 0;  
}

