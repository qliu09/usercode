/****

Off peak status (RestrictToMassPeak) : 

THIS MODULE WILL NOT BE ADAPTED AS IT ONLY CONTAINS OUTDATED FUNCTIONS!


.  Necessary adaptations identified
.  Started working on necessary adaptations
.  Necessary adaptations implemented
.  Necessary adaptations tested



****/

/*

Contains functions that were used in the past but are out now ... 

*/
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

string centralupdownname(int num) {
  if (num==0) return "central";
  if (num==1) return "down";
  if (num==2) return "up";
  return "centralupdownnameERROR";
}

string dataormc(int isdata) {
  if(isdata) return "Data";
  else return "MC";
}

int histocounter=0;
string give_histo_number(string basename="", int isdata=-1, int centralcounter=-1) {
  histocounter++;
  stringstream histocounternum;
  if(isdata==-1) {
  if(basename=="") histocounternum << histocounter;
  else histocounternum << basename << "_" << histocounter;
  }
  else {
    histocounternum << basename << "_" << dataormc(isdata) << "_" << centralupdownname(centralcounter);
  }
  return histocounternum.str();
}

int central=0;
int down=1;
int up=2;

void crunch_the_numbers(TH1F *RcorrJZBeemmop[3][2],TH1F *RcorrJZBeeop[3][2],TH1F *RcorrJZBmmop[3][2],TH1F *LcorrJZBeemmop[3][2],TH1F *LcorrJZBeeop[3][2],TH1F *LcorrJZBmmop[3][2],TH1F *RcorrJZBemop[3][2],TH1F *LcorrJZBemop[3][2],float zjetsestimate[3][2][20],float ttbarestimate[3][2][2][20],float predicted[3][2][20],float observed[3][2][20],int isdata,vector<float> jzbcuts,float eemmmumu[2][3][20]) {
//  dout << "Crunching the numbers for is_data=" << isdata << endl;
for(int k=0;k<20;k++) {for(int i=0;i<2;i++){for(int j=0;j<3;j++) {eemmmumu[i][j][k]=0;}}}
  for(int icut=0;icut<jzbcuts.size()-1;icut++) {//do calculation for each JZB cut
    for(int ibin=1;ibin<jzbcuts.size();ibin++) {//and to do that, we need to consider each bin.
      if(RcorrJZBeemmop[0][isdata]->GetBinLowEdge(ibin)<jzbcuts[icut]) continue;
//      if(icut==0) dout << "Predicted+=" << LcorrJZBeemmop[central][isdata]->GetBinContent(ibin) << " eemm,L" << endl;
      predicted[central][isdata][icut]+=LcorrJZBeemmop[central][isdata]->GetBinContent(ibin);
      predicted[up][isdata][icut]+=LcorrJZBeemmop[up][isdata]->GetBinContent(ibin);
      predicted[down][isdata][icut]+=LcorrJZBeemmop[down][isdata]->GetBinContent(ibin);
      
//      if(icut==0) dout << "Predicted+=" << RcorrJZBemop[central][isdata]->GetBinContent(ibin) << " em, R" << endl;
      predicted[central][isdata][icut]+=RcorrJZBemop[central][isdata]->GetBinContent(ibin);
      predicted[up][isdata][icut]+=RcorrJZBemop[up][isdata]->GetBinContent(ibin);
      predicted[down][isdata][icut]+=RcorrJZBemop[down][isdata]->GetBinContent(ibin);
      
//      if(icut==0) dout << "Predicted-=" << LcorrJZBemop[central][isdata]->GetBinContent(ibin) <<" em, L" << endl;
      predicted[central][isdata][icut]-=LcorrJZBemop[central][isdata]->GetBinContent(ibin);
      predicted[up][isdata][icut]-=LcorrJZBemop[up][isdata]->GetBinContent(ibin);
      predicted[down][isdata][icut]-=LcorrJZBemop[down][isdata]->GetBinContent(ibin);
      
//      if(icut==0) dout << "Observed=" << RcorrJZBeemmop[central][isdata]->GetBinContent(ibin) << "eemm, R" << endl;
      observed[central][isdata][icut]+=RcorrJZBeemmop[central][isdata]->GetBinContent(ibin);
      observed[up][isdata][icut]+=RcorrJZBeemmop[up][isdata]->GetBinContent(ibin);
      observed[down][isdata][icut]+=RcorrJZBeemmop[down][isdata]->GetBinContent(ibin);
      
      zjetsestimate[central][isdata][icut]+=LcorrJZBeemmop[central][isdata]->GetBinContent(ibin);
      ttbarestimate[central][isdata][0][icut]+=RcorrJZBemop[central][isdata]->GetBinContent(ibin);
      ttbarestimate[central][isdata][1][icut]+=LcorrJZBemop[central][isdata]->GetBinContent(ibin);
      
      eemmmumu[0][0][icut]+=LcorrJZBeeop[central][isdata]->GetBinContent(ibin);
      eemmmumu[0][1][icut]+=LcorrJZBmmop[central][isdata]->GetBinContent(ibin);
      eemmmumu[0][2][icut]+=LcorrJZBemop[central][isdata]->GetBinContent(ibin);
      
      eemmmumu[1][0][icut]+=RcorrJZBeeop[central][isdata]->GetBinContent(ibin);
      eemmmumu[1][1][icut]+=RcorrJZBmmop[central][isdata]->GetBinContent(ibin);
      eemmmumu[1][2][icut]+=RcorrJZBemop[central][isdata]->GetBinContent(ibin);
      
    }//end of ibin loop
  }//end of icut loop
}

