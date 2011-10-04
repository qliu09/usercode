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
#include <fstream>

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
#include <TProfile.h>
#include <TSystem.h>
#include "LimitDroplet.C"

//#include "TTbar_stuff.C"
using namespace std;

using namespace PlottingSetup;


void rediscover_the_top(string mcjzb, string datajzb) {
  dout << "Hi! today we are going to (try to) rediscover the top!" << endl;
  TCanvas *c3 = new TCanvas("c3","c3");
  c3->SetLogy(1);
  vector<float> binning;
  //binning=allsamples.get_optimal_binsize(mcjzb,cutmass&&cutOSSF&&cutnJets,20,50,800);
  /*
  binning.push_back(50);
  binning.push_back(100);
  binning.push_back(150);
  binning.push_back(200);
  binning.push_back(500);
  
  
  TH1F *dataprediction = allsamples.Draw("dataprediction",    "-"+datajzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,  luminosity);
  TH1F *puresignal     = allsamples.Draw("puresignal",    datajzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data,  luminosity);
//  TH1F *puresignal     = allsamples.Draw("puresignal",        mcjzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,mc,  luminosity,allsamples.FindSample("TTJets"));
  TH1F *observed       = allsamples.Draw("observed",          datajzb,binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,luminosity);
  /*
  ofstream myfile;
  TH1F *ratio = (TH1F*)observed->Clone();
  ratio->Divide(dataprediction);
  ratio->GetYaxis()->SetTitle("Ratio obs/pred");
  ratio->Draw();
  c3->SaveAs("testratio.png");
  myfile.open ("ShapeFit_log.txt");
  establish_upper_limits(observed,dataprediction,puresignal,"LM4",myfile);
  myfile.close();
  */
  
  
  int nbins=100;
  float low=0;
  float hi=500;
  TCanvas *c4 = new TCanvas("c4","c4",900,900);
  c4->Divide(2,2);
  c4->cd(1);
  c4->cd(1)->SetLogy(1);
  TH1F *datapredictiont = allsamples.Draw("datapredictiont",    "-"+datajzb, nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,  luminosity);
  TH1F *datapredictiono = allsamples.Draw("datapredictiono",    "-"+datajzb, nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data,  luminosity);
  datapredictiont->Add(datapredictiono,-1);
  dout << "Second way of doing this !!!! Analytical shape to the left :-D" << endl;
  vector<TF1*> functions = do_cb_fit_to_plot(datapredictiont,10);
  datapredictiont->SetMarkerColor(kRed);
  datapredictiont->SetLineColor(kRed);
  datapredictiont->Draw();
  functions[1]->Draw("same");
  TText *title1 = write_title("Top Background Prediction (JZB<0, with osof subtr)");
  title1->Draw();
  
  c4->cd(2);
  c4->cd(2)->SetLogy(1);
  TH1F *observedt = allsamples.Draw("observedt", datajzb,  nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,  luminosity);
  observedt->Draw();
  datapredictiont->Draw("histo,same");
  functions[1]->Draw("same");
  TText *title2 = write_title("Observed and predicted background");
  title2->Draw();
  
  c4->cd(3);
  c4->cd(3)->SetLogy(1);
//  TH1F *ratio = (TH1F*)observedt->Clone();

  TH1F *analytical_background_prediction= new TH1F("analytical_background_prediction","",nbins,low,hi);
  for(int i=0;i<=nbins;i++) {
    analytical_background_prediction->SetBinContent(i+1,functions[1]->Eval(((hi-low)/((float)nbins))*(i+0.5)));
    analytical_background_prediction->SetBinError(i+1,TMath::Sqrt(functions[1]->Eval(((hi-low)/((float)nbins))*(i+0.5))));
  }
  analytical_background_prediction->GetYaxis()->SetTitle("JZB [GeV]");
  analytical_background_prediction->GetYaxis()->CenterTitle();
  TH1F *analyticaldrawonly = (TH1F*)analytical_background_prediction->Clone();
  analytical_background_prediction->SetFillColor(TColor::GetColor("#3399FF"));
  analytical_background_prediction->SetMarkerSize(0);
  analytical_background_prediction->Draw("e5");
  analyticaldrawonly->Draw("histo,same");
  functions[1]->Draw("same");
  TText *title3 = write_title("Analytical bg pred histo");
  title3->Draw();
  
  c4->cd(4);
//  c4->cd(4)->SetLogy(1);
  vector<float> ratio_binning;
  ratio_binning.push_back(0);
  ratio_binning.push_back(5);
  ratio_binning.push_back(10);
  ratio_binning.push_back(20);
  ratio_binning.push_back(50);
//  ratio_binning.push_back(60);
/*
  ratio_binning.push_back(51);
  ratio_binning.push_back(52);
  ratio_binning.push_back(53);
  ratio_binning.push_back(54);
  ratio_binning.push_back(55);
  ratio_binning.push_back(56);
  ratio_binning.push_back(57);
  ratio_binning.push_back(58);
  ratio_binning.push_back(59);
  ratio_binning.push_back(60);
//  ratio_binning.push_back(70);*/
//  ratio_binning.push_back(80);
//  ratio_binning.push_back(90);
ratio_binning.push_back(80);
//  ratio_binning.push_back(110);
  ratio_binning.push_back(500);
  
  TH1F *observedtb = allsamples.Draw("observedtb", datajzb,  ratio_binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,  luminosity);
  TH1F *datapredictiontb = allsamples.Draw("datapredictiontb",    "-"+datajzb, ratio_binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,  luminosity);
  TH1F *datapredictiontbo = allsamples.Draw("datapredictiontbo",    "-"+datajzb, ratio_binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data,  luminosity);
  datapredictiontb->Add(datapredictiontbo,-1);
  TH1F *analytical_background_predictionb = allsamples.Draw("analytical_background_predictionb",datajzb, ratio_binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets&&"mll<2",data,  luminosity);
  for(int i=0;i<=ratio_binning.size();i++) {
    analytical_background_predictionb->SetBinContent(i+1,functions[1]->Eval(analytical_background_predictionb->GetBinCenter(i)));
    analytical_background_predictionb->SetBinError(i+1,TMath::Sqrt(functions[1]->Eval(analytical_background_predictionb->GetBinCenter(i))));
  }
  
  TH1F *ratio = (TH1F*) observedtb->Clone();
  ratio->Divide(datapredictiontb);
  
  for (int i=0;i<=ratio_binning.size();i++) {
    dout << observedtb->GetBinLowEdge(i+1) << ";"<<observedtb->GetBinContent(i+1) << ";" << datapredictiontb->GetBinContent(i+1) << " --> " << ratio->GetBinContent(i+1) << "+/-" << ratio->GetBinError(i+1) << endl;
  }
  
//  ratio->Divide(datapredictiontb);
//  ratio->Divide(analytical_background_predictionb);
//  TGraphAsymmErrors *JZBratio= histRatio(observedtb,analytical_background_predictionb,data,ratio_binning);
//  ratio->Divide(analytical_background_prediction);
//  ratio->Divide(datapredictiont);
//  ratio->GetYaxis()->SetTitle("obs/pred");
//  JZBratio->Draw("AP");
  ratio->GetYaxis()->SetRangeUser(0,10);
  ratio->Draw();
//analytical_background_predictionb->Draw();
//  JZBratio->SetTitle("");
  TText *title4 = write_title("Ratio of observed to predicted");
  title4->Draw();
  
//  CompleteSave(c4,"test/ttbar_discovery_dataprediction___analytical_function");
  CompleteSave(c4,"test/ttbar_discovery_dataprediction__analytical__new_binning_one_huge_bin_from_80");

  
  
  
  
}

