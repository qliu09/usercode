#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <algorithm>
#include <cmath>

#include <TMath.h>
#include <TColor.h>
#include <TPaveText.h>
#include <TRandom.h>
#include <TF1.h>

#ifndef SampleClassLoaded
#include "ActiveSamples.C"
#endif

#ifndef Verbosity
#define Verbosity 0
#endif

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCut.h>
#include <TMath.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TF1.h>



Int_t nBins    =  100;
Float_t jzbMin = -207;
Float_t jzbMax =  243;
Float_t jzbSel =  100;
int iplot=0;
int verbose=0;
string geqleq;
string mcjzbexpression;
bool automatized=false;//if we're running this fully automatized we don't want each function to flood the screen

TString geq_or_leq() {
  if(geqleq=="geq") return TString(">=");
  if(geqleq=="leq") return TString("<=");
  return TString("GEQ_OR_LEQ_ERROR");
}

TString ngeq_or_leq() {
  if(geqleq=="geq") return TString("<=");
  if(geqleq=="leq") return TString(">=");
  return TString("NGEQ_OR_LEQ_ERROR");
}

//______________________________________________________________________________
Double_t Interpolate(Double_t x, TH1 *histo)
{
   // Given a point x, approximates the value via linear interpolation
   // based on the two nearest bin centers
   // Andy Mastbaum 10/21/08
   // in newer ROOT versions but not in the one I have so I had to work around that ... 

   Int_t xbin = histo->FindBin(x);
   Double_t x0,x1,y0,y1;

   if(x<=histo->GetBinCenter(1)) {
      return histo->GetBinContent(1);
   } else if(x>=histo->GetBinCenter(histo->GetNbinsX())) {
      return histo->GetBinContent(histo->GetNbinsX());
   } else {
      if(x<=histo->GetBinCenter(xbin)) {
         y0 = histo->GetBinContent(xbin-1);
         x0 = histo->GetBinCenter(xbin-1);
         y1 = histo->GetBinContent(xbin);
         x1 = histo->GetBinCenter(xbin);
      } else {
         y0 = histo->GetBinContent(xbin);
         x0 = histo->GetBinCenter(xbin);
         y1 = histo->GetBinContent(xbin+1);
         x1 = histo->GetBinCenter(xbin+1);
      }
      return y0 + (x-x0)*((y1-y0)/(x1-x0));
   }
}

//____________________________________________________________________________________
// Plotting with all contributions, i.e. sidebands, peak, osof,ossf ... (for a systematic)
float allcontributionsplot(TTree* events, TCut kBaseCut, TCut kMassCut, TCut kSidebandCut, TCut JZBPosCut, TCut JZBNegCut) {
	iplot++;
	int count=iplot;
	// Define new histogram
	string hname=GetNumericHistoName();
	TH1F* hossfp = new TH1F(hname.c_str(),hname.c_str(),1,-14000,14000);
	events->Draw(TString(mcjzbexpression)+">>"+TString(hname),kBaseCut&&kMassCut&&JZBPosCut&&cutOSSF,"goff");
	hname=GetNumericHistoName();
	TH1F* hossfn = new TH1F(hname.c_str(),hname.c_str(),1,-14000,14000);
	events->Draw(TString(mcjzbexpression)+">>"+TString(hname),kBaseCut&&kMassCut&&JZBNegCut&&cutOSSF,"goff");

	hname=GetNumericHistoName();
	TH1F* hosofp = new TH1F(hname.c_str(),hname.c_str(),1,-14000,14000);
	events->Draw(TString(mcjzbexpression)+">>"+TString(hname),kBaseCut&&kMassCut&&JZBPosCut&&cutOSOF,"goff");
	hname=GetNumericHistoName();
	TH1F* hosofn = new TH1F(hname.c_str(),hname.c_str(),1,-14000,14000);
	events->Draw(TString(mcjzbexpression)+">>"+TString(hname),kBaseCut&&kMassCut&&JZBNegCut&&cutOSOF,"goff");
	
	float obs=0;
	float pred=0;
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	if(!PlottingSetup::RestrictToMassPeak) {
	  hname=GetNumericHistoName();
	  TH1F* sbhossfp = new TH1F(hname.c_str(),hname.c_str(),1,-14000,14000);
	  events->Draw(TString(mcjzbexpression)+">>"+TString(hname),kBaseCut&&kSidebandCut&&JZBPosCut&&cutOSSF,"goff");
	  hname=GetNumericHistoName();
	  TH1F* sbhossfn = new TH1F(hname.c_str(),hname.c_str(),1,-14000,14000);
	  events->Draw(TString(mcjzbexpression)+">>"+TString(hname),kBaseCut&&kSidebandCut&&JZBNegCut&&cutOSSF,"goff");
	  
	  hname=GetNumericHistoName();
	  TH1F* sbhosofp = new TH1F(hname.c_str(),hname.c_str(),1,-14000,14000);
	  events->Draw(TString(mcjzbexpression)+">>"+TString(hname),kBaseCut&&kSidebandCut&&JZBPosCut&&cutOSOF,"goff");
	  hname=GetNumericHistoName();
	  TH1F* sbhosofn = new TH1F(hname.c_str(),hname.c_str(),1,-14000,14000);
	  events->Draw(TString(mcjzbexpression)+">>"+TString(hname),kBaseCut&&kSidebandCut&&JZBNegCut&&cutOSOF,"goff");
	  
	  obs = hossfp->Integral();
	  pred= hossfn->Integral() + (1.0/3)*( hosofp->Integral() - hosofn->Integral() + sbhossfp->Integral() - sbhossfn->Integral() + sbhosofp->Integral() - sbhosofn->Integral());
	  delete sbhossfp,sbhossfn,sbhosofp,sbhosofn;
	} else {
	  obs = hossfp->Integral();
	  pred= hossfn->Integral() + (hosofp->Integral() - hosofn->Integral());
	}
	
	delete hossfp,hossfn,hosofp,hosofn;
	return obs-pred;
}