void make_table(float eemmmumu[2][3][20],int icut,float JZBcut) {
  vector< vector <string> > entries;
  vector <string> line;
  line.push_back("");
  line.push_back("eemm (ee/mm)");
  line.push_back("em");
  entries.push_back(line);
  line.clear();
  line.push_back("JZB>"+any2string(JZBcut));
  line.push_back(any2string(eemmmumu[1][0][icut]+eemmmumu[1][1][icut])+"("+any2string(eemmmumu[1][0][icut])+"/"+any2string(eemmmumu[1][1][icut])+")");
  line.push_back(any2string(eemmmumu[1][2][icut]));
  entries.push_back(line);
  line.clear();
  line.push_back("JZB<-"+any2string(JZBcut));
  line.push_back(any2string(eemmmumu[0][0][icut]+eemmmumu[0][1][icut])+"("+any2string(eemmmumu[0][0][icut])+"/"+any2string(eemmmumu[0][1][icut])+")");
  line.push_back(any2string(eemmmumu[0][2][icut]));
  entries.push_back(line);
  make_nice_jzb_table(entries);
  /*
  dout << "       \t\t | \t eemm (ee/mm) \t | \t  em" << endl;
  dout << "JZB>peak\t | \t " << eemmmumu[1][0]+eemmmumu[1][1] << "(" << eemmmumu[1][0] << "/" << eemmmumu[1][1] << ")\t | \t" << eemmmumu[1][2] << endl;
  dout << "JZB<peak\t | \t " << eemmmumu[0][0]+eemmmumu[0][1] << "(" << eemmmumu[0][0] << "/" << eemmmumu[0][1] << ")\t | \t" << eemmmumu[0][2] << endl;
  dout << endl;
  */
}

void present_result(vector<float> &jzbcuts,float predicted[3][2][20],float observed[3][2][20],float zjetsestimate[3][2][20],float ttbarestimate[3][2][2][20],int icut, float eemmmumu[2][3][20]) {
  
  //blublu
  dout << endl << endl;
  dout << "--------------------------------------------------------------------------------------" << endl;
  dout << "DATA: " << endl;
  dout << "   For JZB>" << jzbcuts[icut];
  float max,downvar,upvar;
  stringstream printtitle;
  printtitle << "JZB>" << jzbcuts[icut] << " (data)";
  ComputePoissonError(zjetsestimate[central][data][icut],ttbarestimate[central][data][0][icut],ttbarestimate[central][data][1][icut],max,downvar,upvar,printtitle.str());
  //dout << " Predicted: " << print_range(predicted[central][data][icut],predicted[up][data][icut],predicted[down][data][icut]) << " (stat) +" << 0.2*zjetsestimate[central][data][icut] << "-" << 0.4*zjetsestimate[central][data][icut] << " (sys) " << "           using Poisson: " << print_range(max,max+upvar,max-downvar) << ")" << endl;
  float pred=predicted[central][data][icut];
  float sysP=abs(predicted[up][data][icut]-pred);
  float sysN=abs(pred-predicted[down][data][icut]);
  sysN = sysN + pred*(1-fitresultconstdata);//fitresultconst comes from the fit in the 0-30 GeV range!
  dout << " Predicted: " << pred << "+" << statErrorP(pred) << "-" << statErrorN(pred) << " (stat) +" << sysP << " - " << sysN << " (sys) " << "           using Poisson: " << print_range(max,max+upvar,max-downvar) << ")" << endl;
  //0.2*zjetsestimate[central][data][icut] << "-" << 0.4*zjetsestimate[central][data][icut] << " (sys) " << "           using Poisson: " << print_range(max,max+upvar,max-downvar) << ")" << endl;
  dout << "       Details: ZJetsEstimate= " << zjetsestimate[central][data][icut] << ", TTbar estimate=" << ttbarestimate[central][data][0][icut]-ttbarestimate[central][data][1][icut] << " [" << ttbarestimate[central][data][0][icut] << " R -" << ttbarestimate[central][data][1][icut] << " L ]" << endl;
  dout << "   For JZB>" << jzbcuts[icut] << " Observed: " << observed[central][data][icut] << endl;
  dout << "TABLE:" << endl;
  make_table(eemmmumu,icut,jzbcuts[icut]);
  dout << "MC: " << endl;
  printtitle.str("");
  printtitle<<"JZB>"<<jzbcuts[icut]<<" (MC)";
  ComputePoissonError(zjetsestimate[central][mc][icut],ttbarestimate[central][mc][0][icut],ttbarestimate[central][mc][1][icut],max,downvar,upvar,printtitle.str());
  pred=predicted[central][mc][icut];
  sysP=abs(predicted[up][mc][icut]-pred);
  sysN=abs(pred-predicted[down][mc][icut]);
  sysN = sysN + pred*(1-fitresultconstmc);//fitresultconst comes from the fit in the 0-30 GeV range!
  dout << " Predicted: " << pred << "+" << statErrorP(pred) << "-" << statErrorN(pred) << " (stat) +" << sysP << " - " << sysN << " (sys) " << "           using Poisson: " << print_range(max,max+upvar,max-downvar) << ")" << endl;
//  dout << " Predicted: " << print_range(pred,statErrorP(pred),statErrorN(pred)) << " (stat) +" << sysP << " - " << sysN << " (sys) " << "           using Poisson: " << print_range(max,max+upvar,max-downvar) << ")" << endl;
  //dout << " Predicted: " << print_range(predicted[central][mc][icut],predicted[down][mc][icut],predicted[up][mc][icut]) << " (stat) +" << 0.2*zjetsestimate[central][mc][icut] << "-" << 0.4*zjetsestimate[central][mc][icut] << " (sys) " << "           using Poisson: " << print_range(max,max+upvar,max-downvar) << ")" << endl;
  //print_range(predicted[central][mc][icut],predicted[central][mc][icut]+upvar,predicted[central][mc][icut]-downvar) << ")" << endl;
  dout << "       Details: ZJetsEstimate= " << zjetsestimate[central][mc][icut] << ", TTbar estimate=" << ttbarestimate[central][mc][0][icut]-ttbarestimate[central][mc][1][icut] << " [" << ttbarestimate[central][mc][0][icut] << " R -" << ttbarestimate[central][mc][1][icut] << " L ] )" << endl;
  dout << "   For JZB>" << jzbcuts[icut] << " Observed: " << observed[central][mc][icut] << endl;
  dout << endl << endl; 
}

