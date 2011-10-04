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

//#include "TTbar_stuff.C"
using namespace std;

using namespace PlottingSetup;

void fill_result_histos(float &zossfp, float &zossfperr, float &zosofp, float &zossfn, float &zosofn, float &sbossfp, float &sbosofp, float &sbossfn, float &sbosofn,string datajzb,float cut, float cuthigh, int mcordata, float &result, vector<int> sel, samplecollection &sampleC, string addcut="") {
  if(!RestrictToMassPeak) write_warning(__FUNCTION__,"Watch out, once we go offpeak this function needs rewriting!");
  string xlabel="JZB [GeV] -- for algoritm internal use only!";
  bool dosignal=false;
  if(mcordata==mcwithsignal) {
    dosignal=true;
    mcordata=0;
  }
  TCut basiccutplus;
  if(addcut=="") basiccutplus=basiccut;
  else basiccutplus=basiccut&&addcut.c_str();
  TH1F *ZOSSFP;
  TH1F *ZOSOFP;
  TH1F *ZOSSFN;
  TH1F *ZOSOFN;
  
  TH1F *SBOSSFP;
  TH1F *SBOSOFP;
  TH1F *SBOSSFN;
  TH1F *SBOSOFN;
    
  if(mcordata==mc||mcordata==data||mcordata==mcwithsignal) {
    ZOSSFP = sampleC.Draw("ZOSSFP",datajzb,1,cut,cuthigh, xlabel, "events",cutmass&&cutOSSF&&cutnJets&&basiccutplus,mcordata,luminosity,dosignal);
    ZOSOFP = sampleC.Draw("ZOSOFP",datajzb,1,cut,cuthigh, xlabel, "events",cutmass&&cutOSOF&&cutnJets&&basiccutplus,mcordata,luminosity,dosignal);
    ZOSSFN = sampleC.Draw("ZOSSFN","-"+datajzb,1,cut,cuthigh, xlabel, "events",cutmass&&cutOSSF&&cutnJets&&basiccutplus,mcordata,luminosity,dosignal);
    ZOSOFN = sampleC.Draw("ZOSOFN","-"+datajzb,1,cut,cuthigh, xlabel, "events",cutmass&&cutOSOF&&cutnJets&&basiccutplus,mcordata,luminosity,dosignal);
    
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
    if(PlottingSetup::RestrictToMassPeak) {
      SBOSSFP = sampleC.Draw("SBOSSFP",datajzb,1,cut,cuthigh, xlabel, "events",cutOSSF&&cutnJets&&basiccutplus&&sidebandcut,mcordata,luminosity,dosignal);
      SBOSOFP = sampleC.Draw("SBOSOFP",datajzb,1,cut,cuthigh, xlabel, "events",cutOSOF&&cutnJets&&basiccutplus&&sidebandcut,mcordata,luminosity,dosignal);
      SBOSSFN = sampleC.Draw("SBOSSFN","-"+datajzb,1,cut,cuthigh, xlabel, "events",cutOSSF&&cutnJets&&basiccutplus&&sidebandcut,mcordata,luminosity,dosignal);
      SBOSOFN = sampleC.Draw("SBOSOFN","-"+datajzb,1,cut,cuthigh, xlabel, "events",cutOSOF&&cutnJets&&basiccutplus&&sidebandcut,mcordata,luminosity,dosignal);
    }
  } else {
    //doing signal only!
    ZOSSFP = sampleC.Draw("ZOSSFP",datajzb,1,cut,cuthigh, xlabel, "events",cutmass&&cutOSSF&&cutnJets&&basiccutplus,mcordata,luminosity,sel);
    ZOSOFP = sampleC.Draw("ZOSOFP",datajzb,1,cut,cuthigh, xlabel, "events",cutmass&&cutOSOF&&cutnJets&&basiccutplus,mcordata,luminosity,sel);
    ZOSSFN = sampleC.Draw("ZOSSFN","-"+datajzb,1,cut,cuthigh, xlabel, "events",cutmass&&cutOSSF&&cutnJets&&basiccutplus,mcordata,luminosity,sel);
    ZOSOFN = sampleC.Draw("ZOSOFN","-"+datajzb,1,cut,cuthigh, xlabel, "events",cutmass&&cutOSOF&&cutnJets&&basiccutplus,mcordata,luminosity,sel);
    
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
    if(PlottingSetup::RestrictToMassPeak) {
      SBOSSFP = sampleC.Draw("SBOSSFP",datajzb,1,cut,cuthigh, xlabel, "events",cutOSSF&&cutnJets&&basiccutplus&&sidebandcut,mcordata,luminosity,sel);
      SBOSOFP = sampleC.Draw("SBOSOFP",datajzb,1,cut,cuthigh, xlabel, "events",cutOSOF&&cutnJets&&basiccutplus&&sidebandcut,mcordata,luminosity,sel);
      SBOSSFN = sampleC.Draw("SBOSSFN","-"+datajzb,1,cut,cuthigh, xlabel, "events",cutOSSF&&cutnJets&&basiccutplus&&sidebandcut,mcordata,luminosity,sel);
      SBOSOFN = sampleC.Draw("SBOSOFN","-"+datajzb,1,cut,cuthigh, xlabel, "events",cutOSOF&&cutnJets&&basiccutplus&&sidebandcut,mcordata,luminosity,sel);
    }
  }

  double err; // UGH!
  zossfp=IntegralAndError(ZOSSFP,1,ZOSSFP->GetNbinsX(),err,"");//making this compatible with my computer which has an outdated version of root.
  zossfperr = err;
  zosofp=ZOSOFP->Integral();
  zossfn=ZOSSFN->Integral();
  zosofn=ZOSOFN->Integral();
  
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    sbossfp=SBOSSFP->Integral();
    sbosofp=SBOSOFP->Integral();
    sbossfn=SBOSSFN->Integral();
    sbosofn=SBOSOFN->Integral();
  } else {
    sbossfp=0;
    sbosofp=0;
    sbossfn=0;
    sbosofn=0;
  }
  
  delete ZOSSFP;
  delete ZOSOFP;
  delete ZOSSFN;
  delete ZOSOFN;
  
  if(PlottingSetup::RestrictToMassPeak) {
    delete SBOSSFP;
    delete SBOSOFP;
    delete SBOSSFN;
    delete SBOSOFN;
  }
  
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) result = zossfn + (1.0/3.)*(zosofp-zosofn)+ (1.0/3.)*(sbossfp-sbossfn)+ (1.0/3.)*(sbosofp-sbosofn);
  else result = zossfn + (zosofp-zosofn);
    
}