vector<float> compute_one_upper_limit(float mceff,float mcefferr, int ibin, string mcjzb, string plotfilename, bool doexpected) {
  float sigma95=-9.9,sigma95A=-9.9;
/*
USAGE OF ROOSTATS_CL95
" Double_t             limit = roostats_cl95(ilum, slum, eff, seff, bck, sbck, n, gauss = false, nuisanceModel, method, plotFileName, seed); \n"
" LimitResult expected_limit = roostats_clm(ilum, slum, eff, seff, bck, sbck, ntoys, nuisanceModel, method, seed); \n"
" Double_t     average_limit = roostats_cla(ilum, slum, eff, seff, bck, sbck, nuisanceModel, method, seed); \n"
"                                                                     \n"
" 
" Double_t obs_limit = limit.GetObservedLimit();                      \n"
" Double_t exp_limit = limit.GetExpectedLimit();                      \n"
" Double_t exp_up    = limit.GetOneSigmaHighRange();                  \n"
" Double_t exp_down  = limit.GetOneSigmaLowRange();                   \n"
" Double_t exp_2up   = limit.GetTwoSigmaHighRange();                  \n"
" Double_t exp_2down = limit.GetTwoSigmaLowRange();                   \n"
*/
  if(mceff<=0) {
    write_warning(__FUNCTION__,"Cannot compute upper limit in this configuration as the efficiency is negative:");
    dout << "mc efficiency=" << mceff << " +/- " << mcefferr;
    vector<float> sigmas;
    sigmas.push_back(-1);
    sigmas.push_back(-1);
    return sigmas;
  } else {
    int nlimittoysused=1;
    
    ///------------------------------------------ < NEW > ----------------------------------------------------------
    
    int secondssince1970=time(NULL);
    stringstream repname;
    repname << PlottingSetup::cbafbasedir << "/exchange/report_" << secondssince1970 << "_"<<plotfilename<< "__"<< ".txt";
    
      /* - report filename [1]
         - luminosity [2]
         - lumi uncert [3]
         - MC efficiency [4]
         - MC efficiency error [5]
         - Npred [6]
         - Nprederr [7]
         - Nobs [8]
         - JZB cut [9]
         - plot name  [10]*/

  dout << "Calling limit capsule instead of calling : CL95(" << luminosity << "," <<  lumiuncert*luminosity << "," << mceff << "," << mcefferr << "," << Npred[ibin] << "," << Nprederr[ibin] << "," << Nobs[ibin] << "," << false << "," << nuisancemodel<< ") " << endl;
    
    stringstream command;
    command << PlottingSetup::cbafbasedir << "/DistributedModelCalculations/Limits/NewLimitCapsule.exec " << repname.str() << " " << luminosity << " " << luminosity*lumiuncert << " " << mceff << " " << mcefferr << " " << Npred[ibin] << " " << Nprederr[ibin] << " " << Nobs[ibin] << " " << -1 << " " << PlottingSetup::basedirectory << "/" << plotfilename << " " << doexpected;
    dout << command.str() << endl;
    
    int retval = 256;
    int attempts=0;
    while(!(retval==0||attempts>=5)) {//try up to 5 times
        attempts++;
        dout << "Starting limit calculation (LimitCapsule) now : Attempt " << attempts << endl;
        retval=gSystem->Exec(command.str().c_str());
    }
    
    LimitDroplet limres;
    limres.readDroplet(repname.str());
    dout << limres << endl;
    remove(repname.str().c_str());
    sigma95=limres.observed;

    
    ///------------------------------------------ < /NEW > ----------------------------------------------------------
  vector<float> sigmas;
  sigmas.push_back(sigma95);
  if(doexpected) {
    sigmas.push_back(limres.expected);
    sigmas.push_back(limres.upper68);
    sigmas.push_back(limres.lower68);
    sigmas.push_back(limres.upper95);
    sigmas.push_back(limres.lower95);
  }
  
  return sigmas;
  

  }//end of mc efficiency is ok
}

