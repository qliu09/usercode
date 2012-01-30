#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TCut.h>
#include <THStack.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TError.h>
#include <TText.h>
#include <TLegend.h>
#include <TError.h>
#include <TTreeFormula.h>
#include <TDCacheFile.h>

#define SampleClassLoaded

#ifndef Verbosity
#define Verbosity 0
#endif
#ifndef HUSH
#define HUSH 1
#endif
#ifndef GeneralToolBoxLoaded
#include "GeneralToolBox.C"
#endif
using namespace std;

bool doesROOTFileExist(string filename);
bool addoverunderflowbins=false;
string treename="PFevents";

class sample
{
public:
  string filename;
  string samplename;
  long Nentries;
  float xs;//cross section
  float weight;
  bool is_data;
  bool is_signal;
  bool is_active;
  int groupindex;
  Color_t samplecolor;
  
  TFile *tfile;
  TTree *events;
  
  sample(string m_filename,string m_samplename,long m_Nentries, float m_xs,bool is_data, bool m_is_signal, int m_groupindex, Color_t color);
  void closeFile();
};

string write_mc_or_data(bool is_data)
{
  if(is_data) return "data";
  return "MC";
}

sample::sample(string m_filename, string m_samplename, long m_Nentries, float m_xs,bool m_is_data, bool m_is_signal, int m_groupindex, Color_t mycolor)
{
  this->filename=m_filename;
  this->samplename=m_samplename;
  this->Nentries=m_Nentries;
  this->xs=m_xs;
  this->is_data=m_is_data;
  this->is_signal=m_is_signal;
  this->groupindex=m_groupindex;
  this->is_active=true;
  this->samplecolor=mycolor;
  if(!doesROOTFileExist(this->filename)) {
    stringstream message;
    message << "The " << write_mc_or_data(is_data) << " sample " << this->samplename << " is invalid because the associated file path, " << this->filename << " is incorrect.";
    write_error("Sample::Sample",message.str());
    this->is_active=false;
  }
    
  if(doesROOTFileExist(this->filename)) {
    //suppressing  stupid 64/32 errors here (Warning in <TFile::ReadStreamerInfo>: /scratch/buchmann/MC_Spring11_PU_PF/TToBLNu_TuneZ2_t-channel_7TeV-madgraph.root: not a TStreamerInfo object)
    Int_t currlevel=gErrorIgnoreLevel;
    gErrorIgnoreLevel=5000;
    this->tfile = TFile::Open(m_filename.c_str(),"read");
    gErrorIgnoreLevel=currlevel;
    this->events=(TTree*)(this->tfile)->Get(treename.c_str());
    if(Verbosity>0) dout << "The " << write_mc_or_data(is_data) << " file " << this->filename << " has been added successfully to the list of samples. " << endl;
    long long measured_nevents=(this->events)->GetEntries();
    if(((this->Nentries>1)||(this->Nentries==0))&measured_nevents!=this->Nentries) {
      //special cases: m_Nentries=1 : we want to give each event the full weight (->scans!)
      //               m_Nentries=0: detect the number of events and set the nevents automatically

      stringstream warning;
      warning << "Detected incorrect number of events in sample initialization of sample " << m_filename << " (detected Nevents: " << measured_nevents << " , definition claims: " << this->Nentries << "; will use measured number of events. If you want to use this algorithm to set the number of events anyway, set the number of events to 0.";
      if(m_Nentries>1) write_warning(__FUNCTION__,warning.str());
      this->Nentries=measured_nevents;
    }
    this->weight=(xs/(float)Nentries);
  }
  else {
    this->is_active=false;
  }
  
}

void sample::closeFile()
{
  if(doesROOTFileExist(this->filename)) {
    (this->tfile)->Close();
  }
  else {
    dout << "SAMPLE " << this->samplename << " cannot be closed as the underlying file (" << this->filename << ") does not exist!" << endl;
    this->is_active=false;
  }
}

bool doesROOTFileExist(string filename)
{
  //suppressing  stupid 64/32 errors here (Warning in <TFile::ReadStreamerInfo>: /scratch/buchmann/MC_Spring11_PU_PF/TToBLNu_TuneZ2_t-channel_7TeV-madgraph.root: not a TStreamerInfo object)
  Int_t currlevel=gErrorIgnoreLevel;
  gErrorIgnoreLevel=5000;
  TFile *f = TFile::Open(filename.c_str(),"read");
  
  if (!f) return 0;

  f->Close();
  gErrorIgnoreLevel=currlevel;
  return 1;
}
//********************************************************