//____________________________________________________________________________________
// Efficiency plot
TH1F* plotEff(TTree* events, TCut kbase, TString informalname) {
	iplot++;
	int count=iplot;
	// Define new histogram
	char hname[30]; sprintf(hname,"hJzbEff%d",count);
	TH1F* hJzbEff = new TH1F(hname,"JZB selection efficiency ; JZB (GeV/c); Efficiency",
							 nBins,jzbMin,jzbMax);
	Float_t step = (jzbMax-jzbMin)/static_cast<Float_t>(nBins);
	
	events->Draw(mcjzbexpression.c_str(),"genJZB>-400"&&kbase,"goff");
	Float_t maxEff = events->GetSelectedRows();
	if(verbose>0) dout << hname << " (" << informalname <<") " << maxEff <<  std::endl;
	
	if(verbose>0) dout <<  "JZB max = " << jzbMax << std::endl;
	// Loop over steps to get efficiency curve
	char cut[256];
	for ( Int_t iBin = 0; iBin<nBins; ++iBin ) {
		sprintf(cut,"genJZB>%3f",jzbMin+iBin*step);
		events->Draw(mcjzbexpression.c_str(),TCut(cut)&&kbase,"goff");
		Float_t eff = static_cast<Float_t>(events->GetSelectedRows())/maxEff;
		//     dout << "COUCOU " << __LINE__ << std::endl;
		hJzbEff->SetBinContent(iBin+1,eff);
		hJzbEff->SetBinError(iBin+1,TMath::Sqrt(eff*(1-eff)/maxEff));
	}
	return hJzbEff;
	
	
}


//________________________________________________________________________________________
// Master Formula
void master_formula(std::vector<float> eff, float &errHi, float &errLo) {

  float x0 = eff[0];
  float deltaPos = 0, deltaNeg = 0;
  for(int k = 0; k < (eff.size()-1)/2; k++) {
    float xneg = eff[2*k+2];
    float xpos = eff[2*k+1];
    if(xpos-x0>0 || xneg-x0>0) {
      if(xpos-x0 > xneg-x0) {
        deltaPos += (xpos-x0)*(xpos-x0);
      } else { 
        deltaPos += (xneg-x0)*(xneg-x0);
      }
    }
    if(x0-xpos>0 || x0-xneg>0) {
      if(x0-xpos > x0-xneg) {
        deltaNeg += (xpos-x0)*(xpos-x0);
      } else { 
        deltaNeg += (xneg-x0)*(xneg-x0);
      }
    }
  }
  errHi = sqrt(deltaPos);
  errLo = sqrt(deltaNeg);

} 