void compute_upper_limits_from_counting_experiment(vector<vector<float> > uncertainties,vector<float> jzbcuts, string mcjzb, bool doexpected) {
  dout << "Doing counting experiment ... " << endl;
  vector<vector<string> > limits;
  vector<vector<float> > vlimits;
  
  
  for(int isample=0;isample<signalsamples.collection.size();isample++) {
    vector<string> rows;
    vector<float> vrows;
    dout << "Considering sample " << signalsamples.collection[isample].samplename << endl;
    rows.push_back(signalsamples.collection[isample].samplename);
    for(int ibin=0;ibin<jzbcuts.size();ibin++) {
      dout << "_________________________________________________________________________________" << endl;
      float JZBcutat=uncertainties[isample*jzbcuts.size()+ibin][0];
      float mceff=uncertainties[isample*jzbcuts.size()+ibin][1];
      float staterr=uncertainties[isample*jzbcuts.size()+ibin][2];
      float systerr=uncertainties[isample*jzbcuts.size()+ibin][3];
      float toterr =uncertainties[isample*jzbcuts.size()+ibin][4];
      float observed,observederr,null,result;
      
//      fill_result_histos(observed,observederr, null,null,null,null,null,null,null,mcjzb,JZBcutat,14000,(int)5,result,(signalsamples.FindSample(signalsamples.collection[isample].filename)),signalsamples);
//      observed-=result;//this is the actual excess we see!
//      float expected=observed/luminosity;
      string plotfilename=(string)(TString(signalsamples.collection[isample].samplename)+TString("___JZB_geq_")+TString(any2string(JZBcutat))+TString(".png"));
      dout << "Sample: " << signalsamples.collection[isample].samplename << ", JZB>"<<JZBcutat<< " : " << mceff << " +/- " << staterr << " (stat) +/- " << systerr << " (syst) --> toterr = " << toterr << endl;
      vector<float> sigmas = compute_one_upper_limit(mceff,toterr,ibin,mcjzb,plotfilename,doexpected);
      
      if(doexpected) {
//	rows.push_back(any2string(sigmas[0])+";"+any2string(sigmas[1])+";"+"("+any2string(expected)+")");
	rows.push_back(any2string(sigmas[0])+";"+any2string(sigmas[1])+";"+"("+any2string(signalsamples.collection[isample].xs)+")");
	vrows.push_back(sigmas[0]);
	vrows.push_back(sigmas[1]);
//	vrows.push_back(expected);
	vrows.push_back(signalsamples.collection[isample].xs);
      }
      else {
//	rows.push_back(any2string(sigmas[0])+"("+any2string(expected)+")");
	rows.push_back(any2string(sigmas[0]));
	vrows.push_back(sigmas[0]);
	vrows.push_back(signalsamples.collection[isample].xs);
//	vrows.push_back(expected);
      }
    }//end of bin loop
    limits.push_back(rows);
    vlimits.push_back(vrows);
  }//end of sample loop
  dout << endl << endl << endl << "_________________________________________________________________________________________________" << endl << endl;
  dout << endl << endl << "PAS table 3:   (notation: limit [95%CL])" << endl << endl;
  dout << "\t";
  for (int irow=0;irow<jzbcuts.size();irow++) {
    dout << jzbcuts[irow] << "\t";
  }
  dout << endl;
  for(int irow=0;irow<limits.size();irow++) {
    for(int ientry=0;ientry<limits[irow].size();ientry++) {
      if (limits[irow][ientry]>0) dout << limits[irow][ientry] << "\t";
      else dout << " (N/A) \t";
    }
    dout << endl;
  }
  
  if(!doexpected) {
    dout << endl << endl << "LIMITS: (Tex)" << endl;
    tout << "\\begin{table}[hbtp]" << endl;
    tout << "\\renewcommand{\\arraystretch}{1.3}" << endl;
    tout << "\\begin{center}" << endl;
    tout << "\\caption{Observed upper limits on the cross section of different LM benchmark points " << (ConsiderSignalContaminationForLimits?"  (accounting for signal contamination)":"  (not accounting for signal contamination)") << "}\\label{tab:lmresults}" << endl;
    tout << "" << endl;
    tout << "\\begin{tabular}{ | l | ";
    for (int irow=0;irow<jzbcuts.size();irow++) tout << " l |";
    tout << "} " << endl << " \\hline " << endl << "& \t ";
    for (int irow=0;irow<jzbcuts.size();irow++) {
      tout << "JZB $>$ " << jzbcuts[irow] << " GeV & \t ";
    }
    tout << " \\\\ \\hline " << endl;
    for(int irow=0;irow<limits.size();irow++) {
      tout << limits[irow][0] << " \t";
      for(int ientry=0;ientry<jzbcuts.size();ientry++) {
	if(vlimits[irow][2*ientry]>0) tout << " & " << Round(vlimits[irow][2*ientry],2) << " \t (" << Round(vlimits[irow][2*ientry] / vlimits[irow][2*ientry+1],3)<< "x \\sigma ) \t";
	else tout << " & ( N / A ) \t";
//	dout << Round(vlimits[irow][2*ientry],3) << " / " << Round(vlimits[irow][2*ientry+1],3)<< "\t";
      }
      tout << " \\\\ \\hline " << endl;
    }
      tout << "\\end{tabular}" << endl;
      tout << "      \\end{tabular}"<< endl;
      tout << "\\end{center}"<< endl;
      tout << "\\end{table} "<< endl;

  }//do observed
  
  dout << endl << endl << "Final selection efficiencies with total statistical and systematic errors, and corresponding observed and expected upper limits (UL) on ($\\sigma\\times$  BR $\\times$ acceptance) for the LM4 and LM8 scenarios, in the different regions. The last column contains the predicted ($\\sigma \\times $BR$\\times$ acceptance) at NLO obtained from Monte Carlo simulation." << endl;
  dout << "Scenario \t Efficiency [%] \t Upper limits [pb] \t \\sigma [pb]" << endl;
  for(int icut=0;icut<jzbcuts.size();icut++) {
    dout << "Region with JZB>" << jzbcuts[icut] << (ConsiderSignalContaminationForLimits?"  (accounting for signal contamination)":"  (not accounting for signal contamination)") << endl;
    for(int isample=0;isample<signalsamples.collection.size();isample++) {
      dout << limits[isample][0] << "\t" << Round(100*uncertainties[isample*jzbcuts.size()+icut][1],3) << "+/-" << Round(100*uncertainties[isample*jzbcuts.size()+icut][2],3) << " (stat) +/- " << Round(100*uncertainties[isample*jzbcuts.size()+icut][3],3) << " (syst) \t" << Round((vlimits[isample][2*icut]),3) << "\t" << Round(vlimits[isample][2*icut+1],3) << endl;
    }
    dout << endl;
  }
}


 
/********************************************************************** new : Limits using SHAPES ***********************************


   SSSSSSSSSSSSSSS hhhhhhh                                                                                       
 SS:::::::::::::::Sh:::::h                                                                                       
S:::::SSSSSS::::::Sh:::::h                                                                                       
S:::::S     SSSSSSSh:::::h                                                                                       
S:::::S             h::::h hhhhh         aaaaaaaaaaaaa  ppppp   ppppppppp       eeeeeeeeeeee        ssssssssss   
S:::::S             h::::hh:::::hhh      a::::::::::::a p::::ppp:::::::::p    ee::::::::::::ee    ss::::::::::s  
 S::::SSSS          h::::::::::::::hh    aaaaaaaaa:::::ap:::::::::::::::::p  e::::::eeeee:::::eess:::::::::::::s 
  SS::::::SSSSS     h:::::::hhh::::::h            a::::app::::::ppppp::::::pe::::::e     e:::::es::::::ssss:::::s
    SSS::::::::SS   h::::::h   h::::::h    aaaaaaa:::::a p:::::p     p:::::pe:::::::eeeee::::::e s:::::s  ssssss 
       SSSSSS::::S  h:::::h     h:::::h  aa::::::::::::a p:::::p     p:::::pe:::::::::::::::::e    s::::::s      
            S:::::S h:::::h     h:::::h a::::aaaa::::::a p:::::p     p:::::pe::::::eeeeeeeeeee        s::::::s   
            S:::::S h:::::h     h:::::ha::::a    a:::::a p:::::p    p::::::pe:::::::e           ssssss   s:::::s 
SSSSSSS     S:::::S h:::::h     h:::::ha::::a    a:::::a p:::::ppppp:::::::pe::::::::e          s:::::ssss::::::s
S::::::SSSSSS:::::S h:::::h     h:::::ha:::::aaaa::::::a p::::::::::::::::p  e::::::::eeeeeeee  s::::::::::::::s 
S:::::::::::::::SS  h:::::h     h:::::h a::::::::::aa:::ap::::::::::::::pp    ee:::::::::::::e   s:::::::::::ss  
 SSSSSSSSSSSSSSS    hhhhhhh     hhhhhhh  aaaaaaaaaa  aaaap::::::pppppppp        eeeeeeeeeeeeee    sssssssssss    
                                                         p:::::p                                                 
                                                         p:::::p                                                 
                                                        p:::::::p                                                
                                                        p:::::::p                                                
                                                        p:::::::p                                                
                                                        ppppppppp                                                
                                                                                                                 

*********************************************************************** new : Limits using SHAPES ***********************************/


