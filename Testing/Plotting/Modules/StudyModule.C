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
#include <TProfile.h>
#include <TPaveStats.h>

//#include "TTbar_stuff.C"
using namespace std;

using namespace PlottingSetup;


void do_experimental_pred_obs_calculation(float cut ,string mcjzb,string datajzb, int mcordata) {
  dout << "Crunching the numbers for JZB>" << cut << endl;
  string xlabel="JZB [GeV] -- for algoritm internal use only!";
  TH1F *ZOSSFP = allsamples.Draw("ZOSSFP",datajzb,1,cut,14000, xlabel, "events",cutmass&&cutOSSF&&cutnJets&&basiccut,mcordata,luminosity);
  TH1F *ZOSOFP = allsamples.Draw("ZOSOFP",datajzb,1,cut,14000, xlabel, "events",cutmass&&cutOSOF&&cutnJets&&basiccut,mcordata,luminosity);
  TH1F *ZOSSFN = allsamples.Draw("ZOSSFN","-"+datajzb,1,cut,14000, xlabel, "events",cutmass&&cutOSSF&&cutnJets&&basiccut,mcordata,luminosity);
  TH1F *ZOSOFN = allsamples.Draw("ZOSOFN","-"+datajzb,1,cut,14000, xlabel, "events",cutmass&&cutOSOF&&cutnJets&&basiccut,mcordata,luminosity);
  
  TH1F *SBOSSFP;
  TH1F *SBOSOFP;
  TH1F *SBOSSFN;
  TH1F *SBOSOFN;
  
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    SBOSSFP = allsamples.Draw("SBOSSFP",datajzb,1,cut,14000, xlabel, "events",cutOSSF&&cutnJets&&basiccut&&sidebandcut,mcordata,luminosity);
    SBOSOFP = allsamples.Draw("SBOSOFP",datajzb,1,cut,14000, xlabel, "events",cutOSOF&&cutnJets&&basiccut&&sidebandcut,mcordata,luminosity);
    SBOSSFN = allsamples.Draw("SBOSSFN","-"+datajzb,1,cut,14000, xlabel, "events",cutOSSF&&cutnJets&&basiccut&&sidebandcut,mcordata,luminosity);
    SBOSOFN = allsamples.Draw("SBOSOFN","-"+datajzb,1,cut,14000, xlabel, "events",cutOSOF&&cutnJets&&basiccut&&sidebandcut,mcordata,luminosity);
  }
    
    
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    dout << "   Observed : " << ZOSSFP->Integral() << endl;
    dout << "   Predicted: " << ZOSSFN->Integral() << " + (1/3)*(" << ZOSOFP->Integral() << "-" << ZOSOFN->Integral()<<") + (1/3)*(" << SBOSSFP->Integral() << "-" << SBOSSFN->Integral()<<") + (1/3)*(" << SBOSOFP->Integral() << "-" << SBOSOFN->Integral()<<")" << endl;
    dout << "        P(ZJets ) \t " << ZOSSFN->Integral() << endl;
    dout << "        P(e&mu;]) \t " << ZOSOFP->Integral() << "-" << ZOSOFN->Integral() << " = " << ZOSOFP->Integral()-ZOSOFN->Integral()<<endl;
    dout << "        P(ossf,sb]) \t " << SBOSSFP->Integral() << "-" << SBOSSFN->Integral()<<" = "<<SBOSSFP->Integral()-SBOSSFN->Integral()<<endl;
    dout << "        P(osof,sb]) \t " << SBOSOFP->Integral() << "-" << SBOSOFN->Integral()<<" = "<<SBOSOFP->Integral()-SBOSOFN->Integral()<<endl;
  } else {
    dout << "   Observed : " << ZOSSFP->Integral() << endl;
    dout << "   Predicted: " << ZOSSFN->Integral() << " + (" << ZOSOFP->Integral() << "-" << ZOSOFN->Integral()<<")" << endl;
    dout << "        P(ZJets ) \t " << ZOSSFN->Integral() << endl;
    dout << "        P(e&mu;]) \t " << ZOSOFP->Integral() << "-" << ZOSOFN->Integral() << " = " << ZOSOFP->Integral()-ZOSOFN->Integral()<<endl;
  }
    
  
  delete ZOSSFP;
  delete ZOSOFP;
  delete ZOSSFN;
  delete ZOSOFN;
  
  delete SBOSSFP;
  delete SBOSOFP;
  delete SBOSSFN;
  delete SBOSOFN;
}

