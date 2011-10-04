/****

Off peak status (RestrictToMassPeak) : 

x  Necessary adaptations identified
x  Started working on necessary adaptations
x  Necessary adaptations implemented
x  Necessary adaptations tested

NO ADAPTATIONS REQUIRED


****/
#include <iostream>
#include <sstream>
#include <iomanip>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <vector>
#include <TROOT.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TRandom.h>
#ifndef GeneralToolBoxLoaded
#include "GeneralToolBox.C"
#endif
#ifndef Verbosity
#define Verbosity 0
#endif

using namespace std;

Double_t LogParabola(Double_t *x,Double_t *par)
{
  return par[0]*TMath::Exp(-par[1]*(x[0]-par[2])*(x[0]-par[2])); // we're adding a "logarithmic parabola" :-)
  //note: the abs() around the first parameter ensures that, when fitting, no negative values are chosen.
}

Double_t LogParabolaP(Double_t *x,Double_t *par)
{
  float fitval = par[0]*TMath::Exp(-par[1]*(x[0]-par[2])*(x[0]-par[2])); // we're adding a "logarithmic parabola" :-)
  fitval+= statErrorP(fitval);
  return fitval;
}

Double_t LogParabolaN(Double_t *x,Double_t *par)
{
  float fitval = par[0]*TMath::Exp(-par[1]*(x[0]-par[2])*(x[0]-par[2])); // we're adding a "logarithmic parabola" :-)
  fitval-= statErrorN(fitval);
  return fitval;
}



bool doreject=false;
float low_reject=-10;
float hi_reject=10;

bool dofixed=true;


bool addparabola=true;
float parabola_height=0;
float parabola_inclination=0;
float parabola_pointzero=0;

float find_KM_peak(TH1F *all, TH1F *ttbar, float minfit, float maxfit, bool is_data, float &error,float &Sigma);
float find_Gauss_peak(TH1F *all, TH1F *ttbar, float minfit, float maxfit, bool is_data, float &error,float &Sigma,int numsig);

Double_t CrystalBallPlusLogParabola(double *x, double *par)
{
  //parameters: 
  //N: the way we scale the function
  //alpha (where the function changes)
  //n: exponent of the power expression in the left area
  //xbar: peak of the gaussian part (RHS)
  //sigma: width of the gaussian part (RHS)
  float N=par[0];
  float alpha=par[3]; //verified (orig: 1)
  float n=par[4]; // verified (orig: 2)
  float xbar=par[1]; //verified (orig: 3)
  float sigma=par[2]; //verified (orig: 4)
  float altX=-x[0];
  float result=-999;
  if(((altX-xbar)/sigma>-alpha)){
    result=N*TMath::Exp(-(altX-xbar)*(altX-xbar)/(2*sigma*sigma));
  }
  else
  {
    //if we are outside the central (Gaussian) area things become more difficult ...
    float A=TMath::Power(n/TMath::Abs(alpha),n)*TMath::Exp(-alpha*alpha/2);
    float B=n/TMath::Abs(alpha) - TMath::Abs(alpha);
    if((altX-xbar)/sigma<=-alpha) result=N*A*TMath::Power((B-((altX-xbar)/sigma)),-n);
    if((altX-xbar)/sigma>=alpha) result=N*A*TMath::Power((B+((altX-xbar)/sigma)),-n);
  }
  
  result+=par[5]*TMath::Exp(-par[6]*(x[0]-par[7])*(x[0]-par[7])); // we're adding a "logarithmic parabola" :-)
  if(par[5]<0) return -999; // there can be no negative ttbar contribution, so just return a value which is going to be a horrible fit.
  if(par[6]<0) return -999; // the parabola needs to close (i.e. tend to negative values for large |jzb|, not to large positive values)

  return result;
}

Double_t CrystalBallPlusLogParabolaP(double *x, double *par)
{
  float parameter_bkp=par[5];
  par[5]=0;
  float zjetsresult=CrystalBallPlusLogParabola(x,par);
  par[5]=parameter_bkp;
  parameter_bkp=par[0];
  par[0]=0;
  float ttbarresult=CrystalBallPlusLogParabola(x,par);
  par[0]=parameter_bkp;
  return zjetsresult+ttbarresult+TMath::Sqrt(zjetsresult+(1.0/3.0)*ttbarresult);
}

