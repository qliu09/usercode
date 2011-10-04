#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <pthread.h>

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
#include <TKey.h>

//#include "TTbar_stuff.C"
using namespace std;

using namespace PlottingSetup;

void susy_scan_axis_labeling(TH2F *histo) {
  histo->GetXaxis()->SetTitle("m_{#Chi_{2}^{0}}-m_{LSP}");
  histo->GetXaxis()->CenterTitle();
  histo->GetYaxis()->SetTitle("m_{#tilde{q}}");
  histo->GetYaxis()->CenterTitle();
}

bool isThreadActive=false;

struct limit_args {
  float mceff;
  float toterr;
  int ibin;
  string mcjzb;
  vector<float> sigmas;
  string plotfilename;
};

void* compute_one_upper_limit_wrapper(void* data) {
  isThreadActive=true;
  limit_args* args = (limit_args*) data;
  std::cout << "Thread to compute limits has been started" << std::endl;
  args->sigmas=compute_one_upper_limit(args->mceff,args->toterr,args->ibin,args->mcjzb,args->plotfilename,true);
  std::cout << "Thread to compute limits has finished" << std::endl;
  isThreadActive=false;
  return NULL;
}

void do_limit_wrapper(float mceff,float toterr,int ibin,string mcjzb,vector<float> &sigmas,string plotfilename) {
  pthread_t limitthread;
  limit_args limargs={mceff,toterr,ibin,mcjzb,sigmas,plotfilename};
  pthread_create( &limitthread, NULL, compute_one_upper_limit_wrapper, (void*) &limargs);
  int counter=0;
  int counterinterval=5;
  sleep(1); //waiting a second for the process to become active
  while(counter<limitpatience*60 && isThreadActive) {
    std::cout << "Limits are being calculated; Checking round " << counter/counterinterval << " ( corresponds to " << seconds_to_time(counter) << " ) , patience will end in " << seconds_to_time(60*limitpatience-counter) << std::endl;
    counter+=counterinterval;
    sleep(counterinterval);
  }

  if(!isThreadActive) {
    cout << "Thread finished sucessfully" << endl;
    pthread_join( limitthread, NULL );
    std::cout<< __FUNCTION__ << " : going to save sigmas " << std::endl;
    sigmas=limargs.sigmas;
  } else {
    pthread_cancel(limitthread);
    std::cout << "DID NOT TERMINATE IN TIME - ABORTED!" << std::endl;
    sigmas.push_back(-1);sigmas.push_back(-1);sigmas.push_back(-1);
  }
}

void prepare_scan_axis(TH2 *variablemap,bool ismSUGRA) {
  variablemap->GetXaxis()->SetTitle("m_{glu}");
  variablemap->GetYaxis()->SetTitle("m_{LSP}");
 
  if(ismSUGRA) {
    variablemap->GetXaxis()->SetTitle("m_{0}");
    variablemap->GetYaxis()->SetTitle("m_{1/2}");
  }
 
  variablemap->GetXaxis()->CenterTitle();
  variablemap->GetYaxis()->CenterTitle();
}

void set_SUSY_style() {
  Int_t MyPalette[100];
  Double_t r[]    = {0., 0.0, 1.0, 1.0, 1.0};
  Double_t g[]    = {0., 0.0, 0.0, 1.0, 1.0};
  Double_t b[]    = {0., 1.0, 0.0, 0.0, 1.0};
  Double_t stop[] = {0., .25, .50, .75, 1.0};
  Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 110);
  for (int i=0;i<100;i++) MyPalette[i] = FI+i;
  
  gStyle->SetPalette(100, MyPalette);
 
  float rightmargin=gStyle->GetPadRightMargin();
  gStyle->SetPadRightMargin(0.15);

}