void fill_result_histos(float &zossfp, float &zossfperr, float &zosofp, float &zossfn, float &zosofn, float &sbossfp, float &sbosofp, float &sbossfn, float &sbosofn,string datajzb,float cut, float cuthigh, int mcordata, float &result, string addcut="") {
  vector<int> emptyvector;
  fill_result_histos(zossfp, zossfperr,zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,datajzb,cut,cuthigh,mcordata,result,emptyvector,allsamples,addcut);
}
  
/*
void get_result_above_one_fixed_jzb_value(float cut ,string mcjzb,string datajzb, int mcordata,float jzbpeakerrorMC, float jzbpeakerrorData, TCanvas *rescan, bool chatty=false, bool dopoisson=false) {
  rescan->cd();
  if(mcordata==data) dout << "***\nCrunching numbers for JZB>" << cut << " : " << endl;
  
  float zossfp, zossfperr, zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,result;
  if(mcordata==mc) fill_result_histos(zossfp, zossfperr, zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,mcjzb,cut,mcordata,result);
  if(mcordata==data) fill_result_histos(zossfp, zossfperr, zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,datajzb,cut,mcordata,result);
  if(mcordata==mcwithsignal) fill_result_histos(zossfp, zossfperr, zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,mcjzb,cut,mcordata,result);
  
  float zossfpee, zossfpeeerr, zosofpee, zossfnee, zosofnee, sbossfpee, sbosofpee, sbossfnee, sbosofnee,resultee;
  float zossfpmm, zossfpmmerr, zosofpmm, zossfnmm, zosofnmm, sbossfpmm, sbosofpmm, sbossfnmm, sbosofnmm,resultmm;
  if(mcordata==data) {
    fill_result_histos(zossfpee, zossfpeeerr, zosofpee, zossfnee, zosofnee, sbossfpee, sbosofpee, sbossfnee, sbosofnee,datajzb,cut,mcordata,resultee,"id1==0");
    fill_result_histos(zossfpmm, zossfpmmerr, zosofpmm, zossfnmm, zosofnmm, sbossfpmm, sbosofpmm, sbossfnmm, sbosofnmm,datajzb,cut,mcordata,resultmm,"id1==1");
  }
  
  float ppzossfp, ppzossfperr, ppzosofp, ppzossfn, ppzosofn, ppsbossfp, ppsbosofp, ppsbossfn, ppsbosofn, ppresult;
  if(mcordata==mc) fill_result_histos(ppzossfp, ppzossfperr,ppzosofp, ppzossfn, ppzosofn, ppsbossfp, ppsbosofp, ppsbossfn, ppsbosofn,newjzbexpression(mcjzb,jzbpeakerrorMC),cut,mcordata,ppresult);
  if(mcordata==data) fill_result_histos(ppzossfp, ppzossfperr,ppzosofp, ppzossfn, ppzosofn, ppsbossfp, ppsbosofp, ppsbossfn, ppsbosofn,newjzbexpression(datajzb,jzbpeakerrorData),cut,mcordata,ppresult);
  if(mcordata==mcwithsignal) fill_result_histos(ppzossfp, ppzossfperr,ppzosofp, ppzossfn, ppzosofn, ppsbossfp, ppsbosofp, ppsbossfn, ppsbosofn,newjzbexpression(mcjzb,jzbpeakerrorMC),cut,mcordata,ppresult);
  
  float pnzossfp, pnzossfperr, pnzosofp, pnzossfn, pnzosofn, pnsbossfp, pnsbosofp, pnsbossfn, pnsbosofn, pnresult;
  if(mcordata==mc) fill_result_histos(pnzossfp,  pnzossfperr,pnzosofp, pnzossfn, pnzosofn, pnsbossfp, pnsbosofp, pnsbossfn, pnsbosofn,newjzbexpression(mcjzb,-jzbpeakerrorMC),cut,mcordata,pnresult);
  if(mcordata==data) fill_result_histos(pnzossfp,  pnzossfperr,pnzosofp, pnzossfn, pnzosofn, pnsbossfp, pnsbosofp, pnsbossfn, pnsbosofn,newjzbexpression(datajzb,-jzbpeakerrorData),cut,mcordata,pnresult);
  if(mcordata==mcwithsignal) fill_result_histos(pnzossfp,  pnzossfperr,pnzosofp, pnzossfn, pnzosofn, pnsbossfp, pnsbosofp, pnsbossfn, pnsbosofn,newjzbexpression(mcjzb,-jzbpeakerrorMC),cut,mcordata,pnresult);
  
  float syserr=0;
  float peakerr=0;
  float staterr=0;
  float poissonstaterrup=-999;
  float poissonstaterrdown=-999;
  if(fabs(result-pnresult)>fabs(result-ppresult)) peakerr=fabs(result-pnresult); else peakerr=fabs(result-ppresult);
  
  float zjetsestimateuncert=0.5;
  float emuncert=0.5;
  float emsidebanduncert=0.5;
  float eemmsidebanduncert=0.5;
  syserr = (zjetsestimateuncert*zossfn)*(zjetsestimateuncert*zossfn);//first term
  syserr+= ((zosofp)*(zosofp) + (zosofn)*(zosofn))*(1.0/9)*emuncert*emuncert;//sys err from emu method
  syserr+= ((sbossfp)*(sbossfp)+(sbossfn)*(sbossfn))*(1.0/9)*eemmsidebanduncert*eemmsidebanduncert; // sys err from eemm sidebands
  syserr+= ((sbosofp)*(sbosofp)+(sbosofn)*(sbosofn))*(1.0/9)*emsidebanduncert*emsidebanduncert; // sys err from emu sidebands
  syserr=TMath::Sqrt(syserr);
  
  staterr=TMath::Sqrt(zossfn + (1.0/9)*(zosofp+zosofn)+ (1.0/9)*(sbossfp+sbossfn)+ (1.0/9)*(sbosofp+sbosofn));
  if(dopoisson) advanced_poisson(zossfn,zosofp,zosofn,sbossfp,sbossfn,sbosofp,sbosofn,poissonstaterrdown,poissonstaterrup);
  
  float e_to_emu=0.5;
  float m_to_emu=0.5;
  
  if(mcordata==mc) dout << "    MC :: ";
  if(mcordata==mcwithsignal)  dout << "    MC with S :: ";
  if(mcordata==data) dout << "   ";
  dout << "Observed : " << zossfp << "+-" << zossfperr << endl;
  if(mcordata==data) dout << "      Composition: " << zossfpee << " (ee), " << zossfpmm << " (mm) " << endl;
  
  if(mcordata==mc) dout << "    MC :: ";
  if(mcordata==mcwithsignal) dout << "    MC with S :: ";
  if(mcordata==data) dout << "   ";
  dout << "Predicted: " << zossfn << " + (1/3)*(" << zosofp << "-" << zosofn<<") [e&mu]+ (1/3)*(" << sbossfp << "-" << sbossfn<<") [SF,SB]+ (1/3)*(" << sbosofp << "-" << sbosofn<<") [OF,SB] = ";
  dout << zossfn + (1.0/3)*(zosofp-zosofn)+ (1.0/3)*(sbossfp-sbossfn)+ (1.0/3)*(sbosofp-sbosofn)<< " +/- " << peakerr << " (peak ) " << " +/- " << syserr << " (sys) +/- " << staterr << " (stat)" << endl;
  if(dopoisson) dout << "               stat error with Poisson : +" << poissonstaterrup << " - " << poissonstaterrdown << endl;
  
  if(mcordata==data) {
    dout << "      Composition:  (ee)" << zossfnee << " + (1/3)*(" << e_to_emu*zosofp << "-" << e_to_emu*zosofn<<") ["<<e_to_emu<<"*e&mu]+ (1/3)*(" << sbossfpee << "-" << sbossfnee<<") [SF,SB]+ (1/3)*(" << sbosofp*e_to_emu << "-" << sbosofn*e_to_emu<<") ["<<e_to_emu<<"*OF,SB] = ";
    dout << zossfnee + (1.0/3)*(e_to_emu*zosofp-e_to_emu*zosofn)+ (1.0/3)*(sbossfpee-sbossfnee) + (1.0/3)*(sbosofp*e_to_emu - sbosofn*e_to_emu) << endl;
    dout << "                    (mm)" << zossfnmm << " + (1/3)*(" << m_to_emu*zosofp << "-" << m_to_emu*zosofn<<") ["<<m_to_emu<<"*e&mu]+ (1/3)*(" << sbossfpmm << "-" << sbossfnmm<<") [SF,SB]+ (1/3)*(" << sbosofp*m_to_emu << "-" << sbosofn*m_to_emu<<") ["<<m_to_emu<<"*OF,SB] = ";
    dout << zossfnmm + (1.0/3)*(m_to_emu*zosofp - m_to_emu*zosofn) + (1.0/3)*(sbossfpmm -sbossfnmm)+ (1.0/3)*(sbosofp*m_to_emu-sbosofn*e_to_emu) << endl;
  }
  
  if(chatty) {
  dout << "        Pred(ZJets ) \t " << zossfn << endl;
  dout << "        Pred(e&mu;]) \t " << zosofp << "-" << zosofn << " = " << zosofp-zosofn<<endl;
  dout << "        Pred(ossf,sb]) \t " << sbossfp << "-" << sbossfn<<" = "<<sbossfp-sbossfn<<endl;
  dout << "        Pred(osof,sb]) \t " << sbosofp << "-" << sbosofn<<" = "<<sbosofp-sbosofn<<endl;
  }
  
  if(mcordata==data) {
    //store the result!
    Nobs.push_back(zossfp);
    Npred.push_back(zossfn + (1.0/3)*(zosofp-zosofn)+ (1.0/3)*(sbossfp-sbossfn)+ (1.0/3)*(sbosofp-sbosofn));
    float totprederr=0;
    totprederr+=peakerr*peakerr;
    totprederr+=syserr*syserr;
    totprederr+=staterr*staterr;
    totprederr=TMath::Sqrt(totprederr);
    Nprederr.push_back(totprederr);
  }
}
*/
vector<float> get_result_between_two_fixed_jzb_values(float cut , float cuthigh, string mcjzb,string datajzb, int mcordata,float jzbpeakerrorMC, float jzbpeakerrorData, TCanvas *rescan, bool chatty=false, bool dopoisson=false, bool writeanything=true) {
  if(!RestrictToMassPeak) write_warning(__FUNCTION__,"Watch out, once we go offpeak this function needs rewriting!");
/*return vector of floats
  [0] Bpred [1] Bpred uncert [2] Observed [3] Observed error
  // if we use this for the ratio plot we don't want to see any of the results (hence writeanything=false)
  */
  rescan->cd();
  if(writeanything&&mcordata==data) dout << "***\nCrunching numbers for JZB>" << cut << " : " << endl;
  
  float zossfp, zossfperr, zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,result;
  if(mcordata==mc) fill_result_histos(zossfp, zossfperr, zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,mcjzb,cut,cuthigh,mcordata,result);
  if(mcordata==data) fill_result_histos(zossfp, zossfperr, zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,datajzb,cut,cuthigh,mcordata,result);
  if(mcordata==mcwithsignal) fill_result_histos(zossfp, zossfperr, zosofp, zossfn, zosofn, sbossfp, sbosofp, sbossfn, sbosofn,mcjzb,cut,cuthigh,mcordata,result);
  
  float zossfpee, zossfpeeerr, zosofpee, zossfnee, zosofnee, sbossfpee, sbosofpee, sbossfnee, sbosofnee,resultee;
  float zossfpmm, zossfpmmerr, zosofpmm, zossfnmm, zosofnmm, sbossfpmm, sbosofpmm, sbossfnmm, sbosofnmm,resultmm;
  if(mcordata==data) {
    fill_result_histos(zossfpee, zossfpeeerr, zosofpee, zossfnee, zosofnee, sbossfpee, sbosofpee, sbossfnee, sbosofnee,datajzb,cut,cuthigh,mcordata,resultee,"id1==0");
    fill_result_histos(zossfpmm, zossfpmmerr, zosofpmm, zossfnmm, zosofnmm, sbossfpmm, sbosofpmm, sbossfnmm, sbosofnmm,datajzb,cut,cuthigh,mcordata,resultmm,"id1==1");
  }
  
//  float ppzossfp, ppzossfperr, ppzosofp, ppzossfn, ppzosofn, ppsbossfp, ppsbosofp, ppsbossfn, ppsbosofn, ppresult;
//  if(mcordata==mc) fill_result_histos(ppzossfp, ppzossfperr,ppzosofp, ppzossfn, ppzosofn, ppsbossfp, ppsbosofp, ppsbossfn, ppsbosofn,newjzbexpression(mcjzb,jzbpeakerrorMC),cut,cuthigh,mcordata,ppresult);
//  if(mcordata==data) fill_result_histos(ppzossfp, ppzossfperr,ppzosofp, ppzossfn, ppzosofn, ppsbossfp, ppsbosofp, ppsbossfn, ppsbosofn,newjzbexpression(datajzb,jzbpeakerrorData),cut,cuthigh,mcordata,ppresult);
//  if(mcordata==mcwithsignal) fill_result_histos(ppzossfp, ppzossfperr,ppzosofp, ppzossfn, ppzosofn, ppsbossfp, ppsbosofp, ppsbossfn, ppsbosofn,newjzbexpression(mcjzb,jzbpeakerrorMC),cut,cuthigh,mcordata,ppresult);
  
  float pnzossfp, pnzossfperr, pnzosofp, pnzossfn, pnzosofn, pnsbossfp, pnsbosofp, pnsbossfn, pnsbosofn, pnresult;  
  if(mcordata==mc) fill_result_histos(pnzossfp,  pnzossfperr,pnzosofp, pnzossfn, pnzosofn, pnsbossfp, pnsbosofp, pnsbossfn, pnsbosofn,newjzbexpression(mcjzb,-jzbpeakerrorMC),cut,cuthigh,mcordata,pnresult);
  if(mcordata==data) fill_result_histos(pnzossfp,  pnzossfperr,pnzosofp, pnzossfn, pnzosofn, pnsbossfp, pnsbosofp, pnsbossfn, pnsbosofn,newjzbexpression(datajzb,-jzbpeakerrorData),cut,cuthigh,mcordata,pnresult);
  if(mcordata==mcwithsignal) fill_result_histos(pnzossfp,  pnzossfperr,pnzosofp, pnzossfn, pnzosofn, pnsbossfp, pnsbosofp, pnsbossfn, pnsbosofn,newjzbexpression(mcjzb,-jzbpeakerrorMC),cut,cuthigh,mcordata,pnresult);
  
  float syserr=0;
  float peakerr=0;
  float staterr=0;
  float poissonstaterrup=-999;
  float poissonstaterrdown=-999;
//  if(fabs(result-pnresult)>fabs(result-ppresult)) peakerr=fabs(result-pnresult); else peakerr=fabs(result-ppresult);
  
  float zjetsestimateuncert=0.25;
  float emuncert=0.5;
  float emsidebanduncert=0.5;
  float eemmsidebanduncert=0.5;
  syserr = (zjetsestimateuncert*zossfn)*(zjetsestimateuncert*zossfn);//first term
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    syserr+= ((zosofp)*(zosofp) + (zosofn)*(zosofn))*(1.0/9)*emuncert*emuncert;//sys err from emu method
    syserr+= ((sbossfp)*(sbossfp)+(sbossfn)*(sbossfn))*(1.0/9)*eemmsidebanduncert*eemmsidebanduncert; // sys err from eemm sidebands
    syserr+= ((sbosofp)*(sbosofp)+(sbosofn)*(sbosofn))*(1.0/9)*emsidebanduncert*emsidebanduncert; // sys err from emu sidebands
  } else {
    syserr+= ((zosofp)*(zosofp) + (zosofn)*(zosofn))*emuncert*emuncert;//sys err from emu method
  }
    
  syserr=TMath::Sqrt(syserr);
  
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) {
    staterr=TMath::Sqrt(zossfn + (1.0/9)*(zosofp+zosofn)+ (1.0/9)*(sbossfp+sbossfn)+ (1.0/9)*(sbosofp+sbosofn));
  } else {
    staterr=TMath::Sqrt(zossfn + (zosofp+zosofn));
  }
  if(dopoisson) advanced_poisson(zossfn,zosofp,zosofn,sbossfp,sbossfn,sbosofp,sbosofn,poissonstaterrdown,poissonstaterrup);
  
  float e_to_emu=0.5;
  float m_to_emu=0.5;
  
  if(writeanything) {
    if(mcordata==mc) dout << "    MC :: ";
    if(mcordata==mcwithsignal)  dout << "    MC with S :: ";
    if(mcordata==data) dout << "   ";
    dout << "Observed : " << zossfp << "+-" << zossfperr << endl;
    if(mcordata==data) dout << "      Composition: " << zossfpee << " (ee), " << zossfpmm << " (mm) " << endl;
    
    if(mcordata==mc) dout << "    MC :: ";
    if(mcordata==mcwithsignal) dout << "    MC with S :: ";
    if(mcordata==data) dout << "   ";
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
    if(PlottingSetup::RestrictToMassPeak) {
      dout << "Predicted: " << zossfn << " + (1/3)*(" << zosofp << "-" << zosofn<<") [e&mu]+ (1/3)*(" << sbossfp << "-" << sbossfn<<") [SF,SB]+ (1/3)*(" << sbosofp << "-" << sbosofn<<") [OF,SB] = ";
      dout << zossfn + (1.0/3)*(zosofp-zosofn)+ (1.0/3)*(sbossfp-sbossfn)+ (1.0/3)*(sbosofp-sbosofn)<< " +/- " << peakerr << " (peak ) " << " +/- " << syserr << " (sys) +/- " << staterr << " (stat)" << endl;
      if(dopoisson) dout << "               stat error with Poisson : +" << poissonstaterrup << " - " << poissonstaterrdown << endl;
    } else {
      dout << "Predicted: " << zossfn << " + (" << zosofp << "-" << zosofn<<") [e&mu]= ";
      dout << zossfn + (zosofp-zosofn) << " +/- " << peakerr << " (peak ) " << " +/- " << syserr << " (sys) +/- " << staterr << " (stat)" << endl;
      if(dopoisson) dout << "               stat error with Poisson : +" << poissonstaterrup << " - " << poissonstaterrdown << endl;
    }
    
    
    if(mcordata==data) {
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
    if(PlottingSetup::RestrictToMassPeak) {
	dout << "      Composition:  (ee)" << zossfnee << " + (1/3)*(" << e_to_emu*zosofp << "-" << e_to_emu*zosofn<<") ["<<e_to_emu<<"*e&mu]+ (1/3)*(" << sbossfpee << "-" << sbossfnee<<") [SF,SB]+ (1/3)*(" << sbosofp*e_to_emu << "-" << sbosofn*e_to_emu<<") ["<<e_to_emu<<"*OF,SB] = ";
	dout << zossfnee + (1.0/3)*(e_to_emu*zosofp-e_to_emu*zosofn)+ (1.0/3)*(sbossfpee-sbossfnee) + (1.0/3)*(sbosofp*e_to_emu - sbosofn*e_to_emu) << endl;
	dout << "                    (mm)" << zossfnmm << " + (1/3)*(" << m_to_emu*zosofp << "-" << m_to_emu*zosofn<<") ["<<m_to_emu<<"*e&mu]+ (1/3)*(" << sbossfpmm << "-" << sbossfnmm<<") [SF,SB]+ (1/3)*(" << sbosofp*m_to_emu << "-" << sbosofn*m_to_emu<<") ["<<m_to_emu<<"*OF,SB] = ";
	dout << zossfnmm + (1.0/3)*(m_to_emu*zosofp - m_to_emu*zosofn) + (1.0/3)*(sbossfpmm -sbossfnmm)+ (1.0/3)*(sbosofp*m_to_emu-sbosofn*e_to_emu) << endl;
      } else {
	dout << "      Composition:  (ee)" << zossfnee << " + (" << e_to_emu*zosofp << "-" << e_to_emu*zosofn<<") ["<<e_to_emu<<"*e&mu] = ";
	dout << zossfnee + (e_to_emu*zosofp-e_to_emu*zosofn) << endl;
	dout << "                    (mm)" << zossfnmm << " + (" << m_to_emu*zosofp << "-" << m_to_emu*zosofn<<") ["<<m_to_emu<<"*e&mu] = ";
	dout << zossfnmm + (m_to_emu*zosofp - m_to_emu*zosofn) << endl;
      }
    }
    
    if(chatty) {
      dout << "        Pred(ZJets ) \t " << zossfn << endl;
      dout << "        Pred(e&mu;]) \t " << zosofp << "-" << zosofn << " = " << zosofp-zosofn<<endl;
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
    if(PlottingSetup::RestrictToMassPeak) {
	dout << "        Pred(ossf,sb]) \t " << sbossfp << "-" << sbossfn<<" = "<<sbossfp-sbossfn<<endl;
	dout << "        Pred(osof,sb]) \t " << sbosofp << "-" << sbosofn<<" = "<<sbosofp-sbosofn<<endl;
      }
    }
  }//end of writeanything
  
  if(mcordata==data) {
    //store the result!
    Nobs.push_back(zossfp);
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
    if(PlottingSetup::RestrictToMassPeak) Npred.push_back(zossfn + (1.0/3)*(zosofp-zosofn)+ (1.0/3)*(sbossfp-sbossfn)+ (1.0/3)*(sbosofp-sbosofn));
    else Npred.push_back(zossfn + (zosofp-zosofn));
    float totprederr=0;
    totprederr+=peakerr*peakerr;
    totprederr+=syserr*syserr;
    totprederr+=staterr*staterr;
    totprederr=TMath::Sqrt(totprederr);
    Nprederr.push_back(totprederr);
  }
  vector<float> resultvector;
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) resultvector.push_back(zossfn + (1.0/3)*(zosofp-zosofn)+ (1.0/3)*(sbossfp-sbossfn)+ (1.0/3)*(sbosofp-sbosofn));
  else resultvector.push_back(zossfn + (zosofp-zosofn));
  
  resultvector.push_back(TMath::Sqrt(peakerr*peakerr+syserr*syserr+staterr*staterr));
  resultvector.push_back(zossfp);
  resultvector.push_back(zossfperr);
  
  return resultvector;
}