Double_t CrystalBallPlusLogParabolaN(double *x, double *par)
{
  float parameter_bkp=par[5];
  par[5]=0;
  float zjetsresult=CrystalBallPlusLogParabola(x,par);
  par[5]=parameter_bkp;
  parameter_bkp=par[0];
  par[0]=0;
  float ttbarresult=CrystalBallPlusLogParabola(x,par);
  par[0]=parameter_bkp;
  return zjetsresult+ttbarresult-TMath::Sqrt(zjetsresult+(1.0/3.0)*ttbarresult);
}

Double_t KrystalMallLogPar(double *x, double *par)
{
  //parameters: 
  //N: the way we scale the function
  //alpha (where the function changes)
  //n: exponent of the power expression in the left area
  //xbar: peak of the gaussian part (RHS)
  //sigma: width of the gaussian part (RHS)
  float N=par[0];
  float alpha=par[1];
  float n=par[2];
  float xbar=par[3];
  float sigma=par[4];
  float altX=x[0];
  float result=-999;
  if(doreject&&x[0]>low_reject&&x[0]<hi_reject)
  {
    TF1::RejectPoint();
    return 0;
  }
  if(((altX-xbar)/sigma>-alpha)&&((altX-xbar)/sigma<alpha)){
    result=N*TMath::Exp(-(altX-xbar)*(altX-xbar)/(2*sigma*sigma));
  }
  else
  {
    //if we are outside the central (Gaussian) area things become more difficult ...
    float A=TMath::Power(n/TMath::Abs(alpha),n)*TMath::Exp(-alpha*alpha/2);
    float B=n/TMath::Abs(alpha) - TMath::Abs(alpha);
    if((altX-xbar)/sigma<=-alpha) result=N*A*TMath::Power((B-((altX-xbar)/sigma)),-n);
    if((altX-xbar)/sigma>=alpha) result=N*A*TMath::Power((B+((altX-xbar)/sigma)),-n);
  }

  if(addparabola) {
    if(dofixed) {
      result+=parabola_height*TMath::Exp(-parabola_inclination*(x[0]-parabola_pointzero)*(x[0]-parabola_pointzero)); // we're adding a "logarithmic parabola" :-)
    }
    else {
      result+=par[5]*TMath::Exp(-par[6]*(x[0]-par[7])*(x[0]-par[7])); // we're adding a "logarithmic parabola" :-)
      if(par[5]<0) return -999; // there can be no negative ttbar contribution, so just return a value which is going to be a horrible fit.
      if(par[6]<0) return -999; // the parabola needs to close (i.e. tend to negative values for large |jzb|, not to large positive values)
    }
  }
  return result;
}


void do_ttbar_fit(TH1F *ttbar,TF1 *logpar, TF1 *KM)
{
  logpar->SetParameters(10,2,3);
  ttbar->Fit(logpar,"NQ");
  ttbar->Fit(logpar,"NQ");
  ttbar->Fit(logpar,"NQ");
  ttbar->Fit(logpar,"NQ");
  ttbar->SetStats(0);
  parabola_height=logpar->GetParameter(0);
  parabola_inclination=logpar->GetParameter(1);
  parabola_pointzero=logpar->GetParameter(2);
}
  