//________________________________________________________________________________________
// Get normalization factor for the PDFs 
float get_norm_pdf_factor(TTree *events, int k) {

  TH1F *haux = new TH1F("haux", "", 10000, 0, 5);
  char nameVar[20];
  sprintf(nameVar, "pdfW[%d]", k);
  events->Project("haux", nameVar);
  float thisW = haux->Integral();
  events->Project("haux", "pdfW[0]");
  float normW = haux->Integral();

  float factor=thisW/normW;

  delete haux;

  return factor;

}



//________________________________________________________________________________________
// Pile-up efficiency
float pileup(TTree *events, bool requireZ, string informalname, string addcut="",Float_t myJzbMax = 140. ) {
	nBins = 16;
	jzbMax = myJzbMax;
	
	// Acceptance cuts
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	TCut kbase(PlottingSetup::genMassCut&&"genNjets>2&&genZPt>0"&&cutmass&&cutOSSF);
	if(addcut!="") kbase=kbase&&addcut.c_str();//this is mostly for SUSY scans (adding requirements on masses)
	
	if(requireZ) kbase=kbase&&"TMath::Abs(genMID)==23";
	TH1F* hLM4 = plotEff(events,kbase,informalname);
	hLM4->SetMinimum(0.);
	
	// Nominal function
	TF1* func = new TF1("func","0.5*TMath::Erfc([0]*x-[1])",jzbMin,jzbMax);
	func->SetParameter(0,0.03);
	func->SetParameter(1,0.);  
	hLM4->Fit(func,"Q");
	
	// Pimped-up function
	TF1* funcUp = (TF1*)func->Clone();
	funcUp->SetParameter( 0., func->GetParameter(0)/1.1); // 10% systematic error (up in sigma => 0.1 in erfc)
	if(!automatized) dout << "  PU: " << funcUp->Eval(jzbSel) << " " <<  func->Eval(jzbSel) 
	<< "(" << (funcUp->Eval(jzbSel)-func->Eval(jzbSel))/func->Eval(jzbSel)*100. << "%)" << std::endl;
	
	return (funcUp->Eval(jzbSel)-func->Eval(jzbSel))/func->Eval(jzbSel);
	
}

//____________________________________________________________________________________
// Effect of peak shifting
void PeakError(TTree *events,float &result, string mcjzb, float peakerr,string addcut="") {
	TString peakup("("+TString(mcjzb)+"+"+TString(any2string(TMath::Abs(peakerr)))+")"+geq_or_leq()+TString(any2string(jzbSel)));
	TString peakdown("("+TString(mcjzb)+"-"+TString(any2string(TMath::Abs(peakerr)))+")"+geq_or_leq()+TString(any2string(jzbSel)));
	TString peakcentral("("+TString(mcjzb)+")"+geq_or_leq()+TString(any2string(jzbSel)));
	TString npeakup("("+TString(mcjzb)+"+"+TString(any2string(TMath::Abs(peakerr)))+")"+ngeq_or_leq()+"-"+TString(any2string(jzbSel)));
	TString npeakdown("("+TString(mcjzb)+"-"+TString(any2string(TMath::Abs(peakerr)))+")"+ngeq_or_leq()+"-"+TString(any2string(jzbSel)));
	TString npeakcentral("("+TString(mcjzb)+")"+ngeq_or_leq()+"-"+TString(any2string(jzbSel)));
	
	nBins = 1;
	string informalname="PeakErrorCalculation";
	float resup,resdown,rescent;
	for(int i=0;i<3;i++) {
	  string poscut,negcut;
	  if(i==0) {
	    poscut=peakcentral;
	    negcut=npeakcentral;
	  } else if(i==1) {
	    poscut=peakdown;
	    negcut=npeakdown;
	  } else if(i==2) {
	    poscut=peakup;
	    negcut=npeakup;
	  }
	  float res;
	  if(addcut=="") res=allcontributionsplot(events,cutnJets,cutmass,sidebandcut,poscut.c_str(),negcut.c_str());
	  else res=allcontributionsplot(events,cutnJets&&addcut.c_str(),cutmass,sidebandcut,poscut.c_str(),negcut.c_str());
	  if(i==0) rescent=res;
	  else if(i==1) resdown=res;
	  else if(i==2) resup=res;
	}
	if(TMath::Abs(rescent-resup)>TMath::Abs(rescent-resdown)) result=(TMath::Abs(rescent-resup)/rescent);
	else result=(TMath::Abs(rescent-resdown)/rescent);
}