void calculate_predicted_and_observed_eemm(float MCPeak,float MCPeakError,float DataPeak,float DataPeakError,vector<float> jzbcuts) { // DO NOT CHANGE THIS TO ... &jzbcuts !!! we add one!
  jzbcuts.push_back(14000);
  
  /*
   * We want the following numbers: For all JZB cuts, we want: ee,mm,em; observed, predicted. we want final results with errors from the peak and statistical error.
   * How to accomplish this; we draw histos for MC&data, once with the peak (to extract obs/pred), once with peak+sigma, once with peak-sigma. this gives us the peak error.
   * Statistical error: Comes from Poissonian errors ... for this we have a special little macro that will give us the value for three given parameters (cool, right?)
   */
  
  string jzbpeak[3][2];
  jzbpeak[central][data]=give_jzb_expression(DataPeak,data);
  jzbpeak[up][data]=give_jzb_expression(DataPeak+DataPeakError,data);
  jzbpeak[down][data]=give_jzb_expression(DataPeak-DataPeakError,data);
  jzbpeak[central][mc]=give_jzb_expression(MCPeak,mc);
  jzbpeak[up][mc]=give_jzb_expression(MCPeak+MCPeakError,mc);
  jzbpeak[down][mc]=give_jzb_expression(MCPeak-MCPeakError,mc);
  
  TH1F *RcorrJZBeemmop[3][2];
  TH1F *LcorrJZBeemmop[3][2];
  TH1F *RcorrJZBeeop[3][2];
  TH1F *LcorrJZBeeop[3][2];
  TH1F *RcorrJZBmmop[3][2];
  TH1F *LcorrJZBmmop[3][2];
  TH1F *RcorrJZBemop[3][2];
  TH1F *LcorrJZBemop[3][2];
  float zjetsestimate[3][2][20];
  float predicted[3][2][20];
  float observed[3][2][20];
  float ttbarestimate[3][2][2][20];
  float eemmmumu[2][3][20];
  
  
  for(int isdata=0;isdata<=1;isdata++) {
    for(int centraldownup=0;centraldownup<=2;centraldownup++) {
      RcorrJZBeemmop[centraldownup][isdata] = allsamples.Draw(give_histo_number("RcorrJZBeemmop",isdata,centraldownup),jzbpeak[centraldownup][isdata].c_str(),jzbcuts, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,isdata, luminosity);
      LcorrJZBeemmop[centraldownup][isdata] = allsamples.Draw(give_histo_number("LcorrJZBeemmop",isdata,centraldownup),("-"+jzbpeak[centraldownup][isdata]).c_str(),jzbcuts, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,isdata, luminosity);
      
      RcorrJZBemop[centraldownup][isdata]   = allsamples.Draw(give_histo_number("RcorrJZBemop",isdata,centraldownup),jzbpeak[centraldownup][isdata].c_str(),jzbcuts, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,isdata, luminosity);
      LcorrJZBemop[centraldownup][isdata]   = allsamples.Draw(give_histo_number("LcorrJZBemop",isdata,centraldownup),("-"+jzbpeak[centraldownup][isdata]).c_str(),jzbcuts, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,isdata,luminosity);
      RcorrJZBemop[centraldownup][isdata]->Add(allsamples.Draw(give_histo_number("RcorrJZBemopSB",isdata,centraldownup),jzbpeak[centraldownup][isdata].c_str(),jzbcuts, "JZB [GeV]", "events", sidebandcut&&cutOSOF&&cutnJets,isdata, luminosity));
      LcorrJZBemop[centraldownup][isdata]->Add(allsamples.Draw(give_histo_number("LcorrJZBemopSB",isdata,centraldownup),("-"+jzbpeak[centraldownup][isdata]).c_str(),jzbcuts, "JZB [GeV]", "events", sidebandcut&&cutOSOF&&cutnJets,isdata,luminosity));
      RcorrJZBemop[centraldownup][isdata]->Add(allsamples.Draw(give_histo_number("RcorrJZBeemmopSB",isdata,centraldownup),jzbpeak[centraldownup][isdata].c_str(),jzbcuts, "JZB [GeV]", "events", sidebandcut&&cutOSSF&&cutnJets,isdata, luminosity));
      LcorrJZBemop[centraldownup][isdata]->Add(allsamples.Draw(give_histo_number("LcorrJZBeemmopSB",isdata,centraldownup),("-"+jzbpeak[centraldownup][isdata]).c_str(),jzbcuts, "JZB [GeV]", "events", sidebandcut&&cutOSSF&&cutnJets,isdata,luminosity));
      LcorrJZBemop[centraldownup][isdata]->Scale(1.0/3);
      RcorrJZBemop[centraldownup][isdata]->Scale(1.0/3);
      RcorrJZBeeop[centraldownup][isdata] = allsamples.Draw(give_histo_number("RcorrJZBeeop",isdata,centraldownup),jzbpeak[centraldownup][isdata].c_str(),jzbcuts, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets&&"id1==0",isdata, luminosity);
      LcorrJZBeeop[centraldownup][isdata] = allsamples.Draw(give_histo_number("LcorrJZBeeop",isdata,centraldownup),("-"+jzbpeak[centraldownup][isdata]).c_str(),jzbcuts, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets&&"id1==0",isdata, luminosity);
      RcorrJZBmmop[centraldownup][isdata] = allsamples.Draw(give_histo_number("RcorrJZBmmop",isdata,centraldownup),jzbpeak[centraldownup][isdata].c_str(),jzbcuts, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets&&"id1==1",isdata, luminosity);
      LcorrJZBmmop[centraldownup][isdata] = allsamples.Draw(give_histo_number("LcorrJZBmmop",isdata,centraldownup),("-"+jzbpeak[centraldownup][isdata]).c_str(),jzbcuts, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets&&"id1==1",isdata, luminosity);
      for(int icut=0;icut<=jzbcuts.size();icut++) {
	zjetsestimate[centraldownup][isdata][icut]=0;
	predicted[centraldownup][isdata][icut]=0;
	observed[centraldownup][isdata][icut]=0;
	ttbarestimate[centraldownup][isdata][0][icut]=0;
	ttbarestimate[centraldownup][isdata][1][icut]=0;
      }
    }//end of for central,up,down loop
    crunch_the_numbers(RcorrJZBeemmop,RcorrJZBeeop,RcorrJZBmmop,LcorrJZBeemmop,LcorrJZBeeop,LcorrJZBmmop,RcorrJZBemop,LcorrJZBemop,zjetsestimate,ttbarestimate,predicted,observed,isdata,jzbcuts,eemmmumu);
    
//    void crunch_the_numbers(TH1F *RcorrJZBeemmop[3][2],TH1F *RcorrJZBeeop[3][2],TH1F *RcorrJZBmmop[3][2],TH1F *LcorrJZBeemmop[3][2],TH1F *LcorrJZBeeop[3][2],TH1F *LcorrJZBmmop[3][2],TH1F *RcorrJZBemop[3][2],TH1F *LcorrJZBemop[3][2],float zjetsestimate[3][2][20],float ttbarestimate[3][2][2][20],float predicted[3][2][20],float observed[3][2][20],int isdata,vector<float> jzbcuts,float eemmmumu[2][3]) {

  }//end of is data loop


dout << "We obtain the following results: " << endl;
for(int icut=0;icut<jzbcuts.size()-1;icut++) {
  present_result(jzbcuts,predicted,observed,zjetsestimate,ttbarestimate,icut,eemmmumu);
}


}
 












