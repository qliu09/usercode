#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "Modules/GeneralToolBox.C"
#include "Modules/SampleClass.C"
#include "Modules/setTDRStyle.C"
#include "Modules/Setup.C"
#include "Modules/Poisson_Calculator.C"
#include "Modules/ActiveSamples.C"
#include "Modules/PeakFinder.C"
#include "Modules/UpperLimitsWithShape.C"
#include "Modules/Plotting_Functions.C"
#include "Modules/LimitCalculation.C"
#include "Modules/ResultModule.C"
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

using namespace PlottingSetup;

void usage(int passed=0	) {
  std::cout << "USAGE : " << std::endl;
  std::cout << "You can use different options when running this program : " << std::endl;
  std::cout << std::endl;
  std::cout << "\033[1;34m all\033[0m  \t\t  All processes of the standard workflow" << std::endl;
  std::cout << "\033[1;34m peak\033[0m  \t\t  Peak finding" << std::endl;
  std::cout << "\033[1;34m kin\033[0m  \t\t  Kinematic plots" << std::endl;
  std::cout << "\033[1;34m pfkin\033[0m  \t  Kinematic plots for PF variables" << std::endl;
  std::cout << "\033[1;34m region\033[0m  \t  Region comparison plots" << std::endl;
  std::cout << "\033[1;34m lepton\033[0m  \t  Lepton comparison plots" << std::endl;
  std::cout << "\033[1;34m jzb\033[0m  \t\t  JZB plots" << std::endl;
  std::cout << "\033[1;34m pred\033[0m  \t\t  Prediction plots" << std::endl;
  std::cout << "\033[1;34m ratio\033[0m  \t  Ratio plots (standalone ratio plot)" << std::endl;
  std::cout << "\033[1;34m signalbg\033[0m  \t  Signal vs. background plots (shape comparison)" << std::endl;
  std::cout << "\033[1;34m ttbar\033[0m  \t  TTbar sideband comparison" << std::endl;
  std::cout << "\033[1;34m zjets\033[0m  \t  ZJets plots" << std::endl;
  std::cout << "\033[1;34m results\033[0m  \t  Compute results (JZB>X)" << std::endl;
  std::cout << "\033[1;34m syst\033[0m  \t\t  Compute systematics" << std::endl;
  std::cout << "\033[1;34m eff\033[0m  \t\t  Compute all (partial) efficiencies" << std::endl;
  std::cout << "\033[1;34m shapes\033[0m  \t  Prepare ROOT files for computing limits using shapes" << std::endl;
  std::cout << "\033[1;34m counting\033[0m  \t  Establish limits with counting experiment" << std::endl;
  std::cout << "\033[1;34m response\033[0m  \t  Compute response correction" << std::endl;
  std::cout << "\033[1;34m pickup\033[0m  \t  Pick up interesting events" << std::endl;
  std::cout << "\033[1;34m yields\033[0m  \t  Compute MC yields for this lumi" << std::endl;
  std::cout << "\033[1;34m sideassess\033[0m\t  Assessment of the mass sidebands definition in the data" << std::endl;
  std::cout << "\033[1;34m jes\033[0m \t\t  Make Jet Energy Scale (JES) plot" << std::endl;
  std::cout << "\033[1;34m save\033[0m  \t\t  Save results in a template file (for reuse when establishing limits for scans)" << std::endl;
  std::cout << "\033[1;34m png\033[0m   \t\t  Save all plots as pngs (also available: --pdf, --root, --eps, --C) standard: png&pdf" << std::endl;
  std::cout << "\033[1;34m offpeak\033[0m   \t  Sets offpeak to true (i.e. carry out off-peak analysis!)" << std::endl;
  std::cout << "\033[1;34m onpeak\033[0m   \t  Sets onpeak to true (i.e. carry out standard on-peak analysis!)" << std::endl;
  std::cout << "\033[1;34m jzbcuts\033[0m  \t  Sets the jzb cuts (please provide them in the format 50,100,150,200 and so on (comma separated)" << endl; 
  std::cout << std::endl;
  std::cout << "\033[1;34m dir (d)\033[0m\t  Directory where all plots will be saved" << std::endl;
  std::cout << "\033[1;34m public \033[0m\t  Switch to be activated when using the algorithm in public (no branding etc.)" << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "\033[1;34m met\033[0m \t\t  Compute obs/pred for met cut vs jzb cut (in data, lm4 and lm8)" << std::endl;
  exit(-1);
}