void look_at_sidebands(string mcjzb, string datajzb, bool includejetcut, float cutat=0) {
  
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak -- this funciton is meaningless for the offpeak case
  if(!PlottingSetup::RestrictToMassPeak) return;
  dout << "Looking at sidebands ... " << endl;
  int mcordata=data;//data     // you can perform this study for mc or data ...
  
  TCut specialjetcut;
  if(includejetcut) specialjetcut=cutnJets&&basiccut;
  else specialjetcut="mll>0";
  dout << "The datajzb variable is defined as " << datajzb << endl;
  stringstream addcut;
  addcut<<"(pt>"<<cutat<<")";
  TCut additionalcut=addcut.str().c_str();
  
  int nbins=75; float min=51;float max=201;string xlabel="mll [GeV]";
  TCanvas *c1 = new TCanvas("c1","c1");
  c1->SetLogy(1);
  TH1F *datahistoOSSF = allsamples.Draw("datahistoOSSF","mll",nbins,min,max, xlabel, "events",cutOSSF&&specialjetcut&&additionalcut,data,luminosity);
  THStack mcstackOSSF = allsamples.DrawStack("mcstackOSSF","mll",nbins,min,max, xlabel, "events",cutOSSF&&specialjetcut&&additionalcut,mc,luminosity);
  
  datahistoOSSF->SetMinimum(1);
  datahistoOSSF->Draw();
  mcstackOSSF.Draw("same");
  datahistoOSSF->Draw("same");
  TLegend *kinleg = allsamples.allbglegend();
  kinleg->AddEntry(datahistoOSSF,"OSSF (data)","p");
  kinleg->Draw();
  if(includejetcut) CompleteSave(c1,"sidebands/"+any2string(cutat)+"/OSSF");
  else CompleteSave(c1,"sidebands/"+any2string(cutat)+"/OSSF_nojetcut");
  
  TH1F *datahistoOSOF = allsamples.Draw("datahistoOSOF","mll",nbins,min,max, xlabel, "events",cutOSOF&&specialjetcut&&additionalcut,data,luminosity);
  THStack mcstackOSOF = allsamples.DrawStack("mcstackOSOF","mll",nbins,min,max, xlabel, "events",cutOSOF&&specialjetcut&&additionalcut,mc,luminosity);
//  datahistoOSOF->SetMinimum(0.4);
  datahistoOSOF->Draw();
  mcstackOSOF.Draw("same");
  datahistoOSOF->Draw("same");
  TLegend *kinleg2 = allsamples.allbglegend();
  kinleg2->AddEntry(datahistoOSOF,"OSOF (data)","p");
  kinleg2->Draw();
  if(includejetcut) CompleteSave(c1,"sidebands/"+any2string(cutat)+"/OSOF");
  else CompleteSave(c1,"sidebands/"+any2string(cutat)+"/OSOF_nojetcut");
  
  TH1F *rawmlleemmData  = allsamples.Draw("rawmlleemmData","mll",200,0,200, "mll [GeV]", "events", cutOSSF&&specialjetcut&&sidebandcut&&additionalcut,data, luminosity);
  TH1F *rawmllemData    = allsamples.Draw("rawmllemData"  ,"mll",200,0,200,  "mll [GeV]", "events", cutmass&&cutOSOF&&specialjetcut&&additionalcut,data, luminosity);
  dout << "Number of events in peak for OSOF: " << rawmllemData->GetEntries() << endl;
  dout << "Number of events in SB for OSSF: " << rawmlleemmData->GetEntries() << endl;
  
  TH1F *SFttbarZpeak  = allsamples.Draw("SFttbarZpeak",mcjzb,100,-200,400, "JZB [GeV]", "events",cutmass&&cutOSSF&&specialjetcut&&additionalcut,mc,luminosity,allsamples.FindSample("TTJets"));
  TH1F *OFttbarZpeak  = allsamples.Draw("OFttbarZpeak",mcjzb,100,-200,400, "JZB [GeV]", "events",cutmass&&cutOSOF&&specialjetcut&&additionalcut,mc,luminosity,allsamples.FindSample("TTJets"));
  TH1F *SFttbarsideb  = allsamples.Draw("SFttbarsideb",mcjzb,100,-200,400, "JZB [GeV]", "events",cutOSSF&&specialjetcut&&sidebandcut&&additionalcut,mc,luminosity,allsamples.FindSample("TTJets"));
  
  SFttbarZpeak->SetLineColor(kBlack);
  OFttbarZpeak->SetLineColor(kBlue);
  OFttbarZpeak->SetMarkerColor(kBlue);
  SFttbarsideb->SetLineColor(kPink);
  SFttbarsideb->SetMarkerColor(kPink);
  
  SFttbarZpeak->Draw("histo");
  OFttbarZpeak->Draw("histo,same");
  SFttbarsideb->Draw("histo,same");
  
  TLegend *leg3 = new TLegend(0.6,0.8,0.89,0.89);
  leg3->AddEntry(SFttbarZpeak,"SF ttbar Z peak","l");
  leg3->AddEntry(OFttbarZpeak,"OF ttbar Z peak","l");
  leg3->AddEntry(SFttbarsideb,"SF ttbar SB","l");
  leg3->SetFillColor(kWhite);
  leg3->SetLineColor(kWhite);
  leg3->SetBorderSize(0);
    
  leg3->Draw();
  if(includejetcut) CompleteSave(c1,"sidebands/"+any2string(cutat)+"/ttbar_comparison");
  else CompleteSave(c1,"sidebands/"+any2string(cutat)+"/ttbar_comparison_nojetcut");
  
  
  c1->SetLogy(0);
  
  SFttbarsideb->SetFillColor(TColor::GetColor("#F5A9A9"));
  SFttbarsideb->SetLineColor(TColor::GetColor("#F5A9A9"));
  SFttbarsideb->SetFillStyle(3004);
  OFttbarZpeak->SetFillColor(TColor::GetColor("#819FF7"));
  OFttbarZpeak->SetLineColor(TColor::GetColor("#819FF7"));
  OFttbarZpeak->SetFillColor(kBlue);
  OFttbarZpeak->SetFillStyle(3005);
  
  OFttbarZpeak->Rebin(2);
  SFttbarZpeak->Rebin(2);
  SFttbarsideb->Rebin(2);
  OFttbarZpeak->Divide(SFttbarZpeak);
  SFttbarsideb->Divide(SFttbarZpeak);
  OFttbarZpeak->GetYaxis()->SetRangeUser(0,5);
  OFttbarZpeak->GetYaxis()->SetTitle("ratio");
  TF1 *centralfitO = new TF1("centralfitO","pol1",-40,120);
  TF1 *centralfit1 = new TF1("centralfit1","pol1",-200,400);
//  TF1 *centralfitS = new TF1("centralfitS","pol1",-40,120);
  SFttbarsideb->Fit(centralfitO,"R");
  //OFttbarZpeak->Fit(centralfitO,"R");
  centralfit1->SetParameters(centralfitO->GetParameters());
//  SFttbarZpeak->Fit(centralfitS,"R");
  OFttbarZpeak->Draw("e5");
  SFttbarsideb->Draw("e5,same");
  OFttbarZpeak->Draw("same");
  SFttbarsideb->Draw("same");
  centralfit1->SetLineColor(kOrange);
//  centralfitS->SetLineColor(kOrange);
  centralfit1->SetLineWidth(2);
//  centralfitS->SetLineWidth(2);
  centralfit1->Draw("same");
//  centralfitS->Draw("same");
  TLine *oneline = new TLine(-200,1,400,1);
  oneline->SetLineColor(kBlue);
  TLine *point5  = new TLine(-200,0.5,400,0.5);
  point5->SetLineStyle(2);
  point5->SetLineColor(kGreen);
  TLine *op5     = new TLine(-200,1.5,400,1.5);
  op5->SetLineStyle(2);
  op5->SetLineColor(kGreen);
  TLine *point7  = new TLine(-200,0.7,400,0.7);
  point7->SetLineStyle(2);
  point7->SetLineColor(kBlack);
  TLine *op7     = new TLine(-200,1.3,400,1.3);
  op7->SetLineStyle(2);
  op7->SetLineColor(kBlack);
  oneline->Draw("same");
  point5->Draw("same");
  point7->Draw("same");
  op5->Draw("same");
  op7->Draw("same");
  TLegend *leg4 = new TLegend(0.6,0.65,0.89,0.89);
  leg4->AddEntry(OFttbarZpeak,"OF ttbar Z peak / truth","l");
  leg4->AddEntry(SFttbarsideb,"SF ttbar SB / truth","l");
  leg4->AddEntry(centralfit1,"Fit to [-40,120] GeV region (OF)","l");
  leg4->AddEntry(point5,"50% systematic envelope","l");
  leg4->AddEntry(point7,"30% systematic envelope","l");
//  leg4->AddEntry(centralfitS,"Fit to [-40,120] GeV region (SF)","l");
  leg4->SetFillColor(kWhite);
  leg4->SetLineColor(kWhite);
  leg4->SetBorderSize(0);
  leg4->Draw("same");
  if(includejetcut) CompleteSave(c1,"sidebands/"+any2string(cutat)+"/ttbar_comparison_ratio");
  else CompleteSave(c1,"sidebands/"+any2string(cutat)+"/ttbar_comparison_ratio_nojetcut");
  dout << "Moving on to predicted / observed yields! " << endl;
  dout << "Sideband definition: " << (const char*) sidebandcut << endl;
  /*
  do_experimental_pred_obs_calculation(50,mcjzb,datajzb,mcordata);
  do_experimental_pred_obs_calculation(75,mcjzb,datajzb,mcordata);
  do_experimental_pred_obs_calculation(100,mcjzb,datajzb,mcordata);
  do_experimental_pred_obs_calculation(125,mcjzb,datajzb,mcordata);
  do_experimental_pred_obs_calculation(150,mcjzb,datajzb,mcordata);
  */
  
  delete rawmlleemmData;
  delete rawmllemData;
  delete SFttbarZpeak;
  delete OFttbarZpeak;
  delete SFttbarsideb;
  delete datahistoOSOF;
  delete datahistoOSSF;
  
  
}