//____________________________________________________________________________________
// Total selection efficiency (MC)
//returns the efficiency WITHOUT signal contamination, and the result and resulterr contain the result and the corresponding error
Value MCefficiency(TTree *events,float &result, float &resulterr,string mcjzb,bool requireZ,int Neventsinfile, string addcut="", int k = 0) {
	
	if(!events) {
	  write_error(__FUNCTION__,"Tree passed for efficiency calculation is invalid!");
	  return Value(0,0);
	}
	
	char jzbSelStr[256]; sprintf(jzbSelStr,"%f",jzbSel);
	// All acceptance cuts at gen. level
	//TCut kbase("abs(genMll-91.2)<20&&genNjets>2&&genZPt>0&&genJZB"+geq_or_leq()+TString(jzbSelStr)+"&&genId1==-genId2");
	TCut kbase("");
	
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	if(requireZ&&PlottingSetup::RestrictToMassPeak) kbase=kbase&&"TMath::Abs(genMID)==23";
	if(addcut!="") kbase=kbase&&addcut.c_str();//this is mostly for SUSY scans (adding requirements on masses)
	// Corresponding reco. cuts
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	write_warning(__FUNCTION__,"There is a problem here ...");

	TCut ksel;("pfJetGoodNum>2"&&cutmass&&"id1==id2&&"+TString(mcjzb)+geq_or_leq()+TString(jzbSelStr));
	TCut ksel2;("pfJetGoodNum>2"&&cutmass&&"id1==id2&&"+TString(mcjzb)+ngeq_or_leq()+TString("-")+TString(jzbSelStr));
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	if(PlottingSetup::RestrictToMassPeak||!ConsiderSignalContaminationForLimits) {
	  ksel=TCut("pfJetGoodNum>2"&&cutmass&&"id1==id2&&"+TString(mcjzb)+geq_or_leq()+TString(jzbSelStr));
	  ksel2=TCut("pfJetGoodNum>2"&&cutmass&&"id1==id2&&"+TString(mcjzb)+ngeq_or_leq()+TString("-")+TString(jzbSelStr));
	} else {
	  ksel=TCut("pfJetGoodNum>2"&&cutmass&&(TString(mcjzb)+geq_or_leq()+TString(jzbSelStr)));
	  ksel2=TCut("pfJetGoodNum>2"&&cutmass&&(TString(mcjzb)+ngeq_or_leq()+TString("-")+TString(jzbSelStr)));
	}
	    
	TCut posSide = kbase&&ksel;
	TCut negSide = kbase&&ksel2;
        string sposSide(posSide);
        string snegSide(negSide);
        char var[20];
        sprintf(var, "pdfW[%d]", k);
        string svar(var);
        string newPosSide = "((id1==id2)&&(" + sposSide + "))*" + svar;
        string newNegSide = "((id1==id2)&&(" + snegSide + "))*" + svar;
        string emnewPosSide = "((id1!=id2)&&(" + sposSide + "))*" + svar;
        string emnewNegSide = "((id1!=id2)&&(" + snegSide + "))*" + svar;

	TH1F *effh= new TH1F("effh","effh",1,-14000,14000);
        if(k>=0)events->Draw((mcjzbexpression+">>effh").c_str(), newPosSide.c_str(),"goff");
	else events->Draw((mcjzbexpression+">>effh").c_str(), sposSide.c_str(),"goff");
	Float_t sel = effh->Integral();
	Float_t nsel=0;
	
	write_error(__FUNCTION__,"Need to adapt signal contamination for offpeak analysis!");
	write_error(__FUNCTION__,"This requires intensive testing!");
	if(ConsiderSignalContaminationForLimits) {
	  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	  if(PlottingSetup::RestrictToMassPeak) {
	    if(k>=0)events->Draw((mcjzbexpression+">>effh").c_str(), newNegSide.c_str(),"goff");
	    else events->Draw((mcjzbexpression+">>effh").c_str(), snegSide.c_str(),"goff");
	    nsel = effh->Integral();
	  } else {
	    if(k>=0)events->Draw((mcjzbexpression+">>effh").c_str(), newNegSide.c_str(),"goff");
	    else events->Draw((mcjzbexpression+">>effh").c_str(), snegSide.c_str(),"goff");
	    nsel = effh->Integral();
	    events->Draw((mcjzbexpression+">>effh").c_str(), emnewPosSide.c_str(),"goff");
	    nsel += effh->Integral();
	    events->Draw((mcjzbexpression+">>effh").c_str(), newNegSide.c_str(),"goff");
	    nsel -= effh->Integral();
	  }
	}

        //Corrections due to normalization in the PDF. This has to be applied as well to the number of events in a file if the definition changes at some point.
        float normFactor = 1;
	if(k>=0) get_norm_pdf_factor(events, k);
        sel = sel/normFactor;
        nsel = nsel/normFactor;

//	events->Draw(mcjzbexpression.c_str(),kbase,"goff");
//	Float_t tot = events->GetSelectedRows();
	Float_t tot = Neventsinfile;
	
	Value result_wo_signalcont;

	if(ConsiderSignalContaminationForLimits) {
	  result=(sel-nsel)/tot;
	  resulterr=(1.0/tot)*TMath::Sqrt(sel+nsel+(sel-nsel)*(sel-nsel)/tot);
	  result_wo_signalcont=Value(sel/tot,TMath::Sqrt(sel/tot*(1+sel/tot)/tot));
	} else {//no signal contamination considered:
	  result=(sel)/tot;
	  resulterr=TMath::Sqrt(sel/tot*(1+sel/tot)/tot);
	  result_wo_signalcont=Value(result,resulterr);
	}
	if(!automatized && k>0 ) dout << "PDF assessment: ";
	if(!automatized) dout << "  MC efficiency: " << result << "+-" << resulterr << "  ( JZB>" << jzbSel << " : " << sel << " , JZB<-" << jzbSel << " : " << nsel << " and nevents=" << tot << ") with normFact=" << normFactor << std::endl;
	delete effh;
	return result_wo_signalcont;
}


