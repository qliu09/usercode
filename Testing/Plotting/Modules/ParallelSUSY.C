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

using namespace std;

using namespace PlottingSetup;
/*
struct thread_broker_data_container {
  TH2F *efficiencymap;
  TH2F *Neventsmap;
  float mglu;
  float mlsp;
  string massgluname;
  string massLSPname;
}; 

int activethreads=0;
int MaxNprocesses=4;//number of total concurrent processes

void blank_thread_broker(void* data) {
  cout << "ANYTHING" << endl;
}

void old_SUSY_efficiency_thread_broker(void* data) {
  activethreads+=1;
  thread_broker_data_container* args = (thread_broker_data_container*) data;
  float MGlu=(args->mglu);
  float Mlsp=(args->mlsp);
  float result,resulterr;
  stringstream addcut;
  addcut << "(TMath::Abs("<<(args->massgluname)<<"-"<<MGlu<<")<5)&&(TMath::Abs("<<(args->massLSPname)<<"-"<<Mlsp<<")<5)";
/*  
  
  
  std::cout << "Thread " << args->procnum << " gestartet" << std::endl;
  float result,resulterr;
  anyfunction(args->procnum,result,resulterr);
  args->result=result;
  args->resulterr=resulterr;
  sleep(3); // 3 Sekunden warten
  std::cout << "Thread " << args->procnum << " beendet" << std::endl;
  activethreads-=1;
  return 0;
  
  
  //
  
  
      MCefficiency((scansample.collection)[0].events,result,resulterr,mcjzb,requireZ,addcut.str());
      if(result!=result||isanyinf(result)) {
	dout << "Watch out the result for mlsp=" << mlsp << " and mglu=" << mglu << " (" << result << ") has been detected as being not a number (nan) or infinity (inf) !" << endl;
	result=0; // kicking nan and inf errors
      }
      else {
	efficiencymap->Fill(mglu,mlsp,result);
	dout << "      ok! Added efficiency " << result << " for mlsp=" << mlsp << " and mglu=" << mglu << endl;
      }
      
      (scansample.collection)[0].events->Draw(mcjzb.c_str(),addcut.str().c_str(),"goff");
      float nevents = (scansample.collection)[0].events->GetSelectedRows();
      Neventsmap->Fill(mglu,mlsp,nevents);
*//*
//  return 0;
}

void PARALLEL_efficiency_scan_in_susy_space(string mcjzb, string datajzb, bool requireZ, float peakerror) {
  dout << "Doing efficiency scan using " << (scansample.collection)[0].filename << endl;
  geqleq="geq";
  automatized=true;
  mcjzbexpression=mcjzb;
  
  string massgluname="MassGlu";
  string massLSPname="MassLSP";
  
  Int_t MyPalette[100];
  Double_t r[]    = {0., 0.0, 1.0, 1.0, 1.0};
  Double_t g[]    = {0., 0.0, 0.0, 1.0, 1.0};
  Double_t b[]    = {0., 1.0, 0.0, 0.0, 1.0};
  Double_t stop[] = {0., .25, .50, .75, 1.0};
  Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 110);
  for (int i=0;i<100;i++) MyPalette[i] = FI+i;
   
  gStyle->SetPalette(100, MyPalette);
  
  TH2F *efficiencymap = new TH2F("efficiencymap","",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *Neventsmap    = new TH2F("Neventsmap","",   (mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  
  float rightmargin=gStyle->GetPadRightMargin();
  gStyle->SetPadRightMargin(0.14);
  
  TCanvas *effcanvas = new TCanvas("effcanvas","Efficiency canvas");
  
  int Nthreads=(((mgluend-mglustart)/mglustep)*((mLSPend-mLSPstart)/mLSPstep));
  pthread_t threads[Nthreads];
  thread_broker_data_container *args[Nthreads];
  
  int ithread=0;
  for(float mglu=mglustart;mglu<=mgluend;mglu+=mglustep) {
    for (float mlsp=mLSPstart;mlsp<=mLSPend&&mlsp<=mglu;mlsp+=mLSPstep)
    {
      ithread++;
      args[ithread] = {efficiencymap,Neventsmap,mglu,mlsp,massgluname,massLSPname};
      cout << "STILL MISSING WAITING HERE!" << endl;
      pthread_create( &threads[ithread], NULL, blank_thread_broker, (void*) &args[ithread]);
//      pthread_create( &threads[ithread], NULL, SUSY_efficiency_thread_broker, (void*) &args[ithread]);
    }
  }
  
  ithread=0;//below: we wait on the threads to finish
  for(int mglu=mglustart;mglu<=mgluend;mglu+=mglustep) {
    for (int mlsp=mLSPstart;mlsp<=mLSPend&&mlsp<=mglu;mlsp+=mLSPstep)
    {
      ithread++;
      pthread_join( threads[ithread], NULL );
    }
  }
  
  
  
  efficiencymap->GetXaxis()->SetTitle("m_{glu}");
  efficiencymap->GetXaxis()->CenterTitle();
  efficiencymap->GetYaxis()->SetTitle("m_{LSP}");
  efficiencymap->GetYaxis()->CenterTitle();
  
  Neventsmap->GetXaxis()->SetTitle("m_{glu}");
  Neventsmap->GetXaxis()->CenterTitle();
  Neventsmap->GetYaxis()->SetTitle("m_{LSP}");
  Neventsmap->GetYaxis()->CenterTitle();
    
  efficiencymap->Draw("COLZ");
  TText *title = write_title("Efficiency in LSP-Glu plane");
  title->Draw();
  CompleteSave(effcanvas,"SUSYScan/Efficency");
  
  Neventsmap->Draw("COLZ");
  TText *title2 = write_title("Number of events in LSP-Glu plane");
  title2->Draw();
  CompleteSave(effcanvas,"SUSYScan/Nevents");
}
*/