float get_xs(float mglu, float mlsp, string massgluname, string massLSPname, map <  pair<float, float>, map<string, float>  >  &xsec, string mcjzb, bool requireZ)  {
  float weightedpointxs=0;
  stringstream addcut;
  addcut << "(TMath::Abs("<<massgluname<<"-"<<mglu<<")<5)&&(TMath::Abs("<<massLSPname<<"-"<<mlsp<<")<5)";
  cout << "About to calculate the process efficiencies " << endl;
/*  for(int iproc=1;iproc<=10;iproc++) {
    float process_xs = GetXSecForPointAndChannel(mglu,mlsp,xsec,iproc);
    stringstream addcutplus;
    addcutplus<<addcut.str()<<"&&(pfJetGoodNum=="<<iproc<<")";
    write_warning(__FUNCTION__,"replaced process with pfJetGoodNum for testing purposes only!");
    (scansample.collection)[0].events->Draw("eventNum",addcutplus.str().c_str(),"goff");
    float nprocessevents = (scansample.collection)[0].events->GetSelectedRows();
    float nselectedprocessevents,nselectedprocesseventserr;
    cout << "nprocessevents = " << nprocessevents << endl;
    MCefficiency((scansample.collection)[0].events,nselectedprocessevents,nselectedprocesseventserr,mcjzb,requireZ,1,addcutplus.str(),-1);//1 is the number to normalize to :-)
    float weight=0;
    if(nprocessevents>0) weight=(process_xs)/(nprocessevents);//*luminosity;
    weightedpointxs+=weight*nselectedprocessevents;
  }
  return weightedpointxs;
*/
 return 1.0;
}

void establish_SUSY_limits(string mcjzb,string datajzb,vector<float> jzb_cut,bool requireZ, float peakerror, TFile *fsyst, int ibin,float njobs=-1, float jobnumber=-1) {
  bool runninglocally=true;
  if(njobs>-1&&jobnumber>-1) {
    runninglocally=false;
    dout << "Running on the GRID (this is job " << jobnumber << "/" << njobs << ") for a jzb cut at " << jzb_cut[ibin] << endl;
  } else {
    dout << "Running locally " << endl;
    dout << "This will take a really, really long time - if  you want to see the results within hours instead of weeks try running the worker script on the grid (DistributedModelCalculation/Limits/)" << endl;
  }
 
  string massgluname="MassGlu";
  string massLSPname="MassLSP";
  jzbSel=jzb_cut[ibin];
  geqleq="geq";
  automatized=true;
  mcjzbexpression=mcjzb;

  string prefix="SMS_";
  // up to here, everything is set up for SMS; now we need to switch stuff around if we're dealing with an mSUGRA scan!
  bool ismSUGRA=false;
  TIter nextkey(fsyst->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)nextkey()))
    {
      TObject *obj = key->ReadObj();
      if(Contains((string)(obj->GetName()),"mSUGRA")) ismSUGRA=true;
    }
   
    map <  pair<float, float>, map<string, float>  >  xsec;

  if(ismSUGRA) {
    massgluname="M0"; // this is the "x axis" in the limit plot (like the gluino in the SMS case)
    massLSPname="M12"; // this is the "y axis" in the limit plot (like the LSP in the SMS case)
    mglustart=m0start;
    xsec=getXsec("/scratch/buchmann/C/scale_xsection_nlo1.0_m0_m12_10_0_1v1.txt");
      write_warning(__FUNCTION__,"Don't have the correct XS file yet");
    mgluend=m0end;
    mglustep=m0step;
    mLSPstart=m12start;
    mLSPend=m12end;
    mLSPstep=m12step;
    prefix="mSUGRA_";
    cout << "mSUGRA scan has been set up." << endl;
  } else {
    cout << "SMS scan has been set up." << endl;
      write_warning(__FUNCTION__,"Don't have the correct XS file yet");
    xsec=getXsec("/scratch/buchmann/C/scale_xsection_nlo1.0_m0_m12_10_0_1v1.txt");
  }
 
  set_SUSY_style();
 
  TCanvas *limcanvas = new TCanvas("limcanvas","Limit canvas");
 
  int Npoints=0;
  for(int mglu=mglustart;mglu<=mgluend;mglu+=mglustep) {
    for (int mlsp=mLSPstart;mlsp<=mLSPend&&(ismSUGRA||mlsp<=mglu);mlsp+=mLSPstep) Npoints++;
  }
  TH2F *mceff     = (TH2F*)fsyst->Get((prefix+"efficiencymap"+any2string(jzb_cut[ibin])).c_str());