void limit_shapes_for_systematic_effect(TFile *limfile, string identifier, string mcjzb, string datajzb, int JES,vector<float> binning, TCanvas *limcan) {
  dout << "Creatig shape templates ... ";
  if(identifier!="") dout << "for systematic called "<<identifier;
  dout << endl;
  int dataormc=mcwithsignal;//this is only for tests - for real life you want dataormc=data !!!
  if(dataormc!=data) write_warning(__FUNCTION__,"WATCH OUT! Not using data for limits!!!! this is ok for tests, but not ok for anything official!");
  
  TCut limitnJetcut;
  if(JES==noJES) limitnJetcut=cutnJets;
  else {
    if(JES==JESdown) limitnJetcut=cutnJetsJESdown;
    if(JES==JESup) limitnJetcut=cutnJetsJESup;
  }
  TH1F *ZOSSFP = allsamples.Draw("ZOSSFP",datajzb,binning, "JZB4limits", "events",cutmass&&cutOSSF&&limitnJetcut&&basiccut,dataormc,luminosity);
  TH1F *ZOSOFP = allsamples.Draw("ZOSOFP",datajzb,binning, "JZB4limits", "events",cutmass&&cutOSOF&&limitnJetcut&&basiccut,dataormc,luminosity);
  TH1F *ZOSSFN = allsamples.Draw("ZOSSFN","-"+datajzb,binning, "JZB4limits", "events",cutmass&&cutOSSF&&limitnJetcut&&basiccut,dataormc,luminosity);
  TH1F *ZOSOFN = allsamples.Draw("ZOSOFN","-"+datajzb,binning, "JZB4limits", "events",cutmass&&cutOSOF&&limitnJetcut&&basiccut,dataormc,luminosity);
  
  TH1F *SBOSSFP;
  TH1F *SBOSOFP;
  TH1F *SBOSSFN;
  TH1F *SBOSOFN;
  
  TH1F *LZOSSFP = allsamples.Draw("LZOSSFP",mcjzb,binning, "JZB4limits", "events",cutmass&&cutOSSF&&limitnJetcut&&basiccut,mc,luminosity,allsamples.FindSample("LM4"));
  TH1F *LZOSOFP = allsamples.Draw("LZOSOFP",mcjzb,binning, "JZB4limits", "events",cutmass&&cutOSOF&&limitnJetcut&&basiccut,mc,luminosity,allsamples.FindSample("LM4"));
  TH1F *LZOSSFN = allsamples.Draw("LZOSSFN","-"+mcjzb,binning, "JZB4limits", "events",cutmass&&cutOSSF&&limitnJetcut&&basiccut,mc,luminosity,allsamples.FindSample("LM4"));
  TH1F *LZOSOFN = allsamples.Draw("LZOSOFN","-"+mcjzb,binning, "JZB4limits", "events",cutmass&&cutOSOF&&limitnJetcut&&basiccut,mc,luminosity,allsamples.FindSample("LM4"));
  
  TH1F *LSBOSSFP;
  TH1F *LSBOSOFP;
  TH1F *LSBOSSFN;
  TH1F *LSBOSOFN;
  
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
      SBOSSFP = allsamples.Draw("SBOSSFP",datajzb,binning, "JZB4limits", "events",cutOSSF&&limitnJetcut&&basiccut&&sidebandcut,dataormc,luminosity);
      SBOSOFP = allsamples.Draw("SBOSOFP",datajzb,binning, "JZB4limits", "events",cutOSOF&&limitnJetcut&&basiccut&&sidebandcut,dataormc,luminosity);
      SBOSSFN = allsamples.Draw("SBOSSFN","-"+datajzb,binning, "JZB4limits", "events",cutOSSF&&limitnJetcut&&basiccut&&sidebandcut,dataormc,luminosity);
      SBOSOFN = allsamples.Draw("SBOSOFN","-"+datajzb,binning, "JZB4limits", "events",cutOSOF&&limitnJetcut&&basiccut&&sidebandcut,dataormc,luminosity);
      
      LSBOSSFP = allsamples.Draw("LSBOSSFP",mcjzb,binning, "JZB4limits", "events",cutOSSF&&limitnJetcut&&basiccut&&sidebandcut,mc,luminosity,allsamples.FindSample("LM4"));
      LSBOSOFP = allsamples.Draw("LSBOSOFP",mcjzb,binning, "JZB4limits", "events",cutOSOF&&limitnJetcut&&basiccut&&sidebandcut,mc,luminosity,allsamples.FindSample("LM4"));
      LSBOSSFN = allsamples.Draw("LSBOSSFN","-"+mcjzb,binning, "JZB4limits", "events",cutOSSF&&limitnJetcut&&basiccut&&sidebandcut,mc,luminosity,allsamples.FindSample("LM4"));
      LSBOSOFN = allsamples.Draw("LSBOSOFN","-"+mcjzb,binning, "JZB4limits", "events",cutOSOF&&limitnJetcut&&basiccut&&sidebandcut,mc,luminosity,allsamples.FindSample("LM4"));
  }
  
  string obsname="data_obs";
  string predname="background";
  string signalname="signal";
  if(identifier!="") {
    obsname=("data_"+identifier);
    predname=("background_"+identifier);
    signalname="signal_"+identifier;
  }
  
  TH1F *obs = (TH1F*)ZOSSFP->Clone();
  obs->SetName(obsname.c_str());
  obs->Write();
  TH1F *pred = (TH1F*)ZOSSFN->Clone();
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    pred->Add(ZOSOFP,1.0/3);
    pred->Add(ZOSOFN,-1.0/3);
    pred->Add(SBOSSFP,1.0/3);
    pred->Add(SBOSSFN,-1.0/3);
    pred->Add(SBOSOFP,1.0/3);
    pred->Add(SBOSOFN,-1.0/3);
  } else {
    pred->Add(ZOSOFP,1.0);
    pred->Add(ZOSOFN,-1.0);
  }
    
  pred->SetName(predname.c_str());
  pred->Write();
  