struct thread_broker_data_container {
  int procnum;
  TH2F *efficiencymap;
  TH2F *Neventsmap;
  float mglu;
  float mlsp;
  string massgluname;
  string massLSPname;
}; 

int activethreads=0;
int nmaxprocesses=4;

void anyfunction(int numthread, float &result, float &resulterr) {
  std::cout << "This is just any text from thread " << numthread << std::endl;
  result=5*numthread;
  resulterr=5*numthread+1;
}

string SUSYScanmcexpression;
bool SUSYScanrequireZ;

void old_SUSY_efficiency_thread_broker(void* data) {
  activethreads+=1;
  thread_broker_data_container* args = (thread_broker_data_container*) data;
  float MGlu=(args->mglu);
  float Mlsp=(args->mlsp);
  float result,resulterr;
  stringstream addcut;
  addcut << "(TMath::Abs("<<(args->massgluname)<<"-"<<MGlu<<")<5)&&(TMath::Abs("<<(args->massLSPname)<<"-"<<Mlsp<<")<5)";
/*  
  
  
  std::cout << "Thread " << args->procnum << " gestartet" << std::endl;
  float result,resulterr;
  anyfunction(args->procnum,result,resulterr);
  args->result=result;
  args->resulterr=resulterr;
  sleep(3); // 3 Sekunden warten
  std::cout << "Thread " << args->procnum << " beendet" << std::endl;
  activethreads-=1;
  return 0;
  
  
  //
  
  
      MCefficiency((scansample.collection)[0].events,result,resulterr,mcjzb,requireZ,addcut.str());
      if(result!=result||isanyinf(result)) {
	dout << "Watch out the result for mlsp=" << mlsp << " and mglu=" << mglu << " (" << result << ") has been detected as being not a number (nan) or infinity (inf) !" << endl;
	result=0; // kicking nan and inf errors
      }
      else {
	efficiencymap->Fill(mglu,mlsp,result);
	dout << "      ok! Added efficiency " << result << " for mlsp=" << mlsp << " and mglu=" << mglu << endl;
      }
      
      (scansample.collection)[0].events->Draw(mcjzb.c_str(),addcut.str().c_str(),"goff");
      float nevents = (scansample.collection)[0].events->GetSelectedRows();
      Neventsmap->Fill(mglu,mlsp,nevents);
*/
//  return 0;
}

void PMCefficiency(TTree *events,float &result, float &resulterr,string mcjzb,bool requireZ,string addcut="") {
	
	char jzbSelStr[256]; sprintf(jzbSelStr,"%f",jzbSel);
	// All acceptance cuts at gen. level
	TCut kbase("abs(genMll-91.2)<20&&genNjets>2&&genZPt>0&&genJZB"+geq_or_leq()+TString(jzbSelStr)+"&&genId1==-genId2");
	if(requireZ) kbase=kbase&&"TMath::Abs(genMID)==23";
	if(addcut!="") kbase=kbase&&addcut.c_str();//this is mostly for SUSY scans (adding requirements on masses)
	// Corresponding reco. cuts
	TCut ksel("pfJetGoodNum>2&&abs(mll-91.2)<20&&id1==id2&&"+TString(mcjzb)+geq_or_leq()+TString(jzbSelStr));
	
	string histname=GetNumericHistoName();
	TH1F *hist = new TH1F(histname.c_str(),histname.c_str(),2,-14000,14000);
	events->Draw((mcjzbexpression+">>"+histname).c_str(),kbase&&ksel);
	Float_t sel = hist->Integral();
	/*
	
	events->Draw((mcjzbexpression+">>"+histname).c_str(),kbase);
	Float_t tot = hist->Integral();
	
	result=sel/tot;
	resulterr=TMath::Sqrt(sel/tot*(1-sel/tot)/tot);
	if(!automatized) dout << "  MC efficiency: " << result << "+-" << resulterr << std::endl;
	delete hist;
	cout << "Survived efficiency!" << endl;*/
}