void draw_complete_fit(TH1F *all, TH1F *ttbar, float minfit, float maxfit, bool is_data, TF1 *KM)
{
  TCanvas *fitsummary;
  if(is_data) {
    fitsummary= new TCanvas("fitsummary","Fit Summary",1000,500);
    fitsummary->Divide(2,1);
  }
  else {
    fitsummary= new TCanvas("fitsummary","Fit Summary",1200,400);
    fitsummary->Divide(3,1);
  }
  TF1 *logpar = new TF1("logpar",LogParabola,minfit,maxfit,3);
  
  logpar->SetParameters(KM->GetParameter(5),KM->GetParameter(6),KM->GetParameter(7));
  logpar->SetLineColor(kOrange);
  logpar->SetLineStyle(2);
  if(!is_data)
  {
    ttbar->GetXaxis()->SetTitle("JZB (GeV/c)");
    ttbar->GetYaxis()->SetTitle("events");
    ttbar->GetXaxis()->CenterTitle();
    ttbar->GetYaxis()->CenterTitle();
    ttbar->SetLineColor(kRed);
    fitsummary->cd(1);
    ttbar->Draw();
    fitsummary->cd(1);
    logpar->Draw("same");
    TLegend *leg = new TLegend(0.3,0.25,0.65,0.4);
    leg->AddEntry(ttbar,"t#bar{t} (mc)","l");
    leg->AddEntry(logpar,"Fit with Log. Parabola","l");
    leg->SetLineColor(kWhite);
    leg->SetFillColor(kWhite);
    leg->Draw();
    TText *title1=write_title("t#bar{t} Distribution and Fit");
    title1->Draw();
  }
  fitsummary->cd(2-int(is_data));
  fitsummary->cd(2-int(is_data))->SetLogy(1);
  all->GetYaxis()->SetTitle("events");
  all->GetYaxis()->CenterTitle();
  all->Draw();
  ttbar->SetLineColor(kRed);
  if(!is_data) ttbar->Draw("same");
  KM->SetLineWidth(1);
  KM->Draw("same");
  logpar->SetLineWidth(1);
  logpar->Draw("same");
  if(!is_data)ttbar->Draw("same");
  TLegend *leg2 = new TLegend(0.65,0.65,0.89,0.89);
  if(is_data) leg2->AddEntry(all,"Data","l");
  else leg2->AddEntry(all,"Stacked MC","l");
  leg2->AddEntry(KM,"Fitted KM Function","l");
  if(!is_data) leg2->AddEntry(ttbar,"t#bar{t} MC","l");
  leg2->AddEntry(logpar,"t#bar{t} (Fit)","l");
  leg2->SetFillColor(kWhite);
  leg2->SetLineColor(kWhite);
  leg2->Draw();
  TText *title2=write_title("Distribution and Fits (log.)");
  title2->Draw();
  fitsummary->cd(3-is_data);
  all->Draw();
  KM->Draw("same");
  float peaklocation=KM->GetParameter(3);
  TLine *muline = new TLine(peaklocation,0,peaklocation,all->GetMaximum());
  muline->SetLineColor(kBlue);
  muline->SetLineStyle(2);
  muline->Draw();
  TLegend *leg = new TLegend(0.75,0.75,0.89,0.89);
  if(is_data) leg2->AddEntry(all,"Data","l");
  else leg->AddEntry(all,"Stacked MC","l");
  leg->AddEntry(KM,"Fitted KM Function","l");
  stringstream mulinelabel;
  mulinelabel<<"Peak position at #mu="<<peaklocation;
  leg->AddEntry(muline,mulinelabel.str().c_str(),"l");
  leg->SetLineColor(kWhite);
  leg->SetFillColor(kWhite);
  leg->Draw();
  mulinelabel<<"+/-"<<TMath::Abs(KM->GetParError(3));
  TText *title3=write_title("Distribution and Fits");
  title3->Draw();
  TText *titlel=write_title_low(mulinelabel.str().c_str());
  titlel->Draw();
  
  stringstream printtop;
  printtop << "#mu="<<std::setprecision(3)<<KM->GetParameter(3)<<"+/-"<<std::setprecision(3)<<KM->GetParError(3);
  TLatex *toptext = new TLatex(0,all->GetMaximum()*1.3,printtop.str().c_str());
  toptext->SetTextAlign(22);
//  toptext->Draw();
  
  doreject=false;
  TF1 *wholefitfunc=(TF1*)KM->Clone();
  doreject=true;
  wholefitfunc->SetLineColor(kRed);
  wholefitfunc->SetLineStyle(2);
  wholefitfunc->Draw("same");
  
  fitsummary->cd(2-is_data);
  wholefitfunc->Draw("same");

  if(is_data) CompleteSave(fitsummary, "fit/Fit_Summary_Data");
  else CompleteSave(fitsummary,"fit/Fit_Summary_MC");
  
}