TCut essentialcut("mll>0");
// The following cut (cutWeight) will reweight all the events: use "weight" to correct MC for pileUP, "1.0" otherwise
TCut cutWeight("weight");

void setessentialcut(TCut ess) {
  essentialcut=ess;
}

class samplecollection
{
public:
  string name;
  vector<sample> collection;
  int nsamples;
  int ndatasamples;
  int nmcsamples;
  int ngroups;
  
  samplecollection(string m_name);
  void AddSample(string m_filename,string m_samplename,long m_Nentries, float m_xs,bool is_data, bool m_is_signal, int groupindex, Color_t color);

  //vector quantities
  TH1F* Draw(string m_histoname,string m_var, vector<float> binning, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, vector<int> onlyindex, bool drawsignal);
  TH1F* Draw(string m_histoname,string m_var, vector<float> binning, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, bool drawsignal);
  
  //minx,maxs quantities
  TH1F* Draw(string m_histoname,string m_var, int m_nbins, float m_minx, float m_maxx, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, bool drawsignal);
  TH1F* Draw(string m_histoname,string m_var, int m_nbins, float m_minx, float m_maxx, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, vector<int> onlyindex, bool drawsignal);
  
  TH2F* Draw(string m_histoname,string m_var, vector<float> binningx, vector<float> binningy, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, vector<int> onlyindex, bool drawsignal);
  
  vector<float> get_optimal_binsize(string variable, TCut cut,int nbins, float low, float high);
    
  THStack DrawStack(string m_histoname,string m_var, int m_nbins, float m_minx, float m_maxx, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, bool drawsignal);
  THStack DrawStack(string m_histoname,string m_var, vector<float> binning, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, bool drawsignal);
  vector<int> FindSample(string what);
  void ListSamples();
  bool do_sample(int thissample, vector<int> &selected_samples);
  Color_t GetColor(string filename);
  Color_t GetColor(int sampleindex);
  
  TLegend* allbglegend(string title,float x, float y);
  TLegend* allbglegend(string title, TH1F *data, float x, float y);
  
  void PickUpFromThisFile(int isamp, string cut, vector<string> &output, vector<string> &pickupfile);
  void PickUpEvents(string cut);
  string find_units(string&);

  void RemoveLastSample();
};

samplecollection::samplecollection(string m_name)
{
  this->name=m_name;
  if(Verbosity>0) dout << "Initiated sample collection " << this->name << " " << endl;
}

void samplecollection::ListSamples()
{
  dout << "---------------------------------------------------------------------------------------------------" << endl;
  dout << "Listing all " << this->nsamples << " sample(s) of the sample collection called " << this->name << " : " << endl;
  if(this->ndatasamples>0) {
    dout << "Data sample(s): " << endl;
    for (int isamp=0;isamp<this->collection.size();isamp++)
      {
	if((this->collection)[isamp].is_data) dout << " - " << (this->collection)[isamp].samplename << " from " << (this->collection)[isamp].filename << endl;
      }
  }
  if(this->nmcsamples>0) {
    dout << "MC sample(s): " << endl;
    for (int isamp=0;isamp<this->collection.size();isamp++)
      {
	if(!(this->collection)[isamp].is_data) dout << " - " << (this->collection)[isamp].samplename << " from " << (this->collection)[isamp].filename << endl;
      }
  }
  dout << "---------------------------------------------------------------------------------------------------" << endl;
}
    
void samplecollection::AddSample(string m_filename,string m_samplename,long m_Nentries, float m_xs,bool is_data, bool m_is_signal, int groupindex, Color_t newcolor)
{
  sample NewSample(m_filename,m_samplename,m_Nentries,m_xs,is_data,m_is_signal,groupindex,newcolor);
  (this->collection).push_back(NewSample);
  if((this->collection).size()==1) {
    this->nmcsamples=0;
    this->ndatasamples=0;
    this->nsamples=0;
    this->ngroups=0;
  }
  if(groupindex>this->ngroups) this->ngroups=groupindex;
  if(is_data) this->ndatasamples++;
  else this->nmcsamples++;
  this->nsamples=this->collection.size();
}