void* SUSY_efficiency_thread_broker(void* data) { // Der Type ist wichtig: void* als Parameter und Rückgabe
  activethreads+=1;
  thread_broker_data_container* args = (thread_broker_data_container*) data;
  float MGlu=(args->mglu);
  float Mlsp=(args->mlsp);
  float result,resulterr;
  stringstream addcut;
  addcut << "(TMath::Abs("<<(args->massgluname)<<"-"<<MGlu<<")<5)&&(TMath::Abs("<<(args->massLSPname)<<"-"<<Mlsp<<")<5)";
  //TTree *specialtree=((scansample.collection)[0].events)->CloneTree();
//  cout << ((scansample.collection)[0].filename) << endl;
  PMCefficiency(((scansample.collection)[0].events),result,resulterr,SUSYScanmcexpression,SUSYScanrequireZ,addcut.str());
  
  //--------------------------------------------------------------------------------------------
  std::cout << "Thread " << args->procnum << " gestartet" << std::endl;
//  anyfunction(args->procnum,result,resulterr);
//  args->result=result;
//  args->resulterr=resulterr;
//  (args->efficiencymap)->Fill(args->procnum,result);
//  sleep(3); // 3 Sekunden warten
  std::cout << "Thread " << args->procnum << " beendet" << std::endl;
  activethreads-=1;
  return NULL; // oder in C++: return 0;// Damit kann man Werte zurückgeben
}


void PARALLEL_efficiency_scan_in_susy_space(string mcjzb, string datajzb, bool requireZ, float peakerror) {
  
  dout << "Doing PARALLEL efficiency scan using " << (scansample.collection)[0].filename << endl;
  SUSYScanmcexpression=datajzb;
  SUSYScanrequireZ=requireZ;
  geqleq="geq";
  automatized=true;
  mcjzbexpression=mcjzb;
  
  string massgluname="MassGlu";
  string massLSPname="MassLSP";
  
  Int_t MyPalette[100];
  Double_t r[]    = {0., 0.0, 1.0, 1.0, 1.0};
  Double_t g[]    = {0., 0.0, 0.0, 1.0, 1.0};
  Double_t b[]    = {0., 1.0, 0.0, 0.0, 1.0};
  Double_t stop[] = {0., .25, .50, .75, 1.0};
  Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 110);
  for (int i=0;i<100;i++) MyPalette[i] = FI+i;
   
  gStyle->SetPalette(100, MyPalette);
  
  TH2F *efficiencymap = new TH2F("efficiencymap","",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *Neventsmap    = new TH2F("Neventsmap","",   (mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  
  float rightmargin=gStyle->GetPadRightMargin();
  
  gStyle->SetPadRightMargin(0.14);
  
  TCanvas *effcanvas = new TCanvas("effcanvas","Efficiency canvas");
  
//  int Nthreads=(((mgluend-mglustart)/mglustep)*((mLSPend-mLSPstart)/mLSPstep));
  int Nthreads=5;

  pthread_t threads[Nthreads];
  thread_broker_data_container args[Nthreads];
  TH2F *test = new TH2F("test","test",10,0,10,10,0,50);

  int ithread=-1;
  for(float mglu=mglustart;mglu<=mgluend;mglu+=mglustep) {
    for (float mlsp=mLSPstart;mlsp<=mLSPend&&mlsp<=mglu;mlsp+=mLSPstep)
    {
      ithread++;
      if(ithread>=Nthreads) {
	continue;
      } else {
	cout << ithread << " : PLEASE REMOVE THIS IF CONDITION - IT IS FOR DEBUGGING ONLY!" << endl;
      }
      while(activethreads>=nmaxprocesses) {
      std::cout << "----------------------- Thread " << ithread << " : There are currently " << activethreads << " running while only " << nmaxprocesses << " are allowed. Waiting ... " << std::endl;
      sleep(1);
      }
      args[ithread] = {ithread,efficiencymap,Neventsmap,mglu,mlsp,massgluname,massLSPname};
      pthread_create( &threads[ithread], NULL, SUSY_efficiency_thread_broker, (void*) &args[ithread]);
    }
  }
  sleep(5);
  std::cout << "Now going to wait on threads ... " << std::endl;
  for(int i=0;i<Nthreads;i++) {
    cout << "Waiting on thread " << i << endl;
    pthread_join( threads[i], NULL );
  }
  cout << "Threads finished! " << endl;
}