//____________________________________________________________________________________
// Selection efficiency for one process (MC)
vector<float> processMCefficiency(TTree *events,string mcjzb,bool requireZ,int Neventsinfile, string addcut) {
  vector<float> process_efficiencies;
  for(int iprocess=0;iprocess<=10;iprocess++) {
    float this_process_efficiency,efferr;
    stringstream addcutplus;
    addcutplus<<addcut<<"&&(process=="<<iprocess<<")";
    MCefficiency(events,this_process_efficiency, efferr,mcjzb,requireZ,Neventsinfile, addcutplus.str(),-1);
    process_efficiencies.push_back(this_process_efficiency);
  }
  return process_efficiencies;
}
	

void JZBefficiency(TTree *events, string informalname, float &jzbeff, float &jzbefferr, bool requireZ, string addcut="") {
	TCut kbase(genMassCut&&"genNjets>2&&genZPt>0"&&cutmass&&cutOSSF);
	if(addcut!="") kbase=kbase&&addcut.c_str();//this is mostly for SUSY scans (adding requirements on masses)
	if(requireZ) kbase=kbase&&"TMath::Abs(genMID)==23";
	TH1F* hLM4 = plotEff(events,kbase,informalname);
	Int_t bin = hLM4->FindBin(jzbSel); // To get the error
	jzbeff=Interpolate(jzbSel,hLM4);
	jzbefferr=hLM4->GetBinError(bin);
	if(!automatized) dout << "  Efficiency at JZB==" << jzbSel  << std::endl;
	if(!automatized) dout << "    " << jzbeff << "+-" << jzbefferr  << std::endl;
}