void look_at_sidebands(string mcjzb, string datajzb) {
//  for (int i=0;i<100;i+=10) {
int i=0;
  {
    look_at_sidebands(mcjzb,datajzb, true,i);
    look_at_sidebands(mcjzb,datajzb, false,i);
  }
}
  
void find_sideband_definition() {
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(!PlottingSetup::RestrictToMassPeak) return; // this function is meaningless for the offpeak analysis

  TH1F *mllttbar  = allsamples.Draw("mllttbar","mll",145,55,200, "mll [GeV]", "events",cutOSSF&&cutnJets&&!cutmass,mc,luminosity,allsamples.FindSample("TTJets"));
  TH1F *mllttbarz  = allsamples.Draw("mllttbarz","mll",1,50,200, "mll [GeV]", "events",cutOSSF&&cutnJets&&cutmass,mc,luminosity,allsamples.FindSample("TTJets"));
  float leftstop=0;
  float rightstop=0;
  int pos90=mllttbar->FindBin(90);
  float leftsum=0; float rightsum=0;
  float peaksum=mllttbarz->Integral();
  for(int i=0;i<mllttbar->GetNbinsX()&&!(leftstop&&rightstop);i++) {
    if(pos90-i<1) leftstop=mllttbar->GetBinLowEdge(1);
    if(mllttbar->GetBinLowEdge(pos90+i)+mllttbar->GetBinWidth(pos90+i)>190) rightstop=190;
    if(!leftstop) leftsum+=mllttbar->GetBinContent(pos90-i);
    if(!rightstop) rightsum+=mllttbar->GetBinContent(pos90+i);
    if(leftsum+rightsum>peaksum) {
      if(!leftstop) leftstop=mllttbar->GetBinLowEdge(pos90-i);
      if(!rightstop) rightstop=mllttbar->GetBinLowEdge(pos90+i)+mllttbar->GetBinWidth(pos90+i);
      dout << "Found the boundaries! on the left: " << leftstop << " and on the right " << rightstop << endl;
      dout << "Total sum : " << leftsum+rightsum << " which supposedly corresponds ~ to " << peaksum << endl;
    }
  }
  TH1F *mllttbart  = allsamples.Draw("mllttbart","mll",1,55,155, "mll [GeV]", "events",cutOSSF&&cutnJets&&!cutmass,mc,luminosity,allsamples.FindSample("TTJets"));
  dout << mllttbart->Integral() << endl;
  
  
}