bool samplecollection::do_sample(int thissample, vector<int> &selected_samples)
{
  bool drawit=false;
  for(int isel=0;isel<selected_samples.size();isel++)
  {
    if(selected_samples[isel]==thissample) {
      drawit=true;
      break;
    }
  }
  return drawit;
}
  
TH1F* samplecollection::Draw(string m_histoname,string m_var, int m_nbins, float m_minx, float m_maxx, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, bool drawsignal=false)
{
  vector<int> emptyvector;
  TH1F *histo = this->Draw(m_histoname,m_var, m_nbins, m_minx, m_maxx, m_xlabel, m_ylabel, Cut, m_is_data, luminosity,emptyvector,drawsignal);
  return histo;
}
  
TH1F* samplecollection::Draw(string m_histoname,string m_var, vector<float> binning, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, bool drawsignal=false)
{
  vector<int> emptyvector;
  TH1F *histo = this->Draw(m_histoname,m_var, binning, m_xlabel, m_ylabel, Cut, m_is_data, luminosity,emptyvector,drawsignal);
  return histo;
}
  
TH1F* samplecollection::Draw(string m_histoname,string m_var, vector<float> binning, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, vector<int> onlyindex, bool drawsignal=false) {
  if(Verbosity>0) dout << endl << endl;
  if(Verbosity>0) dout << "-------------------------------------------------------------------------------------" << endl;
  if(Verbosity>0) dout << "histoname : " << m_histoname << " , m_var = " << m_var << ", m_xlabel=" << m_xlabel << " m_ylabel=" << m_ylabel << " m_is_data = " << m_is_data << " lumi = " << luminosity << " onlyindex size: " << onlyindex.size() << endl;
  if(HUSH==0) dout << "Drawing histo called " << m_histoname << "... " << endl;
  bool do_only_selected_samples=false;
  if(onlyindex.size()>0&&onlyindex[0]!=-1) {
    if(Verbosity>0) {dout << "Requested to only draw sample corresponding to the following sample(s) : " << endl;}
    for(int is=0;is<onlyindex.size();is++) {
      if(Verbosity>0) dout << "   - " << (this->collection)[onlyindex[is]].filename << " (sample index: " << onlyindex[is] << ")" << endl;
    }
    do_only_selected_samples=true;
  }
  if(onlyindex.size()==1&&onlyindex[0]==-1) {
    do_only_selected_samples=true; // this is the special case when we request a non-existing sample - we shouldn't draw anything then.
    onlyindex.clear();
  }
  stringstream h_histoname;
  h_histoname<<"h_"<<m_histoname;
  float binningarray[binning.size()+1];
  for(int i=0;i<binning.size();i++) {binningarray[i]=binning[i];}
  TH1F *histo = new TH1F(m_histoname.c_str(),"",binning.size()-1,binningarray);
  histo->Sumw2();
  
  stringstream drawthis;
  drawthis<<m_var<<">>tempdrawhisto";
  
  for (unsigned int isample=0;isample<(this->collection).size();isample++) {
    bool use_this_sample=false;
    if(!(this->collection)[isample].is_active) continue; // skip inactive samples right away
    if(((this->collection)[isample].is_data==m_is_data)&&(!do_only_selected_samples)) {
      if(drawsignal==false&&(this->collection)[isample].is_signal==true) continue;
      use_this_sample=true;
    }
    if(do_only_selected_samples&&this->do_sample(isample,onlyindex)) {
      use_this_sample=true;
    }
    TH1F *tempdrawhisto = new TH1F("tempdrawhisto","tempdrawhisto",binning.size()-1,binningarray);
    tempdrawhisto->Sumw2();
    if(use_this_sample) {
      if(Verbosity>0) dout << "[samplecollection::Draw] : Added contribution from sample " << (this->collection)[isample].samplename << endl;
      (this->collection)[isample].events->Draw(drawthis.str().c_str(),(essentialcut&&Cut)*cutWeight);//this weight is based on PU etc. not XS
      if(addoverunderflowbins) {
	//now also adding the overflow & underflow bins:
	tempdrawhisto->SetBinContent(tempdrawhisto->GetNbinsX(),tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX()+1)+tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX()));
	tempdrawhisto->SetBinError(tempdrawhisto->GetNbinsX(),TMath::Sqrt(tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX())));
	tempdrawhisto->SetBinContent(1,tempdrawhisto->GetBinContent(0)+tempdrawhisto->GetBinContent(1));
	tempdrawhisto->SetBinError(1,TMath::Sqrt(tempdrawhisto->GetBinContent(1)));
      }

      if(!(this->collection)[isample].is_data) tempdrawhisto->Scale(luminosity*((this->collection)[isample].weight));//weight applied here is XS & N(entries)
      histo->Add(tempdrawhisto);
    }
    tempdrawhisto->Delete();
  }//end of loop over isample
  if(Verbosity>0) dout << "Histo has been filled and now contains " << histo->Integral() << " points (integral)" << endl;
  histo->GetXaxis()->SetTitle(m_xlabel.c_str());
  // Try to add bin width information: look for units in m_xlabel
  string units = find_units(m_xlabel);
  if ( units.length()>0 ) {
    stringstream ylabel;
    ylabel << m_ylabel << " / " << histo->GetBinWidth(1) << " " << units;
    histo->GetYaxis()->SetTitle( ylabel.str().c_str() );
  } else {
    histo->GetYaxis()->SetTitle(m_ylabel.c_str());
  }
  histo->GetXaxis()->CenterTitle();
  histo->GetYaxis()->CenterTitle();
  if(do_only_selected_samples) histo->SetLineColor(this->GetColor(onlyindex[0]));
  return histo;
}