//________________________________________________________________________
// Effect of energy scale on efficiency
void JZBjetScale(TTree *events, float &jesdown, float &jesup, string informalname,bool requireZ,string addcut="",float syst=0.1, Float_t jzbSelection=-1, TString plotName = "" ) {
	TCut kbase(genMassCut&&"genZPt>0");
	if(addcut!="") kbase=kbase&&addcut.c_str();//this is mostly for SUSY scans (adding requirements on masses)
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	if(requireZ&&PlottingSetup::RestrictToMassPeak) kbase=kbase&&"TMath::Abs(genMID)==23";

	TCut ksel(cutmass&&cutOSSF);
	TCut nJets("pfJetGoodNum>2");
	stringstream down,up;
	down << "pfJetGoodNum"<<30*(1-syst)<<">=3";
	up << "pfJetGoodNum"<<30*(1+syst)<<">=3";
	
	TCut nJetsP(up.str().c_str());
	TCut nJetsM(down.str().c_str());
	
	if ( !(plotName.Length()>1) ) plotName = informalname;
	
	nBins = 1; jzbMin = jzbSel*0.95; jzbMax = jzbSel*1.05;
	TH1F* hist = plotEff(events,(kbase&&ksel&&nJets),informalname);
	
	TH1F* histp = plotEff(events,(kbase&&ksel&&nJetsP),informalname);
	
	TH1F* histm = plotEff(events,(kbase&&ksel&&nJetsM),informalname);
	
	// Dump some information
	Float_t eff  = Interpolate(jzbSel,hist);
	Float_t effp = Interpolate(jzbSel,histp);
	Float_t effm = Interpolate(jzbSel,histm);
	if(!automatized) dout << "  Efficiency at JZB==" << jzbSel  << std::endl;
	if(!automatized) dout << "    JESup: " << effp << " (" << (effp-eff)/eff*100. << "%)" << std::endl; 
	if(!automatized) dout << "    central:  " << eff << std::endl; 
	if(!automatized) dout << "    JESdown: " << effm << " (" << (effm-eff)/eff*100. << "%)" << std::endl; 
	jesup=(effp-eff)/eff;
	jesdown=(effm-eff)/eff;
}

//________________________________________________________________________
// Effect of energy scale on JZB efficiency
void doJZBscale(TTree *events, float &down, float &up, float &syst, float systematic, string informalname, bool requireZ, string addcut) {
	
	TCut kbase(genMassCut&&"genZPt>0&&genNjets>2");
	if(addcut!="") kbase=kbase&&addcut.c_str();//this is mostly for SUSY scans (adding requirements on masses)
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	if(requireZ&&PlottingSetup::RestrictToMassPeak) kbase=kbase&&"TMath::Abs(genMID)==23";
	TCut ksel(cutmass&&cutOSSF);
	
	nBins =    50;
	jzbMin =   0.5*jzbSel;
	jzbMax =   2.0*jzbSel;
	
	TH1F* hist = plotEff(events,kbase&&ksel,informalname);
	
	// Dump some information
	Float_t eff  = Interpolate(jzbSel,hist);
	Float_t effp = Interpolate(jzbSel*(1.+systematic),hist);
	Float_t effm = Interpolate(jzbSel*(1.-systematic),hist);
	if(!automatized) dout << "  efficiency at JZB==" << jzbSel*(1.+systematic)  << "(-"<<systematic*100<<"%) : " << effp << " (" << ((effp-eff)/eff)*100. << "%)"  << std::endl; 
	if(!automatized) dout << "  efficiency at JZB==" << jzbSel  << ": " << eff << std::endl; 
	if(!automatized) dout << "  efficiency at JZB==" << jzbSel*(1.-systematic)  << "(-"<<systematic*100<<"%) : " << effm << " (" << ((effm-eff)/eff)*100. << "%)"  << std::endl;
	up=((effp-eff)/eff);
	down=((effm-eff)/eff);
}

//________________________________________________________________________
// JZB response (true/reco. vs. true)
void JZBresponse(TTree *events, bool requireZ, float &resp, float &resperr, string addcut="",bool isMET = kFALSE, Float_t myJzbMax = 200., Int_t nPeriods = 9 ) {
	
	jzbMin = 20;
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	TCut kbase(genMassCut&&"genZPt>0&&genNjets>2");
	if(addcut!="") kbase=kbase&&addcut.c_str();//this is mostly for SUSY scans (adding requirements on masses)
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	if(requireZ) kbase=kbase&&"TMath::Abs(genMID)==23";
	flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
	TCut ksel(cutmass&&cutOSSF);
	
	TProfile* hJzbResp = new TProfile("hJzbResp","JZB response  ; JZB true (GeV/c); JZB reco. / JZB true", nPeriods, jzbMin, myJzbMax, "" );
	
	if (!isMET) events->Project("hJzbResp","("+TString(mcjzbexpression)+")/genJZB:genJZB",kbase&&ksel);
	else events->Project("hJzbResp","met[4]/genMET:genMET",kbase&&ksel);
	
	hJzbResp->SetMaximum(1.2);
	hJzbResp->SetMinimum(0.2);
	hJzbResp->Fit("pol0","Q");
	TF1 *fittedfunction = hJzbResp->GetFunction("pol0");
	if(!fittedfunction) {
		// in case there are not enough points passing our selection
		cout << "OOPS response function invalid, assuming 100% error !!!!" << endl;
		resp=1;
		resperr=1;
	} else {
		resp=fittedfunction->GetParameter(0);
		resperr=fittedfunction->GetParError(0);
		if(!automatized) dout << "  Response: " << resp << " +/- " << resperr << endl;
	}
	delete hJzbResp;
}


