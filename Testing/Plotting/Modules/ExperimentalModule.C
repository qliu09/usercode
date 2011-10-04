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
#ifndef GeneralToolBoxLoaded
#include "GeneralToolBox.C"
#endif

using namespace std;

using namespace PlottingSetup;

//--------------------------------------------- below: ttbar

void ttbar_limit_shapes_for_systematic_effect(TFile *limfile, string identifier, string mcjzb, string datajzb, int JES,vector<float> binning, TCanvas *limcan) {
  dout << "Creatig shape templates ... ";
  if(identifier!="") dout << "for systematic called "<<identifier;
  dout << endl;
  int dataormc=mc;//this is only for tests - for real life you want dataormc=data !!!
  if(dataormc!=data) write_warning("limit_shapes_for_systematic_effect","WATCH OUT! Not using data for limits!!!! this is ok for tests, but not ok for anything official!");
  
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

  TH1F *LZOSSFP = allsamples.Draw("LZOSSFP",mcjzb,binning, "JZB4limits", "events",cutmass&&cutOSSF&&limitnJetcut&&basiccut,mc,luminosity,allsamples.FindSample("TTJets"));
  TH1F *LZOSOFP = allsamples.Draw("LZOSOFP",mcjzb,binning, "JZB4limits", "events",cutmass&&cutOSOF&&limitnJetcut&&basiccut,mc,luminosity,allsamples.FindSample("TTJets"));
  TH1F *LZOSSFN = allsamples.Draw("LZOSSFN","-"+mcjzb,binning, "JZB4limits", "events",cutmass&&cutOSSF&&limitnJetcut&&basiccut,mc,luminosity,allsamples.FindSample("TTJets"));
  TH1F *LZOSOFN = allsamples.Draw("LZOSOFN","-"+mcjzb,binning, "JZB4limits", "events",cutmass&&cutOSOF&&limitnJetcut&&basiccut,mc,luminosity,allsamples.FindSample("TTJets"));
  
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
  pred->Add(ZOSOFN,-1.0);
  pred->SetName(predname.c_str());
  pred->Write();
  
  TH1F *Lobs = new TH1F("Lobs","Lobs",binning.size()-1,&binning[0]);
  TH1F *Lpred = new TH1F("Lpred","Lpred",binning.size()-1,&binning[0]);
  Lobs->Add(LZOSSFP);
  Lpred->Add(LZOSSFN);
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
  
  delete LZOSSFP;
  delete LZOSOFP;
  delete LZOSSFN;
  delete LZOSOFN;
  
}