void calculate_upper_limits(string mcjzb, string datajzb) {
  write_warning("calculate_upper_limits","Upper limit calculation temporarily deactivated");
//  write_warning("calculate_upper_limits","Calculation of SUSY upper limits has been temporarily suspended in favor of top discovery");
//  rediscover_the_top(mcjzb,datajzb);
/*  
  TCanvas *c3 = new TCanvas("c3","c3");
  c3->SetLogy(1);
  vector<float> binning;
  //binning=allsamples.get_optimal_binsize(mcjzb,cutmass&&cutOSSF&&cutnJets,20,50,800);
  binning.push_back(50);
  binning.push_back(100);
  binning.push_back(150);
  binning.push_back(200);
  binning.push_back(500);
  TH1F *datapredictiona = allsamples.Draw("datapredictiona",    "-"+datajzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,mc,  luminosity);
  TH1F *datapredictionb = allsamples.Draw("datapredictionb",    "-"+datajzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,mc,  luminosity);
  TH1F *datapredictionc = allsamples.Draw("datapredictionc",    datajzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,mc,  luminosity);
  TH1F *dataprediction = (TH1F*)datapredictiona->Clone();
  dataprediction->Add(datapredictionb,-1);
  dataprediction->Add(datapredictionc);
  TH1F *puresignal     = allsamples.Draw("puresignal",        mcjzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,mc,  luminosity,allsamples.FindSample("LM4"));
  TH1F *signalpred     = allsamples.Draw("signalpred",    "-"+mcjzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,mc,  luminosity,allsamples.FindSample("LM4"));
  TH1F *signalpredlo   = allsamples.Draw("signalpredlo",  "-"+mcjzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,mc,  luminosity,allsamples.FindSample("LM4"));
  TH1F *signalpredro   = allsamples.Draw("signalpredro",      mcjzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,mc,  luminosity,allsamples.FindSample("LM4"));
  TH1F *puredata       = allsamples.Draw("puredata",          datajzb,binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,luminosity);
  signalpred->Add(signalpredlo,-1);
  signalpred->Add(signalpredro);
  puresignal->Add(signalpred,-1);//subtracting signal contamination
  ofstream myfile;
  myfile.open ("ShapeFit_log.txt");
  establish_upper_limits(puredata,dataprediction,puresignal,"LM4",myfile);
  myfile.close();
*/ 
}

TH1F *runcheckhisto(string cut) {
  string histoname=GetNumericHistoName();
  TH1F *histo = new TH1F(histoname.c_str(),histoname.c_str(),100,163000,168000);
  (allsamples.collection)[0].events->Draw(("runNum>>"+histoname).c_str(),cut.c_str());
  return histo;
}

