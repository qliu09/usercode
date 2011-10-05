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
#include <TLegendEntry.h>

//#include "TTbar_stuff.C"
using namespace std;

using namespace PlottingSetup;

void todo() {
  dout << "My to do list: " << endl;
  dout << "  - ExperimentalModule::Poisson_ratio_plot : Get the second part to work!" << endl;
  dout << "  - Exclusion plots (SMS done, mSUGRA still to be completed!)" << endl;
  dout << "  - Nicer ratio plots (separate pad)" << endl;
}  
  
void find_peaks(float &MCPeak,float &MCPeakError, float &DataPeak, float &DataPeakError, float &MCSigma, float &DataSigma, stringstream &result)
{
  TCanvas *tempcan = new TCanvas("tempcan","Temporary canvas for peak finding preparations");
  TH1F *rawJZBeemmMC      = allsamples.Draw("rawJZBeemmMC",jzbvariablemc,100,-50,50, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,mc, luminosity);
  TH1F *rawJZBeemmData    = allsamples.Draw("rawJZBeemmData",jzbvariabledata,100, -50,50, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data, luminosity);
  TH1F *rawJZBemMC      = allsamples.Draw("rawJZBemMC",jzbvariablemc,100,-50,50, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,mc, luminosity);
  TH1F *rawJZBemData    = allsamples.Draw("rawJZBemData",jzbvariabledata,100, -50,50, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data, luminosity);
  TH1F *rawttbarjzbeemmMC;

  if(method==doKM) {
    //we only need this histo for the KM fitting...
    rawttbarjzbeemmMC = allsamples.Draw("rawttbarjzbeemmMC",jzbvariablemc,100, -50,50, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets,mc,luminosity,allsamples.FindSample("TTJet"));
    MCPeak=find_peak(rawJZBeemmMC, rawttbarjzbeemmMC, -40, 40, mc, MCPeakError,MCSigma,method);
    DataPeak=find_peak(rawJZBeemmData, rawJZBeemmData, -40, 40, data, DataPeakError,DataSigma,method);
  }
  else {
    TH1F *reducedMC = (TH1F*)rawJZBeemmMC->Clone();
    TH1F *reducedData = (TH1F*)rawJZBeemmData->Clone();
    reducedMC->Add(rawJZBemMC,-1);
    reducedData->Add(rawJZBemData,-1);
    //this is Kostas' way of doing it - we subtract em to get rid of some of the ttbar contribution (in reality, of flavor-symmetric contribution)
    MCPeak=find_peak(reducedMC, rawttbarjzbeemmMC, -40, 40, mc, MCPeakError,MCSigma,method);
    DataPeak=find_peak(reducedData, rawJZBeemmData, -40, 40, data, DataPeakError,DataSigma,method);
    
  }
  
    
//  MCPeak=find_peak(rawJZBeemmMC, rawttbarjzbeemmMC, -40, 40, mc, MCPeakError,MCSigma,method);
//  DataPeak=find_peak(rawJZBeemmData, rawJZBeemmData, -40, 40, data, DataPeakError,DataSigma,method);
  dout << "We have found the peak in Data at " << DataPeak << " +/- " << DataPeakError << " with sigma=" << DataSigma << " +/- ?? (not impl.)" << endl;
  result << "We have found the peak in Data at " << DataPeak << " +/- " << DataPeakError << " with sigma=" << DataSigma << " +/- ?? (not impl.)" << endl;
  dout << "We have found the peak in MC at " << MCPeak << " +/- " << MCPeakError << " with sigma=" << MCSigma << " +/- ?? (not impl.)" << endl;
  result << "We have found the peak in MC at " << MCPeak << " +/- " << MCPeakError << " with sigma=" << MCSigma << " +/- ?? (not impl.)" << endl;
}

void make_special_mll_plot(int nbins, float min, float max, bool logscale,string xlabel) {
  
  TCanvas *ckin = new TCanvas("ckin","Kinematic Plots (in the making)",600,600);

  TH1F *datahistoOSSF = allsamples.Draw("datahistoOSSF","mll",nbins,min,max, xlabel, "events",cutOSSF&&cutnJets&&basiccut,data,luminosity);
  THStack mcstackOSSF = allsamples.DrawStack("mcstackOSSF","mll",nbins,min,max, xlabel, "events",cutOSSF&&cutnJets&&basiccut,mc,luminosity);
  TH1F *datahistoOSOF = allsamples.Draw("datahistoOSOF","mll",nbins,min,max, xlabel, "events",cutOSOF&&cutnJets&&basiccut,data,luminosity);
  
  if(logscale) ckin->SetLogy(1);
  datahistoOSSF->SetMarkerSize(DataMarkerSize);
  datahistoOSSF->GetXaxis()->SetTitle(xlabel.c_str());
  datahistoOSSF->GetXaxis()->CenterTitle();
  datahistoOSSF->GetYaxis()->SetTitle("events");
  datahistoOSSF->GetYaxis()->CenterTitle();
  datahistoOSOF->SetMarkerSize(DataMarkerSize);
  datahistoOSSF->SetMarkerSize(DataMarkerSize);
  datahistoOSSF->Draw();

  mcstackOSSF.Draw("same");
  datahistoOSSF->Draw("same");

  datahistoOSOF->SetMarkerColor(TColor::GetColor("#FE642E"));
  datahistoOSOF->SetLineColor(kRed);
  datahistoOSOF->SetMarkerStyle(21);
  datahistoOSOF->Draw("same");

  // Try to re-arrange legend...
  TLegend *bgleg = allsamples.allbglegend("",datahistoOSSF);
  TLegend *kinleg = make_legend();
  kinleg->AddEntry(datahistoOSSF,"SF (data)","p");
  kinleg->AddEntry(datahistoOSOF,"OF (data)","p");\
  TIter next(bgleg->GetListOfPrimitives());
  TObject* obj;
  // Copy the nice bkgd legend skipping the "data"
  while ( obj = next() ) 
    if ( strcmp(((TLegendEntry*)obj)->GetObject()->GetName(),"datahistoOSSF") )
      kinleg->GetListOfPrimitives()->Add(obj);

  kinleg->Draw();
  CompleteSave(ckin,"kin/mll_ossf_osof_distribution");
  
  delete datahistoOSOF;
  delete datahistoOSSF;
  delete ckin;
}
  

void draw_ratio_plot(TH1* hdata, THStack& hmc, float ymin=0.5, float ymax=1.5) {

  // Make a histogram from stack
  TIter next(hmc.GetHists());
  TObject* obj;
  TH1* hratio = NULL;
  while ( obj = next() ) {
    if ( !hratio ) {
      hratio = (TH1*)obj->Clone();
      hratio->SetName("hratio");
    } else hratio->Add( (TH1*)obj );
  }
  hratio->Divide(hdata);
  hratio->SetMaximum(ymax);
  hratio->SetMinimum(ymin);
  hratio->SetMarkerStyle(2);
  hratio->SetLineWidth(1);
  hratio->GetYaxis()->SetLabelSize(0.08);
  hratio->GetXaxis()->SetLabelSize(0.0);

  TPad* rpad = new TPad("rpad","",0.15,0.73,0.4,0.88);
  rpad->SetTopMargin(0.0);
  rpad->SetBottomMargin(0.0);
  rpad->SetRightMargin(0.0);
  rpad->Draw();
  rpad->cd();
//  hratio->GetXaxis()->SetNdivisions(0);
  hratio->GetYaxis()->SetNdivisions(502,false);
  hratio->Draw("e1x0");

  TF1* oneline = new TF1("","1.0",0,1000);
  oneline->SetLineColor(kBlue);
  oneline->SetLineStyle(1); 
  oneline->SetLineWidth(1);
  oneline->Draw("same"); 
}

float lastrange_min=0;
float lastrange_max=0;

void make_kin_plot(string variable, int nbins, float min, float max, bool logscale,
                   string xlabel, string filename, bool isPF=true, bool plotratio=false, bool loadlastminmax=false ) {
//  TCut basiccut("(pfJetGoodNum>=2&&pfJetGoodID[0])&&(pfJetGoodNum>=2&&pfJetGoodID[1])&&(passed_triggers||!is_data)");
  TCut ibasiccut=basiccut;
  
  if(isPF) ibasiccut=basiccut&&"pfjzb[0]>-998";
  //Step 1: Adapt the variable (if we're dealing with PF we need to adapt the variable!)
  if(isPF) {
    if(variable=="mll") variable="pfmll[0]";
    if(variable=="jetpt[0]") variable="pfJetGoodPt[0]";
    if(variable=="jeteta[0]") variable="pfJetGoodEta[0]";
    if(variable=="pt") variable="pfpt[0]";
    if(variable=="pt1") variable="pfpt1[0]";
    if(variable=="pt2") variable="pfpt2[0]";
    if(variable=="eta1") variable="pfeta1[0]";
    if(variable=="jzb[1]") variable="pfjzb[0]";
    //if(variable=="pfJetGoodNum") variable="pfJetGoodNum"; // pointless.
  }

  //Step 2: Refine the cut
  TCut cut;
  cut=cutmass&&cutOSSF&&cutnJets&&ibasiccut;
  if(filename=="nJets") cut=cutmass&&cutOSSF&&ibasiccut;
  if(filename=="nJets_nocuts_except_mll_ossf") cut=cutmass&&cutOSSF;
  if(filename=="mll") cut=cutOSSF&&cutnJets&&ibasiccut;
  if(filename=="mll_ee") cut=cutOSSF&&cutnJets&&ibasiccut&&"id1==0";
  if(filename=="mll_osof") cut=cutOSOF&&cutnJets&&ibasiccut;
  if(filename=="mll_mm") cut=cutOSSF&&cutnJets&&ibasiccut&&"id1==1";
  if(filename=="mll_inclusive"||filename=="mll_inclusive_highrange") cut=cutOSSF;
  if(filename=="mll_inclusive_osof") cut=cutOSOF;
  if(filename=="mll_inclusive_ee") cut=cutOSSF&&"id1==0";
  if(filename=="mll_inclusive_mm") cut=cutOSSF&&"id1==1";
  if(filename=="pfJetGoodEta_0") cut=cutOSSF&&cutmass&&ibasiccut&&cutnJets;
  if(filename=="pfJetGoodPt_0") cut=cutOSSF&&cutmass&&ibasiccut&&cutnJets;

  TCanvas *ckin = new TCanvas("ckin","Kinematic Plots (in the making)",600,600);
  ckin->SetLogy(logscale);
  TH1F *datahisto = allsamples.Draw("datahisto",variable,nbins,min,max, xlabel, "events",cut,data,luminosity);
  datahisto->SetMarkerSize(DataMarkerSize);
  THStack mcstack = allsamples.DrawStack("mcstack",variable,nbins,min,max, xlabel, "events",cut,mc,luminosity);
  if(variable=="pfJetGoodPt[0]") datahisto->SetMaximum(10*datahisto->GetMaximum());
  if(variable=="pt") datahisto->SetMaximum(10*datahisto->GetMaximum());
  if(filename=="mll_inclusive") datahisto->SetMinimum(1);
  if(filename=="mll_osof") datahisto->SetMaximum(10*datahisto->GetMaximum());
  if(filename=="mll_osof") datahisto->SetMinimum(9);

  datahisto->SetMaximum(5.3*datahisto->GetMaximum());
  datahisto->Draw("e1");
  ckin->Update();
  if(loadlastminmax) {
	datahisto->SetMinimum(lastrange_min);
	datahisto->SetMaximum(lastrange_max);
	if(logscale) {
		datahisto->SetMinimum(pow(10,lastrange_min));
		datahisto->SetMaximum(pow(10,lastrange_max));
	}		
  }
  lastrange_min=ckin->GetUymin();
  lastrange_max=ckin->GetUymax();

  mcstack.Draw("same");
  datahisto->Draw("same,e1");
  TLegend *kinleg = allsamples.allbglegend();
  kinleg->Draw();
  if(filename=="mll_osof") kinleg->SetHeader("OSOF");
  if(filename=="mll") kinleg->SetHeader("OSSF");
  TText* write_cut = write_cut_on_canvas(decipher_cut(cut,basicqualitycut));
  write_cut->Draw();
  TText* write_variable = write_text(0.99,0.01,variable);
  write_variable->SetTextAlign(31);
  write_variable->SetTextSize(0.02);

  if(isPF) CompleteSave(ckin,"kin/"+filename+"__PF");
  else CompleteSave(ckin,"kin/"+filename);

  if ( plotratio ) {
	TPad *kinpad = new TPad("kinpad","kinpad",0,0,1,1);
	kinpad->cd();
	kinpad->SetLogy(logscale);
	datahisto->Draw("e1");
	mcstack.Draw("same");
	datahisto->Draw("same,e1");
	datahisto->Draw("same,axis");
	kinleg->Draw();
	write_cut->Draw();
	DrawPrelim();
	TCanvas *with_ratio = draw_ratio_on_canvas(datahisto,mcstack,kinpad->cd());
	if(isPF) CompleteSave(with_ratio,"kin/"+filename+"__PF_withratio");
	else CompleteSave(with_ratio,"kin/"+filename+"_withratio");
	delete with_ratio;
  }
  datahisto->Delete();
  delete ckin;
}