float Kostas_algorithm(TH1F *hist, float &error, float &sigma, TF1* fitFunc, float lowlimit, float highlimit,bool is_data)
{
    float mean = hist->GetBinCenter( hist->GetMaximumBin());
    float rms = hist->GetRMS();
    mean = hist->GetBinCenter( hist->GetMaximumBin());

    fitFunc->SetParameter(1,mean);

    hist->Fit(fitFunc,"QLL0","",mean-10,mean+10);

    mean=fitFunc->GetParameter(1);
    rms=fitFunc->GetParameter(2);
    error=fitFunc->GetParError(1);
 
    bool printOut = false; // print the peak estimate in the i-th iteration
 
    // --- perform iterations
    int numIterations=5;

    if(printOut) dout << " ( ";
    for(int i=1;i<numIterations+1;i++) //--- modify the number of iterations until peak is stable
    {
           hist->Fit(fitFunc,"QLLN","same",mean - lowlimit*rms, mean + highlimit*rms);  // fit -2 +1 sigma from previous iteration
    mean=fitFunc->GetParameter(1);
        fitFunc->SetRange(mean - lowlimit*rms, mean + highlimit*rms);
    if(printOut) dout << mean << ",";
    }
    if(printOut) dout << " ) ";
    if(printOut) dout << endl;
    mean=fitFunc->GetParameter(1);
    sigma=fitFunc->GetParameter(2);
    error=1.0*fitFunc->GetParError(1);

    // below this point we're merely doing cosmetics :-) 
    TCanvas *peakfitcanvas = new TCanvas("peakfitcanvas","Fitting Canvas");
    peakfitcanvas->cd();
    
    hist->SetMinimum(0);
    if(is_data) hist->Draw("e1");
    else hist->Draw("histo");
    fitFunc->SetLineColor(kBlue);
    fitFunc->SetLineWidth(1);
    fitFunc->Draw("same");
    hist->SetStats(0);
    TLegend *leg;
    if(is_data) {
      leg= make_legend("Fit (Data)");
      leg->AddEntry(hist,"Data","p");
    }
    else {
      leg= make_legend("Fit (MC)");
      leg->AddEntry(hist,"MC","l");
    }
  
    leg->AddEntry(fitFunc,"Fit","l");
    leg->SetX1(0.7);
    leg->SetY1(0.7);
    leg->Draw();
    
    TText *ftitle=write_text(0.20,0.86,"Fit results:");
    ftitle->SetTextSize(0.03);
    ftitle->SetTextAlign(11);
    stringstream fitresult;
    fitresult << "#mu=" << std::setprecision(4) <<  mean << "+/-" << std::setprecision(4) <<  error;
//    TText *title1=write_text(0.20,0.96,fitresult.str().c_str());
    TText *title1=write_text(0.20,0.82,fitresult.str().c_str());
    title1->SetTextSize(0.03);
    title1->SetTextAlign(11);
    stringstream sigmainfo;
    sigmainfo << "#sigma=" << std::setprecision(4) << fitFunc->GetParameter(2) << "+/-" << std::setprecision(4) << fitFunc->GetParError(2);
//    TText *sigmatext=write_text(0.80,0.96,sigmainfo.str().c_str());
    TText *sigmatext=write_text(0.20,0.78,sigmainfo.str().c_str());
    sigmatext->SetTextSize(0.03);
    sigmatext->SetTextAlign(11);
    
//    TText* toptitle;
//    if(is_data) toptitle = write_title("Fit Result (data)");
//    else toptitle = write_title("Fit Result (MC)");
//    toptitle->Draw();
    ftitle->Draw();
    title1->Draw();
    sigmatext->Draw();
    if(!is_data) {
      CompleteSave(peakfitcanvas,"fit/Fit_Summary_MC");
      PlottingSetup::JZBPeakPositionMC=mean;
      PlottingSetup::JZBPeakWidthMC=fitFunc->GetParameter(2);
    } else {
      CompleteSave(peakfitcanvas,"fit/Fit_Summary_Data");
      PlottingSetup::JZBPeakPositionData=mean;
      PlottingSetup::JZBPeakWidthData=fitFunc->GetParameter(2);
    }
    delete peakfitcanvas;
    
    return mean;
}



float find_peak(TH1F *all, TH1F *ttbar, float minfit, float maxfit, bool is_data, float &error,float &Sigma, int method)
{
  float peak_position=0;
  if(method==0||method>1) {
    //looking at a gaus request
    int numsig=1;
    if(method>1) numsig=method;
    peak_position=find_Gauss_peak(all,ttbar,minfit,maxfit,is_data,error,Sigma,numsig);
  }
  if(method==1) {
    //looking at a KM request
    peak_position=find_KM_peak(all,ttbar,minfit,maxfit,is_data,error,Sigma);
  }
  if(method==-99) { // KOSTAS!!
    TF1 *f1 = new TF1("f1","gaus",-40,40);
    peak_position=Kostas_algorithm(all,error,Sigma,f1,2.5,2.5,is_data);
  }
  return peak_position;
}