void prepare_ttbar_datacard(TFile *f) {
 TH1F *dataob = (TH1F*)f->Get("data_obs");
 TH1F *signal = (TH1F*)f->Get("signal");
 TH1F *background = (TH1F*)f->Get("background");
 
 ofstream datacard;
 ensure_directory_exists(get_directory()+"/limits");
 datacard.open ((get_directory()+"/limits/ttbardatacard.txt").c_str());
 datacard << "Writing this to a file.\n";
 datacard << "imax 1\n";
 datacard << "jmax 1\n";
 datacard << "kmax *\n";
 datacard << "---------------\n";
 datacard << "shapes * * ttbarlimitfile.root $PROCESS $PROCESS_$SYSTEMATIC\n";
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

void prepare_ttbar_limits(string mcjzb, string datajzb, float jzbpeakerrordata, float jzbpeakerrormc, vector<float> jzbbins) {
  ensure_directory_exists(get_directory()+"/limits");
  TFile *limfile = new TFile((get_directory()+"/limits/ttbarlimitfile.root").c_str(),"RECREATE");
  TCanvas *limcan = new TCanvas("limcan","Canvas for calculating limits");
  ttbar_limit_shapes_for_systematic_effect(limfile,"",mcjzb,datajzb,noJES,jzbbins,limcan);
  ttbar_limit_shapes_for_systematic_effect(limfile,"peakUp",newjzbexpression(mcjzb,jzbpeakerrormc),newjzbexpression(datajzb,jzbpeakerrordata),noJES,jzbbins,limcan);
  ttbar_limit_shapes_for_systematic_effect(limfile,"peakDown",newjzbexpression(mcjzb,-jzbpeakerrormc),newjzbexpression(datajzb,-jzbpeakerrordata),noJES,jzbbins,limcan);
  ttbar_limit_shapes_for_systematic_effect(limfile,"JESUp",mcjzb,datajzb,JESup,jzbbins,limcan);
  ttbar_limit_shapes_for_systematic_effect(limfile,"JESDown",mcjzb,datajzb,JESdown,jzbbins,limcan);
  
  prepare_ttbar_datacard(limfile);
  
  write_info("prepare_limits","limitfile.root and datacard.txt have been generated. You can now use them to calculate limits!");
  limfile->Close();
  
}

void Poisson_ratio_plot(int is_data,vector<float> binning, string jzb, TCanvas *can, float high=-9999, float jzbpearerrorMC=-999, float jzbpeakerrorData=-999) {
  can->SetLogy(0);
  cout << "About to prepare ratio plots with Poisson errors and obs/pred plots, in the new style!" << endl;
  TH1F *pred = new TH1F("pred","",binning.size()-1,&binning[0]);
  TH1F *obs = new TH1F("obs","",binning.size()-1,&binning[0]);
  for(int ibin=0;ibin<binning.size()-1;ibin++) {
//    cout << "________________________________________________________________________" << endl;
    cout << "Ratio plot: Working on bin " << ibin+1 << " of " << binning.size()-1 << " for step " << is_data+1 << " of 3" << endl;
    float binerr=0;
    vector<float> contentanderror=get_result_between_two_fixed_jzb_values(binning[ibin],binning[ibin+1], jzb,jzb, is_data,jzbpearerrorMC, jzbpeakerrorData, can,false, false,false);
    //[0] Bpred [1] Bpred uncert [2] Observed [3] Observed error
//    cout << "Bpred : " << contentanderror[0] << " +/- " << contentanderror[1] << endl;
//    cout << "Obs   : " << contentanderror[2] << " +/- " << contentanderror[3] << endl;
    pred->SetBinContent(ibin+1,contentanderror[0]);
    pred->SetBinError(ibin+1,contentanderror[1]);
    obs->SetBinContent(ibin+1,contentanderror[2]);
    obs->SetBinError(ibin+1,contentanderror[3]);
  }
  TH1F *JZBratioforfitting=(TH1F*)obs->Clone("JZBratioforfitting");
  JZBratioforfitting->Divide(pred);
  TGraphAsymmErrors *JZBratio = histRatio(obs,pred,is_data,binning,true);//the last argument basically forces the function to give us the real error, instead of the stat one
  
  JZBratio->SetTitle("");
  
  TF1 *pol0 = new TF1("pol0","[0]",0,1000);
  TF1 *pol0d = new TF1("pol0","[0]",0,1000);
  JZBratioforfitting->Fit(pol0,"Q0R","",0,30);
  pol0d->SetParameter(0,pol0->GetParameter(0));
  
  can->cd();
  JZBratio->GetYaxis()->SetTitle("Observed/Predicted");
  JZBratio->GetXaxis()->SetTitle("JZB [GeV]");
  if ( high>0 ) JZBratio->GetXaxis()->SetRangeUser(0.0,high);
  JZBratio->GetYaxis()->SetNdivisions(519);
  JZBratio->GetYaxis()->SetRangeUser(0.0,4.0);
  JZBratio->GetYaxis()->CenterTitle();
  JZBratio->GetXaxis()->CenterTitle();
  JZBratio->SetMarkerSize(DataMarkerSize);
  JZBratio->Draw("AP");
  /////----------------------------
  TPaveText *writeresult = new TPaveText(0.15,0.78,0.49,0.91,"blNDC");
  writeresult->SetFillStyle(4000);
  writeresult->SetFillColor(kWhite);
  writeresult->SetTextFont(42);
  writeresult->SetTextSize(0.03);
  writeresult->SetTextAlign(12);
  ostringstream jzb_agreement_data_text;
  jzb_agreement_data_text<< setprecision(2) << "mean =" << pol0->GetParameter(0) << " #pm " << setprecision(1) <<  pol0->GetParError(0);
  if(is_data==1) fitresultconstdata=pol0->GetParameter(0);// data
  if(is_data==0) fitresultconstmc=pol0->GetParameter(0); // monte carlo, no signal
    
  writeresult->AddText(jzb_agreement_data_text.str().c_str());
//  writeresult->Draw("same");
//  pol0d->Draw("same");
  TF1 *topline =  new TF1("","1.5",0,1000);
  TF1 *bottomline =  new TF1("","0.5",0,1000);
  topline->SetLineColor(kBlue);
  topline->SetLineStyle(2);
  bottomline->SetLineColor(kBlue);
  bottomline->SetLineStyle(2);
//  topline->Draw("same");
//  bottomline->Draw("same");
  TF1 *oneline = new TF1("","1.0",0,1000);
  oneline->SetLineColor(kBlue);
  oneline->SetLineStyle(1); 
  oneline->Draw("same"); 
  if(is_data==1) DrawPrelim();
  else DrawMCPrelim();
  TLegend *leg = new TLegend(0.5,0.55,0.94,0.89);
  leg->SetTextFont(42);
  leg->SetTextSize(0.04);

  TString MCtitle("MC ");
  if (is_data==1) MCtitle = "";

  leg->SetFillStyle(4000);
  leg->SetFillColor(kWhite);
  leg->SetTextFont(42);
  leg->AddEntry(JZBratio,MCtitle+"obs / "+MCtitle+"pred","p");
  leg->AddEntry(oneline,"ratio = 1","l");
  leg->AddEntry(pol0d,"fit in [0,30] GeV","l");
  leg->AddEntry(bottomline,"#pm50% envelope","l");
//  leg->Draw("same");
  if(is_data==1) CompleteSave(can, "precise_jzb_ratio_data");
  if(is_data==0) CompleteSave(can, "precise_jzb_ratio_mc");
  if(is_data==2) CompleteSave(can, "precise_jzb_ratio_mc_BandS");//special case, MC with signal!
  
  can->SetLogy(1);
  if(is_data==1) DrawPrelim();
  else DrawMCPrelim();
  TLegend *secondleg = make_legend("Observed vs Predicted");
  secondleg->AddEntry(pred,"Predicted (stat&syst band)","f");
  secondleg->AddEntry(obs,"Observed","p");
  secondleg->SetY1(0.7);
  secondleg->SetX1(0.4);
  
  pred->GetXaxis()->SetTitle("JZB [GeV]");
  pred->GetXaxis()->CenterTitle();
  pred->GetYaxis()->SetTitle("events");
  pred->GetYaxis()->CenterTitle();
  pred->SetFillColor(kGreen);
  pred->SetMarkerColor(kWhite);
  pred->SetMarkerSize(0.000001);
  pred->SetLineColor(pred->GetFillColor());
  pred->Draw("e3");
  DrawPrelim();
  obs->Draw("e1x0,same");
  secondleg->Draw("same");
  
  if(is_data==1) CompleteSave(can, "Exp/precise_obs_pred_data");
  if(is_data==0) CompleteSave(can, "Exp/precise_obs_pred_mc");
  if(is_data==2) CompleteSave(can, "Exp/precise_obs_pred_mc_BandS");//special case, MC with signal!
    
  delete obs;
  delete pred;
}

void Poisson_ratio_plots(string mcjzb,string datajzb,vector<float> ratio_binning,float jzbpearerrorMC,float jzbpeakerrorData) {
  TCanvas *globalc = new TCanvas("globalc","Ratio Plot Canvas");
  globalc->SetLogy(0);
  
  Poisson_ratio_plot(data,ratio_binning,datajzb,globalc, jzbHigh,jzbpearerrorMC,jzbpeakerrorData);
  Poisson_ratio_plot(mc,ratio_binning,mcjzb,globalc, jzbHigh,jzbpearerrorMC,jzbpeakerrorData);
  Poisson_ratio_plot(mcwithsignal,ratio_binning,mcjzb,globalc, jzbHigh,jzbpearerrorMC,jzbpeakerrorData);
}





TF1* do_new_logpar_fit_to_plot(TH1F *osof) {
  TCanvas *logpar_fit_can = new TCanvas("logpar_fit_can","Fit canvas for LogPar");
  TF1 *logparfunc = new TF1("logparfunc",LogParabola,60,200,3);
  TF1 *logparfunc2 = new TF1("logparfunc2",LogParabola,0,(osof->GetXaxis()->GetBinLowEdge(osof->GetNbinsX())+osof->GetXaxis()->GetBinWidth(osof->GetNbinsX())),3);
  TF1 *logparfuncN = new TF1("logparfuncN",LogParabolaN,0,(osof->GetXaxis()->GetBinLowEdge(osof->GetNbinsX())+osof->GetXaxis()->GetBinWidth(osof->GetNbinsX())),3);
  TF1 *logparfuncP = new TF1("logparfuncP",LogParabolaP,0,(osof->GetXaxis()->GetBinLowEdge(osof->GetNbinsX())+osof->GetXaxis()->GetBinWidth(osof->GetNbinsX())),3);
  osof->SetMinimum(0);
  osof->Fit(logparfunc,"QR");
  osof->Draw();
  logparfunc->SetLineWidth(2);
  logparfunc2->SetParameters(logparfunc->GetParameters());
  logparfuncN->SetParameters(logparfunc->GetParameters());
  logparfuncP->SetParameters(logparfunc->GetParameters());
  stringstream fitinfo;
  fitinfo << "#Chi^{2} / NDF : " << logparfunc->GetChisquare() << " / " << logparfunc->GetNDF();
  TText *writefitinfo = write_text(0.8,0.8,fitinfo.str());
  writefitinfo->SetTextSize(0.03);
  DrawPrelim();
  writefitinfo->Draw();
  logparfunc->Draw("same");
  logparfunc2->Draw("same");
  logparfuncN->SetLineStyle(2);
  logparfuncP->SetLineStyle(2);
  logparfuncN->Draw("same");
  logparfuncP->Draw("same");
  CompleteSave(logpar_fit_can,"Bpred_ttbar_LogPar_Fit_To_OSOF");
  delete logpar_fit_can;
  return logparfunc2;
}

vector<TF1*> do_new_extended_fit_to_plot(TH1F *prediction, TH1F *ossf, TH1F *osof) {
  /* there are mainly two background contributions: Z+Jets (a) and ttbar (b). So:
  a) The case is clear - we take the OSSF prediction - OSOF prediction, and fit a crystal ball function to it. We then extract the CB parameters.
  b) For ttbar, we use the OSOF distribution and look at the [10,100] GeV JZB range, and fit our log parabola. We then extract the LP parameters.
  Once we have these two components, we use the combined parameters to get the final function and we're done.
  */
  //Step 1: take the OSSF prediction - OSOF prediction, and fit a crystal ball function to it
  TH1F *step1cb = (TH1F*)ossf->Clone("step1cb");
  step1cb->Add(osof,-1);
  vector<TF1*> functions = do_cb_fit_to_plot(step1cb,PlottingSetup::JZBPeakWidthData);
  TF1 *zjetscrystalball = functions[1];
  
  //Step 2: use the OSOF distribution and look at the [10,100] GeV JZB range, and fit our log parabola
  TH1F *ttbarprediction=(TH1F*)prediction->Clone("ttbarprediction");
  ttbarprediction->Add(ossf,-1);//without the Z+Jets estimate, this is really just the ttbar estimate!
  TF1 *ttbarlogpar = do_new_logpar_fit_to_plot(ttbarprediction);
  TF1 *ttbarlogparP = new TF1("ttbarlogparP",LogParabolaP,0,(prediction->GetXaxis()->GetBinLowEdge(prediction->GetNbinsX())+prediction->GetXaxis()->GetBinWidth(prediction->GetNbinsX())),8);
  TF1 *ttbarlogparN = new TF1("ttbarlogparN",LogParabolaN,0,(prediction->GetXaxis()->GetBinLowEdge(prediction->GetNbinsX())+prediction->GetXaxis()->GetBinWidth(prediction->GetNbinsX())),8);
  
  //and now fuse the two!
  TF1 *kmlp = new TF1("kmlp", CrystalBallPlusLogParabola, 0,(prediction->GetXaxis()->GetBinLowEdge(prediction->GetNbinsX())+prediction->GetXaxis()->GetBinWidth(prediction->GetNbinsX())),8);
  TF1 *kmlpP= new TF1("kmlpP",CrystalBallPlusLogParabolaP,0,(prediction->GetXaxis()->GetBinLowEdge(prediction->GetNbinsX())+prediction->GetXaxis()->GetBinWidth(prediction->GetNbinsX())),8);
  TF1 *kmlpN= new TF1("kmlpN",CrystalBallPlusLogParabolaN,0,(prediction->GetXaxis()->GetBinLowEdge(prediction->GetNbinsX())+prediction->GetXaxis()->GetBinWidth(prediction->GetNbinsX())),8);
  double kmlp_pars[10];
  for(int i=0;i<5;i++) kmlp_pars[i]=zjetscrystalball->GetParameter(i);
  for(int i=0;i<3;i++) kmlp_pars[5+i]=ttbarlogpar->GetParameter(i);
  ttbarlogparP->SetParameters(ttbarlogpar->GetParameters());
  ttbarlogparN->SetParameters(ttbarlogpar->GetParameters());
  kmlp->SetParameters(kmlp_pars);
  prediction->Fit(kmlp);
  
  kmlpP->SetParameters(kmlp->GetParameters());
  kmlpN->SetParameters(kmlp->GetParameters());
  
  // now that we're done, let's save all of this so we can have a look at it afterwards.
  TCanvas *can = new TCanvas("can","Prediction Fit Canvas");
  can->SetLogy(1);
  prediction->SetMarkerColor(kRed);
  prediction->Draw();
  
  kmlp->SetLineColor(TColor::GetColor("#04B404"));
  kmlpP->SetLineColor(TColor::GetColor("#04B404"));
  kmlpN->SetLineColor(TColor::GetColor("#04B404"));
  kmlp->Draw("same");
  kmlpN->SetLineStyle(2);
  kmlpP->SetLineStyle(2);
  kmlpN->Draw("same");
  kmlpP->Draw("same");
  
  ttbarlogpar->SetLineColor(TColor::GetColor("#CC2EFA"));//purple
  ttbarlogpar->Draw("same");
  ttbarlogparP->SetLineColor(TColor::GetColor("#CC2EFA"));//purple
  ttbarlogparN->SetLineColor(TColor::GetColor("#CC2EFA"));//purple
  ttbarlogparP->SetLineStyle(2);
  ttbarlogparN->SetLineStyle(2);
  ttbarlogparP->Draw("same");
  ttbarlogparN->Draw("same");
  
  functions[0]->Draw("same");functions[1]->Draw("same");functions[2]->Draw("same");
  
  TLegend *analyticalBpredLEG = make_legend("",0.5,0.55);
  analyticalBpredLEG->AddEntry(prediction,"predicted","p");
  analyticalBpredLEG->AddEntry(functions[1],"Crystal Ball fit","l");
  analyticalBpredLEG->AddEntry(functions[0],"1#sigma Crystal Ball fit","l");
  analyticalBpredLEG->AddEntry(ttbarlogparN,"TTbar fit","l");
  analyticalBpredLEG->AddEntry(ttbarlogpar,"1#sigma TTbar fit","l");
  analyticalBpredLEG->AddEntry(kmlp,"Combined function","l");
  analyticalBpredLEG->AddEntry(kmlpN,"1#sigma combined function","l");
  analyticalBpredLEG->Draw("same");
  
  CompleteSave(can,"Bpred_MC_Analytical_Function_Composition");
  delete can;
  
  //and finally: prep return functions
  vector<TF1*> return_functions;
  return_functions.push_back(kmlpN);
  return_functions.push_back(kmlp);
  return_functions.push_back(kmlpP);
  
  return_functions.push_back(ttbarlogparN);
  return_functions.push_back(ttbarlogpar);
  return_functions.push_back(ttbarlogparP);
  
  return_functions.push_back(functions[0]);
  return_functions.push_back(functions[1]);
  return_functions.push_back(functions[2]);
  
  return return_functions;
}
void do_new_prediction_plot(string jzb, TCanvas *globalcanvas, float sigma, float high, bool is_data, bool overlay_signal = false )
{
  int nbins=100;
  if(is_data) nbins=50;
  float low=0;
  float hi=500;
  
  
  TH1F *RcorrJZBeemm   = allsamples.Draw("RcorrJZBeemm",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
  TH1F *LcorrJZBeemm   = allsamples.Draw("LcorrJZBeemm",("-"+jzb).c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
  TH1F *RcorrJZBem     = allsamples.Draw("RcorrJZBem",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
  TH1F *LcorrJZBem     = allsamples.Draw("LcorrJZBem",("-"+jzb).c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
  
  TH1F *RcorrJZBSBem;
  TH1F *LcorrJZBSBem;
  TH1F *RcorrJZBSBeemm;
  TH1F *LcorrJZBSBeemm;
  
  TH1F *lm4RcorrJZBeemm   = allsamples.Draw("lm4RcorrJZBeemm",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,is_data, luminosity,allsamples.FindSample("LM4"));
  
  if(RestrictToMassPeak) {
    RcorrJZBSBem   = allsamples.Draw("RcorrJZBSBem",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", sidebandcut&&cutOSOF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
    LcorrJZBSBem   = allsamples.Draw("LcorrJZBSBem",("-"+jzb).c_str(),nbins,low,hi, "JZB [GeV]", "events", sidebandcut&&cutOSOF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
    RcorrJZBSBeemm = allsamples.Draw("RcorrJZBSBeemm",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", sidebandcut&&cutOSSF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
    LcorrJZBSBeemm = allsamples.Draw("LcorrJZBSBeemm",("-"+jzb).c_str(),nbins,low,hi, "JZB [GeV]", "events", sidebandcut&&cutOSSF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
  }
  
  TH1F *conta   = allsamples.Draw("conta",jzb.c_str(),2*nbins,-hi,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,is_data, luminosity,allsamples.FindSample("TTJet"));
  
  TFile *ttbarfile=new TFile("ttbarfile.root","RECREATE");
  conta->Write();
  ttbarfile->Close();

  TH1F *Bpred = (TH1F*)LcorrJZBeemm->Clone("Bpred");
  if(RestrictToMassPeak) {
    Bpred->Add(RcorrJZBem,1.0/3.);
    Bpred->Add(LcorrJZBem,-1.0/3.);
    Bpred->Add(RcorrJZBSBem,1.0/3.);
    Bpred->Add(LcorrJZBSBem,-1.0/3.);
    Bpred->Add(RcorrJZBSBeemm,1.0/3.);
    Bpred->Add(LcorrJZBSBeemm,-1.0/3.);
  } else {
    Bpred->Add(RcorrJZBem,1.0);
    Bpred->Add(LcorrJZBem,-1.0);
  }
    
  globalcanvas->cd();
  globalcanvas->SetLogy(1);

  RcorrJZBeemm->SetMarkerStyle(20);
  RcorrJZBeemm->GetXaxis()->SetRangeUser(0,high);
  RcorrJZBeemm->Draw("e1x0");

  Bpred->SetLineColor(kRed);
  Bpred->SetStats(0);
  Bpred->Draw("hist,same");

  if ( overlay_signal ) {
    lm4RcorrJZBeemm->SetLineColor(TColor::GetColor("#088A08"));
    lm4RcorrJZBeemm->Draw("histo,same");
  }
  RcorrJZBeemm->SetMarkerSize(DataMarkerSize);
  
  TLegend *legBpred = make_legend("",0.6,0.55);
  if(is_data)
  {
    vector<TF1*> analytical_function = do_new_extended_fit_to_plot(Bpred,LcorrJZBeemm,LcorrJZBem);
    globalcanvas->cd();
    analytical_function[0]->Draw("same"); analytical_function[1]->Draw("same");analytical_function[2]->Draw("same");
    legBpred->AddEntry(RcorrJZBeemm,"observed","p");
    legBpred->AddEntry(Bpred,"predicted","l");
    legBpred->AddEntry(analytical_function[1],"predicted fit","l");
    legBpred->AddEntry(analytical_function[2],"stat. uncert.","l");
    if ( overlay_signal ) legBpred->AddEntry(lm4RcorrJZBeemm,"LM4","l");
    legBpred->Draw();
    CompleteSave(globalcanvas,"Bpred_Data");
  }
  else {
    vector<TF1*> analytical_function = do_new_extended_fit_to_plot(Bpred,LcorrJZBeemm,LcorrJZBem);
    globalcanvas->cd();
    analytical_function[0]->Draw("same"); analytical_function[1]->Draw("same");analytical_function[2]->Draw("same");
    legBpred->AddEntry(RcorrJZBeemm,"MC observed","p");
    legBpred->AddEntry(Bpred,"MC predicted","l");
    legBpred->AddEntry(analytical_function[1],"predicted fit","l");
    legBpred->AddEntry(analytical_function[2],"stat. uncert.","l");
    if ( overlay_signal ) legBpred->AddEntry(lm4RcorrJZBeemm,"LM4","l");
    legBpred->Draw();
    CompleteSave(globalcanvas,"Bpred_MC");
  }
  
  TH1F *Bpredem = (TH1F*)LcorrJZBeemm->Clone("Bpredem");
  Bpredem->Add(RcorrJZBem);
  Bpredem->Add(LcorrJZBem,-1);
  TH1F *BpredSBem = (TH1F*)LcorrJZBeemm->Clone("BpredSBem");
  BpredSBem->Add(RcorrJZBSBem);
  Bpred->Add(LcorrJZBSBem,-1);
  TH1F *BpredSBeemm = (TH1F*)LcorrJZBeemm->Clone("BpredSBeemm");  
  BpredSBeemm->Add(RcorrJZBSBeemm);
  BpredSBeemm->Add(LcorrJZBSBeemm,-1.0);
  globalcanvas->cd();
  globalcanvas->SetLogy(1);

  RcorrJZBeemm->SetMarkerStyle(20);
  RcorrJZBeemm->GetXaxis()->SetRangeUser(0,high);
  RcorrJZBeemm->Draw("e1x0");
  RcorrJZBeemm->SetMarkerSize(DataMarkerSize);
  
  Bpredem->SetLineColor(kRed+1);
  Bpredem->SetStats(0);
  Bpredem->Draw("hist,same");

  BpredSBem->SetLineColor(kGreen+2);//TColor::GetColor("#0B6138"));
  BpredSBem->SetLineStyle(2);
  BpredSBem->Draw("hist,same");

  BpredSBeemm->SetLineColor(kBlue+1);
  BpredSBeemm->SetLineStyle(3);
  BpredSBeemm->Draw("hist,same");

  TLegend *legBpredc = make_legend("",0.6,0.55);
  if(is_data)
  {
    legBpredc->AddEntry(RcorrJZBeemm,"observed","p");
    legBpredc->AddEntry(Bpredem,"OFZP","l");
    legBpredc->AddEntry(BpredSBem,"OFSB","l");
    legBpredc->AddEntry(BpredSBeemm,"SFSB","l");
    legBpredc->Draw();
    CompleteSave(globalcanvas,"Bpred_Data_comparison");
  }
  else {
    legBpredc->AddEntry(RcorrJZBeemm,"MC observed","p");
    legBpredc->AddEntry(Bpredem,"MC OFZP","l");
    legBpredc->AddEntry(BpredSBem,"MC OFSB","l");
    legBpredc->AddEntry(BpredSBeemm,"MC SFSB","l");
    legBpredc->Draw();
    legBpredc->Draw();
    CompleteSave(globalcanvas,"Bpred_MC_comparison_ttbar");
  }
  delete RcorrJZBeemm;
  delete LcorrJZBeemm;
  delete RcorrJZBem;
  delete LcorrJZBem;
  delete RcorrJZBSBem;
  delete LcorrJZBSBem;
  delete RcorrJZBSBeemm;
  delete LcorrJZBSBeemm;
  delete lm4RcorrJZBeemm;
}

void do_new_prediction_plots(string mcjzb, string datajzb, float DataSigma, float MCSigma, bool overlay_signal ) {
  TCanvas *globalcanvas = new TCanvas("globalcanvas","Prediction Canvas");
//  do_new_prediction_plot(datajzb,globalcanvas,DataSigma,jzbHigh ,data,overlay_signal);
  do_new_prediction_plot(mcjzb,globalcanvas,MCSigma,jzbHigh ,mc,overlay_signal);
}