void do_kinematic_plots(bool doPF=false)
{
  if(doPF) write_warning(__FUNCTION__,"Please use caution when trying to produce PF plots; not all versions of the JZB trees have these variables!");
  bool dolog=true;
  bool nolog=false;
  make_kin_plot("mll",21,55,160,dolog,"m_{ll} [GeV]","mll",doPF,true);
  make_kin_plot("mll",21,55,160,dolog,"m_{ll} [GeV]","mll_osof",doPF,true,true);
  make_kin_plot("mll",21,55,160,dolog,"m_{ll} [GeV]","mll_ee",doPF,true);
  make_kin_plot("mll",21,55,160,dolog,"m_{ll} [GeV]","mll_mm",doPF,true);
  make_kin_plot("mll",105,55,160,dolog,"m_{ll} [GeV]","mll_inclusive",doPF,true);
  make_kin_plot("mll",105,55,160,dolog,"m_{ll} [GeV]","mll_inclusive_ee",doPF,true);
  make_kin_plot("mll",105,55,160,dolog,"m_{ll} [GeV]","mll_inclusive_mm",doPF,true);
  make_kin_plot("mll",105,55,160,dolog,"m_{ll} [GeV]","mll_inclusive_osof",doPF,true);
  make_kin_plot("mll",305,55,350,dolog,"m_{ll} [GeV]","mll_inclusive_highrange",doPF);
  make_kin_plot("jetpt[0]",40,0,200,dolog,"leading jet p_{T} [GeV]","pfJetGoodPt_0",doPF);
  make_kin_plot("jeteta[0]",40,-5,5,nolog,"leading jet #eta","pfJetGoodEta_0",doPF);
  make_kin_plot("pt",50,0,200,dolog,"Z p_{T} [GeV]","Zpt",doPF);
  make_kin_plot("pt1",50,0,100,nolog,"p_{T} [GeV]","pt1",doPF);
  make_kin_plot("pt2",50,0,100,nolog,"p_{T} [GeV]","pt2",doPF);
  make_kin_plot("eta1",40,-5,5,nolog,"#eta_{l}","eta",doPF);
  make_kin_plot("jzb[1]",100,-150,150,dolog,"JZB [GeV]","jzb_ossf",doPF);
  make_kin_plot("pfJetGoodNum",8,0.5,8.5,dolog,"nJets","nJets",doPF);
  make_kin_plot("pfJetGoodNum",8,0.5,8.5,dolog,"nJets","nJets_nocuts_except_mll_ossf",doPF);
  if(!doPF) make_special_mll_plot(21,55,160,dolog,"m_{ll} [GeV]");
}

void make_comp_plot( string var, string xlabel, string filename, float jzbcut, string mcjzb, string datajzb,
                     int nbins, float xmin, float xmax, bool log,
                     float ymin=0, float ymax=0, bool leftJustified=false ) {
      flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak ---- WATCH OUT: the argument in the function changed!

  TCut weightbackup=cutWeight;//backing up the correct weight (restoring below!)
  if(weightbackup==TCut("1.0")||weightbackup==TCut("1")) write_warning(__FUNCTION__,"WATCH OUT THE WEIGHT HAS POSSIBLY NOT BEEN RESET!!!! PLEASE CHANGE LINE "+any2string(__LINE__));
  if(var=="numVtx") cutWeight=TCut("1.0");
  TCut jzbData[]= { TCut(TString(datajzb+">"+any2string(jzbcut))),TCut(TString(datajzb+"<-"+any2string(jzbcut))) };
  TCut jzbMC[]  = { TCut(TString(mcjzb+">"+any2string(jzbcut))),TCut(TString(mcjzb+"<-"+any2string(jzbcut))) };

  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak ---- below: the next ~20 lines changed!
  int nRegions=4;
  if(!PlottingSetup::RestrictToMassPeak) {
    nRegions=2;
  }
  
  string sRegions[] = { "SFZP","OFZP","SFSB","OFSB" };
  TCut kRegions[] = { cutOSSF&&cutnJets&&cutmass,     cutOSOF&&cutnJets&&cutmass,
                      cutOSSF&&cutnJets&&sidebandcut, cutOSOF&&cutnJets&&sidebandcut };
  
  //find ymax
  TH1F *Refdatahisto = allsamples.Draw("datahisto",   var,nbins,xmin,xmax,xlabel,"events",kRegions[0]&&jzbData[0],data,luminosity);
  ymax=int((Refdatahisto->GetMaximum()+2*TMath::Sqrt(Refdatahisto->GetMaximum()))*1.2+0.5);
  delete Refdatahisto;
		      
  for ( int iregion=0; iregion<nRegions; ++iregion ) 
    for ( int ijzb=0; ijzb<2; ++ijzb ) {
      TCanvas *ccomp = new TCanvas("ccomp","Comparison plot",600,400);
      ccomp->SetLogy(log);
      TH1F *datahisto = allsamples.Draw("datahisto",   var,nbins,xmin,xmax,xlabel,"events",kRegions[iregion]&&jzbData[ijzb],data,luminosity);
      TH1F *lm4histo = signalsamples.Draw("lm4histo",   var,nbins,xmin,xmax,xlabel,"events",kRegions[iregion]&&jzbData[ijzb],data,luminosity,signalsamples.FindSample("LM4"));
      THStack mcstack = allsamples.DrawStack("mcstack",var,nbins,xmin,xmax,xlabel,"events",kRegions[iregion]&&jzbMC[ijzb],  mc,  luminosity);
      datahisto->SetMarkerSize(DataMarkerSize);
      if (ymax>ymin) datahisto->SetMaximum(ymax);
      lm4histo->SetLineStyle(2);
      datahisto->Draw("e1");
      mcstack.Draw("same");
      datahisto->Draw("same,e1");
//      lm4histo->Draw("hist,same");
      TLegend *kinleg = allsamples.allbglegend((sRegions[iregion]+(ijzb?"neg":"pos")).c_str());
      if ( leftJustified ) {
        Float_t w = kinleg->GetX2()-kinleg->GetX1();
        kinleg->SetX1(0.2);
        kinleg->SetX2(0.2+w);
      }
//      kinleg->AddEntry(lm4histo,"LM4","l");
      kinleg->Draw();
      TText* write_variable = write_text(0.99,0.01,var);
      write_variable->SetTextAlign(31);
      write_variable->SetTextSize(0.02);
      ccomp->RedrawAxis();
      CompleteSave(ccomp,"compare/JZBcut_at_"+any2string(jzbcut)+"/"+filename+"/"+filename+sRegions[iregion]+(ijzb?"neg":"pos"));
      delete datahisto;
      delete ccomp;
      delete lm4histo;
    }
  cutWeight=weightbackup;
}


void region_comparison_plots(string mcjzb, string datajzb, vector<float> jzb_cuts) {
  dout << "Creating comparison plots for signal and control regions" << endl;
  // Compare a few quantities in the signal region and all 7 control regions

  switch_overunderflow(true);  // switching overflow/underflow bins on
    
  
    flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak ---- the arguments changed
  for(int ijzb=0;ijzb<jzb_cuts.size();ijzb++) {
    float jzbcut=jzb_cuts[ijzb]; // Comparison plots are done for this JZB cut
    float mll_low=50;float mll_high=170;
    if(!PlottingSetup::RestrictToMassPeak) mll_high=200;
    make_comp_plot("mll","m_{ll} [GeV]","mll",jzbcut,mcjzb,datajzb,30,mll_low,mll_high,false,0,16.);
    make_comp_plot("met[4]","pfMET [GeV]","pfmet",jzbcut,mcjzb,datajzb,18,0,360,false,0,16.);
    make_comp_plot("pfJetGoodNum","#(jets)","njets",jzbcut,mcjzb,datajzb,10,0,10, false,0,35.);
    make_comp_plot("pt","Z p_{T} [GeV]","Zpt",jzbcut,mcjzb,datajzb,26,0,525,false,0.,21.);
    make_comp_plot("numVtx","#(prim. vertices)","nvtx",jzbcut,mcjzb,datajzb,20,0.,20.,false,0,16.);
    make_comp_plot("TMath::Abs(dphi)","#Delta#phi(leptons)","dphilep",jzbcut,mcjzb,datajzb,10,0.,3.1415,false,0,16.,true);
    make_comp_plot("TMath::Abs(dphi_sumJetVSZ[1])","#Delta#phi(Z,jets)","dphiZjets",jzbcut,mcjzb,datajzb,10,0.,3.1415,false,0,16.,true);
  }

  switch_overunderflow(false); // switching overflow/underflow bins off
}

void do_kinematic_PF_plots()
{
  do_kinematic_plots(true);
}