float get_Gaussian_peak(TH1F *hist, float &error, float &sigma, TF1* fitFunc, float lowlimit, float highlimit,bool is_data,int numsig)
{
    TCanvas *fitcanvas = new TCanvas("fitcanvas","fitcanvas");
    float mean = hist->GetBinCenter( hist->GetMaximumBin());
    float rms = hist->GetRMS();

    mean = hist->GetBinCenter( hist->GetMaximumBin());

    fitFunc->SetParameter(1,mean);

    hist->Fit(fitFunc,"QLL0","",mean-10,mean+10);

    mean=fitFunc->GetParameter(1);
    rms=fitFunc->GetParameter(2);
    error=fitFunc->GetParError(1);
  
    bool printOut = false; // print the peak estimate in the i-th iteration
 
    // --- perform iterations
    int numIterations=5;

    if(printOut) dout << " ( ";
    for(int i=1;i<numIterations+1;i++) //--- modify the number of iterations until peak is stable 
    {
       	hist->Fit(fitFunc,"QLLN","same",mean - numsig*rms, mean + numsig*rms);  // fit -2 +1 sigma from previous iteration
	mean=fitFunc->GetParameter(1);
        fitFunc->SetRange(mean - numsig*rms, mean + numsig*rms);
	if(printOut) dout << mean << ",";
    }
    if(printOut) dout << " ) ";
    if(printOut) dout << endl;
    mean=fitFunc->GetParameter(1);
	sigma=fitFunc->GetParameter(2);
    error=1.0*fitFunc->GetParError(1);
    fitcanvas->cd();
    hist->SetMinimum(0);
    if(is_data) hist->Draw("e1");
    else hist->Draw("histo");
    fitFunc->SetLineColor(kBlue);
    fitFunc->SetLineWidth(1);
    fitFunc->Draw("same");
    hist->SetStats(0);
    TLegend *leg;
    if(is_data) {
      leg= make_legend("Fit (Data)");
      leg->AddEntry(hist,"Data","p");
    }
    else {
      leg= make_legend("Fit (MC)");
      leg->AddEntry(hist,"MC","l");
    }
    
    leg->AddEntry(fitFunc,"Fit","l");
    leg->Draw();
    
    TText *ftitle=write_text(0.20,0.86,"Fit results:");
    ftitle->SetTextSize(0.03);
    ftitle->SetTextAlign(11);
    stringstream fitresult;
    fitresult << "#mu=" << std::setprecision(4) <<  mean << "+/-" << std::setprecision(4) <<  error;
//    TText *title1=write_text(0.20,0.96,fitresult.str().c_str());
    TText *title1=write_text(0.20,0.82,fitresult.str().c_str());
    title1->SetTextSize(0.03);
    title1->SetTextAlign(11);
    stringstream sigmainfo;
    sigmainfo << "#sigma=" << std::setprecision(4) << fitFunc->GetParameter(2) << "+/-" << std::setprecision(4) << fitFunc->GetParError(2);
//    TText *sigmatext=write_text(0.80,0.96,sigmainfo.str().c_str());
    TText *sigmatext=write_text(0.20,0.78,sigmainfo.str().c_str());
    sigmatext->SetTextSize(0.03);
    sigmatext->SetTextAlign(11);
    
//    TText* toptitle;
//    if(is_data) toptitle = write_title("Fit Result (data)");
//    else toptitle = write_title("Fit Result (MC)");
//    toptitle->Draw();
    ftitle->Draw();
    title1->Draw();
    sigmatext->Draw();
    if(!is_data) CompleteSave(fitcanvas,"fit/Fit_Summary_MC");
    if(is_data) CompleteSave(fitcanvas,"fit/Fit_Summary_Data");
    

//    dout << "[" << fitFunc->GetParameter(1) << " , " << fitFunc->GetParError(1) << "]" << endl;
    return mean;
}