TH2F* samplecollection::Draw(string m_histoname,string m_var, vector<float> binningx, vector<float> binningy, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, vector<int> onlyindex, bool drawsignal=false) {
  if(Verbosity>0) dout << endl << endl;
  if(Verbosity>0) dout << "-------------------------------------------------------------------------------------" << endl;
  if(Verbosity>0) dout << "histoname : " << m_histoname << " , m_var = " << m_var << ", m_xlabel=" << m_xlabel << " m_ylabel=" << m_ylabel << " m_is_data = " << m_is_data << " lumi = " << luminosity << " onlyindex size: " << onlyindex.size() << endl;
  if(HUSH==0) dout << "Drawing histo called " << m_histoname << "... " << endl;
  bool do_only_selected_samples=false;
  if(onlyindex.size()>0&&onlyindex[0]!=-1) {
    if(Verbosity>0) {dout << "Requested to only draw sample corresponding to the following sample(s) : " << endl;}
    for(int is=0;is<onlyindex.size();is++) {
      if(Verbosity>0) dout << "   - " << (this->collection)[onlyindex[is]].filename << " (sample index: " << onlyindex[is] << ")" << endl;
    }
    do_only_selected_samples=true;
  }
  if(onlyindex.size()==1&&onlyindex[0]==-1) {
    do_only_selected_samples=true; // this is the special case when we request a non-existing sample - we shouldn't draw anything then.
    onlyindex.clear();
  }
  stringstream h_histoname;
  h_histoname<<"h_"<<m_histoname;
  float binningarray[binningx.size()+1];
  float binningyarray[binningy.size()+1];
  for(int i=0;i<binningx.size();i++) {binningarray[i]=binningx[i];binningyarray[i]=binningy[i];}
  TH2F *histo = new TH2F(m_histoname.c_str(),"",binningx.size()-1,binningarray,binningy.size(),binningyarray);
  histo->Sumw2();
  
  stringstream drawthis;
  drawthis<<m_var<<">>tempdrawhisto";
  
  for (unsigned int isample=0;isample<(this->collection).size();isample++) {
    bool use_this_sample=false;
    if(!(this->collection)[isample].is_active) continue; // skip inactive samples right away
    if(((this->collection)[isample].is_data==m_is_data)&&(!do_only_selected_samples)) {
      if(drawsignal==false&&(this->collection)[isample].is_signal==true) continue;
      use_this_sample=true;
    }
    if(do_only_selected_samples&&this->do_sample(isample,onlyindex)) {
      use_this_sample=true;
    }
    TH2F *tempdrawhisto = new TH2F("tempdrawhisto","tempdrawhisto",binningx.size()-1,binningarray,binningy.size()-1,binningyarray);
    tempdrawhisto->Sumw2();
    if(use_this_sample) {
      if(Verbosity>0) dout << "[samplecollection::Draw] : Added contribution from sample " << (this->collection)[isample].samplename << endl;
      (this->collection)[isample].events->Draw(drawthis.str().c_str(),(essentialcut&&Cut)*cutWeight);//this weight is based on PU etc. not XS
      if(addoverunderflowbins) {
	//now also adding the overflow & underflow bins:
	tempdrawhisto->SetBinContent(tempdrawhisto->GetNbinsX(),tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX()+1)+tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX()));
	tempdrawhisto->SetBinError(tempdrawhisto->GetNbinsX(),TMath::Sqrt(tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX())));
	tempdrawhisto->SetBinContent(1,tempdrawhisto->GetBinContent(0)+tempdrawhisto->GetBinContent(1));
	tempdrawhisto->SetBinError(1,TMath::Sqrt(tempdrawhisto->GetBinContent(1)));
      }

      if(!(this->collection)[isample].is_data) tempdrawhisto->Scale(luminosity*((this->collection)[isample].weight));//weight applied here is XS & N(entries)
      histo->Add(tempdrawhisto);
    }
    tempdrawhisto->Delete();
  }//end of loop over isample
  if(Verbosity>0) dout << "Histo has been filled and now contains " << histo->Integral() << " points (integral)" << endl;
  histo->GetXaxis()->SetTitle(m_xlabel.c_str());
  histo->GetYaxis()->SetTitle(m_ylabel.c_str());
  histo->GetXaxis()->CenterTitle();
  histo->GetYaxis()->CenterTitle();
  if(do_only_selected_samples) histo->SetLineColor(this->GetColor(onlyindex[0]));
  return histo;
}