void alpha_scan_susy_space(string mcjzb, string datajzb) {
  TCanvas *c3 = new TCanvas("c3","c3");
  vector<float> binning;
  binning=allsamples.get_optimal_binsize(mcjzb,cutmass&&cutOSSF&&cutnJets,20,50,800);
  float arrbinning[binning.size()];
  for(int i=0;i<binning.size();i++) arrbinning[i]=binning[i];
  TH1F *puredata   = allsamples.Draw("puredata",  datajzb,binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,luminosity);
  puredata->SetMarkerSize(DataMarkerSize);
  TH1F *allbgs   = allsamples.Draw("allbgs",  "-"+datajzb,binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,luminosity);
  TH1F *allbgsb   = allsamples.Draw("allbgsb",  "-"+datajzb,binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data,luminosity);
  TH1F *allbgsc   = allsamples.Draw("allbgsc",  datajzb,binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data,luminosity);
  allbgs->Add(allbgsb,-1);
  allbgs->Add(allbgsc);
  int ndata=puredata->Integral();
  ofstream myfile;
  myfile.open ("susyscan_log.txt");
  TFile *susyscanfile = new TFile("/scratch/fronga/SMS/T5z_SqSqToQZQZ_38xFall10.root");
  TTree *suevents = (TTree*)susyscanfile->Get("events");
  TH2F *exclusionmap = new TH2F("exclusionmap","",20,0,500,20,0,1000);
  TH2F *exclusionmap1s = new TH2F("exclusionmap1s","",20,0,500,20,0,1000);
  TH2F *exclusionmap2s = new TH2F("exclusionmap2s","",20,0,500,20,0,1000);
  TH2F *exclusionmap3s = new TH2F("exclusionmap3s","",20,0,500,20,0,1000);
  
  susy_scan_axis_labeling(exclusionmap);
  susy_scan_axis_labeling(exclusionmap1s);
  susy_scan_axis_labeling(exclusionmap2s);
  susy_scan_axis_labeling(exclusionmap3s);
  
  Int_t MyPalette[100];
  Double_t r[]    = {0., 0.0, 1.0, 1.0, 1.0};
  Double_t g[]    = {0., 0.0, 0.0, 1.0, 1.0};
  Double_t b[]    = {0., 1.0, 0.0, 0.0, 1.0};
  Double_t stop[] = {0., .25, .50, .75, 1.0};
  Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 100);
  for (int i=0;i<100;i++) MyPalette[i] = FI+i;
   
  gStyle->SetPalette(100, MyPalette);
  
  for(int m23=50;m23<500;m23+=25) {
    for (int m0=(2*(m23-50)+150);m0<=1000;m0+=50)
    {
      c3->cd();
      stringstream drawcondition;
      drawcondition << "pfJetGoodNum>=3&&(TMath::Abs(masses[0]-"<<m0<<")<10&&TMath::Abs(masses[2]-masses[3]-"<<m23<<")<10)&&mll>5&&id1==id2";
      TH1F *puresignal = new TH1F("puresignal","puresignal",binning.size()-1,arrbinning);
      TH1F *puresignall= new TH1F("puresignall","puresignal",binning.size()-1,arrbinning);
      stringstream drawvar,drawvar2;
      drawvar<<mcjzb<<">>puresignal";
      drawvar2<<"-"<<mcjzb<<">>puresignall";
      suevents->Draw(drawvar.str().c_str(),drawcondition.str().c_str());
      suevents->Draw(drawvar2.str().c_str(),drawcondition.str().c_str());
      if(puresignal->Integral()<60) {
	delete puresignal;
	continue;
      }
      puresignal->Add(puresignall,-1);//we need to correct for the signal contamination - we effectively only see (JZB>0)-(JZB<0) !!
      puresignal->Scale(ndata/(20*puresignal->Integral()));//normalizing it to 5% of the data
      stringstream saveas;
      saveas<<"Model_Scan/m0_"<<m0<<"__m23_"<<m23;
      dout << "PLEASE KEEP IN MIND THAT SIGNAL CONTAMINATION IS NOT REALLY TAKEN CARE OF YET DUE TO LOW STATISTICS! SHOULD BE SOMETHING LIKE THIS : "<< endl;
//        TH1F *signalpredlo   = allsamples.Draw("signalpredlo",  "-"+mcjzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,mc,  luminosity,allsamples.FindSample("LM4"));
//        TH1F *signalpredro   = allsamples.Draw("signalpredro",      mcjzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,mc,  luminosity,allsamples.FindSample("LM4"));
//        TH1F *puredata       = allsamples.Draw("puredata",          datajzb,binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data,luminosity);
//        signalpred->Add(signalpredlo,-1);
//        signalpred->Add(signalpredro);
//        puresignal->Add(signalpred,-1);//subtracting signal contamination
//---------------------
//      dout << "(m0,m23)=("<<m0<<","<<m23<<") contains " << puresignal->Integral() << endl;
//    TH1F *puresignal = allsamples.Draw("puresignal",mcjzb,  binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,mc,  luminosity,allsamples.FindSample("LM4"));
      vector<float> results=establish_upper_limits(puredata,allbgs,puresignal,saveas.str(),myfile);  
      if(results.size()==0) {
	delete puresignal;
	continue;
      }
      exclusionmap->Fill(m23,m0,results[0]);
      exclusionmap1s->Fill(m23,m0,results[1]);
      exclusionmap2s->Fill(m23,m0,results[2]);
      exclusionmap3s->Fill(m23,m0,results[3]);
      delete puresignal;
      dout << "(m0,m23)=("<<m0<<","<<m23<<") : 3 sigma at " << results[3] << endl;
    }
  }//end of model scan for loop
  
  dout << "Exclusion Map contains" << exclusionmap->Integral() << " (integral) and entries: " << exclusionmap->GetEntries() << endl;
  c3->cd();
  exclusionmap->Draw("CONTZ");
  CompleteSave(c3,"Model_Scan/CONT/Model_Scan_Mean_values");
  exclusionmap->Draw("COLZ");
  CompleteSave(c3,"Model_Scan/COL/Model_Scan_Mean_values");
  
  exclusionmap1s->Draw("CONTZ");
  CompleteSave(c3,"Model_Scan/CONT/Model_Scan_1sigma_values");
  exclusionmap1s->Draw("COLZ");
  CompleteSave(c3,"Model_Scan/COL/Model_Scan_1sigma_values");
  
  exclusionmap2s->Draw("CONTZ");
  CompleteSave(c3,"Model_Scan/CONT/Model_Scan_2sigma_values");
  exclusionmap2s->Draw("COLZ");
  CompleteSave(c3,"Model_Scan/COL/Model_Scan_2sigma_values");
  
  exclusionmap3s->Draw("CONTZ");
  CompleteSave(c3,"Model_Scan/CONT/Model_Scan_3sigma_values");
  exclusionmap3s->Draw("COLZ");
  CompleteSave(c3,"Model_Scan/COL/Model_Scan_3sigma_values");
  
  TFile *exclusion_limits = new TFile("exclusion_limits.root","RECREATE");
  exclusionmap->Write();
  exclusionmap1s->Write();
  exclusionmap2s->Write();
  exclusionmap3s->Write();
  exclusion_limits->Close();
  susyscanfile->Close();
  
  myfile.close();
}