float find_Gauss_peak(TH1F *all, TH1F *ttbar, float minfit, float maxfit, bool is_data, float &error,float &Sigma,int numsig)
{
  TF1 *fitfunc = new TF1("fitfunc","gaus",minfit,maxfit);
  float peakpos = get_Gaussian_peak(all,error,Sigma,fitfunc, minfit, maxfit,is_data,numsig);
  return peakpos;
}

  float find_KM_peak(TH1F *all, TH1F *ttbar, float minfit, float maxfit, bool is_data, float &error,float &Sigma)
{
  all->SetLineColor(kBlue);
  all->SetStats(0);
  all->SetTitle("");
  all->GetXaxis()->SetTitle("JZB (GeV/c)");
  all->GetYaxis()->SetTitle("events");
  all->GetXaxis()->CenterTitle();
  all->GetYaxis()->CenterTitle();
  TF1 *fitfunc = new TF1("fitfunc",KrystalMallLogPar,0.8*minfit,0.8*maxfit,8);
  if(!is_data)
  {
    TF1 *logpar = new TF1("logpar",LogParabola,minfit,maxfit,3);
    do_ttbar_fit(ttbar,logpar,fitfunc);
    fitfunc->SetParameters(1000,2,2.5,-1.6,4,logpar->GetParameter(0),logpar->GetParameter(1),logpar->GetParameter(2));
    parabola_height=logpar->GetParameter(0);
    parabola_inclination=logpar->GetParameter(1);
    parabola_pointzero=logpar->GetParameter(2);
    dofixed=true;//ttbar is known so we can fix the parameters and don't need to use them for fitting!
  }
  else
  {
    fitfunc->SetParameters(1000,2,2.5,-1.6,4,5.45039,0.000324593,12.3528);
    dofixed=false;
  }

  vector<float> chi2values;
  addparabola=true;
  for (int ifit=0;ifit<100;ifit++)
  {
    all->Fit(fitfunc,"NQ");
    chi2values.push_back(fitfunc->GetChisquare());
    if(ifit>5&&chi2values[ifit-2]==chi2values[ifit]) break;
  }
  /*
  The parameters represent the following quantities:
  float N=par[0];
  float alpha=par[1];
  float n=par[2];
  float xbar=par[3];
  float sigma=par[4];
  */
  //we are clearing an area of two sigma to the left and to the right of the center of the function for the "real fit".
  low_reject=-2*fitfunc->GetParameter(4)+fitfunc->GetParameter(3);
  hi_reject=fitfunc->GetParameter(3)+2*fitfunc->GetParameter(4);
  if(low_reject>-15) low_reject=-10;
  if(hi_reject<15) hi_reject=10;
  doreject=true;//activating the rejection :-)
  
  for (int ifit=0;ifit<100;ifit++)
  {
    all->Fit(fitfunc,"NQ");
    chi2values.push_back(fitfunc->GetChisquare());
    if(ifit>5&&chi2values[ifit-2]==chi2values[ifit]) break;
  }
  
  draw_complete_fit(all,ttbar,minfit,maxfit,is_data,fitfunc);
  doreject=true;
  error=fitfunc->GetParError(3);
  Sigma=fitfunc->GetParameter(4);//sigma
  
  return fitfunc->GetParameter(3);
}
 
Double_t InvCrystalBall(Double_t *x,Double_t *par)
{
        Double_t arg1=0,arg2=0,A=0,B=0;
        Double_t f1=0;
        Double_t f2=0;
        Double_t lim=0;
        Double_t fitval=0;
        Double_t N=0;
        Double_t n=par[4];

    Double_t invX = -x[0];

        if (par[2] != 0)
        arg1 = (invX-par[1])/par[2];

        arg2 = ( -pow( TMath::Abs(par[3]) , 2 ) ) / 2 ;

        if (par[3] != 0)
        A = pow( ( n / TMath::Abs( par[3] ) ) , n) * TMath::Exp(arg2);

        if (par[3] != 0)
        B = n / TMath::Abs(par[3]) -  TMath::Abs(par[3]);

        f1 =  TMath::Exp(-0.5*arg1*arg1);
        if (par[2] != 0)
        f2 = A * pow( ( B - (invX - par[1])/par[2] ) , -n );

        if (par[2] != 0)
        lim = ( par[1] - invX ) / par[2] ;

        N = par[0];



        if(lim < par[3])
        fitval = N * f1;
        if(lim >= par[3])
        fitval = N * f2;

        return fitval;
}