TH1F* samplecollection::Draw(string m_histoname,string m_var, int m_nbins, float m_minx, float m_maxx, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, vector<int> onlyindex, bool drawsignal=false)
{
  vector<float> binning;
  for(int i=0;i<=m_nbins;i++)
  {
    binning.push_back(((float)(m_maxx-m_minx)/((float)m_nbins))*i+m_minx);
  }
  
  TH1F *histo = this->Draw(m_histoname,m_var, binning, m_xlabel, m_ylabel, Cut, m_is_data, luminosity, onlyindex,drawsignal);
  return histo;
  }

THStack samplecollection::DrawStack(string m_histoname,string m_var, vector<float> binning, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, bool drawsignal=false) {
  stringstream h_histoname;
  h_histoname<<"h_"<<m_histoname;
  THStack thestack("thestack",m_histoname.c_str());

  stringstream drawthis;
  drawthis<<m_var<<">>tempdrawhisto";
  
  TH1F *histogroups[this->ngroups+1];
  int bookedhistos[this->ngroups+1];
  for(int ih=0;ih<=this->ngroups;ih++) bookedhistos[ih]=0;
  
  float binningarray[binning.size()+1];
  for(int i=0;i<binning.size();i++) {binningarray[i]=binning[i];}
  
  for (unsigned int isample=0;isample<(this->collection).size();isample++) {
    if(!drawsignal&&(this->collection)[isample].is_signal) continue;
    if((this->collection)[isample].is_active&&(this->collection)[isample].is_data==m_is_data) {//fills mc if we want mc, else fills data.
      TH1F *tempdrawhisto = new TH1F("tempdrawhisto","",binning.size()-1,binningarray);
      (this->collection)[isample].events->Draw(drawthis.str().c_str(),(essentialcut&&Cut)*cutWeight);
      
      if(addoverunderflowbins) {
	//now also adding the overflow & underflow bins:
	tempdrawhisto->SetBinContent(tempdrawhisto->GetNbinsX(),tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX()+1)+tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX()));
//	tempdrawhisto->SetBinError(tempdrawhisto->GetNbinsX(),TMath::Sqrt(tempdrawhisto->GetBinContent(tempdrawhisto->GetNbinsX())));  // errors not necessary for stack!
	tempdrawhisto->SetBinContent(1,tempdrawhisto->GetBinContent(0)+tempdrawhisto->GetBinContent(1));
//	tempdrawhisto->SetBinError(1,TMath::Sqrt(tempdrawhisto->GetBinContent(1)));  // errors not necessary for stack!
      }

      if(!(this->collection)[isample].is_data) tempdrawhisto->Scale(luminosity*((this->collection)[isample].weight));
      tempdrawhisto->SetFillColor(this->GetColor(isample));
      if(bookedhistos[(this->collection)[isample].groupindex]==0) {
	tempdrawhisto->SetName(GetNumericHistoName().c_str());
	histogroups[(this->collection)[isample].groupindex]=(TH1F*)tempdrawhisto->Clone();
	bookedhistos[(this->collection)[isample].groupindex]=1;
      }
      else
      {
	histogroups[(this->collection)[isample].groupindex]->Add(tempdrawhisto);
	bookedhistos[(this->collection)[isample].groupindex]++;
      }
      tempdrawhisto->Delete();
    }
  }
  vector<int> ordered_indices;
  vector<float> ordered_integrals;
  
  for(int ig=0;ig<=this->ngroups;ig++) {
    if(bookedhistos[ig]>0) { 
      ordered_indices.push_back(ig);
      ordered_integrals.push_back(histogroups[ig]->Integral());
    }
  }
  
  /*
  bubbleSort(ordered_integrals,ordered_indices);
//  for(int index=ordered_indices.size()-1;index>=0;index--) {
  for(int index=0;index<ordered_indices.size();index++) {
    thestack.Add(histogroups[ordered_indices[index]]);
  }
  */
  for(int index=0;index<ordered_indices.size();index++) {
    thestack.Add(histogroups[ordered_indices[index]]);
  }
  return thestack;
}