void signal_bg_comparison()
{
  TCanvas *can = new TCanvas("can","Signal Background Comparison Canvas");
  can->SetLogy(1);

  int sbg_nbins=130;
  float sbg_min=-500; //-110;
  float sbg_max=800; //jzbHigh;
  
  float simulatedlumi=luminosity;//in pb please - adjust to your likings
  
  TH1F *JZBplotZJETs = allsamples.Draw("JZBplotZJETs",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("DYJetsToLL"));
  TH1F *JZBplotLM4 = allsamples.Draw("JZBplotLM4",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("LM4"));
  TH1F *JZBplotTtbar = allsamples.Draw("JZBplotTtbar",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("TTJets"));
  
  JZBplotTtbar->SetLineColor(allsamples.GetColor("TTJet"));
  JZBplotZJETs->SetFillColor(allsamples.GetColor("DY"));
  JZBplotZJETs->SetLineColor(kBlack);
  JZBplotLM4->SetLineStyle(2);
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
  JZBplotLM4->Draw("histo,same");


  TLegend *signal_bg_comparison_leg2 =  make_legend("",0.55,0.75,false);
  signal_bg_comparison_leg2->AddEntry(JZBplotZJETs,"Z+Jets","f");
  signal_bg_comparison_leg2->AddEntry(JZBplotTtbar,"t#bar{t}","f");
  signal_bg_comparison_leg2->AddEntry(JZBplotLM4,"LM4","f");
  signal_bg_comparison_leg2->Draw();
  DrawMCPrelim(simulatedlumi);
  CompleteSave(can,"jzb_bg_vs_signal_distribution");

  // Define illustrative set of SMS points
  TCut kSMS1("MassGlu==250&&MassLSP==75");
  TCut kSMS2("MassGlu==800&&MassLSP==200");
  TCut kSMS3("MassGlu==1050&&MassLSP==850");
  TCut kSMS4("MassGlu==1200&&MassLSP==100");
  TH1F *JZBplotSMS1 = scansample.Draw("JZBplotSMS1",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets&&kSMS1,mc,simulatedlumi,scansample.FindSample("t"));
  JZBplotSMS1->Scale(JZBplotLM4->Integral()/JZBplotSMS1->Integral());
  
  TH1F *JZBplotSMS2 = scansample.Draw("JZBplotSMS2",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets&&kSMS2,mc,simulatedlumi,scansample.FindSample("t"));
  JZBplotSMS2->Scale(JZBplotLM4->Integral()/JZBplotSMS2->Integral());

  TH1F *JZBplotSMS3 = scansample.Draw("JZBplotSMS3",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets&&kSMS3,mc,simulatedlumi,scansample.FindSample("t"));
  JZBplotSMS3->Scale(JZBplotLM4->Integral()/JZBplotSMS3->Integral());
  
  TH1F *JZBplotSMS4 = scansample.Draw("JZBplotSMS4",jzbvariablemc,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",cutmass&&cutOSSF&&cutnJets&&kSMS4,mc,simulatedlumi,scansample.FindSample("t"));
  JZBplotSMS4->Scale(JZBplotLM4->Integral()/JZBplotSMS4->Integral());

  // Draw all plots overlaid
  JZBplotTtbar->SetMaximum(JZBplotZJETs->GetMaximum());
  JZBplotTtbar->SetMinimum(0.01);
  JZBplotTtbar->SetFillColor(allsamples.GetColor("TTJets"));
  JZBplotTtbar->DrawClone("histo");
  JZBplotZJETs->Draw("histo,same");
  JZBplotTtbar->SetFillColor(0);
  JZBplotTtbar->DrawClone("histo,same");
  JZBplotTtbar->SetFillColor(allsamples.GetColor("TTJets"));

  JZBplotSMS1->SetLineColor(kRed+1);
  JZBplotSMS2->SetLineColor(kBlue+1);
  JZBplotSMS3->SetLineColor(kRed+1);
  JZBplotSMS4->SetLineColor(kBlue+1);
  JZBplotSMS3->SetLineStyle(2);
  JZBplotSMS4->SetLineStyle(2);
  
  JZBplotSMS1->Draw("histo,same");
  JZBplotSMS2->Draw("histo,same");
  JZBplotSMS3->Draw("histo,same");
  JZBplotSMS4->Draw("histo,same");
  JZBplotLM4->SetLineColor(kGreen);JZBplotLM4->Draw("histo,same");
  TLegend *signal_bg_comparison_leg6 = make_legend("",0.55,0.55,false);
  signal_bg_comparison_leg6->AddEntry(JZBplotZJETs,"Z+Jets","f");
  signal_bg_comparison_leg6->AddEntry(JZBplotTtbar,"t#bar{t}","f");
  signal_bg_comparison_leg6->AddEntry(JZBplotSMS1,"","");
  signal_bg_comparison_leg6->AddEntry(JZBplotSMS1,"SMS parameters","");
  signal_bg_comparison_leg6->AddEntry(JZBplotSMS1,"(250,75) [GeV]","f");
  signal_bg_comparison_leg6->AddEntry(JZBplotSMS2,"(800,200) [GeV]","f");
  signal_bg_comparison_leg6->AddEntry(JZBplotSMS3,"(1050,850) [GeV]","f");
  signal_bg_comparison_leg6->AddEntry(JZBplotSMS4,"(1200,100) [GeV]","f");
  signal_bg_comparison_leg6->AddEntry(JZBplotLM4,"LM4","f");
  signal_bg_comparison_leg6->Draw();
  DrawMCPrelim(simulatedlumi);
  CompleteSave(can,"jzb_bg_vs_signal_distribution_SMS__summary");
}

vector<TF1*> do_cb_fit_to_plot(TH1F *histo, float Sigma, float doingfitacrosstheboard=false) {
  TF1 *BpredFunc = new TF1("BpredFunc",InvCrystalBall,0,1000,5);
  BpredFunc->SetParameter(0,histo->GetBinContent(1));
  if(doingfitacrosstheboard) BpredFunc->SetParameter(0,histo->GetMaximum());
  BpredFunc->SetParameter(1,0.);
  if(method==1) BpredFunc->SetParameter(2,10*Sigma);//KM
  else BpredFunc->SetParameter(2,Sigma);//Gaussian based methods
  if(method==-99) BpredFunc->SetParameter(2,2.0*Sigma);//Kostas
  BpredFunc->SetParameter(3,1.8);
  BpredFunc->SetParameter(4,2.5);
  histo->Fit(BpredFunc,"QN0");
  BpredFunc->SetLineColor(kBlue);
  
  TF1 *BpredFuncP = new TF1("BpredFuncP",InvCrystalBallP,-1000,histo->GetBinLowEdge(histo->GetNbinsX())+histo->GetBinWidth(histo->GetNbinsX()),5);
  TF1 *BpredFuncN = new TF1("BpredFuncN",InvCrystalBallN,-1000,histo->GetBinLowEdge(histo->GetNbinsX())+histo->GetBinWidth(histo->GetNbinsX()),5);
  
  BpredFuncP->SetParameters(BpredFunc->GetParameters());
  BpredFuncP->SetLineColor(kBlue);
  BpredFuncP->SetLineStyle(2);
  
  BpredFuncN->SetParameters(BpredFunc->GetParameters());
  BpredFuncN->SetLineColor(kBlue);
  BpredFuncN->SetLineStyle(2);
  
  vector<TF1*> functions;
  functions.push_back(BpredFuncN);
  functions.push_back(BpredFunc);
  functions.push_back(BpredFuncP);
  return functions;
}


TF1* do_logpar_fit_to_plot(TH1F *osof) {
  TCanvas *logpar_fit_can = new TCanvas("logpar_fit_can","Fit canvas for LogPar");
  TF1 *logparfunc = new TF1("logparfunc",LogParabola,0,300,3);
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
  CompleteSave(logpar_fit_can,"Bpred_Data_LogPar_Fit_To_TTbarPred");
  delete logpar_fit_can;
  return logparfunc2;
}

vector<TF1*> do_extended_fit_to_plot(TH1F *prediction, TH1F *Tprediction, TH1F *ossf, TH1F *osof,int isdata) {
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
//  TH1F *ttbarprediction=(TH1F*)prediction->Clone("ttbarprediction");
//  ttbarprediction->Add(ossf,-1);//without the Z+Jets estimate, this is really just the ttbar estimate!
//      the line above is not necessary anymore as we're now looking at a prediction without Z+Jets, and not multiplied with (1.0/3)
  TF1 *ttbarlogpar = do_logpar_fit_to_plot(Tprediction);
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) ttbarlogpar->SetParameter(0,1.0/3*ttbarlogpar->GetParameter(0));//correcting for the fact that we didn't multiply with (1.0/3);

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
  prediction->Fit(kmlp,"Q");//fitting the final result (done this in the past but kicked it)
/*
if you want to start from scratch (without the partial fitting and only fitting the whole thing, some good start values could be : 
  */
  kmlp_pars[0]=kmlp->GetParameter(0);
  kmlp_pars[1]=3.6198;
  kmlp_pars[2]=16.4664;
  kmlp_pars[3]=1.92253;
  kmlp_pars[4]=3.56099;
  kmlp_pars[5]=5.83;
  kmlp_pars[6]=0.000757479;
  kmlp_pars[7]=95.6157;
  kmlp_pars[8]=0;
  kmlp_pars[9]=0;
  kmlp->SetParameters(kmlp_pars);
/**/
  prediction->Fit(kmlp,"Q");//fitting the final result (done this in the past but kicked it)

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
  
  if(isdata==0) CompleteSave(can,"MakingOfBpredFunction/Bpred_MC_Analytical_Function_Composition");
  if(isdata==1) CompleteSave(can,"MakingOfBpredFunction/Bpred_data_Analytical_Function_Composition");
  if(isdata==2) CompleteSave(can,"MakingOfBpredFunction/Bpred_MCBnS_Analytical_Function_Composition");
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

void do_prediction_plot(string jzb, TCanvas *globalcanvas, float sigma, float high, int use_data, bool overlay_signal = false )
{
  bool is_data=false;
  bool use_signal=false;
  if(use_data==1) is_data=true;
  if(use_data==2) use_signal=true;
  int nbins=100;
  if(is_data) nbins=50;
  float low=0;
  float hi=500;
  
  TH1F *RcorrJZBeemm   = allsamples.Draw("RcorrJZBeemm",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,is_data, luminosity,use_signal);
  TH1F *LcorrJZBeemm   = allsamples.Draw("LcorrJZBeemm",("-"+jzb).c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,is_data, luminosity,use_signal);
  TH1F *RcorrJZBem     = allsamples.Draw("RcorrJZBem",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,is_data, luminosity,use_signal);
  TH1F *LcorrJZBem     = allsamples.Draw("LcorrJZBem",("-"+jzb).c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,is_data, luminosity,use_signal);
  
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak ---- prediction changed.
  TH1F *RcorrJZBSBem;
  TH1F *LcorrJZBSBem;
  TH1F *RcorrJZBSBeemm;
  TH1F *LcorrJZBSBeemm;
    
  if(PlottingSetup::RestrictToMassPeak) {
    RcorrJZBSBem   = allsamples.Draw("RcorrJZBSBem",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", sidebandcut&&cutOSOF&&cutnJets,is_data, luminosity,use_signal);
    LcorrJZBSBem   = allsamples.Draw("LcorrJZBSBem",("-"+jzb).c_str(),nbins,low,hi, "JZB [GeV]", "events", sidebandcut&&cutOSOF&&cutnJets,is_data, luminosity,use_signal);
    RcorrJZBSBeemm = allsamples.Draw("RcorrJZBSBeemm",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", sidebandcut&&cutOSSF&&cutnJets,is_data, luminosity,use_signal);
    LcorrJZBSBeemm = allsamples.Draw("LcorrJZBSBeemm",("-"+jzb).c_str(),nbins,low,hi, "JZB [GeV]", "events", sidebandcut&&cutOSSF&&cutnJets,is_data, luminosity,use_signal);
  }
  
  TH1F *lm4RcorrJZBeemm;
  if(overlay_signal) lm4RcorrJZBeemm = allsamples.Draw("lm4RcorrJZBSBeemm",jzb.c_str(),nbins,low,hi, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,is_data, luminosity,allsamples.FindSample("LM4"));
  
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak ---- prediction changed.
  TH1F *Bpred = (TH1F*)LcorrJZBeemm->Clone("Bpred");
  if(PlottingSetup::RestrictToMassPeak) {
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
    
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak ---- prediction changed
  TH1F *Tpred = (TH1F*)RcorrJZBem->Clone("Bpred");
  Tpred->Add(LcorrJZBem,-1.0);
  if(PlottingSetup::RestrictToMassPeak) {
    Tpred->Add(RcorrJZBSBem,1.0);
    Tpred->Add(LcorrJZBSBem,-1.0);
    Tpred->Add(RcorrJZBSBeemm,1.0);
    Tpred->Add(LcorrJZBSBeemm,-1.0);
  }
  
  globalcanvas->cd();
  globalcanvas->SetLogy(1);
  RcorrJZBeemm->SetMarkerStyle(20);
  RcorrJZBeemm->GetXaxis()->SetRangeUser(0,high);
  RcorrJZBeemm->SetMinimum(0.1);
  RcorrJZBeemm->Draw("e1x0");

  Bpred->SetLineColor(kRed);
  Bpred->SetStats(0);
  Bpred->Draw("hist,same");
  
  int versok=false;
  if(gROOT->GetVersionInt()>=53000) versok=true;
  
  if ( overlay_signal ) {
    lm4RcorrJZBeemm->SetLineColor(TColor::GetColor("#088A08"));
    lm4RcorrJZBeemm->Draw("histo,same");
  }
  RcorrJZBeemm->SetMarkerSize(DataMarkerSize);
  TLegend *legBpred = make_legend("",0.6,0.55);
  if(use_data==1)
  {
    vector<TF1*> analytical_function = do_extended_fit_to_plot(Bpred,Tpred,LcorrJZBeemm,LcorrJZBem,is_data);
    globalcanvas->cd();
    analytical_function[0]->Draw("same"); analytical_function[1]->Draw("same");analytical_function[2]->Draw("same");
    RcorrJZBeemm->Draw("e1x0,same");//HAVE IT ON TOP!
    legBpred->AddEntry(RcorrJZBeemm,"observed","p");
    legBpred->AddEntry(Bpred,"predicted","l");
    legBpred->AddEntry(analytical_function[1],"predicted fit","l");
    legBpred->AddEntry(analytical_function[2],"stat. uncert.","l");
    if ( overlay_signal ) legBpred->AddEntry(lm4RcorrJZBeemm,"LM4","l");
    legBpred->Draw();
    CompleteSave(globalcanvas,"Bpred_Data");
  }
  if(use_data==0) {
    RcorrJZBeemm->Draw("e1x0,same");//HAVE IT ON TOP!
    legBpred->AddEntry(RcorrJZBeemm,"MC observed","p");
    legBpred->AddEntry(Bpred,"MC predicted","l");
    if(versok) legBpred->AddEntry((TObject*)0,"",""); // Just for alignment // causes seg fault on root v5.18
    if(versok) legBpred->AddEntry((TObject*)0,"",""); // causes seg fault on root v5.18
    if ( overlay_signal ) legBpred->AddEntry(lm4RcorrJZBeemm,"LM4","l");
    legBpred->Draw();
    
    CompleteSave(globalcanvas,"Bpred_MC");
  }
  if(use_data==2) {
    RcorrJZBeemm->Draw("e1x0,same");//HAVE IT ON TOP!
    legBpred->AddEntry(RcorrJZBeemm,"MC observed","p");
    legBpred->AddEntry(Bpred,"MC predicted","l");
    if(versok) legBpred->AddEntry((TObject*)0,"",""); // Just for alignment // causes seg fault on root v5.18 --> now only allowed for root >=v5.30
    if(versok) legBpred->AddEntry((TObject*)0,"",""); // causes seg fault on root v5.18 --> now only allowed for root >=v5.30
    if ( overlay_signal ) legBpred->AddEntry(lm4RcorrJZBeemm,"LM4","l");
    legBpred->Draw();
    
    CompleteSave(globalcanvas,"Bpred_MCwithS");
  }
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
  /// The part below is meaningless for the offpeak analysis (it's a comparison of the different estimates but there is but one estimate!)
  if(!PlottingSetup::RestrictToMassPeak) return;
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
  RcorrJZBeemm->Draw("e1x0,same");//HAVE IT ON TOP!


  TLegend *legBpredc = make_legend("",0.6,0.55);
  if(use_data==1)
  {
    legBpredc->AddEntry(RcorrJZBeemm,"observed","p");
    legBpredc->AddEntry(Bpredem,"OFZP","l");
    legBpredc->AddEntry(BpredSBem,"OFSB","l");
    legBpredc->AddEntry(BpredSBeemm,"SFSB","l");
    legBpredc->Draw();
    CompleteSave(globalcanvas,"Bpred_Data_comparison");
  }
  if(use_data==0) {
    legBpredc->AddEntry(RcorrJZBeemm,"MC observed","p");
    legBpredc->AddEntry(Bpredem,"MC OFZP","l");
    legBpredc->AddEntry(BpredSBem,"MC OFSB","l");
    legBpredc->AddEntry(BpredSBeemm,"MC SFSB","l");
    legBpredc->Draw();
    legBpredc->Draw();
    CompleteSave(globalcanvas,"Bpred_MC_comparison");
  }
  if(use_data==2) {
    RcorrJZBeemm->Draw("e1x0,same");//HAVE IT ON TOP!
    legBpred->AddEntry(RcorrJZBeemm,"MC observed","p");
    legBpred->AddEntry(Bpred,"MC predicted","l");
    legBpred->AddEntry((TObject*)0,"",""); // Just for alignment // causes seg fault on root v5.18
//    legBpred->AddEntry((TObject*)0,"",""); // causes seg fault on root v5.18
    if ( overlay_signal ) legBpred->AddEntry(lm4RcorrJZBeemm,"LM4","l");
    legBpred->Draw();
    
    CompleteSave(globalcanvas,"Bpred_MCwithS_comparison");
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

void do_prediction_plots(string mcjzb, string datajzb, float DataSigma, float MCSigma, bool overlay_signal ) {
  TCanvas *globalcanvas = new TCanvas("globalcanvas","Prediction Canvas");
  do_prediction_plot(datajzb,globalcanvas,DataSigma,jzbHigh ,data,overlay_signal);
  do_prediction_plot(mcjzb,globalcanvas,MCSigma,jzbHigh ,mc,overlay_signal);
  do_prediction_plot(mcjzb,globalcanvas,MCSigma,jzbHigh ,mcwithsignal,overlay_signal);
}

void do_ratio_plot(int is_data,vector<float> binning, string jzb, TCanvas *can, float high=-9999) {
  bool do_data=0;
  bool dosignal=0;
  if(is_data==1) do_data=1;
  if(is_data==2) dosignal=1;
  TH1F *RcorrJZBeemm   = allsamples.Draw("RcorrJZBeemm",jzb.c_str(),binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,do_data, luminosity,dosignal);
  TH1F *LcorrJZBeemm   = allsamples.Draw("LcorrJZBeemm",("-"+jzb).c_str(),binning, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,do_data, luminosity,dosignal);
  TH1F *RcorrJZBem     = allsamples.Draw("RcorrJZBem",jzb.c_str(),binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,do_data, luminosity,dosignal);
  TH1F *LcorrJZBem     = allsamples.Draw("LcorrJZBem",("-"+jzb).c_str(),binning, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,do_data, luminosity,dosignal);
  
  TH1F *RcorrJZBSBem   = allsamples.Draw("RcorrJZBSbem",jzb.c_str(),binning, "JZB [GeV]", "events", sidebandcut&&cutOSOF&&cutnJets,do_data, luminosity,dosignal);
  TH1F *LcorrJZBSBem   = allsamples.Draw("LcorrJZBSbem",("-"+jzb).c_str(),binning, "JZB [GeV]", "events", sidebandcut&&cutOSOF&&cutnJets,do_data, luminosity,dosignal);
  TH1F *RcorrJZBSBeemm = allsamples.Draw("RcorrJZBSbeemm",jzb.c_str(),binning, "JZB [GeV]", "events", sidebandcut&&cutOSSF&&cutnJets,do_data, luminosity,dosignal);
  TH1F *LcorrJZBSbeemm = allsamples.Draw("LcorrJZBSbeemm",("-"+jzb).c_str(),binning, "JZB [GeV]", "events", sidebandcut&&cutOSSF&&cutnJets,do_data, luminosity,dosignal);
  
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
  TH1F *Bpred = (TH1F*)LcorrJZBeemm->Clone("Bpred");
  if(PlottingSetup::RestrictToMassPeak) {
    Bpred->Add(RcorrJZBem,1.0/3);
    Bpred->Add(LcorrJZBem,-1.0/3);
    Bpred->Add(RcorrJZBSBem,1.0/3);
    Bpred->Add(LcorrJZBSBem,-1.0/3);
    Bpred->Add(RcorrJZBSBeemm,1.0/3);
    Bpred->Add(LcorrJZBSbeemm,-1.0/3);
  } else {
    Bpred->Add(RcorrJZBem,1.0);
    Bpred->Add(LcorrJZBem,-1.0);
  }
  
  can->cd();
  can->SetLogy(0);
  Bpred->SetLineColor(kRed);
  Bpred->SetStats(0);
  if(high>0) Bpred->GetXaxis()->SetRangeUser(0,high);
  TH1F *JZBratioforfitting=(TH1F*)RcorrJZBeemm->Clone("JZBratioforfitting");
  JZBratioforfitting->Divide(Bpred);
  TGraphAsymmErrors *JZBratio = histRatio(RcorrJZBeemm,Bpred,is_data,binning,false);
  
  
  JZBratio->SetTitle("");
  JZBratio->GetYaxis()->SetRangeUser(0.0,9.0);
//  if(is_data==1) JZBratio->GetXaxis()->SetRangeUser(0,jzbHigh );
  
  TF1 *pol0 = new TF1("pol0","[0]",0,1000);
  TF1 *pol0d = new TF1("pol0","[0]",0,1000);
//  straightline_fit->SetParameter(0,1);
  JZBratioforfitting->Fit(pol0,"Q0R","",0,30);
  pol0d->SetParameter(0,pol0->GetParameter(0));
  
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
/*  if(is_data) writeresult->AddText("Data closure test");
  else writeresult->AddText("MC closure test");
  */
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
  TLegend *phony_leg = make_legend("ratio",0.6,0.55,false);//this line is just to have the default CMS Preliminary (...) on the canvas as well.
  if(is_data==1) DrawPrelim();
  else DrawMCPrelim();
  TLegend *leg = new TLegend(0.55,0.75,0.89,0.89);
  leg->SetTextFont(42);
  leg->SetTextSize(0.04);
//   if(is_data==1) leg->SetHeader("Ratio (data)");
//   else leg->SetHeader("Ratio (MC)");

  TString MCtitle("MC ");
  if (is_data==1) MCtitle = "";

  leg->SetFillStyle(4000);
  leg->SetFillColor(kWhite);
  leg->SetTextFont(42);
//  leg->AddEntry(topline,"+20\% sys envelope","l");
  leg->AddEntry(JZBratio,MCtitle+"obs / "+MCtitle+"pred","p");
  leg->AddEntry(oneline,"ratio = 1","l");
//  leg->AddEntry(pol0d,"fit in [0,30] GeV","l");
//  leg->AddEntry(bottomline,"#pm50% envelope","l");


  //leg->Draw("same"); // no longer drawing legend

  if(is_data==1) CompleteSave(can, "jzb_ratio_data");
  if(is_data==0) CompleteSave(can, "jzb_ratio_mc");
  if(is_data==2) CompleteSave(can, "jzb_ratio_mc_BandS");//special case, MC with signal!
  
  delete RcorrJZBeemm;
  delete LcorrJZBeemm;
  delete RcorrJZBem;
  delete LcorrJZBem;
  
  delete RcorrJZBSBem;
  delete LcorrJZBSBem;
  delete RcorrJZBSBeemm;
  delete LcorrJZBSbeemm;
}

void do_ratio_plots(string mcjzb,string datajzb,vector<float> ratio_binning) {
  TCanvas *globalc = new TCanvas("globalc","Ratio Plot Canvas");
  globalc->SetLogy(0);
  
  do_ratio_plot(mc,ratio_binning,mcjzb,globalc, jzbHigh  );
  do_ratio_plot(data,ratio_binning,datajzb,globalc, jzbHigh  );
  do_ratio_plot(mcwithsignal,ratio_binning,mcjzb,globalc, jzbHigh  );
}

string give_jzb_expression(float peak, int type) {
  stringstream val;
  if(type==data) {
    if(peak<0) val << jzbvariabledata << "+" << TMath::Abs(peak);
    if(peak>0) val << jzbvariabledata << "-" << TMath::Abs(peak);
    if(peak==0) val << jzbvariabledata;
  }
  if(type==mc) {
    if(peak<0) val << jzbvariablemc << "+" << TMath::Abs(peak);
    if(peak>0) val << jzbvariablemc << "-" << TMath::Abs(peak);
    if(peak==0) val << jzbvariablemc;
  }
  return val.str();
}


void lepton_comparison_plots() {
  Float_t ymin = 1.e-5, ymax = 0.25;
  TCanvas *can = new TCanvas("can","Lepton Comparison Canvas");
  can->SetLogy(1);
  TH1F *eemc = allsamples.Draw("eemc","mll",50,50,150, "mll [GeV]", "events", cutOSSF&&cutnJets&&"(id1==0)",mc, luminosity,allsamples.FindSample("DYJetsToLL"));
  TH1F *mmmc = allsamples.Draw("mmmc","mll",50,50,150, "mll [GeV]", "events", cutOSSF&&cutnJets&&"(id1==1)",mc, luminosity,allsamples.FindSample("DYJetsToLL"));
  eemc->SetLineColor(kBlue);
  mmmc->SetLineColor(kRed);
  eemc->SetMinimum(0.1);
  eemc->SetMaximum(10*eemc->GetMaximum());
  eemc->Draw("histo");
  mmmc->Draw("histo,same");
  TLegend *leg = make_legend();
  leg->AddEntry(eemc,"ZJets->ee (MC)","l");
  leg->AddEntry(mmmc,"ZJets->#mu#mu (MC)","l");
  leg->Draw("same");
  CompleteSave(can, "lepton_comparison/mll_effratio_mc");
  
  TH1F *eed = allsamples.Draw("eed","mll",50,50,150, "mll [GeV]", "events", cutOSSF&&cutnJets&&"(id1==0)",data, luminosity);
  TH1F *mmd = allsamples.Draw("mmd","mll",50,50,150, "mll [GeV]", "events", cutOSSF&&cutnJets&&"(id1==1)",data, luminosity);
  eed->SetLineColor(kBlue);
  mmd->SetLineColor(kRed);
  eed->SetMinimum(0.1);
  eed->SetMaximum(10*eed->GetMaximum());
  eed->Draw("histo");
  mmd->Draw("histo,same");
  TLegend *leg2 = make_legend();
  leg2->AddEntry(eed,"ZJets->ee (data)","l");
  leg2->AddEntry(mmd,"ZJets->#mu#mu (data)","l");
  leg2->Draw();
  CompleteSave(can, "lepton_comparison/mll_effratio_data");
  
  TH1F *jeed = allsamples.Draw("jeed",jzbvariabledata,    100,-150,jzbHigh , "JZB [GeV]", "events", cutOSSF&&cutnJets&&"(id1==0)",data, luminosity);
  TH1F *jmmd = allsamples.Draw("jmmd",jzbvariabledata,    100,-150,jzbHigh , "JZB [GeV]", "events", cutOSSF&&cutnJets&&"(id1==1)",data, luminosity);
  TH1F *jeemmd = allsamples.Draw("jeemmd",jzbvariabledata,100,-150,jzbHigh , "JZB [GeV]", "events", cutOSSF&&cutnJets,data, luminosity);
  dout << "ee : " << jeed->GetMean() << "+/-" << jeed->GetMeanError() << endl;
  dout << "ee : " << jmmd->GetMean() << "+/-" << jmmd->GetMeanError() << endl;
  dout << "eemd : " << jeemmd->GetMean() << "+/-" << jeemmd->GetMeanError() << endl;
  jeemmd->SetLineColor(kBlack);
  jeemmd->SetMarkerStyle(25);
  jeed->SetLineColor(kBlue);
  jmmd->SetLineColor(kRed);
  jeed->SetMinimum(0.1);
  jeed->SetMaximum(10*eed->GetMaximum());
  TH1* njeemmd = jeemmd->DrawNormalized();
  njeemmd->SetMinimum(ymin);
  njeemmd->SetMaximum(ymax);

  jeed->DrawNormalized("histo,same");
  jmmd->DrawNormalized("histo,same");
  jeemmd->DrawNormalized("same");
  TLegend *jleg2 = make_legend(" ");
  jleg2->AddEntry(jeemmd,"ee and #mu#mu","p");
  jleg2->AddEntry(jeed,"ee","l");
  jleg2->AddEntry(jmmd,"#mu#mu","l");
  jleg2->Draw();
  CompleteSave(can,"lepton_comparison/jzb_effratio_data");
  
  TH1F *zjeed = allsamples.Draw("zjeed",jzbvariablemc,    100,-150,jzbHigh , "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets&&"(id1==0)",mc,  luminosity,allsamples.FindSample("DYJets"));
  TH1F *zjmmd = allsamples.Draw("zjmmd",jzbvariablemc,    100,-150,jzbHigh , "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets&&"(id1==1)",mc,  luminosity,allsamples.FindSample("DYJets"));
  TH1F *zjeemmd = allsamples.Draw("zjeemmd",jzbvariablemc,100,-150,jzbHigh , "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,            mc,  luminosity,allsamples.FindSample("DYJets"));
  dout << "Z+Jets ee : " << zjeed->GetMean() << "+/-" << zjeed->GetMeanError() << endl;
  dout << "Z+Jets ee : " << zjmmd->GetMean() << "+/-" << zjmmd->GetMeanError() << endl;
  dout << "Z+Jets eemd : " << zjeemmd->GetMean() << "+/-" << zjeemmd->GetMeanError() << endl;
  zjeemmd->SetLineColor(kBlack);
  zjeemmd->SetMarkerStyle(25);
  zjeed->SetLineColor(kBlue);
  zjmmd->SetLineColor(kRed);
  zjeed->SetMinimum(0.1);
  zjeed->SetMaximum(10*eed->GetMaximum());

  TH1* nzjeemmd = zjeemmd->DrawNormalized();
  nzjeemmd->SetMinimum(ymin);
  nzjeemmd->SetMaximum(ymax);
  zjeed->DrawNormalized("histo,same");
  zjmmd->DrawNormalized("histo,same");
  zjeemmd->DrawNormalized("same");
  TLegend *zjleg2 = make_legend("Z+jets MC");
  zjleg2->AddEntry(jeemmd,"ee and #mu#mu","p");
  zjleg2->AddEntry(jeed,"ee","l");
  zjleg2->AddEntry(jmmd,"#mu#mu","l");
  zjleg2->Draw();
  CompleteSave(can,"lepton_comparison/jzb_effratio_ZJets");
  
  TH1F *ld = allsamples.Draw("ld","mll",50,50,150, "mll [GeV]", "events", cutOSSF&&cutnJets,data, luminosity);
  ld->DrawNormalized("e1");
  eed->DrawNormalized("histo,same");
  mmd->DrawNormalized("histo,same");
  TLegend *leg3 = make_legend();
  leg3->AddEntry(ld,"ZJets->ll (data)","p");
  leg3->AddEntry(eed,"ZJets->ee (data)","l");
  leg3->AddEntry(mmd,"ZJets->#mu#mu (data)","l");
  leg3->Draw();
  CompleteSave(can,"lepton_comparison/mll_effratio_data__all_compared");
  /*
  TH1F *jzbld  = allsamples.Draw("jzbld",jzbvariable,75,-150,150, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data, luminosity);
  TH1F *jzbemd = allsamples.Draw("jzbemd",jzbvariable,75,-150,150, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data, luminosity);
  */
  TH1F *jzbld  = allsamples.Draw("jzbld",jzbvariabledata,92,-110,jzbHigh , "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data, luminosity);
  TH1F *jzbemd = allsamples.Draw("jzbemd",jzbvariabledata,92,-110,jzbHigh , "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data, luminosity);
  jzbld->SetMarkerColor(kBlack);
  jzbld->SetMarkerStyle(26);
  jzbemd->SetMarkerStyle(25);
  jzbemd->SetMarkerColor(kRed);
  jzbemd->SetLineColor(kRed);
  jzbld->SetMinimum(0.35);
  jzbld->Draw("e1");
  jzbemd->Draw("e1,same");
  TLegend *leg4 = make_legend();
  leg4->AddEntry(jzbld,"SFZP","p");
  leg4->AddEntry(jzbemd,"OFZP","p");
  leg4->AddEntry((TObject*)0,"",""); //causes segmentation violation
  leg4->AddEntry((TObject*)0,"",""); //causes segmentation violation
  leg4->Draw();
  CompleteSave(can,"lepton_comparison/jzb_eemumu_emu_data");
  
  TH1F *ttbarjzbld  = allsamples.Draw("ttbarjzbld",jzbvariablemc,110,-150,400, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,data, luminosity,allsamples.FindSample("TTJet"));
  TH1F *ttbarjzbemd = allsamples.Draw("ttbarjzbemd",jzbvariablemc,110,-150,400, "JZB [GeV]", "events", cutmass&&cutOSOF&&cutnJets,data, luminosity,allsamples.FindSample("TTJet"));
  ttbarjzbld->SetLineColor(allsamples.GetColor("TTJet"));
  ttbarjzbemd->SetLineColor(allsamples.GetColor("TTJet"));
  ttbarjzbld->Draw("histo");
  ttbarjzbemd->SetLineStyle(2);
  ttbarjzbemd->Draw("histo,same");
  TLegend *leg5 = make_legend();
  leg5->AddEntry(ttbarjzbld,"t#bar{t}->(ee or #mu#mu)","l");
  leg5->AddEntry(ttbarjzbemd,"t#bar{t}->e#mu","l");
  leg5->Draw();
  CompleteSave(can,"lepton_comparison/ttbar_emu_mc");
}

bool is_OF(TCut cut) {
  string scut = (const char*) cut;
  if((int)scut.find("id1!=id2")>-1) return true;
  if((int)scut.find("id1==id2")>-1) return false;
  return false;
}

bool is_ZP(TCut cut) {
  string scut = (const char*) cut;
  if((int)scut.find("91")>-1) return true;
  return false;
}


void draw_pure_jzb_histo(TCut cut,string variable,string savename, TCanvas *can,vector<float> binning) {
  can->cd();
  can->SetLogy(1);
  string xlabel="JZB [GeV]";

  TH1F *datahisto = allsamples.Draw("datahisto",variable,binning, xlabel, "events",cut,data,luminosity);
  THStack mcstack = allsamples.DrawStack("mcstack",variable,binning, xlabel, "events",cut,mc,luminosity);
  
  datahisto->SetMinimum(0.1);
  //if(savename=="jzb_OSOF") datahisto->SetMaximum(10);
  datahisto->SetMarkerSize(DataMarkerSize);
  datahisto->Draw("e1");
  mcstack.Draw("same");
  datahisto->Draw("same,e1");
  
  TLegend *leg;
  if(is_OF(cut) && is_ZP(cut)) leg = allsamples.allbglegend("OFZP",datahisto);
  else if(!is_OF(cut) &&  is_ZP(cut)) leg = allsamples.allbglegend("SFZP",datahisto);
  else if( is_OF(cut) && !is_ZP(cut)) leg = allsamples.allbglegend("OFSB",datahisto);
  else if(!is_OF(cut) && !is_ZP(cut)) leg = allsamples.allbglegend("SFSB",datahisto);
  else {
    std::cerr << "Unable to decode cut: " << cut.GetTitle() << std::endl;
    exit(-1);
      }
  leg->Draw();
  string write_cut = decipher_cut(cut,"");
  TText *writeline1 = write_cut_on_canvas(write_cut.c_str());
  writeline1->SetTextSize(0.035);
  writeline1->Draw();
  CompleteSave(can, ("jzb/"+savename));
  
  datahisto->Delete();
  mcstack.Delete();
}

Double_t GausR(Double_t *x, Double_t *par) {
  return gRandom->Gaus(x[0],par[0]);
}
  
void jzb_plots(string mcjzb, string datajzb,vector<float> ratio_binning) {
  TCanvas *can = new TCanvas("can","JZB Plots Canvas");
  float max=jzbHigh ;
  float min=-120;
  int nbins=(max-min)/5.0; // we want 5 GeV/bin
  int coarserbins=int(nbins/2.0);
  int rebinnedbins=int(nbins/4.0);
  
//  stringstream ss;
//  ss << "GausRandom(" << datajzb << ",0)";
  vector<float>binning;vector<float>coarse_binning;vector<float>coarsest_binning;
  for(int i=0;i<=nbins;i++)binning.push_back(min+i*(max-min)/((float)nbins));
  for(int i=0;i<=coarserbins;i++)coarse_binning.push_back(min+i*(max-min)/((float)coarserbins));
  for(int i=0;i<=rebinnedbins;i++)coarsest_binning.push_back(min+i*(max-min)/((float)rebinnedbins));

/*  draw_pure_jzb_histo(cutOSSF&&cutnJets&&cutmass,datajzb,"jzb_OSSF_altbins",can,ratio_binning);
  draw_pure_jzb_histo(cutOSOF&&cutnJets&&cutmass,datajzb,"jzb_OSOF_altbins",can,ratio_binning);
  draw_pure_jzb_histo(cutOSSF&&cutnJets&&sidebandcut,datajzb,"jzb_OSSF_SB_altbins",can,ratio_binning);
  draw_pure_jzb_histo(cutOSOF&&cutnJets&&sidebandcut,datajzb,"jzb_OSOF_SB_altbins",can,ratio_binning);*/
  draw_pure_jzb_histo(cutOSSF&&cutnJets&&cutmass,datajzb,"jzb_OS_SFZP",can,binning);
  draw_pure_jzb_histo(cutOSOF&&cutnJets&&cutmass,datajzb,"jzb_OS_OFZP",can,binning);
  draw_pure_jzb_histo(cutOSSF&&cutnJets&&cutmass&&"id1==0",datajzb,"ee/jzb_OS_SFZP",can,binning);
  draw_pure_jzb_histo(cutOSSF&&cutnJets&&cutmass&&"id1==1",datajzb,"mm/jzb_OS_SFZP",can,binning);
  draw_pure_jzb_histo(cutOSOF&&cutnJets&&cutmass&&"id1==0",datajzb,"ee/jzb_OS_OFZP",can,binning);
  draw_pure_jzb_histo(cutOSOF&&cutnJets&&cutmass&&"id1==1",datajzb,"mm/jzb_OS_OFZP",can,binning);
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) draw_pure_jzb_histo(cutOSSF&&cutnJets&&sidebandcut,datajzb,"jzb_OS_SFSB",can,binning);
  if(PlottingSetup::RestrictToMassPeak) draw_pure_jzb_histo(cutOSOF&&cutnJets&&sidebandcut,datajzb,"jzb_OS_OFSB",can,binning);
  
  draw_pure_jzb_histo(cutOSSF&&cutnJets&&cutmass,datajzb,"jzb_OS_SFZP_coarse",can,coarse_binning);
  draw_pure_jzb_histo(cutOSOF&&cutnJets&&cutmass,datajzb,"jzb_OS_OFZP_coarse",can,coarse_binning);
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) draw_pure_jzb_histo(cutOSSF&&cutnJets&&sidebandcut,datajzb,"jzb_OS_SFSB_coarse",can,coarse_binning);
  if(PlottingSetup::RestrictToMassPeak) draw_pure_jzb_histo(cutOSOF&&cutnJets&&sidebandcut,datajzb,"jzb_OS_OFSB_coarse",can,coarse_binning);

  draw_pure_jzb_histo(cutOSSF&&cutnJets&&cutmass,datajzb,"jzb_OS_SFZP_coarsest",can,coarsest_binning);
  draw_pure_jzb_histo(cutOSOF&&cutnJets&&cutmass,datajzb,"jzb_OS_OFZP_coarsest",can,coarsest_binning);
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) draw_pure_jzb_histo(cutOSSF&&cutnJets&&sidebandcut,datajzb,"jzb_OS_SFSB_coarsest",can,coarsest_binning);
  if(PlottingSetup::RestrictToMassPeak) draw_pure_jzb_histo(cutOSOF&&cutnJets&&sidebandcut,datajzb,"jzb_OS_OFSB_coarsest",can,coarsest_binning);
}

void calculate_all_yields(string mcdrawcommand,vector<float> jzb_cuts) {
  dout << "Calculating background yields in MC:" << endl;
  jzb_cuts.push_back(14000);
  TH1F *allbgs = allsamples.Draw("allbgs",jzbvariablemc,jzb_cuts, "JZB [GeV]", "events", cutmass&&cutOSSF&&cutnJets,mc,luminosity);
  float cumulative=0;
  for(int i=allbgs->GetNbinsX();i>=1;i--) {
    cumulative+=allbgs->GetBinContent(i);
    dout << "Above " << allbgs->GetBinLowEdge(i) << " GeV/c : " << cumulative << endl;
  }
}

void draw_ttbar_and_zjets_shape_for_one_configuration(string mcjzb, string datajzb, int leptontype=-1, int scenario=0,bool floating=false) {
  //Step 1: Establishing cuts
  stringstream jetcutstring;
  string writescenario="";

  if(scenario==0) jetcutstring << "(pfJetGoodNum>=3)&&"<<(const char*) basicqualitycut;
  if(scenario==1) jetcutstring << "(pfJetPt[0]>50&&pfJetPt[1]>50)&&"<<(const char*)basicqualitycut;
  TCut jetcut(jetcutstring.str().c_str());
  string leptoncut="mll>0";
  if(leptontype==0||leptontype==1) {
    if(leptontype==0) {
      leptoncut="id1==0";
      writescenario="__ee";
    }
    else {
      leptoncut="id1==1";
      writescenario="__ee";
    }
  }
  TCut lepcut(leptoncut.c_str());
  
  TCanvas *c5 = new TCanvas("c5","c5",1500,500);
  TCanvas *c6 = new TCanvas("c6","c6");
  c5->Divide(3,1);

  //STEP 2: Extract Zjets shape in data
  c5->cd(1);
  c5->cd(1)->SetLogy(1);
  TCut massat40("mll>40");
  TH1F *ossfleft   = allsamples.Draw("ossfleft",  "-"+datajzb,40,0,200, "JZB [GeV]", "events", massat40&&cutOSSF&&jetcut&&lepcut,data,luminosity);
  TH1F *osofleft   = allsamples.Draw("osofleft",  "-"+datajzb,40,0,200, "JZB [GeV]", "events", massat40&&cutOSOF&&jetcut&&lepcut,data,luminosity);
  ossfleft->SetLineColor(kRed);
  ossfleft->SetMarkerColor(kRed);
  ossfleft->Add(osofleft,-1); 
  vector<TF1*> functions = do_cb_fit_to_plot(ossfleft,10);
  ossfleft->SetMarkerSize(DataMarkerSize);
  ossfleft->Draw();
  functions[0]->Draw("same");functions[1]->Draw("same");functions[2]->Draw("same");
  TF1 *zjetsfunc = (TF1*) functions[1]->Clone();
  TF1 *zjetsfuncN = (TF1*) functions[0]->Clone();
  TF1 *zjetsfuncP = (TF1*) functions[2]->Clone();
  zjetsfunc->Draw("same");zjetsfuncN->Draw("same");zjetsfuncP->Draw("same");
  TLegend *leg1 = new TLegend(0.6,0.6,0.89,0.80);
  leg1->SetFillColor(kWhite);
  leg1->SetLineColor(kWhite);
  leg1->AddEntry(ossfleft,"OSSF (sub),JZB<peak","p");
  leg1->AddEntry(zjetsfunc,"OSSF fit ('zjets')","l");
  leg1->Draw("same");
  TText *titleleft = write_title("Extracting Z+Jets shape");
  titleleft->Draw();
  
  //Step 3: Extract ttbar shape (in data or MC?)
  c5->cd(2);
  c5->cd(2)->SetLogy(1);
  TH1F *osof;
  TText *titlecenter;
  bool frommc=false;
  if(frommc) {
    osof = allsamples.Draw("osof",datajzb,40,-200,200, "JZB [GeV]", "events", massat40&&cutOSSF&&jetcut&&lepcut,mc,luminosity,allsamples.FindSample("TTJets"));
    titlecenter = write_title("Extracting ttbar shape (from ossf MC)");
  }
  else {
    osof = allsamples.Draw("osof",datajzb,40,-200,200, "JZB [GeV]", "events", massat40&&cutOSOF&&jetcut&&lepcut,data,luminosity);
    titlecenter = write_title("Extracting ttbar shape (from osof data)");
  }
  osof->SetMarkerSize(DataMarkerSize);
  osof->Draw();
  vector<TF1*> ttbarfunctions = do_cb_fit_to_plot(osof,35,true);
  ttbarfunctions[0]->SetLineColor(kRed); ttbarfunctions[0]->SetLineStyle(2);   	ttbarfunctions[0]->Draw("same");
  ttbarfunctions[1]->SetLineColor(kRed); 					ttbarfunctions[1]->Draw("same");
  ttbarfunctions[2]->SetLineColor(kRed); ttbarfunctions[2]->SetLineStyle(2); 	ttbarfunctions[2]->Draw("same");

  TLegend *leg2 = new TLegend(0.15,0.8,0.4,0.89);
  leg2->SetFillColor(kWhite);
  leg2->SetLineColor(kWhite);
  if(frommc) {
    leg2->AddEntry(osof,"t#bar{t} OSSF, MC","p");
    leg2->AddEntry(ttbarfunctions[1],"Fit to t#bar{t} OSSF,MC","l");
  } else {
    leg2->AddEntry(osof,"OSOF","p");
    leg2->AddEntry(ttbarfunctions[1],"Fit to OSOF","l");
  }
  leg2->Draw("same");
  titlecenter->Draw();

//--------------------------------------------------------------------------------------------------------------------------------
  //STEP 4: Present it!
  // actually: if we wanna let it float we need to do that first :-) 
  c5->cd(3);
  c5->cd(3)->SetLogy(1);
  TH1F *observed   = allsamples.Draw("observed",datajzb,100,0,500, "JZB [GeV]", "events", massat40&&cutOSSF&&jetcut&&lepcut,data,luminosity);
  observed->SetMarkerSize(DataMarkerSize);

  TF1 *logparc = new TF1("logparc",InvCrystalBall,0,1000,5);	logparc->SetLineColor(kRed);
  TF1 *logparcn = new TF1("logparcn",InvCrystalBallN,0,1000,5);	logparcn->SetLineColor(kRed);	logparcn->SetLineStyle(2);
  TF1 *logparcp = new TF1("logparcp",InvCrystalBallP,0,1000,5);	logparcp->SetLineColor(kRed);	logparcp->SetLineStyle(2);
  
  TF1 *zjetsc = new TF1("zjetsc",InvCrystalBall,0,1000,5);	zjetsc->SetLineColor(kBlue);	
  TF1 *zjetscn = new TF1("zjetscn",InvCrystalBallN,0,1000,5);	zjetscn->SetLineColor(kBlue);	zjetscn->SetLineStyle(2);
  TF1 *zjetscp = new TF1("zjetscp",InvCrystalBallP,0,1000,5);	zjetscp->SetLineColor(kBlue);	zjetscp->SetLineStyle(2);
  
  TF1 *ZplusJetsplusTTbar = new TF1("ZplusJetsplusTTbar", DoubleInvCrystalBall,0,1000,10);	ZplusJetsplusTTbar->SetLineColor(kBlue);
  TF1 *ZplusJetsplusTTbarP= new TF1("ZplusJetsplusTTbarP",DoubleInvCrystalBallP,0,1000,10);	ZplusJetsplusTTbarP->SetLineColor(kBlue);	ZplusJetsplusTTbarP->SetLineStyle(2);
  TF1 *ZplusJetsplusTTbarN= new TF1("ZplusJetsplusTTbarN",DoubleInvCrystalBallN,0,1000,10);	ZplusJetsplusTTbarN->SetLineColor(kBlue);	ZplusJetsplusTTbarN->SetLineStyle(2);
  
  zjetsc->SetParameters(zjetsfunc->GetParameters());
  zjetscp->SetParameters(zjetsfunc->GetParameters());
  zjetscn->SetParameters(zjetsfunc->GetParameters());
  
  TH1F *observeda   = allsamples.Draw("observeda",datajzb,53,80,jzbHigh, "JZB [GeV]", "events", massat40&&cutOSSF&&jetcut&&lepcut,data,luminosity);
  //blublu
  logparc->SetParameters(ttbarfunctions[1]->GetParameters());
  logparcn->SetParameters(ttbarfunctions[1]->GetParameters());
  logparcp->SetParameters(ttbarfunctions[1]->GetParameters());
  if(floating) {
    dout << "TTbar contribution assumed (before fitting) : " << logparc->GetParameter(0) << endl;
    logparc->SetParameters(ttbarfunctions[1]->GetParameters());
    for(int i=0;i<10;i++) {
      if(i<5) ZplusJetsplusTTbar->FixParameter(i,zjetsfunc->GetParameter(i));
      if(i>=5) {
	if (i>5) ZplusJetsplusTTbar->FixParameter(i,logparc->GetParameter(i-5));
	if (i==5) ZplusJetsplusTTbar->SetParameter(i,logparc->GetParameter(i-5));
      }
    }//end of setting parameters
    observeda->Draw("same");
    ZplusJetsplusTTbar->Draw("same");
    observeda->Fit(ZplusJetsplusTTbar);
    dout << "--> Quality of Z+Jets / TTbar fit : chi2/ndf = " << ZplusJetsplusTTbar->GetChisquare() << "/" << ZplusJetsplusTTbar->GetNDF() << endl;
    ZplusJetsplusTTbar->Draw("same");
    ZplusJetsplusTTbarP->SetParameters(ZplusJetsplusTTbar->GetParameters());
    ZplusJetsplusTTbarN->SetParameters(ZplusJetsplusTTbar->GetParameters());
    dout << "TTbar contribution found (after fitting) : " << ZplusJetsplusTTbar->GetParameter(5) << endl;
    float factor = ZplusJetsplusTTbar->GetParameter(5) / logparc->GetParameter(0);
    dout << "FACTOR: " << factor << endl; 
    logparc->SetParameter(0,factor*ttbarfunctions[1]->GetParameter(0));
    logparcn->SetParameter(0,factor*ttbarfunctions[1]->GetParameter(0));
    logparcp->SetParameter(0,factor*ttbarfunctions[1]->GetParameter(0));
  }
  
  c5->cd(3);
  c5->cd(3)->SetLogy(1);
  observed->Draw();
  zjetsc->Draw("same");zjetscn->Draw("same");zjetscp->Draw("same");
  logparc->Draw("same");
  logparcn->Draw("same");
  logparcp->Draw("same");

  TLegend *leg3 = new TLegend(0.6,0.6,0.89,0.80);
  leg3->SetFillColor(kWhite);
  leg3->SetLineColor(kWhite);
  leg3->AddEntry(observed,"OSSF,JZB>peak","p");
  leg3->AddEntry(ttbarfunctions[1],"OSOF fit ('ttbar')","l");
  leg3->AddEntry(zjetsfunc,"OSSF,JZB<0 fit ('zjets')","l");
  leg3->Draw("same");
  TText *titleright = write_title("Summary of shapes and observed shape");
  titleright->Draw();
  
  c6->cd()->SetLogy(1);
  observed->Draw();  
  zjetsc->Draw("same");zjetscn->Draw("same");zjetscp->Draw("same");
  logparc->Draw("same");
  logparcn->Draw("same");
  logparcp->Draw("same");
  leg3->Draw("same");
  titleright->Draw();
  
  if(scenario==0) {
    CompleteSave(c5,"Shapes2/Making_of___3jetsabove30"+writescenario);
    CompleteSave(c5->cd(1),"Shapes2/Making_of___3jetsabove30"+writescenario+"__cd1");
    CompleteSave(c5->cd(2),"Shapes2/Making_of___3jetsabove30"+writescenario+"__cd2");
    CompleteSave(c5->cd(3),"Shapes2/Making_of___3jetsabove30"+writescenario+"__cd3");
    CompleteSave(c6,"Shapes2/Background_Shapes___3jetsabove30"+writescenario);
  } else {
    CompleteSave(c5,"Shapes2/Making_of___2jetsabove50"+writescenario);
    CompleteSave(c5->cd(1),"Shapes2/Making_of___2jetsabove50"+writescenario+"__cd1");
    CompleteSave(c5->cd(2),"Shapes2/Making_of___2jetsabove50"+writescenario+"__cd2");
    CompleteSave(c5->cd(3),"Shapes2/Making_of___2jetsabove50"+writescenario+"__cd3");
    CompleteSave(c6,"Shapes2/Background_Shapes___2jetsabove50"+writescenario);
  }
  dout << "Statistics about our fits: " << endl; 
  dout << "Z+Jets shape: Chi2/ndf = " << zjetsfunc->GetChisquare() << "/" << ossfleft->GetNbinsX() << endl;
  dout << "ttbar shape: Chi2/ndf = " << ttbarfunctions[1]->GetChisquare() << "/" << osof->GetNbinsX() << endl;

  c6->cd();
  TLegend *additionallegend = new TLegend(0.6,0.6,0.89,0.89);
  additionallegend->SetFillColor(kWhite);
  additionallegend->SetLineColor(kWhite);
  additionallegend->AddEntry(observed,"Data","p");
  additionallegend->AddEntry(ZplusJetsplusTTbar,"Fitted Z+jets & TTbar","l");
  additionallegend->AddEntry(zjetsc,"Z+jets","l");
  additionallegend->AddEntry(logparc,"TTbar","l");
  observed->Draw();
  ZplusJetsplusTTbar->SetLineColor(kGreen);
  ZplusJetsplusTTbarP->SetLineColor(kGreen);
  ZplusJetsplusTTbarN->SetLineColor(kGreen);
  ZplusJetsplusTTbarP->SetLineStyle(2);
  ZplusJetsplusTTbarN->SetLineStyle(2);
  TF1 *ZplusJetsplusTTbar2 = new TF1("ZplusJetsplusTTbar2",DoubleInvCrystalBall,0,1000,10);
  ZplusJetsplusTTbar2->SetParameters(ZplusJetsplusTTbar->GetParameters());
  ZplusJetsplusTTbar2->SetLineColor(kGreen);
  ZplusJetsplusTTbarP->SetFillColor(TColor::GetColor("#81F781"));
  ZplusJetsplusTTbarN->SetFillColor(kWhite);
  ZplusJetsplusTTbarP->Draw("fcsame");
  ZplusJetsplusTTbarN->Draw("fcsame");
  TH1F *hZplusJetsplusTTbar = (TH1F*)ZplusJetsplusTTbar2->GetHistogram();
  TH1F *hZplusJetsplusTTbarN = (TH1F*)ZplusJetsplusTTbarN->GetHistogram();
  TH1F *hZplusJetsplusTTbarP = (TH1F*)ZplusJetsplusTTbarP->GetHistogram();
  hZplusJetsplusTTbar->SetMarkerSize(0);
  hZplusJetsplusTTbarP->SetMarkerSize(0);
  hZplusJetsplusTTbarN->SetMarkerSize(0);
  for (int i=1;i<=hZplusJetsplusTTbar->GetNbinsX();i++) {
    float newerror=hZplusJetsplusTTbarP->GetBinContent(i)-hZplusJetsplusTTbar->GetBinContent(i);
    hZplusJetsplusTTbar->SetBinError(i,newerror);
    if(hZplusJetsplusTTbar->GetBinContent(i)<0.05) hZplusJetsplusTTbar->SetBinContent(i,0); //avoiding a displaying probolem
  }
  hZplusJetsplusTTbarP->SetFillColor(kGreen);
  hZplusJetsplusTTbarN->SetFillColor(kWhite);
  hZplusJetsplusTTbarN->Draw("same");
  
  ZplusJetsplusTTbar2->SetMarkerSize(0);
  ZplusJetsplusTTbar2->Draw("same");

  zjetsc->Draw("same");zjetscn->Draw("same");zjetscp->Draw("same");
  logparc->Draw("same");
  logparcn->Draw("same");
  logparcp->Draw("same");
  additionallegend->Draw("same");
    if(scenario==0) {
    CompleteSave(c6,"Shapes2/Background_Shapes___3jetsabove30__allfits__"+writescenario);
  } else {
    CompleteSave(c6,"Shapes2/Background_Shapes___2jetsabove50__allfits__"+writescenario);
  }
//--------------------------------------------------------------------------------------------------------------------------------
}

void draw_ttbar_and_zjets_shape(string mcjzb, string datajzb) {
  int all_leptons=-1;
  int electrons_only=0;
  int mu_only=1;
  int twojetswith50gev=1;
  int threejetswith30gev=0;
/*  
  draw_ttbar_and_zjets_shape_for_one_configuration(mcjzb,datajzb,all_leptons,twojetswith50gev);
  draw_ttbar_and_zjets_shape_for_one_configuration(mcjzb,datajzb,all_leptons,threejetswith30gev);
  
  draw_ttbar_and_zjets_shape_for_one_configuration(mcjzb,datajzb,electrons_only,twojetswith50gev);
  draw_ttbar_and_zjets_shape_for_one_configuration(mcjzb,datajzb,electrons_only,threejetswith30gev);

  draw_ttbar_and_zjets_shape_for_one_configuration(mcjzb,datajzb,mu_only,twojetswith50gev);
  draw_ttbar_and_zjets_shape_for_one_configuration(mcjzb,datajzb,mu_only,threejetswith30gev);
  */

  draw_ttbar_and_zjets_shape_for_one_configuration(mcjzb,datajzb,all_leptons,threejetswith30gev,true);
}

void find_correction_factors(string &jzbvardata,string &jzbvarmc) {
  //first: colorful plots
  TCanvas *cancorr = new TCanvas("cancorr","Canvas for Response Correction");
  cancorr->SetLogz();
  cancorr->SetRightMargin(0.13);
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  TCut zptforresponsepresentation("pt<600"&&cutmass&&cutOSSF&&"((sumJetPt[1]/pt)<5.0)");
  TH2F *niceresponseplotd = new TH2F("niceresponseplotd","",100,0,600,100,0,5);
  (allsamples.collection)[allsamples.FindSample("Data")[0]].events->Draw("sumJetPt[1]/pt:pt>>niceresponseplotd",zptforresponsepresentation);
  niceresponseplotd->SetStats(0);
  niceresponseplotd->GetXaxis()->SetTitle("Z p_{T} [GeV]");
  niceresponseplotd->GetYaxis()->SetTitle("Response");
  niceresponseplotd->GetXaxis()->CenterTitle();
  niceresponseplotd->GetYaxis()->CenterTitle();
  niceresponseplotd->Draw("COLZ");
  TProfile * profd = (TProfile*)niceresponseplotd->ProfileX();
  profd->SetMarkerSize(DataMarkerSize);
  profd->Fit("pol0","","same,e1",30,400);
  DrawPrelim();
  TText* title = write_text(0.5,0.7,"Data");
  title->SetTextAlign(12);
  title->Draw();
  TF1 *datapol=(TF1*)profd->GetFunction("pol0");
  float datacorrection=datapol->GetParameter(0);
  stringstream dataresstring;
  dataresstring<<"Response: "<<std::setprecision(2)<<100*datacorrection<<" %";
  TText* restitle = write_text(0.5,0.65,dataresstring.str());
  restitle->SetTextAlign(12);
  restitle->SetTextSize(0.03);
  restitle->Draw();
  CompleteSave(cancorr,"ResponseCorrection/Response_Correction_Illustration_Data");
  
  TH2F *niceresponseplotm = new TH2F("niceresponseplotm","",100,0,600,100,0,5);
  (allsamples.collection)[allsamples.FindSample("DY")[0]].events->Draw("sumJetPt[1]/pt:pt>>niceresponseplotm",zptforresponsepresentation);
  niceresponseplotm->SetStats(0);
  niceresponseplotm->GetXaxis()->SetTitle("Z p_{T} [GeV]");
  niceresponseplotm->GetYaxis()->SetTitle("Response");
  niceresponseplotm->GetXaxis()->CenterTitle();
  niceresponseplotm->GetYaxis()->CenterTitle();
  niceresponseplotm->Draw("COLZ");
  (allsamples.collection)[allsamples.FindSample("DY")[0]].events->Draw("sumJetPt[1]/pt:pt",zptforresponsepresentation,"PROF,same");
  TProfile * profm = (TProfile*)niceresponseplotm->ProfileX();
  profm->SetMarkerSize(DataMarkerSize);
  profm->Fit("pol0","","same,e1",30,400);
  DrawMCPrelim();
  title = write_text(0.5,0.7,"MC simulation");
  title->SetTextAlign(12);
  title->Draw();
  TF1 *mcpol=(TF1*)profm->GetFunction("pol0");
  float mccorrection=mcpol->GetParameter(0);
  stringstream mcresstring;
  mcresstring<<"Response: "<<std::setprecision(2)<<100*mccorrection<<" %";
  TText* mcrestitle = write_text(0.5,0.65,mcresstring.str());
  mcrestitle->SetTextAlign(12);
  mcrestitle->SetTextSize(0.03);
  mcrestitle->Draw();
  CompleteSave(cancorr,"ResponseCorrection/Response_Correction_Illustration_MC");
  
  
  //Step 2: Getting the result
//  TCut zptcutforresponse("pt>30&&pt<300&&TMath::Abs(91.2-mll)<20&&id1==id2&&(ch1*ch2<0)");
  stringstream jzbvardatas;
  if(datacorrection>1) jzbvardatas<<"(jzb[1]-"<<datacorrection-1<<"*pt)";
  if(datacorrection<1) jzbvardatas<<"(jzb[1]+"<<1-datacorrection<<"*pt)";
  jzbvardata=jzbvardatas.str();
  stringstream jzbvarmcs;
  if(mccorrection>1) jzbvarmcs<<"(jzb[1]-"<<mccorrection-1<<"*pt)";
  if(mccorrection<1) jzbvarmcs<<"(jzb[1]+"<<1-mccorrection<<"*pt)";
  jzbvarmc=jzbvarmcs.str();
  dout << "JZB Z pt correction summary : " << endl;
  dout << "  Data: The response is " << datacorrection << "  --> jzb variable is now : " << jzbvardata << endl;
  dout << "  MC  : The response is " << mccorrection << "  --> jzb variable is now : " << jzbvarmc << endl;
}

void pick_up_events(string cut) {
  dout << "Picking up events with cut " << cut << endl;
  allsamples.PickUpEvents(cut);
}

void save_template(string mcjzb, string datajzb,vector<float> jzb_cuts,float MCPeakError) {
  cout << "Saving configuration template!" << endl;
  ofstream configfile;
  configfile.open("../DistributedModelCalculations/last_configuration.C");
  configfile<<"#include <iostream>\n";
  configfile<<"#include <vector>\n";
  configfile<<"\nusing namespace std;\n\n";
  
  configfile<<"namespace PlottingSetup { \n";
  configfile<<"string datajzb=\"datajzb_ERROR\";\n";
  configfile<<"string mcjzb=\"mcjzb_ERROR\";\n";
  configfile<<"vector<float>jzb_cuts;\n";
  configfile<<"float MCPeakError=-999;\n";
  configfile<<"}\n\n";

  
  
  configfile<<"void read_config() {\n";
  configfile<<"datajzb=\""<<datajzb<<"\";\n";
  configfile<<"mcjzb=\""<<mcjzb<<"\";\n\n";
  configfile<<"\n\nMCPeakError="<<MCPeakError<<";\n\n";
  for(int i=0;i<jzb_cuts.size();i++) configfile<<"jzb_cuts.push_back("<<jzb_cuts[i]<<");\n";
  configfile<<"\n\n";
  for(int i=0;i<Nobs.size();i++) configfile<<"Nobs.push_back("<<Nobs[i]<<");\n";
  for(int i=0;i<Npred.size();i++) configfile<<"Npred.push_back("<<Npred[i]<<");\n";
  for(int i=0;i<Nprederr.size();i++) configfile<<"Nprederr.push_back("<<Nprederr[i]<<");\n";
  configfile<<"\n\n";
  configfile<<"luminosity="<<luminosity<<";\n";
  
  configfile<<"\n\ncout << \"Configuration successfully loaded!\" << endl; \n \n } \n \n";
  
  configfile.close();

}

float get_nonzero_minimum(TH1F *histo) {
  float min=histo->GetMaximum();
  for(int ibin=1;ibin<=histo->GetNbinsX();ibin++) {
    float curcont=histo->GetBinContent(ibin);
    if(curcont<min&&curcont>0) min=curcont;
  }
  return min;
}

void draw_all_ttbar_histos(TCanvas *can, vector<TH1F*> histos, string drawoption="", float manualmin=-9) {
  can->cd();
  float min=1;
  float max=histos[0]->GetMaximum();
  if(manualmin>=0) min=manualmin;
  else {
    for(int i=1;i<histos.size();i++) {
      float curmin=get_nonzero_minimum(histos[i]);
      float curmax=histos[i]->GetMaximum();
      if(curmin<min) min=curmin;
      if(curmax>max) max=curmax;
    }
  }
  histos[0]->GetYaxis()->SetRangeUser(min,4*max);
  histos[0]->Draw(drawoption.c_str());
  stringstream drawopt;
  drawopt << drawoption << ",same";
  for(int i=1;i<histos.size();i++) {
    histos[i]->Draw(drawopt.str().c_str());
  }
}

void ttbar_sidebands_comparison(string mcjzb, vector<float> jzb_binning) {
  flag_this_change(__FUNCTION__,__LINE__,true);//PlottingSetup::RestrictToMassPeak
  if(!PlottingSetup::RestrictToMassPeak) {
    write_info(__FUNCTION__,"Calling this function for the offpeak analysis is pointless ... ");
    return; // pointless for offpeak analysis
  }

  TCut weightbackup=cutWeight;
  cutWeight="1.0";
  float sbg_min=-120.;
  float sbg_max=380.;
  int sbg_nbins=(sbg_max-sbg_min)/25.0; // 25 GeV bins
  float simulatedlumi = luminosity; //in pb please - adjust to your likings
  

  TH1F *TZem = allsamples.Draw("TZem",mcjzb,sbg_nbins,sbg_min,sbg_max,"JZB [GeV]","events",cutmass&&cutOSOF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("TTJets"));
  TH1F *TSem = allsamples.Draw("TSem",mcjzb,sbg_nbins,sbg_min,sbg_max,"JZB [GeV]","events",sidebandcut&&cutOSOF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("TTJets"));
  TH1F *TZeemm = allsamples.Draw("TZeemm",mcjzb,sbg_nbins,sbg_min,sbg_max,"JZB [GeV]","events",cutmass&&cutOSSF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("TTJets"));
  TH1F *TSeemm = allsamples.Draw("TSeemm",mcjzb,sbg_nbins,sbg_min,sbg_max,"JZB [GeV]","events",sidebandcut&&cutOSSF&&cutnJets,mc,simulatedlumi,allsamples.FindSample("TTJets"));
 

  TCanvas *tcan = new TCanvas("tcan","tcan");
  
  tcan->SetLogy(1);
  
  TZeemm->SetLineColor(kBlack);
  TZem->SetLineColor(kRed);
  TSem->SetLineColor(kGreen);
  TSeemm->SetLineColor(kBlue);
  
  TZeemm->SetMarkerColor(kBlack);
  TZem->SetMarkerColor(kRed);
  TSem->SetMarkerColor(kGreen);
  TSeemm->SetMarkerColor(kBlue);

  TSem->SetLineStyle(2);
  TSeemm->SetLineStyle(2);
  
  vector<TH1F*> histos;
  histos.push_back(TZem);
  histos.push_back(TZeemm);
  histos.push_back(TSem);
  histos.push_back(TSeemm);
  draw_all_ttbar_histos(tcan,histos,"histo",0.04);
  
  TLegend *leg = make_legend("MC ttbar",0.6,0.65,false);
  leg->AddEntry(TZeemm,"SFZP","l");
  leg->AddEntry(TZem,"OFZP","l");
  leg->AddEntry(TSeemm,"SFSB","l");
  leg->AddEntry(TSem,"OFSB","l");
  leg->Draw("same");
  DrawMCPrelim(simulatedlumi);
  CompleteSave(tcan,"ttbar_comparison");
  
  TZem->Rebin();
  TSem->Rebin();
  TSeemm->Rebin();
  TZeemm->Rebin();

  TZem->Divide(TZeemm);
  TSem->Divide(TZeemm);
  TSeemm->Divide(TZeemm);
  
  TZem->SetMarkerStyle(21);
  TSem->SetMarkerStyle(24);
  TSeemm->SetMarkerStyle(30);


  tcan->SetLogy(0);
  TZem->GetYaxis()->SetRangeUser(0,2.5);
  TZem->GetYaxis()->SetTitle("predicted/MC truth");
  TZem->Draw();
  TSem->Draw("same");
  TSeemm->Draw("same");
  
  TLine *top = new TLine(sbg_min,1.5,sbg_max,1.5);
  TLine *center = new TLine(sbg_min,1.0,sbg_max,1.0);
  TLine *bottom = new TLine(sbg_min,0.5,sbg_max,0.5);
  
  top->SetLineColor(kBlue);top->SetLineStyle(2);
  bottom->SetLineColor(kBlue);bottom->SetLineStyle(2);
  center->SetLineColor(kBlue);
  
  top->Draw("same");
  center->Draw("same");
  bottom->Draw("same");
  
  TLegend *leg2 = make_legend("MC ttbar",0.55,0.75,false);
  leg2->AddEntry(TZem,"OFZP / SFZP","ple");
  leg2->AddEntry(TSeemm,"SFSB / SFZP","ple");
  leg2->AddEntry(TSem,"OFSB / SFZP","ple");
  leg2->AddEntry(bottom,"syst. envelope","l");
  leg2->SetX1(0.25);leg2->SetX2(0.6);
  leg2->SetY1(0.65);
  
  leg2->Draw("same");
  
  DrawMCPrelim(simulatedlumi);
  CompleteSave(tcan,"ttbar_comparison_ratio");
  
  delete tcan;
  cutWeight=weightbackup;
}

void zjets_prediction_comparison(string mcjzb) {
  TCut weightbackup=cutWeight;
  cutWeight="1.0";
  float sbg_min=0.;
  float sbg_max=100.;
  int sbg_nbins=5;
  float simulatedlumi = luminosity;//in pb please - adjust to your likings
  
  TCut kPos((mcjzb+">0").c_str());
  TCut kNeg((mcjzb+"<0").c_str());
  string var( "abs("+mcjzb+")" );

  TCut kcut(cutmass&&cutOSSF&&"pfJetGoodNum>2");
  TH1F *hJZBpos = allsamples.Draw("hJZBpos",var,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",
                                  kcut&&kPos,mc,simulatedlumi,allsamples.FindSample("DYJets"));
  TH1F *hJZBneg = allsamples.Draw("hJZBneg",var,sbg_nbins,sbg_min,sbg_max, "JZB [GeV]", "events",
                                  kcut&&kNeg,mc,simulatedlumi,allsamples.FindSample("DYJets"));
  hJZBpos->SetLineColor(kBlack);
  hJZBneg->SetLineColor(kRed);
  
  Int_t nbins = 5;
  Float_t xmax = 100.;

  
  TCanvas *zcan = new TCanvas("zcan","zcan");  
  zcan->SetLogy(1);

  hJZBpos->Draw("e1");
  hJZBneg->Draw("same,hist");
  hJZBpos->Draw("same,e1"); // So it's on top...
  
  TLegend *leg = make_legend("MC Z+jets",0.55,0.75,false);
  leg->AddEntry(hJZBpos,"Observed","pe");
  leg->AddEntry(hJZBneg,"Predicted","l");
  leg->Draw("same");
  DrawMCPrelim(simulatedlumi);
  CompleteSave(zcan,"zjets_prediction");
  
  TH1F* hratio = (TH1F*)hJZBpos->Clone("hratio");
  hratio->Divide(hJZBneg);
  
  zcan->SetLogy(0);
  hratio->GetYaxis()->SetRangeUser(0,2.5);
  hratio->GetYaxis()->SetTitle("Observed/Predicted");
  hratio->Draw("e1");
  
  TLine *top = new TLine(sbg_min,1.25,sbg_max,1.25);
  TLine *center = new TLine(sbg_min,1.0,sbg_max,1.0);
  TLine *bottom = new TLine(sbg_min,0.75,sbg_max,0.75);
  

  top->SetLineColor(kBlue);top->SetLineStyle(2);
  bottom->SetLineColor(kBlue);bottom->SetLineStyle(2);
  center->SetLineColor(kBlue);
  
  top->Draw("same");
  center->Draw("same");
  bottom->Draw("same");
  
  TLegend *leg2 = make_legend("MC Z+jets",0.25,0.75,false);
  leg2->AddEntry(hratio,"obs / pred","pe");
  leg2->AddEntry(bottom,"syst. envelope","l");
  leg2->Draw("same");
  DrawMCPrelim(simulatedlumi);
  CompleteSave(zcan,"zjets_prediction_ratio");
  
  delete zcan;
  cutWeight=weightbackup;
}


void test() {
  
  TCanvas *testcanv = new TCanvas("testcanv","testcanv");
  testcanv->cd();
  switch_overunderflow(true);
  TH1F *ptdistr   = allsamples.Draw("ptdistr","pt1",100,30,200, "p_{T} [GeV]", "events", cutOSSF,data,luminosity);
  switch_overunderflow(false);
  ptdistr->Draw();
  testcanv->SaveAs("test.png");
  dout << "HELLO there!" << endl;
  
}