Double_t DoubleInvCrystalBall(Double_t *x,Double_t *par)
{
        Double_t arg1=0,arg2=0,A=0,B=0;
        Double_t f1=0;
        Double_t f2=0;
        Double_t lim=0;
        Double_t fitval=0;
        Double_t N=0;
        Double_t n=par[4];

        Double_t Sarg1=0,Sarg2=0,SA=0,SB=0;
        Double_t Sf1=0;
        Double_t Sf2=0;
        Double_t Slim=0;
        Double_t Sfitval=0;
        Double_t SN=0;
        Double_t Sn=par[9];

    Double_t invX = -x[0];

        if (par[2] != 0) arg1 = (invX-par[1])/par[2];

        if (par[7] != 0) Sarg1 = (invX-par[6])/par[7];

        arg2 =  ( -pow( TMath::Abs(par[3]) , 2 ) ) / 2 ;
        Sarg2 = ( -pow( TMath::Abs(par[8]) , 2 ) ) / 2 ;

        if (par[3] != 0) A  = pow( (  n / TMath::Abs( par[3] ) ) , n) * TMath::Exp(arg2);
        if (par[8] != 0) SA = pow( ( Sn / TMath::Abs( par[8] ) ) , Sn) * TMath::Exp(Sarg2);

        if (par[3] != 0) B  = n / TMath::Abs(par[3]) -  TMath::Abs(par[3]);
        if (par[8] != 0) SB = Sn / TMath::Abs(par[8]) -  TMath::Abs(par[8]);

        f1  =  TMath::Exp(-0.5*arg1*arg1);
        Sf1 =  TMath::Exp(-0.5*Sarg1*Sarg1);
	
        if (par[2] != 0) f2  = A  * pow( ( B -  (invX - par[1])/par[2] ) , -n );
	if (par[7] != 0) Sf2 = SA * pow( ( SB - (invX - par[6])/par[7] ) , -Sn );

        if (par[2] != 0) lim = ( par[1] - invX ) / par[2] ;
	if (par[7] != 0) Slim = ( par[6] - invX ) / par[7] ;

        N = par[0];
        SN = par[5];



        if(lim < par[3])  fitval = N * f1;
        if(lim >= par[3]) fitval = N * f2;

        if(Slim < par[8]) Sfitval = SN * Sf1;
        if(Slim >= par[8]) Sfitval = SN * Sf2;

        return fitval+Sfitval;
}

Double_t DoubleInvCrystalBallP(Double_t *x,Double_t *par)
{
        Double_t arg1=0,arg2=0,A=0,B=0;
        Double_t f1=0;
        Double_t f2=0;
        Double_t lim=0;
        Double_t fitval=0;
        Double_t N=0;
        Double_t n=par[4];

        Double_t Sarg1=0,Sarg2=0,SA=0,SB=0;
        Double_t Sf1=0;
        Double_t Sf2=0;
        Double_t Slim=0;
        Double_t Sfitval=0;
        Double_t SN=0;
        Double_t Sn=par[9];

    Double_t invX = -x[0];

        if (par[2] != 0) arg1 = (invX-par[1])/par[2];

        if (par[7] != 0) Sarg1 = (invX-par[6])/par[7];

        arg2 =  ( -pow( TMath::Abs(par[3]) , 2 ) ) / 2 ;
        Sarg2 = ( -pow( TMath::Abs(par[8]) , 2 ) ) / 2 ;

        if (par[3] != 0) A  = pow( (  n / TMath::Abs( par[3] ) ) , n) * TMath::Exp(arg2);
        if (par[8] != 0) SA = pow( ( Sn / TMath::Abs( par[8] ) ) , Sn) * TMath::Exp(Sarg2);

        if (par[3] != 0) B  = n / TMath::Abs(par[3]) -  TMath::Abs(par[3]);
        if (par[8] != 0) SB = Sn / TMath::Abs(par[8]) -  TMath::Abs(par[8]);

        f1  =  TMath::Exp(-0.5*arg1*arg1);
        Sf1 =  TMath::Exp(-0.5*Sarg1*Sarg1);
	
        if (par[2] != 0) f2  = A  * pow( ( B -  (invX - par[1])/par[2] ) , -n );
	if (par[7] != 0) Sf2 = SA * pow( ( SB - (invX - par[6])/par[7] ) , -Sn );

        if (par[2] != 0) lim = ( par[1] - invX ) / par[2] ;
	if (par[7] != 0) Slim = ( par[6] - invX ) / par[7] ;

        N = par[0];
        SN = par[5];



        if(lim < par[3])  fitval = N * f1;
        if(lim >= par[3]) fitval = N * f2;

        if(Slim < par[8]) Sfitval = SN * Sf1;
        if(Slim >= par[8]) Sfitval = SN * Sf2;
	
	fitval+=Sfitval;
	fitval+=statErrorP(fitval);

        return fitval;
}