//  TH1F *Lobs = (TH1F*)LZOSSFP->Clone();
//  TH1F *Lpred = (TH1F*)LZOSSFN->Clone();
  
  TH1F *Lobs = new TH1F("Lobs","Lobs",binning.size()-1,&binning[0]);
  TH1F *Lpred = new TH1F("Lpred","Lpred",binning.size()-1,&binning[0]);
  Lobs->Add(LZOSSFP);
  Lpred->Add(LZOSSFN);
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    Lpred->Add(LZOSOFP,1.0/3);
    Lpred->Add(LZOSOFN,-1.0/3);
    Lpred->Add(LSBOSSFP,1.0/3);
    Lpred->Add(LSBOSSFN,-1.0/3);
    Lpred->Add(LSBOSOFP,1.0/3);
    Lpred->Add(LSBOSOFN,-1.0/3);
  } else {
    Lpred->Add(LZOSOFP,1.0);
    Lpred->Add(LZOSOFN,-1.0);
  }

  TH1F *signal = (TH1F*)Lobs->Clone();
  signal->Add(Lpred,-1);
  signal->SetName(signalname.c_str());
  signal->Write();
  
  delete Lobs;
  delete Lpred;
  
  delete ZOSSFP;
  delete ZOSOFP;
  delete ZOSSFN;
  delete ZOSOFN;
  
  delete SBOSSFP;
  delete SBOSOFP;
  delete SBOSSFN;
  delete SBOSOFN;
  
  delete LZOSSFP;
  delete LZOSOFP;
  delete LZOSSFN;
  delete LZOSOFN;
  
  delete LSBOSSFP;
  delete LSBOSOFP;
  delete LSBOSSFN;
  delete LSBOSOFN;

}