int main (int argc, char ** argv)
{
  int do_all           = false;/// DONE
  int do_peak_finding              = false; /// DONE
  int do_kinematic_variables       = false; ///DONE
  int do_kinematic_PF_variables    = false; ///DONE
  int do_region_comparison         = false; /// Comparison between signal and control regions
  int do_lepton_comparison         = false; ///DONE
  int do_jzb_plots                 = false; /// DONE
  int do_pred                      = false; /// DONE
	bool overlay_signal               = false; /// Overlay LM4 signal to predictions
  int do_ratio                     = false; /// DONE ------------------------------------------- still need to adapt the legend!
  int do_signal_bg_comparison_plot = false; /// DONE
  int do_ttbar_comparison          = false; /// DONE
  int do_jzb_efficiency_curve      = false; /// DONE
  int do_zjets_comparison          = false; /// DONE
//  int calculate_pred_and_observed  = false; /// DONE -- now rerouted to the new way of computing results
//  int calculate_yields             = false; ///DONE /// superfluous (replaced by get_new_results)
  int get_new_results              = false; ///DONE
	bool dopoisson              = false;//should we calculate stat err with poisson? ///DONE
	bool verbose                = false;//prints out a lot more information ... ///DONE
  //--------------------------------------------
  // Systematics and limits
  int do_compute_systematics=false; /// DONE (might want to add something related to the peak?)
	bool requireZ=true; // should we require a Z for MC efficiency?
  int do_compute_efficiency=false; /// DONE (might want to add something related to the peak?)
  int do_prepare_limits_using_shapes=false; /// ALMOST DONE **************************************
  int do_compute_upper_limits_from_counting_experiment=false; /// DONE
	bool doobserved=false; // want to get the observed limit as well?
	int doquick=1; // this will cause the number of predicted/observed events to hurry up (0 = do mc and all results, 1 = all results but no mc, 2 = don't save results in library and don't compute mc stuff)
  
  //--------------------------------------------
  // More not-so-standard stuff
  int do_model_scan=false; // this can take days - use the grid version!
  int do_zjet_ttbar_shapes=false; /// DONE
  int do_test=false;/// DONE    (just any test you wish)
  int do_pick_up_events=false; ///DONE
  
  //--------------------------------------------
  // To be run manually !
  int do_response_correction=false; ///DONE 
               //use this to find out the correction factor; originally this was done automatically but now you should do this separately and update Modules/Setup.C
  
  int do_sideband_assessment=false;

  int do_save_template=false;
  int do_mc_yields=false; // compute MC yields
  int do_metjzb=false;
  
  int savepdf=true;
  int saveC=true;
  int saveRoot=true;
  int savepng=true;
  int saveeps=false;

  int dooffpeak=false;
  int doonpeak=false;

  int do_JES=false;
  
  std::string directory="";
  int option_iterator;
  int option_counter=0;
  bool moreoptions=true;

  string jzbcuts_string="";

  while(moreoptions) {
    static struct option long_options[] =
             {
               /* These options set a flag. */
               {"all",      no_argument,       &do_all, 1},
               {"peak",     no_argument,       &do_peak_finding, 1},
               {"kin",      no_argument,       &do_kinematic_variables, 1},
               {"pfkin",    no_argument,       &do_kinematic_PF_variables, 1},
               {"region",   no_argument,       &do_region_comparison, 1},
               {"lepton",   no_argument,       &do_lepton_comparison, 1},
               {"jzb",      no_argument,       &do_jzb_plots, 1},
               {"pred",     no_argument,       &do_pred, 1},
               {"ratio",    no_argument,       &do_ratio, 1},
               {"signalbg", no_argument,       &do_signal_bg_comparison_plot, 1},
               {"ttbar",    no_argument,       &do_ttbar_comparison, 1},
               {"zjets",    no_argument,       &do_zjets_comparison, 1},
               {"results",  no_argument,       &get_new_results, 1},
               {"syst",     no_argument,       &do_compute_systematics, 1},
               {"eff",      no_argument,       &do_compute_efficiency, 1},
               {"shapes",   no_argument,       &do_prepare_limits_using_shapes, 1},
               {"counting", no_argument,       &do_compute_upper_limits_from_counting_experiment, 1},
               {"response", no_argument,       &do_response_correction, 1},
               {"pickup",   no_argument,       &do_pick_up_events, 1},
               {"yields",   no_argument,       &do_mc_yields, 1},
               {"sideassess",no_argument,      &do_sideband_assessment, 1},
               {"save",     no_argument,       &do_save_template,1},
               {"png",      no_argument,       &savepng,1},
               {"eps",      no_argument,       &saveeps,1},
               {"pdf",      no_argument,       &savepdf,1},
               {"root",     no_argument,       &saveRoot,1},
               {"C",        no_argument,       &saveC,1},
               {"offpeak",  no_argument,       &dooffpeak,1},
               {"onpeak",   no_argument,       &doonpeak,1},
               {"met",      no_argument,       &do_metjzb,1},
               {"jes",      no_argument,       &do_JES,1},
               {"effcurve", no_argument,       &do_jzb_efficiency_curve,1},
               {"public",   no_argument,       &PlottingSetup::publicmode,1},
               /* The following options store values.*/
               {"jzbcuts", required_argument, 0, 'j'},
               {"dir",     required_argument, 0, 'd'},
               {0, 0, 0, 0}
             };
    int option_index = 0;
    option_iterator = getopt_long(argc, argv, "ad:j:",long_options, &option_index);
    if(option_iterator == -1) moreoptions=false;
    else {
      option_counter++;
      switch (option_iterator)
             {
             case 0:
               if (long_options[option_index].flag != 0)
                 break;
               printf ("option %s", long_options[option_index].name);
               if (optarg)
                 printf (" with arg %s", optarg);
               printf ("\n");
               break;
             case 'a':
	       do_all=true;
               break;
             case 'd':
	       directory=(std::string)optarg;
	       std::cout<<"Option directory was passed with argument " << optarg << std::endl;
               break;
             case 'j':
	       jzbcuts_string=(std::string)optarg;
	       std::cout<<"JZB cuts were manually defined:" << optarg << std::endl;
               break;
             case '?':
	       usage(option_iterator);
               break;
             default:
               usage(option_iterator);
             }
    }
  }
  
  if(directory!="") PlottingSetup::directoryname=directory;
  if(dooffpeak) PlottingSetup::RestrictToMassPeak=false;
  if(doonpeak) PlottingSetup::RestrictToMassPeak=true;
  if(option_counter==0) usage();
  
  ///----------------------------------- BELOW THIS LINE: NO MORE OPTIONS BUT ACTUAL FUNCTION CALLS! ---------------------------------------------------------
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
  setessentialcut(essential);//this sets the essential cut; this one is used in the draw command so it is AUTOMATICALLY applied everywhere. IMPORTANT: Do NOT store weights here!
  stringstream resultsummary;

  write_analysis_type(PlottingSetup::RestrictToMassPeak);
  do_png(savepng);
  do_pdf(savepdf);
  do_eps(saveeps);
  do_C(saveC);
  do_root(saveRoot);

   
  global_ratio_binning.push_back(0);
  global_ratio_binning.push_back(5);
  global_ratio_binning.push_back(10);
  global_ratio_binning.push_back(20);
  global_ratio_binning.push_back(50);
  global_ratio_binning.push_back(100);
  global_ratio_binning.push_back(200);
  global_ratio_binning.push_back(400);
  //global_ratio_binning.push_back(500);

  //these are the JZB cuts defining our search regions
  vector<float> jzb_cut; //starting where, please?
  if(jzbcuts_string=="") {
	jzb_cut.push_back(50);
	//  jzb_cut.push_back(75);
	jzb_cut.push_back(100);
	//  jzb_cut.push_back(125);
	jzb_cut.push_back(150);
	//  jzb_cut.push_back(175);
	jzb_cut.push_back(200);	
	//  jzb_cut.push_back(225);
	jzb_cut.push_back(250);
	//  jzb_cut.push_back(275);
//	jzb_cut.push_back(300);
  } else {
	manually_set_jzb_cuts(jzb_cut,jzbcuts_string);
	if(jzb_cut.size()==0) {
		write_error(__FUNCTION__,"There are no jzb cuts!");
		return -1;
	}
  }

  todo();
  //**** part 1 : peak finding
  float MCPeak=0,MCPeakError=0,DataPeak=0,DataPeakError=0,MCSigma=10,DataSigma=10;
  method=Kostasmethod;//Kostasmethod;//dogaus3sigma;// options: dogaus,doKM,dogaus2sigma,dogaus3sigma
  if(do_peak_finding||do_sideband_assessment||do_metjzb||do_zjet_ttbar_shapes||do_region_comparison||do_jzb_plots||do_lepton_comparison||do_signal_bg_comparison_plot||do_pred||do_ratio||do_mc_yields||do_save_template||do_compute_upper_limits_from_counting_experiment||do_compute_systematics||do_model_scan||do_prepare_limits_using_shapes||do_all||get_new_results||do_ttbar_comparison||do_kinematic_variables||do_kinematic_PF_variables) find_peaks(MCPeak,MCPeakError, DataPeak, DataPeakError,MCSigma,DataSigma,resultsummary);

  
  stringstream datajzb;
  if(DataPeak>0) datajzb<<"("<<jzbvariabledata<<"-"<<TMath::Abs(DataPeak)<<")";
  else datajzb<<"("<<jzbvariabledata<<"+"<<TMath::Abs(DataPeak)<<")";
  stringstream mcjzb;
  if(MCPeak>0) mcjzb<<"("<<jzbvariablemc<<"-"<<TMath::Abs(MCPeak)<<")";
  else mcjzb<<"("<<jzbvariablemc<<"+"<<TMath::Abs(MCPeak)<<")";
  
  dout << "With peak correction, we get : " << endl;
  dout << "    Data : " << datajzb.str() << endl;
  dout << "    MC : " << mcjzb.str() << endl;
  


  //**** part 2 : kinematic plots
  if(do_kinematic_variables||do_all) do_kinematic_plots(mcjzb.str(),datajzb.str());
  if(do_kinematic_PF_variables) do_kinematic_PF_plots(mcjzb.str(),datajzb.str());
  
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
  
  //**** part 7: Some systematics plots 
  if(do_signal_bg_comparison_plot||do_all) signal_bg_comparison();
  if(do_ttbar_comparison||do_all) ttbar_sidebands_comparison(mcjzb.str(),global_ratio_binning);
  if(do_zjets_comparison||do_all) zjets_prediction_comparison();

  if(do_JES||do_all) make_JES_plot();

  if (do_jzb_efficiency_curve ) plot_jzb_sel_eff(mcjzb.str(),signalsamples,jzb_cut);
  
  //**** part 8: observed and predicted!
  if(do_all||get_new_results) doquick=0;
  if(do_save_template) doquick=2;
  if(do_all||do_save_template||do_compute_upper_limits_from_counting_experiment||do_model_scan||get_new_results) get_result(mcjzb.str(),datajzb.str(),DataPeakError,MCPeakError,jzb_cut,verbose,dopoisson,doquick);
  
  vector<float>jzb_shape_limit_bins;
  jzb_shape_limit_bins.push_back(50);jzb_shape_limit_bins.push_back(75);jzb_shape_limit_bins.push_back(100);
  jzb_shape_limit_bins.push_back(150);jzb_shape_limit_bins.push_back(200);jzb_shape_limit_bins.push_back(500);
  if(do_prepare_limits_using_shapes) prepare_limits(mcjzb.str(),datajzb.str(),DataPeakError,MCPeakError,jzb_shape_limit_bins);
  
  //------------------------ end of standard functions
  
//  if(calculate_yields||do_all) calculate_all_yields(mcjzb.str(),jzb_cut); // now outdated - get_results now replaces this.
  
  
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

  if(do_save_template) save_template(mcjzb.str(),datajzb.str(),jzb_cut,MCPeakError);

  if(do_sideband_assessment) sideband_assessment(datajzb.str(),30.0,50.0);
  
  if(do_mc_yields) compute_MC_yields(mcjzb.str(),jzb_cut);
  
  if(do_metjzb) met_jzb_cut(datajzb.str(),mcjzb.str(),jzb_cut);
  
  if(do_test) test();
  
  return 0;  
}

 