Double_t DoubleInvCrystalBallN(Double_t *x,Double_t *par)
{
        Double_t arg1=0,arg2=0,A=0,B=0;
        Double_t f1=0;
        Double_t f2=0;
        Double_t lim=0;
        Double_t fitval=0;
        Double_t N=0;
        Double_t n=par[4];

        Double_t Sarg1=0,Sarg2=0,SA=0,SB=0;
        Double_t Sf1=0;
        Double_t Sf2=0;
        Double_t Slim=0;
        Double_t Sfitval=0;
        Double_t SN=0;
        Double_t Sn=par[9];

    Double_t invX = -x[0];

        if (par[2] != 0) arg1 = (invX-par[1])/par[2];

        if (par[7] != 0) Sarg1 = (invX-par[6])/par[7];

        arg2 =  ( -pow( TMath::Abs(par[3]) , 2 ) ) / 2 ;
        Sarg2 = ( -pow( TMath::Abs(par[8]) , 2 ) ) / 2 ;

        if (par[3] != 0) A  = pow( (  n / TMath::Abs( par[3] ) ) , n) * TMath::Exp(arg2);
        if (par[8] != 0) SA = pow( ( Sn / TMath::Abs( par[8] ) ) , Sn) * TMath::Exp(Sarg2);

        if (par[3] != 0) B  = n / TMath::Abs(par[3]) -  TMath::Abs(par[3]);
        if (par[8] != 0) SB = Sn / TMath::Abs(par[8]) -  TMath::Abs(par[8]);

        f1  =  TMath::Exp(-0.5*arg1*arg1);
        Sf1 =  TMath::Exp(-0.5*Sarg1*Sarg1);
	
        if (par[2] != 0) f2  = A  * pow( ( B -  (invX - par[1])/par[2] ) , -n );
	if (par[7] != 0) Sf2 = SA * pow( ( SB - (invX - par[6])/par[7] ) , -Sn );

        if (par[2] != 0) lim = ( par[1] - invX ) / par[2] ;
	if (par[7] != 0) Slim = ( par[6] - invX ) / par[7] ;

        N = par[0];
        SN = par[5];



        if(lim < par[3])  fitval = N * f1;
        if(lim >= par[3]) fitval = N * f2;

        if(Slim < par[8]) Sfitval = SN * Sf1;
        if(Slim >= par[8]) Sfitval = SN * Sf2;
	
	fitval+=Sfitval;
	fitval-=statErrorN(fitval);

        return fitval;
}

Double_t InvCrystalBallP(Double_t *x,Double_t *par)
{
        Double_t arg1=0,arg2=0,A=0,B=0;
        Double_t f1=0;
        Double_t f2=0;
        Double_t lim=0;
        Double_t fitval=0;
        Double_t N=0;
        Double_t n=par[4];

    Double_t invX = -x[0];

        if (par[2] != 0)
        arg1 = (invX-par[1])/par[2];

        arg2 = ( -pow( TMath::Abs(par[3]) , 2 ) ) / 2 ;

        if (par[3] != 0)
        A = pow( ( n / TMath::Abs( par[3] ) ) , n) * TMath::Exp(arg2);

        if (par[3] != 0)
        B = n / TMath::Abs(par[3]) -  TMath::Abs(par[3]);

        f1 =  TMath::Exp(-0.5*arg1*arg1);
        if (par[2] != 0)
        f2 = A * pow( ( B - (invX - par[1])/par[2] ) , -n );

        if (par[2] != 0)
        lim = ( par[1] - invX ) / par[2] ;

        N = par[0];



        if(lim < par[3])
        fitval = N * f1;
        if(lim >= par[3])
        fitval = N * f2;

	fitval+= statErrorP(fitval);
        return fitval;
}

Double_t InvCrystalBallN(Double_t *x,Double_t *par)
{
        Double_t arg1=0,arg2=0,A=0,B=0;
        Double_t f1=0;
        Double_t f2=0;
        Double_t lim=0;
        Double_t fitval=0;
        Double_t N=0;
        Double_t n=par[4];

    Double_t invX = -x[0];

        if (par[2] != 0)
        arg1 = (invX-par[1])/par[2];

        arg2 = ( -pow( TMath::Abs(par[3]) , 2 ) ) / 2 ;

        if (par[3] != 0)
        A = pow( ( n / TMath::Abs( par[3] ) ) , n) * TMath::Exp(arg2);

        if (par[3] != 0)
        B = n / TMath::Abs(par[3]) -  TMath::Abs(par[3]);

        f1 =  TMath::Exp(-0.5*arg1*arg1);
        if (par[2] != 0)
        f2 = A * pow( ( B - (invX - par[1])/par[2] ) , -n );

        if (par[2] != 0)
        lim = ( par[1] - invX ) / par[2] ;

        N = par[0];



        if(lim < par[3])
        fitval = N * f1;
        if(lim >= par[3])
        fitval = N * f2;

	fitval-= statErrorN(fitval);
        return fitval;
}

 