//-------------------------------------------------------------

/*


not in use anymore: 

-------------------------------------<Spring11 - used up to Jul 14>----------------------------------------------

//    allsamples.AddSample("/scratch/buchmann/AllData_Jun10___DCS_648pb_MoreTriggers4_DCS.root","Data",1,1,true,false,0,kBlack);
//    allsamples.AddSample("/scratch/buchmann/AllData_Jun10___486pb_MoreTriggers3.root","Data",1,1,true,false,0,kBlack);
    //allsamples.AddSample("/scratch/buchmann/AllData_Jun17_160404-166861_real_complete_2.root","Data",1,1,true,false,0,kBlack);
    allsamples.AddSample("/scratch/buchmann/ntuples2011/data/AllData_Jul6_1078pb.root","Data",1,1,true,false,0,kBlack);
    
//    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/TTJets_TuneZ2_7TeV-madgraph-tauola.root","t#bar{t}",totEventsTTbar,TTbarCrossSection,false,false,1,ttbar_color);//spring11
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TTJets_TuneZ2_7TeV-madgraph-tauola_Summer11_3.root","TTbar",totEventsTTbar,TTbarCrossSection,false,false,1,ttbar_color);//summer11
//    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola.root","WJets",totEventsWJets,WJetsCrossSection,false,false,3,wjets_color); //spring11
    allsamples.AddSample("/scratch/fronga/MC_v1.29/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola.root","WJets",totEventsWJets,WJetsCrossSection,false,false,3,wjets_color);//summer
    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/VVJetsTo4L_TuneD6T_7TeV-madgraph-tauola.root","DiBosons",totEventsVVJets,VVJetsCrossSection,false,false,4,diboson_color);
    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/TToBLNu_TuneZ2_s-channel_7TeV-madgraph_2.root","SingleTop",totEventsSingleTopS,SingleTopSCrossSection,false,false,2,singletop_color);
    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/TToBLNu_TuneZ2_t-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopT,SingleTopTCrossSection,false,false,2,singletop_color);
    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/TToBLNu_TuneZ2_tW-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopU,SingleTopUCrossSection,false,false,2,singletop_color);
    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/ZinvisibleJets_7TeV-madgraph.root","Z nunu",totEventsZnunu,ZnunuCrossSection,false,false,4,diboson_color);
//    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/QCD_TuneD6T_HT-100To250_7TeV-madgraph.root","QCD",totEventsQCD100to250,QCD100to250CrossSection,false,false,5,qcd_color);
//    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/QCD_TuneD6T_HT-250To500_7TeV-madgraph.root","QCD",totEventsQCD250to500,QCD250to500CrossSection,false,false,5,qcd_color);
//    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/QCD_TuneD6T_HT-500To1000_7TeV-madgraph.root","QCD",totEventsQCD500to1000,QCD500to1000CrossSection,false,false,5,qcd_color);
//    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/QCD_TuneD6T_HT-1000_7TeV-madgraph.root","QCD",totEventsQCD1000toInf,QCD1000toInfCrossSection,false,false,5,qcd_color);
//    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola.root","ZJets",totEventsZjets,ZJetsCrossSection,false,false,6,dy_color);//spring11
    allsamples.AddSample("/scratch/fronga/MC_v1.29/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola_Summer11.root","Z+Jets",totEventsZjets,ZJetsCrossSection,false,false,6,dy_color);//summer11
    allsamples.AddSample("/scratch/buchmann/MCSpring2011PU__inclPFvarss__PURW_160404_166861/LM4_SUSY_sftsht_7TeV-pythia6.root","LM4",218536,1.879,false,true,7,lm_color);

-------------------------------------</Spring11 - used up to Jul 14>----------------------------------------------




  
  //how this works: AddSample("FilePath,"Sample name",Nevents,XS,is_data,is_signal,groupindex
  allsamples.AddSample("/scratch/buchmann/AllDataNov4.root","Data",1,1,true,false,0,kBlack);
  allsamples.AddSample("/scratch/buchmann/CutAnalysis/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola.root","WJets to LNu",14350756,31314,false,false,2,kGreen);
//  allsamples.AddSample("/scratch/buchmann/CutAnalysis/DYJetsToLL_TuneD6T_M-10To50_7TeV-madgraph-tauola.root","DY,M10To50",186891,400,false,false,3);
  allsamples.AddSample("/scratch/buchmann/CutAnalysis/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola.root","DY,M50",2552567,3048,false,false,3,kYellow);
  allsamples.AddSample("/scratch/buchmann/CutAnalysis/TTJets_TuneZ2_7TeV-madgraph-tauola.root","TTJets",1144028,157.5,false,false,4,kMagenta-1);
  allsamples.AddSample("/scratch/buchmann/CutAnalysis/TToBLNu_TuneZ2_s-channel_7TeV-madgraph.root","T to blnu,s",489472,1.49,false,false,5,kBlue);
  allsamples.AddSample("/scratch/buchmann/CutAnalysis/TToBLNu_TuneZ2_t-channel_7TeV-madgraph.root","T to blnu,t",477610,20.54,false,false,5,kBlue);
  allsamples.AddSample("/scratch/buchmann/CutAnalysis/TToBLNu_TuneZ2_tW-channel_7TeV-madgraph.root","T to blnu,u",477599,10.6,false,false,5,kBlue);
  allsamples.AddSample("/scratch/buchmann/CutAnalysis/VVJetsTo4L_TuneD6T_7TeV-madgraph-tauola.root","VVJets to 4L",509072,4.8,false,false,1,kGreen);
  allsamples.AddSample("/scratch/buchmann/CutAnalysis/LM4_SUSY_sftsht_7TeV-pythia6.root","20xLM4",218536,1.879*100,false,true,6,kMagenta);

  
  
    /*
    Int_t fermiwjets = TColor::GetColor("#75f075");
    Int_t ferminonW  = TColor::GetColor("#7d99d1");
    Int_t fermiwwwz  = TColor::GetColor("#2b26ac");
    Int_t fermizjets = TColor::GetColor("#0044ff");
    Int_t fermitop   = TColor::GetColor("#353535");
    Int_t fermihiggs = TColor::GetColor("#b30000");
    
    
    dy_color 		= fermiwjets;
    wjets_color 	= ferminonW;
    ttbar_color 	= fermizjets;
    singletop_color	= fermitop;
    qcd_color		= nice_orange;
    diboson_color	= fermiwwwz;
    lm_color		= fermihiggs;
    */
    
    
    /*
    Int_t dy_color        = TColor::GetColor("#ff1300");
    Int_t wjets_color     = TColor::GetColor("#ff7a00");
    Int_t ttbar_color     = TColor::GetColor("#03899c");
    Int_t singletop_color = TColor::GetColor("#00c322");
    Int_t qcd_color       = TColor::GetColor("#4a11ae");
    Int_t diboson_color   = TColor::GetColor("#8f04a8");
    Int_t lm_color        = TColor::GetColor("#dcf900");

    //fugly set of colors:
    dy_color        = TColor::GetColor("#ff1300");
    wjets_color     = TColor::GetColor("#ff7a00");
    ttbar_color     = TColor::GetColor("#03899c");
    singletop_color = TColor::GetColor("#00c322");
    qcd_color       = TColor::GetColor("#4a11ae");
    diboson_color   = TColor::GetColor("#8f04a8");
    lm_color        = TColor::GetColor("#dcf900");

    //twilight
    dy_color        = TColor::GetColor("#141414");
    wjets_color     = TColor::GetColor("#f8f8f8");
    ttbar_color     = TColor::GetColor("#cf6a4c");
    singletop_color = TColor::GetColor("#cda869");
    qcd_color       = TColor::GetColor("#e9c062");
    diboson_color   = TColor::GetColor("#5f5a60");
    lm_color        = TColor::GetColor("#7587a6");
    //http://www.colorschemer.com/schemes/viewscheme.php?id=9126
    
    */