//write_warning(__FUNCTION__,"Currently the efficiencymap name was switched to Pablo's convention. This NEEDS to be switched BACK!");TH2F *mceff     = (TH2F*)fsyst->Get(("efficiency_jzbdiff"+any2string(jzb_cut[ibin])).c_str());
  TH2F *fullerr   = (TH2F*)fsyst->Get((prefix+"systotmap"+any2string(jzb_cut[ibin])).c_str());
  TH2F *NEvents   = (TH2F*)fsyst->Get((prefix+"Neventsmap"+any2string(jzb_cut[ibin])).c_str());
 
  TH2F *limitmap  = new TH2F((prefix+"limitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//observed limit
  TH2F *explimitmap  = new TH2F((prefix+"explimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit
  TH2F *exp1plimitmap  = new TH2F((prefix+"exp1plimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit + 1 sigma
  TH2F *exp2plimitmap  = new TH2F((prefix+"exp2plimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit + 2 sigma
  TH2F *exp1mlimitmap  = new TH2F((prefix+"exp1mlimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit - 1 sigma
  TH2F *exp2mlimitmap  = new TH2F((prefix+"exp2mlimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit - 2 sigma
  
  TH2F *exclmap  = new TH2F((prefix+"exclusionmap"+any2string(jzb_cut[ibin])).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *xsmap  = new TH2F((prefix+"crosssectionmap"+any2string(jzb_cut[ibin])).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
   
  if(!fullerr || !mceff || !NEvents) {
    write_error(__FUNCTION__,"The supplied systematics file did not contain the correct histograms - please check the file");
    dout << "mc eff address: " << mceff << " , error address: " << fullerr << " , NEvents address: " << NEvents << endl;
    delete limcanvas;
    return;
  }
  
  bool doexpected=true;
 
  int ipoint=-1;
  for(int mglu=mglustart;mglu<=mgluend;mglu+=mglustep) {
    for (int mlsp=mLSPstart;mlsp<=mLSPend&&(ismSUGRA||mlsp<=mglu);mlsp+=mLSPstep)
    {
      ipoint++;
      if(!runninglocally&&!do_this_point(ipoint,Npoints,jobnumber,njobs)) continue;
      float currmceff=mceff->GetBinContent(mceff->FindBin(mglu,mlsp));
      float currtoterr=(fullerr->GetBinContent(fullerr->FindBin(mglu,mlsp)))*currmceff;
      float nevents=NEvents->GetBinContent(NEvents->FindBin(mglu,mlsp));
      dout << "Looking at point " << ipoint << " / " << Npoints << " with masses " << massgluname << " = " << mglu << " and " << massLSPname << " = " << mlsp << endl;
      dout << "Found : MCeff=" << currmceff << " and total error=" << currtoterr << " and Nevents=" << nevents << endl;
      string plotfilename=(string)(TString(massgluname)+TString(any2string(mglu))+TString("__")+TString(massLSPname)+TString(any2string(mlsp))+TString(".png"));
      if(currmceff<=0||currtoterr<=0||nevents==0) {
	dout << "   Nothing to work with, skipping this point." << endl;
	continue;
      }
      vector<float> sigmas;
      //do_limit_wrapper(currmceff,currtoterr,ibin,mcjzb,sigmas,plotfilename); // no threading right now.
      sigmas=compute_one_upper_limit(currmceff,currtoterr,ibin,mcjzb,plotfilename,true);
      if(sigmas[0]>0) limitmap->Fill(mglu,mlsp,sigmas[0]); //anything else is an error code
      if(sigmas.size()>1) {
	explimitmap->Fill(mglu,mlsp,sigmas[1]);
	exp1plimitmap->Fill(mglu,mlsp,sigmas[2]);
	exp1mlimitmap->Fill(mglu,mlsp,sigmas[3]);
	exp2plimitmap->Fill(mglu,mlsp,sigmas[4]);
	exp2mlimitmap->Fill(mglu,mlsp,sigmas[5]);
      }

      dout << "An upper limit has been added for this point ( " << massgluname << " = " << mglu << " and " << massLSPname << " = " << mlsp << " ) at " << sigmas[0] << endl;
      if(ismSUGRA) { // for SMS this is a bit easier at the moment - we have a reference XS file which we use when plotting
          dout << "Computing exclusion status" << endl;
          float rel_limit=0;
          float xs=get_xs(mglu,mlsp,massgluname,massLSPname,xsec,mcjzb,requireZ);
          if(xs>0) rel_limit=sigmas[0]/xs;
//          stringstream addcut;
//          addcut << "(TMath::Abs("<<massgluname<<"-"<<mglu<<")<5)&&(TMath::Abs("<<massLSPname<<"-"<<mlsp<<")<5)";
//          vector<float> xs_weights = processMCefficiency((scansample.collection)[0].events,mcjzb,requireZ,nevents, addcut.str());
          exclmap->Fill(mglu,mlsp,rel_limit);
          xsmap->Fill(mglu,mlsp,xs);
      }
    }
  }
 
  prepare_scan_axis(limitmap,ismSUGRA);
  TFile *outputfile=new TFile(("output/DistributedLimitsFromSystematics_job"+string(any2string(jobnumber))+"_of_"+string(any2string(njobs))+".root").c_str(),"UPDATE");//needs to be "UPDATE" as we can get to this point for different JZB cuts and don't want to erase the previous data :-)
  outputfile->cd();
  limitmap->Write();
  if(doexpected) {
    explimitmap->Write();
    exp1plimitmap->Write();
    exp1mlimitmap->Write();
    exp2plimitmap->Write();
    exp2mlimitmap->Write();
  }
  if(ismSUGRA) {
    exclmap->Write();
    xsmap->Write();
  }
  outputfile->Close();
  delete limcanvas;
}

void establish_SUSY_limits(string mcjzb,string datajzb,vector<float> jzb_cut,bool requireZ, float peakerror, string fsystfilename, float njobs=-1, float jobnumber=-1) {
  dout << "Starting the SUSY scan from systematics file now with all " << jzb_cut.size() << " bin(s)" << " and using " << fsystfilename << endl;
  TFile *fsyst = new TFile(fsystfilename.c_str());
  if(!fsyst) {
    write_error(__FUNCTION__,"You provided an invalid systematics file. Please change it ... ");
    return;
  }
  for(int ibin=0;ibin<jzb_cut.size();ibin++) {
    establish_SUSY_limits(mcjzb,datajzb,jzb_cut,requireZ, peakerror, fsyst, ibin,njobs, jobnumber);
  }
}




void scan_SUSY_parameter_space(string mcjzb,string datajzb,vector<float> jzb_cut,bool requireZ, float peakerror, int ibin,float njobs=-1, float jobnumber=-1, bool systematicsonly=false,bool efficiencyonly=false) {
  bool runninglocally=true;
  if(njobs>-1&&jobnumber>-1) {
    runninglocally=false;
    dout << "Running on the GRID (this is job " << jobnumber << "/" << njobs << ") for a jzb cut at " << jzb_cut[ibin] << endl;
  } else {
    dout << "Running locally " << endl;
    dout << "This will take a really, really long time - if  you want to see the results THIS month try running the LimitWorkerScript on the grid (DistributedModelCalculation/Limits/)" << endl;
  }
 
  jzbSel=jzb_cut[ibin];
  geqleq="geq";
  automatized=true;
  mcjzbexpression=mcjzb;
 
  string massgluname="MassGlu";
  string massLSPname="MassLSP";
 
  string prefix="SMS_";
  // up to here, everything is set up for SMS; now we need to switch stuff around if we're dealing with an mSUGRA scan!
  bool ismSUGRA=false;
  if(Contains((scansample.collection)[0].samplename,"mSUGRA")) ismSUGRA=true;
  if(ismSUGRA) {
    massgluname="M0"; // this is the "x axis" in the limit plot (like the gluino in the SMS case)
    massLSPname="M12"; // this is the "y axis" in the limit plot (like the LSP in the SMS case)
    mglustart=m0start;
    mgluend=m0end;
    mglustep=m0step;
    mLSPstart=m12start;
    mLSPend=m12end;
    mLSPstep=m12step;
    prefix="mSUGRA_";
    cout << "mSUGRA scan has been set up." << endl;
  } else {
    cout << "SMS scan has been set up." << endl;
}
 
  Int_t MyPalette[100];
  Double_t r[]    = {0., 0.0, 1.0, 1.0, 1.0};
  Double_t g[]    = {0., 0.0, 0.0, 1.0, 1.0};
  Double_t b[]    = {0., 1.0, 0.0, 0.0, 1.0};
  Double_t stop[] = {0., .25, .50, .75, 1.0};
  Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 110);
  for (int i=0;i<100;i++) MyPalette[i] = FI+i;
  
  gStyle->SetPalette(100, MyPalette);
 
  TH2F *limitmap  = new TH2F((prefix+"limitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//observed limit
  TH2F *explimitmap  = new TH2F((prefix+"explimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit
  TH2F *exp1plimitmap  = new TH2F((prefix+"exp1plimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit + 1 sigma
  TH2F *exp2plimitmap  = new TH2F((prefix+"exp2plimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit + 2 sigma
  TH2F *exp1mlimitmap  = new TH2F((prefix+"exp1mlimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit - 1 sigma
  TH2F *exp2mlimitmap  = new TH2F((prefix+"exp2mlimitmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);//expected limit - 2 sigma
  
  TH2F *exclmap  =       new TH2F((prefix+"exclusionmap"+any2string(jzbSel)).c_str(), "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *sysjesmap =      new TH2F((prefix+"sysjes"+any2string(jzbSel)).c_str(),   "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *sysjsumap =      new TH2F((prefix+"sysjsu"+any2string(jzbSel)).c_str(),   "",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *sysresmap =      new TH2F((prefix+"sysresmap"+any2string(jzbSel)).c_str(),"",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *efficiencymap =  new TH2F((prefix+"efficiencymap"+any2string(jzbSel)).c_str(),"",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *noscefficiencymap = new TH2F((prefix+"nosc_efficiencymap"+any2string(jzbSel)).c_str(),"",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *Neventsmap    =  new TH2F((prefix+"Neventsmap"+any2string(jzbSel)).c_str(),"",   (mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *ipointmap    =   new TH2F((prefix+"ipointmap"+any2string(jzbSel)).c_str(),"",   (mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *syspdfmap =      new TH2F((prefix+"syspdfmap"+any2string(jzbSel)).c_str(),"",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *systotmap =      new TH2F((prefix+"systotmap"+any2string(jzbSel)).c_str(),"",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  TH2F *sysstatmap =     new TH2F((prefix+"sysstatmap"+any2string(jzbSel)).c_str(),"",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);

  TH2F *timemap =        new TH2F((prefix+"timemap"+any2string(jzbSel)).c_str(),"",(mgluend-mglustart)/mglustep+1,mglustart-0.5*mglustep,mgluend+0.5*mglustep,(mLSPend-mLSPstart)/mLSPstep+1,mLSPstart-0.5*mLSPstep,mLSPend+0.5*mLSPstep);
  
write_warning(__FUNCTION__,"CURRENTLY SWITCHING AUTOMATIZED MODE OFF!");automatized=false;
 
  float rightmargin=gStyle->GetPadRightMargin();
  gStyle->SetPadRightMargin(0.15);
 
  TCanvas *limcanvas = new TCanvas("limcanvas","Limit canvas");
 
  
  bool doexpected=true; 
  
  int Npoints=0;
  for(int mglu=mglustart;mglu<=mgluend;mglu+=mglustep) {
    for (int mlsp=mLSPstart;mlsp<=mLSPend&&(ismSUGRA||mlsp<=mglu);mlsp+=mLSPstep) Npoints++;
  }
 
  int ipoint=-1;
  for(int mglu=mglustart;mglu<=mgluend;mglu+=mglustep) {
    for (int mlsp=mLSPstart;mlsp<=mLSPend&&(ismSUGRA||mlsp<=mglu);mlsp+=mLSPstep)
    {
      ipoint++;
      if(!runninglocally&&!do_this_point(ipoint,Npoints,jobnumber,njobs)) continue;
      float result=-987,resulterr=-987;
      int scanfileindex=0;
      int m0trees = PlottingSetup::m0end;
      int m12trees = PlottingSetup::m12end;
      if(!ismSUGRA) {
	m0trees = PlottingSetup::mgluend;
	m12trees = PlottingSetup::mLSPend;
      }
	
      int a = int((PlottingSetup::ScanXzones*mlsp)/(m12trees+1));
      int b = int((PlottingSetup::ScanYzones*mglu)/(m0trees+1));
      scanfileindex=PlottingSetup::ScanYzones*a+b;
      cout << "Going to require file having a=" << a << " and  b=" << b << endl;
      stringstream filetoload;
      filetoload << "/shome/buchmann/ntuples/";
      if(ismSUGRA) {
	filetoload << "mSUGRA/mSUGRA_clean_splitup_" << any2string(a) << "_" << any2string(b) << ".root";
      } else {
	filetoload << "SMS/SMS_clean_splitup_" << any2string(a) << "_" << any2string(b) << ".root";
      }
      if(!Contains(((scansample.collection)[(scansample.collection).size()-1]).filename,"_"+any2string(a)+"_"+any2string(b))) {
	cout << "The last sample is NOT the same one as the current one, possibly popping off last one and adding the new one." << endl;
	if((scansample.collection).size()>1) {
	   scansample.RemoveLastSample();
	}
	scanfileindex=(scansample.collection).size();
	//New: Loading file when necessary, not before (avoiding high memory usage and startup times)
	if(scanfileindex!=0) scansample.AddSample(filetoload.str(),"scansample",1,1,false,true,scanfileindex,kRed);
      } else {
	cout << "Last sample is the same as the current one. Recycling it." << endl;
	scanfileindex=(scansample.collection).size()-1;
	}
      
      clock_t start,finish;
      start = clock(); // starting the clock to measure how long the computation takes!
      stringstream addcut;
      addcut << "(TMath::Abs("<<massgluname<<"-"<<mglu<<")<5)&&(TMath::Abs("<<massLSPname<<"-"<<mlsp<<")<5)";
      (scansample.collection)[scanfileindex].events->Draw("eventNum",addcut.str().c_str(),"goff");
      float nevents = (scansample.collection)[scanfileindex].events->GetSelectedRows();
      vector<vector<float> > systematics;
      if(nevents<10) {
	dout << "This point ("<<ipoint<<") with configuration ("<<massgluname<<"="<<mglu<<" , "<<massLSPname<<"="<<mlsp << ") does not contain enough events and will be skipped."<< endl;
	continue;
      } else {
	dout << "OK! This point ("<<ipoint<<") with configuration ("<<massgluname<<"="<<mglu<<" , "<<massLSPname<<"="<<mlsp << ") contains " << nevents << " and will therefore not be skipped."<< endl;
      }
      if(nevents!=0&&efficiencyonly) {
	  Value effwosigcont = MCefficiency((scansample.collection)[scanfileindex].events,result,resulterr,mcjzb,requireZ,nevents,addcut.str(),-1);
	  efficiencymap->Fill(mglu,mlsp,result);
	  noscefficiencymap->Fill(mglu,mlsp,effwosigcont.getValue());
	  noscefficiencymap->SetBinError(mglu,mlsp,effwosigcont.getError());
	  finish = clock();
	  timemap->Fill(mglu,mlsp,((float(finish)-float(start))/CLOCKS_PER_SEC));
      }
      Neventsmap->Fill(mglu,mlsp,nevents);
      ipointmap->Fill(mglu,mlsp,ipoint);
      if(efficiencyonly) continue;

      do_systematics_for_one_file((scansample.collection)[scanfileindex].events,nevents,"SUSY SCAN", systematics,mcjzb,datajzb,peakerror,requireZ, addcut.str(),ismSUGRA);
      float JZBcutat = systematics[0][0];
      float mceff    = systematics[0][1];
      float mcefferr = systematics[0][2];//MC stat error
      float toterr   = systematics[0][4];
      float sys_jes  = systematics[0][5]; // Jet Energy Scale
      float sys_jsu  = systematics[0][6]; // JZB scale uncertainty
      float sys_res  = systematics[0][7]; // resolution
      float mcwoscef = systematics[0][8]; // efficiency without signal contamination
      float mcwoscefr= systematics[0][9]; // error on efficiency without signal contamination
      float sys_pdf   = 0;
      if(systematics[0].size()>10) sys_pdf = systematics[0][10]; // PDF
      efficiencymap->Fill(mglu,mlsp,mceff);
      efficiencymap->SetBinError(efficiencymap->FindBin(mglu,mlsp),mcefferr);
      noscefficiencymap->Fill(mglu,mlsp,mcwoscef);
      noscefficiencymap->SetBinError(efficiencymap->FindBin(mglu,mlsp),mcwoscefr);
     
      if(mceff!=mceff||toterr!=toterr||mceff<0) {
    dout << "Limits can't be calculated for this configuration (mglu="<<mglu<<" , mlsp="<<mlsp << ") as either the efficiency or its error are not positive numbers! (mceff="<<mceff<<" and toterr="<<toterr<<")"<< endl;
    continue;
      } else {
    if(!systematicsonly&&!efficiencyonly) {
      dout << "Calculating limit now for "<<massgluname<<"="<<mglu<<" , "<<massLSPname<<"="<<mlsp <<endl;
      vector<float> sigmas;
      string plotfilename=(string)(TString((scansample.collection)[scanfileindex].samplename)+TString(massgluname)+TString(any2string(mglu))+TString("__")+TString(massLSPname)+TString(any2string(mlsp))+TString(".png"));
      do_limit_wrapper(mceff,toterr,ibin,mcjzb,sigmas,plotfilename);
      cout << "back in " << __FUNCTION__ << endl;
      if(sigmas[0]>-0.5) { // negative sigmas are the error signature of do_limit_wrapper, so we want to exclude them.
        limitmap->Fill(mglu,mlsp,sigmas[0]);
	if(sigmas.size()>1) {
	  explimitmap->Fill(mglu,mlsp,sigmas[1]);
	  exp1plimitmap->Fill(mglu,mlsp,sigmas[2]);
	  exp1mlimitmap->Fill(mglu,mlsp,sigmas[3]);
	  exp2plimitmap->Fill(mglu,mlsp,sigmas[4]);
	  exp2mlimitmap->Fill(mglu,mlsp,sigmas[5]);
	}

        sysjesmap->Fill(mglu,mlsp,sys_jes);
        sysjsumap->Fill(mglu,mlsp,sys_jsu);
        sysresmap->Fill(mglu,mlsp,sys_res);
        syspdfmap->Fill(mglu,mlsp,sys_pdf);
        systotmap->Fill(mglu,mlsp,toterr/mceff);//total relative (!) error
        sysstatmap->Fill(mglu,mlsp,mcefferr);//total relative (!) error
        dout << "A limit has been added at " << sigmas[0] << " for m_{glu}="<<mglu << " and m_{lsp}="<<mlsp<<endl;
      } //end of if sigma is positive
      finish = clock();
      timemap->Fill(mglu,mlsp,((float(finish)-float(start))/CLOCKS_PER_SEC));

    //end of not systematics only condition
    }
    if(systematicsonly) {
        sysjesmap->Fill(mglu,mlsp,sys_jes);
        sysjsumap->Fill(mglu,mlsp,sys_jsu);
        sysresmap->Fill(mglu,mlsp,sys_res);
        syspdfmap->Fill(mglu,mlsp,sys_pdf);
        systotmap->Fill(mglu,mlsp,toterr/mceff);//total relative (!) error
        sysstatmap->Fill(mglu,mlsp,mcefferr);//total relative (!) error
	finish = clock();
	timemap->Fill(mglu,mlsp,((float(finish)-float(start))/CLOCKS_PER_SEC));
    }
      }//efficiency is valid
    }
  }
 
  prepare_scan_axis(limitmap,ismSUGRA);
  prepare_scan_axis(sysjesmap,ismSUGRA);
  prepare_scan_axis(sysjsumap,ismSUGRA);
  prepare_scan_axis(sysresmap,ismSUGRA);
  prepare_scan_axis(syspdfmap,ismSUGRA);
  prepare_scan_axis(systotmap,ismSUGRA);
  prepare_scan_axis(sysstatmap,ismSUGRA);
  prepare_scan_axis(timemap,ismSUGRA);
 
  if(!systematicsonly&&!efficiencyonly) {
    limcanvas->cd();
    limitmap->Draw("COLZ");
    string titletobewritten="Limits in LSP-Glu plane";
    if(ismSUGRA) titletobewritten="Limits in m_{1/2}-m_{0} plane";
    TText *title = write_title(titletobewritten);
    title->Draw();
    if(runninglocally) {
      CompleteSave(limcanvas,"SUSYScan/Limits_JZB_geq"+any2string(jzb_cut[ibin]));
    } else {
      TFile *outputfile;
      if(systematicsonly) outputfile=new TFile(("output/DistributedSystematics_job"+string(any2string(jobnumber))+"_of_"+string(any2string(njobs))+".root").c_str(),"UPDATE");//needs to be "UPDATE" as we can get to this point for different JZB cuts and don't want to erase the previous data :-)
    else outputfile=new TFile(("output/DistributedLimits_job"+string(any2string(jobnumber))+"_of_"+string(any2string(njobs))+".root").c_str(),"UPDATE");//needs to be "UPDATE" as we can get to this point for
      limitmap->Write();
      if(doexpected) {
	explimitmap->Write();
	exp1plimitmap->Write();
	exp1mlimitmap->Write();
	exp2plimitmap->Write();
	exp2mlimitmap->Write();
      }
      sysjesmap->Write();
      sysjsumap->Write();
      sysresmap->Write();
      efficiencymap->Write();
      noscefficiencymap->Write();
      syspdfmap->Write();
      systotmap->Write();
      sysstatmap->Write();
      Neventsmap->Write();
      ipointmap->Write();
      timemap->Write();
      outputfile->Close();
    }
  }
  if(systematicsonly) { // systematics only :
    limcanvas->cd();
    sysjesmap->Draw("COLZ");
    string titletobewritten="Jet Energy scale in LSP-Glu plane";
    if(ismSUGRA) titletobewritten="Limits in m_{1/2}-m_{0} plane";
    TText *title = write_title(titletobewritten);
    title->Draw();
    if(runninglocally) {
      CompleteSave(limcanvas,"SUSYScan/JES_geq"+any2string(jzb_cut[ibin]));
    }
    sysjsumap->Draw("COLZ");
    titletobewritten="JZB Scale Uncertainty in LSP-Glu plane";
    if(ismSUGRA) titletobewritten="JZB Scale Uncertainty in m_{1/2}-m_{0} plane";
    TText *title2 = write_title(titletobewritten);
    title2->Draw();
    if(runninglocally) {
      CompleteSave(limcanvas,"SUSYScan/JSU_geq"+any2string(jzb_cut[ibin]));
    }
    sysresmap->Draw("COLZ");
    titletobewritten="Resolution in LSP-Glu plane";
    if(ismSUGRA) titletobewritten="Resolution in m_{1/2}-m_{0} plane";
    TText *title3 = write_title(titletobewritten);
    title3->Draw();
    if(runninglocally) {
      CompleteSave(limcanvas,"SUSYScan/Resolution_geq"+any2string(jzb_cut[ibin]));
    }
   
    if(!runninglocally) {
      TFile *outputfile=new TFile(("output/DistributedSystematics_job"+string(any2string(jobnumber))+"_of_"+string(any2string(njobs))+".root").c_str(),"UPDATE");
      sysjesmap->Write();
      sysjsumap->Write();
      sysresmap->Write();
      efficiencymap->Write();
      noscefficiencymap->Write();
      Neventsmap->Write();
      ipointmap->Write();
      syspdfmap->Write();
      systotmap->Write();
      sysstatmap->Write();
      timemap->Write();
      outputfile->Close();
    }
  }//end of systematics only
  if(efficiencyonly) {
    limcanvas->cd();
    efficiencymap->Draw("COLZ");
    string titletobewritten="Efficiencies in LSP-Glu plane";
    if(ismSUGRA) titletobewritten="Efficiencies in m_{1/2}-m_{0} plane";
    TText *title = write_title(titletobewritten);
    title->Draw();
    if(runninglocally) {
      CompleteSave(limcanvas,"SUSYScan/Efficiency_geq"+any2string(jzb_cut[ibin]));
    }
    limcanvas->cd();
    sysjesmap->Draw("COLZ");
    titletobewritten="N(events) in LSP-Glu plane";
    if(ismSUGRA) titletobewritten="N(events) in m_{1/2}-m_{0} plane";
    TText *title2 = write_title(titletobewritten);
    title2->Draw();
    if(runninglocally) {
      CompleteSave(limcanvas,"SUSYScan/Nevents_geq"+any2string(jzb_cut[ibin]));
    }
    if(!runninglocally) {
      TFile *outputfile=new TFile(("output/DistributedSystematics_job"+string(any2string(jobnumber))+"_of_"+string(any2string(njobs))+".root").c_str(),"UPDATE");
      ipointmap->Write();
      Neventsmap->Write();
      noscefficiencymap->Write();
      efficiencymap->Write();
      timemap->Write();
      outputfile->Close();
    }
  }//end of efficiencies only
 
  delete limitmap;
  delete exclmap;
  delete sysjesmap;
  delete sysjsumap;
  delete sysresmap;
  delete noscefficiencymap;
  delete efficiencymap;
  delete Neventsmap;
  delete ipointmap;
  delete syspdfmap;
  delete systotmap;
  delete sysstatmap;
  delete limcanvas;
}

void scan_SUSY_parameter_space(string mcjzb,string datajzb,vector<float> jzb_cut,bool requireZ, float peakerror, float njobs=-1, float jobnumber=-1, bool systonly=false, bool effonly=false) {
  dout << "Starting the SUSY scan now with all " << jzb_cut.size() << " bin(s)" << endl;
  for(int ibin=0;ibin<jzb_cut.size();ibin++) {
    scan_SUSY_parameter_space(mcjzb,datajzb,jzb_cut,requireZ, peakerror, ibin, njobs, jobnumber,systonly,effonly);
  }
}