//________________________________________________________________________________________
// PDF uncertainty  
float get_pdf_uncertainty(TTree *events, string mcjzb, bool requireZ, int Neventsinfile, int NPdfs, string addcut="") {
  std::vector<float> efficiency;
  for(int k = 1; k < NPdfs; k++) {
    float result, resulterr;
    MCefficiency(events, result, resulterr, mcjzb, requireZ, Neventsinfile, addcut, k);  
    efficiency.push_back(result);
  }
  float errHi, errLow,err; 
  master_formula(efficiency, errHi, errLow);
  err=errLow;
  if(errHi>errLow) err=errHi;
  if(!automatized) dout << "  Uncertainty from PDF: " << errLow << " (low) and " << errHi << "(high) ---> Picked " << err << endl;
  return err;
 
}

int get_npdfs(TTree *events) {
  int NPDFs;
  events->SetBranchAddress("NPdfs",&NPDFs);
  events->GetEntry(1);
  return NPDFs;
}
  

void do_systematics_for_one_file(TTree *events,int Neventsinfile,string informalname, vector<vector<float> > &results,string mcjzb,string datajzb,float peakerror,bool requireZ=false, string addcut="", bool ismSUGRA=false) {
  float JetEnergyScaleUncert=0.1;
  float JZBScaleUncert=0.1;
  mcjzbexpression=mcjzb;
  float triggereff=5.0/100;// in range [0,1]
  dout << "Trigger efficiency not implemented in this script  yet, still using external one" << endl;
  float leptonseleff=2.0/100;// in range [0,1]
  leptonseleff=TMath::Sqrt(leptonseleff*leptonseleff+leptonseleff*leptonseleff); // because the 2% is per lepton
  dout << "Lepton selection efficiency not implemented in this script  yet, still using external one" << endl;
  
  int NPdfs=0;
  if(ismSUGRA) NPdfs = get_npdfs(events);
  
  float mceff,mcefferr,jzbeff,jzbefferr;
  if(!automatized) dout << "MC efficiencies:" << endl;
  Value mceff_nosigcont = MCefficiency(events,mceff,mcefferr,mcjzb,requireZ,Neventsinfile,addcut,-1);
  if(!automatized) cout << "   Without signal contamination, we find an efficiency of " << mceff_nosigcont << endl;

  if(PlottingSetup::computeJZBefficiency) JZBefficiency(events,informalname,jzbeff,jzbefferr,requireZ,addcut);
  if(!automatized) dout << "JZB efficiency: " << jzbeff << "+/-" << jzbefferr << endl;
  
  if(!automatized) dout << "Error from Peak position:" << endl;
  float sysfrompeak=0;
  PeakError(events,sysfrompeak,mcjzb,peakerror,addcut);
    
  if(!automatized) dout << "Jet energy scale: " << std::endl;
  float jesup,jesdown;
  JZBjetScale(events,jesdown,jesup,informalname,requireZ,addcut,JetEnergyScaleUncert);
  
  if(!automatized) dout << "JZB scale: " << std::endl;
  float scaleup,scaledown,scalesyst;
  doJZBscale(events,scaledown,scaleup,scalesyst,JZBScaleUncert,informalname,requireZ,addcut);
  
  if(!automatized) dout << "JZB response: " << std::endl;
  float resp,resperr;
  if(PlottingSetup::computeJZBresponse) {
  	if(!automatized) dout << "JZB response: " << std::endl;
	JZBresponse(events,requireZ,resp,resperr,addcut);
  }

  if(!automatized) dout << "Pileup: " << std::endl;
  float resolution;
  resolution=pileup(events,requireZ,informalname,addcut);

  float PDFuncert=0;
  if(!automatized) dout << "Assessing PDF uncertainty: " << std::endl;
  if(ismSUGRA) PDFuncert = get_pdf_uncertainty(events, mcjzb, requireZ, Neventsinfile, NPdfs, addcut);

  dout << "_______________________________________________" << endl;
  dout << "                 SUMMARY FOR " << informalname << " with JZB>" << jzbSel << "  (all in %) ";
  if(addcut!="") dout << "With additional cut: " << addcut;
  dout << endl;
  dout << "MC efficiency: " << mceff << "+/-" << mcefferr << endl; // in range [0,1]
  dout << "Trigger efficiency: " << triggereff << endl; // in range [0,1]
  dout << "Lepton Sel Eff: " << leptonseleff << endl; // in range [0,1]
  dout << "Jet energy scale: " << jesup << " " << jesdown << endl; // in range [0,1]
  dout << "JZB Scale Uncert: " << scaledown << " " << scaleup << endl; // in range [0,1]
  dout << "Resolution : " << resolution << endl; // in range [0,1]
  dout << "From peak : " << sysfrompeak << endl; // in range [0,1]
  if(ismSUGRA) dout << "PDF uncertainty  : " << PDFuncert << endl; // in range [0,1]
  if(PlottingSetup::computeJZBefficiency) dout << "JZB efficiency: " << jzbeff << "+/-" << jzbefferr << " (not yet included below) " << endl; // in range [0,1]
  if(PlottingSetup::computeJZBresponse)dout << "JZB response  : " << resp << " +/-" << resperr << " (not yet included below) " << endl; // in range [0,1]
  
  float toterr=0;
  toterr+=(triggereff)*(triggereff);
  toterr+=(leptonseleff)*(leptonseleff);
  if(fabs(jesup)>fabs(jesdown)) toterr+=(jesup*jesup); else toterr+=(jesdown*jesdown);
  if(fabs(scaleup)>fabs(scaledown)) toterr+=(scaleup*scaleup); else toterr+=(scaledown*scaledown);
  toterr+=(resolution*resolution);
  toterr+=(sysfrompeak*sysfrompeak);
  if(ismSUGRA) toterr+=(PDFuncert*PDFuncert);
  dout << "TOTAL SYSTEMATICS: " << TMath::Sqrt(toterr) << " --> " << TMath::Sqrt(toterr)*mceff << endl;
  float systerr=TMath::Sqrt(toterr)*mceff;
  toterr=TMath::Sqrt(toterr*mceff*mceff+mcefferr*mcefferr);//also includes stat err!
  
  dout << "FINAL RESULT : " << 100*mceff << " +/- "<< 100*mcefferr << " (stat) +/- " << 100*systerr << " (syst)   %" << endl;
  dout << "     we thus use the sqrt of the sum of the squares of the stat & syst err, which is : " << 100*toterr << endl;
  dout << "_______________________________________________" << endl;
  
  //Do not modify the lines below or mess with the order; this order is expected by all limit calculating functions!
  vector<float> res;
  res.push_back(jzbSel);
  res.push_back(mceff);
  res.push_back(mcefferr);
  res.push_back(toterr);
  res.push_back(TMath::Sqrt((mcefferr)*(mcefferr)+(toterr*toterr)));
  if(fabs(jesup)>fabs(jesdown)) res.push_back(fabs(jesup)); else res.push_back(fabs(jesdown));
  if(fabs(scaleup)>fabs(scaledown)) res.push_back(fabs(scaleup)); else res.push_back(fabs(scaledown));
  res.push_back(fabs(resolution));
  res.push_back(mceff_nosigcont.getValue());
  res.push_back(mceff_nosigcont.getError());
  if(ismSUGRA) res.push_back(PDFuncert);
  results.push_back(res);
}

vector<vector<float> > compute_systematics(string mcjzb, float mcpeakerror, string datajzb, samplecollection &signalsamples, vector<float> bins, bool requireZ=false) {
  automatized=true;
  vector< vector<float> > systematics;
  for (int isignal=0; isignal<signalsamples.collection.size();isignal++) {
      dout << "Looking at signal " << (signalsamples.collection)[isignal].filename << endl;
      for(int ibin=0;ibin<bins.size();ibin++) {
	jzbSel=bins[ibin];
	geqleq="geq";
	do_systematics_for_one_file((signalsamples.collection)[isignal].events,(signalsamples.collection)[isignal].Nentries,(signalsamples.collection)[isignal].samplename,systematics,mcjzb,datajzb,mcpeakerror,requireZ);
      }//end of bin loop
  }//end of signal loop
  return systematics;
}