/*      //how this works: AddSample("FilePath,"Sample name",Nevents,XS,is_data,is_signal,groupindex
    allsamples.AddSample("/scratch/fronga/JSON_160404-163869_v1.29/AllCertified191_v1.29.root","Data",1,1,true,false,0,kBlack);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola.root","ZJets",totEventsZjets,ZJetsCrossSection,false,false,3,kYellow);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola.root","WJets",totEventsWJets,WJetsCrossSection,false,false,2,kGray);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TTJets_TuneZ2_7TeV-madgraph-tauola.root","t#bar{t}",totEventsTTbar,TTbarCrossSection,false,false,4,kMagenta-1);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TToBLNu_TuneZ2_s-channel_7TeV-madgraph_2.root","SingleTop",totEventsSingleTopS,SingleTopSCrossSection,false,false,5,kBlue);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TToBLNu_TuneZ2_t-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopT,SingleTopTCrossSection,false,false,5,kBlue);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TToBLNu_TuneZ2_tW-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopU,SingleTopUCrossSection,false,false,5,kBlue);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/VVJetsTo4L_TuneD6T_7TeV-madgraph-tauola.root","Dibosons",totEventsVVJets,VVJetsCrossSection,false,false,1,kGreen);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/ZinvisibleJets_7TeV-madgraph.root","Z nunu",totEventsZnunu,ZnunuCrossSection,false,false,1,kGreen);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-100To250_7TeV-madgraph.root","QCD",totEventsQCD100to250,QCD100to250CrossSection,false,false,6,kRed);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-250To500_7TeV-madgraph.root","QCD",totEventsQCD250to500,QCD250to500CrossSection,false,false,6,kRed);
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-500To1000_7TeV-madgraph.root","QCD",totEventsQCD500to1000,QCD500to1000CrossSection,false,false,6,kRed);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-1000_7TeV-madgraph.root","QCD",totEventsQCD1000toInf,QCD1000toInfCrossSection,false,false,6,kRed);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/LM4_SUSY_sftsht_7TeV-pythia6.root","20xLM4",218536,1.879*20,false,true,7,kMagenta);
*/  

    //how this works: AddSample("FilePath,"Sample name",Nevents,XS,is_data,is_signal,groupindex