void run_check() {
  gROOT->SetStyle("Plain");
  TCanvas *c1 = new TCanvas("runnum","runnum",800,1000);
  c1->Divide(2,4);
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) c1->Divide(2,2); // there are only four regions ... 

  c1->cd(1);
  TH1F *ossfp = runcheckhisto((const char*)(cutmass&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
  ossfp->Draw();
  TText *t1 = write_title("OSSF,P");t1->Draw();
  
  c1->cd(2);
  TH1F *ossfn = runcheckhisto((const char*)(cutmass&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
  ossfn->Draw();
  TText *t2 = write_title("OSSF,N");t2->Draw();
  
  c1->cd(3);
  TH1F *osofp = runcheckhisto((const char*)(cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
  osofp->Draw();
  TText *t3 = write_title("OSOF,P");t3->Draw();
  c1->cd(4);
  TH1F *osofn = runcheckhisto((const char*)(cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
  osofn->Draw();
  TText *t4 = write_title("OSOF,N");t4->Draw();
  
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    c1->cd(5);
    TH1F *sbofp = runcheckhisto((const char*)(sidebandcut&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    sbofp->Draw();
    TText *t5 = write_title("SB,OSOF,P");t5->Draw();
    c1->cd(6);
    TH1F *sbofn = runcheckhisto((const char*)(cutOSOF&&cutnJets&&basiccut&&sidebandcut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
    sbofn->Draw();
    TText *t6 = write_title("SB,OSOF,N");t6->Draw();
    
    c1->cd(7);
    TH1F *sbsfp = runcheckhisto((const char*)(sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    sbsfp->Draw();
    TText *t7 = write_title("SB,OSSF,P");t7->Draw();
    c1->cd(8);
    TH1F *sbsfn = runcheckhisto((const char*)(sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
    sbsfn->Draw();
    TText *t8 = write_title("SB,OSSF,N");t8->Draw();
  }
  
  c1->SaveAs("runNumber.png");
}

TH1F *give_boson_pred(TCut bcut,string mcjzb) {
  int nbins=50;
  TH1F *jzbn   = allsamples.Draw("jzbn","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",  bcut&&cutOSSF&&cutnJets&&cutmass,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
  TH1F *jzbno  = allsamples.Draw("jzbno","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",bcut&&cutOSOF&&cutnJets&&cutmass,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
  TH1F *jzbpo  = allsamples.Draw("jzbp",mcjzb,nbins,0,350, "JZB [GeV]", "events",     bcut&&cutOSOF&&cutnJets&&cutmass,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
  
  //Sidebands
  TH1F *jzbnos;
  TH1F *jzbpos;
  TH1F *jzbnss;
  TH1F *jzbpss;
  
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    jzbnos  = allsamples.Draw("jzbnos","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",bcut&&cutOSOF&&cutnJets&&sidebandcut,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
    jzbpos  = allsamples.Draw("jzbpos",mcjzb,nbins,0,350, "JZB [GeV]", "events",    bcut&&cutOSOF&&cutnJets&&sidebandcut,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
    jzbnss  = allsamples.Draw("jzbnss","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",bcut&&cutOSSF&&cutnJets&&sidebandcut,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
    jzbpss  = allsamples.Draw("jzbpss",mcjzb,nbins,0,350, "JZB [GeV]", "events",    bcut&&cutOSSF&&cutnJets&&sidebandcut,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
  }
  
    
  TH1F *pred = (TH1F*)jzbn->Clone("pred");
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    pred->Add(jzbno,-1.0/3);
    pred->Add(jzbpo,1.0/3);
    pred->Add(jzbnos,-1.0/3);
    pred->Add(jzbpos,1.0/3);
    pred->Add(jzbnss,-1.0/3);
    pred->Add(jzbpss,1.0/3);
  } else {
    pred->Add(jzbno,-1.0);
    pred->Add(jzbpo,1.0);
  }
  pred->SetLineColor(kRed);
  pred->SetMinimum(0);
  delete jzbn;
  delete jzbpo;
  delete jzbno;
  delete jzbpos;
  delete jzbnos;
  delete jzbpss;
  delete jzbnss;
  
  return pred;
}
  

void show_dibosons(string datajzb, string mcjzb) {
  TCut WW("(abs(genMID1)==24&&abs(genMID2)==24)||(abs(genGMID1)==24&&abs(genGMID2)==24)");
  TCut ZZ("(abs(genMID1)==23&&abs(genMID2)==23)||(abs(genGMID1)==23&&abs(genGMID2)==23)");
  TCut WZ("((abs(genMID1)==23&&abs(genMID2)==24)||(abs(genGMID1)==23&&abs(genGMID2)==24))||((abs(genMID1)==24&&abs(genMID2)==23)||(abs(genGMID1)==24&&abs(genGMID2)==23))");
  
  TCanvas *dibs = new TCanvas("dibs","dibs",900,900);
  dibs->Divide(2,2);
  
  dibs->cd(1);
  TH1F *wwjzbp  = allsamples.Draw("wwjzbp",mcjzb,70,0,350, "JZB [GeV]", "events",      WW&&cutOSSF&&cutnJets&&cutmass,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
  TH1F *wwpred = (TH1F*) give_boson_pred(WW,mcjzb);
  wwpred->Draw("histo");
  wwjzbp->Draw("histo,same");
  TLegend *leg = make_legend("WW");
  leg->SetFillColor(kWhite);
  leg->SetLineColor(kWhite);
  leg->SetHeader("WW (MC)");
  leg->AddEntry(wwjzbp,"Observed","l");
  leg->AddEntry(wwpred,"Predicted","l");
  leg->Draw("same");

  dibs->cd(2);
  TH1F *wzjzbp  = allsamples.Draw("wzjzbp",mcjzb,70,0,350, "JZB [GeV]", "events",WZ&&cutOSSF&&cutnJets&&cutmass,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
  TH1F *wzpred = (TH1F*) give_boson_pred(WZ,mcjzb);
  wzpred->Draw("histo");
  wzjzbp->Draw("same,histo");
  TLegend *leg2 = (TLegend*)leg->Clone("leg2");
  leg2->SetHeader("WZ (MC)");
  leg2->Draw("same");
  DrawPrelim();

  dibs->cd(3);
  TH1F *zzjzbp  = allsamples.Draw("zzjzbp",mcjzb,70,0,350, "JZB [GeV]", "events",ZZ&&cutOSSF&&cutnJets&&cutmass,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
  TH1F *zzpred = (TH1F*) give_boson_pred(ZZ,mcjzb);
  zzpred->Draw("histo");
  zzjzbp->Draw("same,histo");
  TLegend *leg3 = (TLegend*)leg->Clone("leg2");
  leg3->SetHeader("ZZ (MC)");
  leg3->Draw("same");
  leg3->Draw("same");
  DrawPrelim();
  
  dibs->cd(4);
  TH1F *alljzbp  = allsamples.Draw("alljzbp",mcjzb,70,0,350, "JZB [GeV]", "events",(WW||WZ||ZZ)&&cutOSSF&&cutnJets&&cutmass,mc,luminosity,allsamples.FindSample("VVJetsTo4L_TuneD6T_7TeV"));
  TH1F *allpred = (TH1F*) give_boson_pred((WW||WZ||ZZ),mcjzb);
  allpred->Draw("histo");
  alljzbp->Draw("same,histo");
  TLegend *leg4 = (TLegend*)leg->Clone("leg2");
  leg4->SetHeader("All dibosons (MC)");
  leg4->Draw("same");
  DrawPrelim();
  
  CompleteSave(dibs,"Studies/Dibosons");
  
}

void show_rare_samples(string datajzb, string mcjzb) {
  TCanvas *rares = new TCanvas("rares","Rare Samples",900,900);
  
  int nbins=50;
  TH1F *jzbp  = raresample.Draw("jzbp",        mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSSF&&cutnJets&&cutmass,mc,luminosity);

  TH1F *jzbn   = raresample.Draw("jzbn",   "-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSSF&&cutnJets&&cutmass,mc,luminosity);
  TH1F *jzbno  = raresample.Draw("jzbno",  "-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSOF&&cutnJets&&cutmass,mc,luminosity);
  TH1F *jzbpo  = raresample.Draw("jzbp",       mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSOF&&cutnJets&&cutmass,mc,luminosity);
  
  //Sidebands
  TH1F *jzbnos  = raresample.Draw("jzbnos","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSOF&&cutnJets&&sidebandcut,mc,luminosity);
  TH1F *jzbpos  = raresample.Draw("jzbpos",    mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSOF&&cutnJets&&sidebandcut,mc,luminosity);
  TH1F *jzbnss  = raresample.Draw("jzbnss","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSSF&&cutnJets&&sidebandcut,mc,luminosity);
  TH1F *jzbpss  = raresample.Draw("jzbpss",    mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSSF&&cutnJets&&sidebandcut,mc,luminosity);
  
  TH1F *pred = (TH1F*)jzbn->Clone("pred");
  pred->Add(jzbno,-1.0/3);
  pred->Add(jzbpo,1.0/3);
  pred->Add(jzbnos,-1.0/3);
  pred->Add(jzbpos,1.0/3);
  pred->Add(jzbnss,-1.0/3);
  pred->Add(jzbpss,1.0/3);
  pred->SetLineColor(kRed);
  pred->SetMinimum(0);
  delete jzbn;
  delete jzbpo;
  delete jzbno;
  delete jzbpos;
  delete jzbnos;
  delete jzbpss;
  delete jzbnss;

  pred->Draw("histo");
  pred->GetYaxis()->SetTitleOffset(1.3);
  jzbp->Draw("histo,same");
  TLegend *leg = make_legend("WW");
  leg->SetFillColor(kWhite);
  leg->SetLineColor(kWhite);
  leg->SetHeader("Rare Samples (MC)");
  leg->AddEntry(jzbp,"Observed","l");
  leg->AddEntry(pred,"Predicted","l");
  leg->Draw("same");

  
  CompleteSave(rares,"Studies/Rare_Samples");
  delete rares;
}


class signature {
public:
  int runNum;
  int eventNum;
  int lumi;
};

vector<signature> get_list_of_events(string cut) {
  float jzb;
  int runNum,lumi,eventNum;
  (allsamples.collection)[0].events->SetBranchAddress("jzb",&jzb);
  (allsamples.collection)[0].events->SetBranchAddress("eventNum",&eventNum);
  (allsamples.collection)[0].events->SetBranchAddress("lumi",&lumi);
  (allsamples.collection)[0].events->SetBranchAddress("runNum",&runNum);
  
  TTreeFormula *select = new TTreeFormula("select", cut.c_str()&&essentialcut, (allsamples.collection)[0].events);
  vector<signature> allevents;
  for (Int_t entry = 0 ; entry < (allsamples.collection)[0].events->GetEntries() ; entry++) {
   (allsamples.collection)[0].events->LoadTree(entry);
   if (select->EvalInstance()) {
     (allsamples.collection)[0].events->GetEntry(entry);
     signature newevent;
     newevent.runNum=runNum;
     newevent.eventNum=eventNum;
     newevent.lumi=lumi;
     allevents.push_back(newevent);
   }
  }
  cout << "Done looping!" << endl;
  return allevents;
}

void make_double_plot(string variable, int nbins, float min, float max, float ymax, bool logscale,string xlabel, string filename,TCut observed, TCut predicted, bool is_data, bool noscale = false) {
  TCut ibasiccut=basiccut;
  
  //Step 2: Refine the cut
  TCanvas *ckin = new TCanvas("ckin","Kinematic Plots (in the making)",600,600);
  ckin->SetLogy(logscale);
  
  TH1F *datahistoa = allsamples.Draw("datahistoa",variable,nbins,min,max, xlabel, "events",observed,is_data,luminosity);
  TH1F *datahistob = allsamples.Draw("datahistob",variable,nbins,min,max, xlabel, "events",predicted,is_data,luminosity);
  
  datahistoa->SetLineColor(kBlue);
  datahistob->SetLineColor(kRed);
  
  TLegend *kinleg = new TLegend(0.6,0.7,0.8,0.89);
  kinleg->SetFillColor(kWhite);
  kinleg->SetLineColor(kWhite);
  kinleg->SetBorderSize(0);
  kinleg->AddEntry(datahistoa,"Observed "+TString(is_data?"Data":"MC"),"l");
  kinleg->AddEntry(datahistob,"Predicted "+TString(is_data?"Data":"MC"),"l");

  datahistoa->SetMaximum(ymax);
  datahistoa->Draw("histo");
  datahistob->SetLineStyle(2); 
  if ( !noscale ) datahistob->Scale(0.3); 
  datahistob->Draw("histo,sames");
  TVirtualPad::Pad()->Update();

  TPaveStats *sa = (TPaveStats*)datahistoa->GetListOfFunctions()->FindObject("stats");
  TPaveStats *sb = (TPaveStats*)datahistob->GetListOfFunctions()->FindObject("stats");
  if ( sa && sb ) {
     sa->SetTextColor(datahistoa->GetLineColor());
     sb->SetTextColor(datahistob->GetLineColor());
     sb->SetY1NDC(sb->GetY1NDC()-0.25);
     sb->SetY2NDC(sb->GetY2NDC()-0.25);
     TVirtualPad::Pad()->Update();
  }
  kinleg->Draw();
  TText* write_cut = write_title(variable);
  write_cut->Draw();
  CompleteSave(ckin,"special_kin/"+filename);
  datahistoa->Delete();
  datahistob->Delete();
  delete ckin;
}

TH1F *give_lm0_pred(TCut bcut,string mcjzb) {
  int nbins=50;
  TH1F *jzbn   = signalsamples.Draw("jzbn","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",  cutOSSF&&cutnJets&&cutmass,mc,luminosity,signalsamples.FindSample("LM0"));
  TH1F *jzbno  = signalsamples.Draw("jzbno","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSOF&&cutnJets&&cutmass,mc,luminosity,signalsamples.FindSample("LM0"));
  TH1F *jzbpo  = signalsamples.Draw("jzbp",mcjzb,nbins,0,350, "JZB [GeV]", "events",     cutOSOF&&cutnJets&&cutmass,mc,luminosity,signalsamples.FindSample("LM0"));
  
  //Sidebands
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  TH1F *jzbnos;
  TH1F *jzbpos;
  TH1F *jzbnss;
  TH1F *jzbpss;
  
  if(PlottingSetup::RestrictToMassPeak) {
    jzbnos  = signalsamples.Draw("jzbnos","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSOF&&cutnJets&&sidebandcut,mc,luminosity,signalsamples.FindSample("LM0"));
    jzbpos  = signalsamples.Draw("jzbpos",mcjzb,nbins,0,350, "JZB [GeV]", "events",    cutOSOF&&cutnJets&&sidebandcut,mc,luminosity,signalsamples.FindSample("LM0"));
    jzbnss  = signalsamples.Draw("jzbnss","-"+mcjzb,nbins,0,350, "JZB [GeV]", "events",cutOSSF&&cutnJets&&sidebandcut,mc,luminosity,signalsamples.FindSample("LM0"));
    jzbpss  = signalsamples.Draw("jzbpss",mcjzb,nbins,0,350, "JZB [GeV]", "events",    cutOSSF&&cutnJets&&sidebandcut,mc,luminosity,signalsamples.FindSample("LM0"));
  }
  
  TH1F *pred = (TH1F*)jzbn->Clone("pred");
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    pred->Add(jzbno,-1.0/3);
    pred->Add(jzbpo,1.0/3);
    pred->Add(jzbnos,-1.0/3);
    pred->Add(jzbpos,1.0/3);
    pred->Add(jzbnss,-1.0/3);
    pred->Add(jzbpss,1.0/3);
  } else {
    pred->Add(jzbno,-1.0);
    pred->Add(jzbpo,1.0);
  }
    
  pred->SetLineColor(kRed);
  pred->SetMinimum(0);
  
  delete jzbn;
  delete jzbpo;
  delete jzbno;
  delete jzbpos;
  delete jzbnos;
  delete jzbpss;
  delete jzbnss;
  cout << "pred contains " << pred->Integral() << "entries" << endl;
  return pred;
}


void lm0_illustration() {
  TCanvas *can = new TCanvas("can","Signal Background Comparison Canvas");
  can->SetLogy(1);

  int sbg_nbins=130;
  float sbg_min=-500; //-110;
  float sbg_max=800; //jzbHigh;
  
  float simulatedlumi=1000;//in pb please
  
  TH1F *JZBplotZJETs = allsamples.Draw("JZBplotZJETs",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("DYJetsToLL"));
  TH1F *JZBplotLM0 = signalsamples.Draw("JZBplotLM0",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets,mc,simulatedlumi,signalsamples.FindSample("LM0"));
  TH1F *JZBplotTtbar = allsamples.Draw("JZBplotTtbar",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("TTJets"));
  
  JZBplotTtbar->SetLineColor(allsamples.GetColor("TTJet"));
  JZBplotZJETs->SetFillColor(allsamples.GetColor("DY"));
  JZBplotZJETs->SetLineColor(kBlack);
  JZBplotLM0->SetLineStyle(2);
  JZBplotZJETs->SetMaximum(JZBplotZJETs->GetMaximum()*5);
  JZBplotZJETs->SetMinimum(1);

  JZBplotTtbar->SetMaximum(JZBplotZJETs->GetMaximum());
  JZBplotTtbar->SetMinimum(0.01);
  JZBplotTtbar->SetFillColor(allsamples.GetColor("TTJets"));
  JZBplotTtbar->DrawClone("histo");
  JZBplotZJETs->Draw("histo,same");
  JZBplotTtbar->SetFillColor(0);
  JZBplotTtbar->DrawClone("histo,same");
  JZBplotTtbar->SetFillColor(allsamples.GetColor("TTJets"));
  JZBplotLM0->Draw("histo,same");


  TLegend *signal_bg_comparison_leg2 =  make_legend("",0.55,0.75,false);
  signal_bg_comparison_leg2->AddEntry(JZBplotZJETs,"Z+Jets","f");
  signal_bg_comparison_leg2->AddEntry(JZBplotTtbar,"t#bar{t}","f");
  signal_bg_comparison_leg2->AddEntry(JZBplotLM0,"LM0","f");
  signal_bg_comparison_leg2->Draw();
  TText* title = write_title("CMS MC simulation, #sqrt{s}= 7 TeV @ L=1 fb^{-1}");
  title->Draw();
  CompleteSave(can,"LM0/jzb_bg_vs_signal_distribution__LM0");
  can->SetLogy(0);
  TH1F *lm0jzbp  = signalsamples.Draw("lm0jzb",jzbvariablemc,70,0,350, "JZB [GeV]", "events",cutOSSF&&cutnJets&&cutmass,mc,luminosity,signalsamples.FindSample("LM0"));
  TH1F *lm0pred = (TH1F*) give_lm0_pred("mll>0",jzbvariablemc);
  lm0pred->Draw("histo");
  lm0jzbp->Draw("histo,same");
  TLegend *leg = make_legend("LM0");
  leg->SetFillColor(kWhite);
  leg->SetLineColor(kWhite);
  leg->SetHeader("LM0 (MC)");
  leg->AddEntry(lm0jzbp,"Observed","l");
  leg->AddEntry(lm0pred,"Predicted","l");
  leg->Draw("same");
  CompleteSave(can,"LM0/LeftRight");
  
  delete lm0jzbp;
  delete lm0pred;
}

void kinematic_dist_of_pred_and_obs() {//former plot_list
  gStyle->SetOptStat("oueMri");
  TCut observed=(cutmass&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)");

  TCut predicted = (cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)")
                || (sidebandcut&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)")
                || (sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)");
  TCut predictedMC = (cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.04*pt-1.82559)>100)")
                || (sidebandcut&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.04*pt-1.82559)>100)")
                || (sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.04*pt-1.82559)>100)");

  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(!PlottingSetup::RestrictToMassPeak) {
    predicted = TCut((cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)"));
    predictedMC = TCut((cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.04*pt-1.82559)>100)"));
  }
//  TCut predicted=((cutmass&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)")
//                ||(cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)")
//                ||(cutmass&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)")
//                ||(sidebandcut&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)")
//                ||(sidebandcut&&cutOSOF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)")
//                ||(sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)>100)")
//                ||(sidebandcut&&cutOSSF&&cutnJets&&basiccut&&"((jzb[1]+0.06*pt-2.84727)<-100)"));
  
  bool doPF=false;
  bool dolog=true;
  bool nolog=false;
  
  // Mll: do not scale
  make_double_plot("mll",20,50,150,11,nolog,"m_{ll} [GeV]","mll",observed,predicted,data,true);
  make_double_plot("mll",20,50,150,11,nolog,"m_{ll} [GeV]","mll_MC",observed,predictedMC,mc,true);
  make_double_plot("met[4]",20,0,400,11,nolog,"pfMET [GeV]","pfMET",observed,predicted,data);
  make_double_plot("met[4]",20,0,400,11,nolog,"pfMET [GeV]","pfMET_MC",observed,predictedMC,mc);
  make_double_plot("jetpt[0]",10,0,400,11,nolog,"leading jet p_{T} [GeV]","pfJetGoodPt_0",observed,predicted,data);
  make_double_plot("jetpt[0]",10,0,400,11,nolog,"leading jet p_{T} [GeV]","pfJetGoodPt_0_MC",observed,predictedMC,mc);
  make_double_plot("jeteta[0]",10,-5,5,11,nolog,"leading jet #eta","pfJetGoodEta_0",observed,predicted,data);
  make_double_plot("jeteta[0]",10,-5,5,11,nolog,"leading jet #eta","pfJetGoodEta_0_MC",observed,predictedMC,mc);
  make_double_plot("pt",10,0,300,11,nolog,"Z p_{T} [GeV]","Zpt",observed,predicted,data);
  make_double_plot("pt",10,0,300,11,nolog,"Z p_{T} [GeV]","Zpt_MC",observed,predictedMC,mc);
  make_double_plot("pt1",10,0,200,15,nolog,"p_{T} [GeV]","pt1",observed,predicted,data);
  make_double_plot("pt1",10,0,200,15,nolog,"p_{T} [GeV]","pt1_MC",observed,predictedMC,mc);
  make_double_plot("pt2",10,0,200,25,nolog,"p_{T} [GeV]","pt2",observed,predicted,data);
  make_double_plot("pt2",10,0,200,25,nolog,"p_{T} [GeV]","pt2_MC",observed,predictedMC,mc);
  make_double_plot("eta1",10,-5,5,11,nolog,"#eta_{1,l}","eta_1",observed,predicted,data);
  make_double_plot("eta1",10,-5,5,11,nolog,"#eta_{1,l}","eta_1_MC",observed,predictedMC,mc);
  make_double_plot("eta2",10,-5,5,11,nolog,"#eta_{2,l}","eta_2",observed,predicted,data);
  make_double_plot("eta2",10,-5,5,11,nolog,"#eta_{2,l}","eta_2_MC",observed,predictedMC,mc);
  make_double_plot("phi1-phi2",10,-6.0,6.0,11,nolog,"#phi_{1}-#phi_{2}","dphi",observed,predicted,data);
  make_double_plot("phi1-phi2",10,-6.0,6.0,11,nolog,"#phi_{1}-#phi_{2}","dphi_MC",observed,predictedMC,mc);
  make_double_plot("pfJetGoodNum",8,0.5,8.5,20,nolog,"nJets","nJets",observed,predicted,data);
  make_double_plot("pfJetGoodNum",8,0.5,8.5,20,nolog,"nJets","nJets_MC",observed,predictedMC,mc);

}