THStack samplecollection::DrawStack(string m_histoname,string m_var, int m_nbins, float m_minx, float m_maxx, string m_xlabel, string m_ylabel, TCut Cut, bool m_is_data, float luminosity, bool
drawsignal=false)
{
  vector<float> binning;
  for(int i=0;i<=m_nbins;i++)
  {
    binning.push_back(((float)(m_maxx-m_minx)/((float)m_nbins))*i+m_minx);
  }
  return this->DrawStack(m_histoname,m_var, binning, m_xlabel, m_ylabel, Cut, m_is_data, luminosity, drawsignal);
  }


vector<int> samplecollection::FindSample(string what)
{
  vector<int> hitcollection;
  for(unsigned int isam=0;isam<(this->collection).size();isam++)
  {
    if(((this->collection)[isam].filename).find(what)!=string::npos) {
      hitcollection.push_back(isam);
    } else {
    }
  }
  if(hitcollection.size()==0) {
    hitcollection.push_back(-1);
    write_warning(__FUNCTION__,"Couldn't find sample "+string(what)+" using sample collection \""+string(this->name)+"\"");
  }
  return hitcollection;
}

Color_t samplecollection::GetColor(string filename) {
  vector<int> corresponding_samples = this->FindSample(filename);
  if(corresponding_samples.size()>0) return this->GetColor(corresponding_samples[0]);
  return kBlack;
}

Color_t samplecollection::GetColor(int sampleindex) {
  return this->collection[sampleindex].samplecolor;
}

TLegend* samplecollection::allbglegend(string title, TH1F *data,float posx=0.65, float posy=0.60) {
//  TLegend *leg = new TLegend(0.65,0.60,0.89,0.77);
  TLegend *leg = new TLegend(posx,posy,0.89,0.89);
  if(title!="") leg->SetHeader(title.c_str());
  if(data->GetName()!="nothing") leg->AddEntry(data,"Data","p");
  leg->SetFillColor(kWhite);
  leg->SetBorderSize(0);
  leg->SetLineColor(kWhite);
  
  TH1F *fakehistos[(this->collection).size()];
  bool donealready[(this->collection).size()];
  for(int i=0;i<(this->collection).size();i++) donealready[i]=false;
  for(int isample=0;isample<(this->collection).size();isample++) {
    if((this->collection)[isample].is_data||(this->collection)[isample].is_signal) continue;

    if(!donealready[(this->collection)[isample].groupindex]) {
    fakehistos[(this->collection)[isample].groupindex] = new TH1F(GetNumericHistoName().c_str(),"",1,0,1);
    fakehistos[(this->collection)[isample].groupindex]->Fill(1);
    fakehistos[(this->collection)[isample].groupindex]->SetFillColor(this->GetColor(isample));
    leg->AddEntry(fakehistos[(this->collection)[isample].groupindex],((this->collection)[isample].samplename).c_str(),"f");
    donealready[(this->collection)[isample].groupindex]=true;
    }
  }
  DrawPrelim();
  leg->SetTextFont(42);
  return leg;
}