void prepare_datacard(TFile *f) {
 TH1F *dataob = (TH1F*)f->Get("data_obs");
 TH1F *signal = (TH1F*)f->Get("signal");
 TH1F *background = (TH1F*)f->Get("background");
 
 ofstream datacard;
 ensure_directory_exists(get_directory()+"/limits");
 datacard.open ((get_directory()+"/limits/susydatacard.txt").c_str());
 datacard << "Writing this to a file.\n";
 datacard << "imax 1\n";
 datacard << "jmax 1\n";
 datacard << "kmax *\n";
 datacard << "---------------\n";
 datacard << "shapes * * limitfile.root $PROCESS $PROCESS_$SYSTEMATIC\n";
 datacard << "---------------\n";
 datacard << "bin 1\n";
 datacard << "observation "<<dataob->Integral()<<"\n";
 datacard << "------------------------------\n";
 datacard << "bin             1          1\n";
 datacard << "process         signal     background\n";
 datacard << "process         0          1\n";
 datacard << "rate            "<<signal->Integral()<<"         "<<background->Integral()<<"\n";
 datacard << "--------------------------------\n";
 datacard << "lumi     lnN    1.10       1.0\n";
 datacard << "bgnorm   lnN    1.00       1.4  uncertainty on our prediction (40%)\n";
 datacard << "JES    shape    1          1    uncertainty on background shape and normalization\n";
 datacard << "peak   shape    1          1    uncertainty on signal resolution. Assume the histogram is a 2 sigma shift, \n";
 datacard << "#                                so divide the unit gaussian by 2 before doing the interpolation\n";
 datacard.close();
}