void get_result_above_one_fixed_jzb_value(float cut ,string mcjzb,string datajzb, int mcordata,float jzbpeakerrorMC, float jzbpeakerrorData, TCanvas *rescan, bool chatty=false, bool dopoisson=false) {
  get_result_between_two_fixed_jzb_values(cut ,15000, mcjzb,datajzb, mcordata,jzbpeakerrorMC, jzbpeakerrorData, rescan, chatty, dopoisson);
}

  
void get_result(string mcjzb, string datajzb, float jzbpeakerrordata, float jzbpeakerrormc, vector<float> jzbcuts, bool chatty=false, bool dopoisson=false,bool doquick=false) {
  TCanvas *rescan = new TCanvas("rescan","Result Canvas");
   for(int icut=0;icut<jzbcuts.size();icut++) {
     get_result_above_one_fixed_jzb_value(jzbcuts[icut],mcjzb,datajzb,data,jzbpeakerrormc,jzbpeakerrordata,rescan,chatty,dopoisson);
     if(!doquick) {
       get_result_above_one_fixed_jzb_value(jzbcuts[icut],mcjzb,datajzb,mc,jzbpeakerrormc,jzbpeakerrordata,rescan,chatty,dopoisson);
       get_result_above_one_fixed_jzb_value(jzbcuts[icut],mcjzb,datajzb,mcwithsignal,jzbpeakerrormc,jzbpeakerrordata,rescan,chatty,dopoisson);
     }
   }
  delete rescan;
}
