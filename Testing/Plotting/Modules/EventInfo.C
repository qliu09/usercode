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
#include <sstream>

#include <TFile.h>
#include <TTree.h>
#include <TError.h>
#include <TMath.h>

using namespace std;


int main() {
  TFile *f = new TFile("/scratch/buchmann/AllData_Jun10___486pb_MoreTriggers4_DCS_incl_ch_id.root");
  TTree *PFevents = (TTree*)f->Get("PFevents");
  TTree *events = (TTree*)f->Get("events");
  
  int pfJetGoodNum,eventNum,lumi,runNum;
  float mll,jzb[30],met[30],pt1,pt2,pt;
  
  vector<string> summary;
  vector<int> VrunNum, VeventNum, Vlumi;
  
  PFevents->SetBranchAddress("pfJetGoodNum",&pfJetGoodNum);
  PFevents->SetBranchAddress("runNum",&runNum);
  PFevents->SetBranchAddress("eventNum",&eventNum);
  PFevents->SetBranchAddress("lumi",&lumi);
  PFevents->SetBranchAddress("mll",&mll);
  PFevents->SetBranchAddress("jzb",&jzb);
  PFevents->SetBranchAddress("met",&met);
  PFevents->SetBranchAddress("pt1",&pt1);
  PFevents->SetBranchAddress("pt2",&pt2);
  PFevents->SetBranchAddress("pt",&pt);
  
  
  for(long int ientry=1;ientry<=PFevents->GetEntries();ientry++) {
    PFevents->GetEntry(ientry);
    if(!(pfJetGoodNum>=3)) continue;
    if(TMath::Abs(mll-91.2)>20) continue;
    if(TMath::Abs(jzb[1])<100) continue;
    stringstream hit;
    hit << runNum <<":"<<lumi<<":"<<eventNum<<":"<<jzb[1]<<":"<<met[4]<<":"<<pt1<<":"<<pt2<<":"<<pt<<":"<<mll<<":"<<pfJetGoodNum;
    summary.push_back(hit.str());
    VrunNum.push_back(runNum);
    VeventNum.push_back(eventNum);
    Vlumi.push_back(lumi);
  }
  
  events->SetBranchAddress("pfJetGoodNum",&pfJetGoodNum);
  events->SetBranchAddress("runNum",&runNum);
  events->SetBranchAddress("eventNum",&eventNum);
  events->SetBranchAddress("lumi",&lumi);
  events->SetBranchAddress("mll",&mll);
  events->SetBranchAddress("jzb",&jzb);
  events->SetBranchAddress("met",&met);
  events->SetBranchAddress("pt1",&pt1);
  events->SetBranchAddress("pt2",&pt2);
  events->SetBranchAddress("pt",&pt);
  
  for(long int ientry=1;ientry<events->GetEntries();ientry++) {
    events->GetEntry(ientry);
    for(int jhits=0;jhits<VrunNum.size();jhits++) {
      if(runNum==VrunNum[jhits]&&eventNum==VeventNum[jhits]&&lumi==Vlumi[jhits]) {
	//present all findings here.
	cout << "PF:"<<summary[jhits]<<endl;
	cout << "RC:"<<runNum <<":"<<lumi<<":"<<eventNum<<":"<<jzb[1]<<":"<<met[4]<<":"<<pt1<<":"<<pt2<<":"<<pt<<":"<<mll<<":"<<pfJetGoodNum<<endl;
/*	VrunNum.erase(VrunNum.begin()+jhits);
	VeventNum.erase(VeventNum.begin()+jhits);
	Vlumi.erase(Vlumi.begin()+jhits);
	summary.erase(summary.begin()+jhits);*/
      }
      else continue;
    }
  }
  
  return 0;
}