void prepare_limits(string mcjzb, string datajzb, float jzbpeakerrordata, float jzbpeakerrormc, vector<float> jzbbins) {
  ensure_directory_exists(get_directory()+"/limits");
  TFile *limfile = new TFile((get_directory()+"/limits/limitfile.root").c_str(),"RECREATE");
  TCanvas *limcan = new TCanvas("limcan","Canvas for calculating limits");
  limit_shapes_for_systematic_effect(limfile,"",mcjzb,datajzb,noJES,jzbbins,limcan);
  limit_shapes_for_systematic_effect(limfile,"peakUp",newjzbexpression(mcjzb,jzbpeakerrormc),newjzbexpression(datajzb,jzbpeakerrordata),noJES,jzbbins,limcan);
  limit_shapes_for_systematic_effect(limfile,"peakDown",newjzbexpression(mcjzb,-jzbpeakerrormc),newjzbexpression(datajzb,-jzbpeakerrordata),noJES,jzbbins,limcan);
  limit_shapes_for_systematic_effect(limfile,"JESUp",mcjzb,datajzb,JESup,jzbbins,limcan);
  limit_shapes_for_systematic_effect(limfile,"JESDown",mcjzb,datajzb,JESdown,jzbbins,limcan);
  
  prepare_datacard(limfile);
  limfile->Close();
  write_info("prepare_limits","limitfile.root and datacard.txt have been generated. You can now use them to calculate limits!");
  
}