TLegend* samplecollection::allbglegend(string title="",float posx=0.65, float posy=0.60) {
  Int_t currlevel=gErrorIgnoreLevel;
  gErrorIgnoreLevel=5000;
  TH1F *blub = new TH1F("nothing","nothing",1,0,1);
  gErrorIgnoreLevel=currlevel;//we know this possibly replaces a previous histo, but we don't care since it's fake anyway.
  return this->allbglegend(title,blub,posx,posy);
}


void set_treename(string treen="events") {
  dout << "Treename has been set to " << treen << endl;
  if(treen=="PFevents"||treen=="events") treename=treen;
  else write_error("sample::set_treename","Setting the treename failed as you provided an invalid tree name.");
}

vector<float> samplecollection::get_optimal_binsize(string variable, TCut cut,int nbins, float low, float high) {
  TH1F *histo = this->Draw("histo",variable,5000,low,high, "finding_optimal_binsize", "events", cut,0,1000);
  float eventsperbin=histo->Integral()/nbins;
  vector<float> binning;
  binning.push_back(high);
  float runningsum=0;
  for(int i=histo->GetNbinsX();i>0;i--) {
    runningsum+=histo->GetBinContent(i);
    if(TMath::Abs(runningsum-eventsperbin)<0.05*eventsperbin||runningsum>eventsperbin) {
      binning.push_back(histo->GetBinLowEdge(i));
      runningsum=0;
    }
  }
  if(runningsum>0) binning.push_back(low);
  for(int i=0;i<int(binning.size()/2);i++) {
    float temp=binning[i];
    binning[i]=binning[binning.size()-1-i];
    binning[binning.size()-1-i]=temp;
  }
  
  return binning;
  
}

void samplecollection::PickUpFromThisFile(int isamp, string cut, vector<string> &output, vector<string> &pickupfile) {
  int lumi,eventNum,runNum;
  float jzb[30];
  (this->collection)[isamp].events->SetBranchAddress("jzb",&jzb);
  (this->collection)[isamp].events->SetBranchAddress("lumi",&runNum);
  (this->collection)[isamp].events->SetBranchAddress("eventNum",&eventNum);
  (this->collection)[isamp].events->SetBranchAddress("lumi",&lumi);
  (this->collection)[isamp].events->SetBranchAddress("runNum",&runNum);

  
  TTreeFormula *select = new TTreeFormula("select", cut.c_str()&&essentialcut, (this->collection)[isamp].events);
  int npickedup=0;
  for (Int_t entry = 0 ; entry < (this->collection)[isamp].events->GetEntries() ; entry++) {
   (this->collection)[isamp].events->LoadTree(entry);
   if (select->EvalInstance()) {
     (this->collection)[isamp].events->GetEntry(entry);
     dout << runNum << ":" << lumi << ":" << eventNum << endl;
     npickedup++;
   }
  }
  dout << "Printed out a total of " << npickedup << " events" << endl;
}

  

void samplecollection::PickUpEvents(string cut) {
  vector<string> output;
  vector<string> pickupfile;
  for (int isamp=0;isamp<this->collection.size();isamp++)
  {
    if((this->collection)[isamp].is_data) {
      //we have a data sample ! 
      this->PickUpFromThisFile(isamp,cut,output,pickupfile);
    }
  }
  //do something with output and of course the pickup file!
}

//________________________________________________________________________________________
// Find units from histogram x-label (looks for '[...]')
string samplecollection::find_units(string& xlabel) {
  
  string units;
  size_t pos1 = xlabel.find("[");
  if ( pos1 != string::npos ) {
    size_t pos2 = xlabel.find("]");
    units = xlabel.substr(pos1+1,pos2-pos1-1);
  }//  else {
//     size_t pos1 = xlabel.find("(");
//     if ( pos1 != string::npos ) {
//       size_t pos2 = xlabel.find(")");
//       units = xlabel.substr(pos1+1,pos2-pos1-1);
//     }
//   }
  return units;
  
}

void samplecollection::RemoveLastSample() {
  dout << "Removing last sample: " << ((this->collection)[(this->collection).size()-1]).filename << std::endl;
  if(((this->collection)[(this->collection).size()-1]).is_data) this->ndatasamples--;
  else this->nmcsamples--;
  this->nsamples--;
  ((this->collection)[(this->collection).size()-1]).closeFile();
  (this->collection).pop_back();
}

void switch_overunderflow(bool newpos=false) {
  addoverunderflowbins=newpos;
}

   