//    allsamples.AddSample("/scratch/fronga/JSON_160404-163869_v1.29/AllCertified191_v1.29.root","Data",1,1,true,false,0,kBlack);
//    allsamples.AddSample("/scratch/buchmann/May2011ReRecoAllMayJSON.root","Data",1,1,true,false,0,kBlack);

/*
    allsamples.AddSample("/scratch/buchmann/DataMay2011ReReco_CertifiedReRecoJSON.root","Data",1,1,true,false,0,kBlack);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TTJets_TuneZ2_7TeV-madgraph-tauola.root","t#bar{t}",totEventsTTbar,TTbarCrossSection,false,false,1,ttbar_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola.root","WJets",totEventsWJets,WJetsCrossSection,false,false,3,wjets_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/VVJetsTo4L_TuneD6T_7TeV-madgraph-tauola.root","Dibosons",totEventsVVJets,VVJetsCrossSection,false,false,4,diboson_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TToBLNu_TuneZ2_s-channel_7TeV-madgraph_2.root","SingleTop",totEventsSingleTopS,SingleTopSCrossSection,false,false,2,singletop_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TToBLNu_TuneZ2_t-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopT,SingleTopTCrossSection,false,false,2,singletop_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/TToBLNu_TuneZ2_tW-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopU,SingleTopUCrossSection,false,false,2,singletop_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/ZinvisibleJets_7TeV-madgraph.root","Z nunu",totEventsZnunu,ZnunuCrossSection,false,false,4,diboson_color);
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-100To250_7TeV-madgraph.root","QCD",totEventsQCD100to250,QCD100to250CrossSection,false,false,5,qcd_color);
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-250To500_7TeV-madgraph.root","QCD",totEventsQCD250to500,QCD250to500CrossSection,false,false,5,qcd_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-500To1000_7TeV-madgraph.root","QCD",totEventsQCD500to1000,QCD500to1000CrossSection,false,false,5,qcd_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-1000_7TeV-madgraph.root","QCD",totEventsQCD1000toInf,QCD1000toInfCrossSection,false,false,5,qcd_color);
    allsamples.AddSample("/scratch/fronga/MC_v1.29/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola.root","ZJets",totEventsZjets,ZJetsCrossSection,false,false,6,dy_color);
    
    
    allsamples.AddSample("/scratch/fronga/MC_v1.29/LM4_SUSY_sftsht_7TeV-pythia6.root","20xLM4",218536,1.879*20,false,true,7,lm_color);
    
    
    
    
    
    
    
    //    allsamples.AddSample("/scratch/buchmann/DataMay2011ReReco_CertifiedReRecoJSON.root","Data",1,1,true,false,0,kBlack);
//    allsamples.AddSample("/scratch/buchmann/AllDataJun3_again.root","Data",1,1,true,false,0,kBlack);
    allsamples.AddSample("/scratch/buchmann/AllData_Jun10___486pb_merged.root","Data",1,1,true,false,0,kBlack);
    allsamples.AddSample("/scratch/buchmann/MC_Spring11_PU_PF/TTJets_TuneZ2_7TeV-madgraph-tauola.root","t#bar{t}",totEventsTTbar,TTbarCrossSection,false,false,1,ttbar_color);
    allsamples.AddSample("/scratch/buchmann/MC_Spring11_PU_PF/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola.root","WJets",totEventsWJets,WJetsCrossSection,false,false,3,wjets_color);
    allsamples.AddSample("/scratch/buchmann/MC_Spring11_PU_PF/VVJetsTo4L_TuneD6T_7TeV-madgraph-tauola.root","Dibosons",totEventsVVJets,VVJetsCrossSection,false,false,4,diboson_color);
    allsamples.AddSample("/scratch/buchmann/MC_Spring11_PU_PF/TToBLNu_TuneZ2_s-channel_7TeV-madgraph_2.root","SingleTop",totEventsSingleTopS,SingleTopSCrossSection,false,false,2,singletop_color);
    allsamples.AddSample("/scratch/buchmann/MC_Spring11_PU_PF/TToBLNu_TuneZ2_t-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopT,SingleTopTCrossSection,false,false,2,singletop_color);
    allsamples.AddSample("/scratch/buchmann/MC_Spring11_PU_PF/TToBLNu_TuneZ2_tW-channel_7TeV-madgraph.root","SingleTop",totEventsSingleTopU,SingleTopUCrossSection,false,false,2,singletop_color);
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/ZinvisibleJets_7TeV-madgraph.root","Z nunu",totEventsZnunu,ZnunuCrossSection,false,false,4,diboson_color);
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-100To250_7TeV-madgraph.root","QCD",totEventsQCD100to250,QCD100to250CrossSection,false,false,5,qcd_color);
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-250To500_7TeV-madgraph.root","QCD",totEventsQCD250to500,QCD250to500CrossSection,false,false,5,qcd_color);
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-500To1000_7TeV-madgraph.root","QCD",totEventsQCD500to1000,QCD500to1000CrossSection,false,false,5,qcd_color);
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/QCD_TuneD6T_HT-1000_7TeV-madgraph.root","QCD",totEventsQCD1000toInf,QCD1000toInfCrossSection,false,false,5,qcd_color);
    allsamples.AddSample("/scratch/buchmann/MC_Spring11_PU_PF/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola.root","ZJets",totEventsZjets,ZJetsCrossSection,false,false,6,dy_color);
    
    
//    allsamples.AddSample("/scratch/fronga/MC_v1.29/LM4_SUSY_sftsht_7TeV-pythia6.root","20xLM4",218536,1.879*20,false,true,7,lm_color);
    allsamples.AddSample("/scratch/buchmann/LM4_SUSY_sftsht_7TeV-pythia.root","20xLM4",218536,1.879*20,false,true,7,lm_color);

    
    
    
    
    
    
    
    
    
    
    
    
    
    
*/
