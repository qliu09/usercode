#include <iostream>
#include <string>
#include <sstream>
#include <TRandom.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TText.h>
#include <TLine.h>
#include <TColor.h>
#ifndef VERBOSITY
#define VERBOSITY 0
#endif
#ifndef GeneralToolBoxLoaded
#include "GeneralToolBox.C"
#endif

#define PoissonCalculatorLoaded

using namespace std;

TH1F *compute_area(TH1F *histo, float low, float hi) {
  TH1F *markedhisto = (TH1F*)histo->Clone();
  markedhisto->SetName("marked");
  markedhisto->SetFillColor(TColor::GetColor("#87CeFA"));
  markedhisto->SetLineWidth(0);
  for(int i=1;i<=histo->GetNbinsX();i++) {
    if(histo->GetBinCenter(i)<low||histo->GetBinCenter(i)>hi) markedhisto->SetBinContent(i,0);
  }
  return markedhisto;
}

Double_t PoissonDistribution(Double_t *par,Double_t *x) {
//    Double_t result = (1.0)/TMath::Gamma(1.2);
    Double_t result = (TMath::Power(par[0],x[0])*TMath::Exp(-par[0]))/(TMath::Gamma(x[0]+1));
    //function << "(TMath::Power(" << lambda << ",x)*TMath::Exp(-" << lambda << "))/(TMath::Gamma(x+1))";
    return result;
}

void find68(TH1F* histo, long N, int rounds,float &maximum,float &errordown,float &errorup)
{
  //float max = histo->GetBinCenter(histo->GetMaximumBin());
  float max=maximum;
  int startbin=histo->GetMaximumBin();
  int left=startbin;
  int right=startbin;
  float want=(float)N * 0.68*rounds;
  float have=histo->GetBinContent(startbin);
  if(VERBOSITY>0) dout << "found maximum at " << max << " ( this is bin " << startbin << ")" << endl;
  while(have<want)
  {
    if(histo->GetBinContent(left-1)>histo->GetBinContent(right+1))
    {
      have+=histo->GetBinContent(left-1);
      left--;
    }
    else
    {
      have+=histo->GetBinContent(right+1);
      right++;
    }
  }
  float loweredge=histo->GetBinCenter(left);
  float upperedge=histo->GetBinCenter(right);
  if(VERBOSITY>0) dout << "we now have " << have << " and we wanted at least " << want << endl;
  if(VERBOSITY>0) dout << "the range is " << loweredge << " to " << upperedge<< " corresponding to :" << endl;
  if(VERBOSITY>0) dout << "\033[1;34m " << max << "+ " << (upperedge-max) << " - " << (max-loweredge) << " \033[0m" << endl;
  errorup=(upperedge-max);
  errordown=(max-loweredge);
  maximum=max;
}

void combine_advanced_dists(TH1F *combdist,float *x1,float *x2,float *x3, float *x4, float *x5, float *x6, float *x7, long N)
{
  float temp=-99;
  for (int i=0;i<N;i++)
  {
    flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
    if(PlottingSetup::RestrictToMassPeak) temp = x1[i]+(1.0/3)*(x2[i]-x3[i])+(1.0/3)*(x4[i]-x5[i])+(1.0/3)*(x6[i]-x7[i]);
    else temp = x1[i]+(x2[i]-x3[i]);
    combdist->Fill(temp);
  }
}

void combine_dists(TH1F *combdist,float *x1,float *x2,float *x3, long N)
{
  float temp=-99;
  for (int i=0;i<N;i++)
  {
    temp = x1[i]+x2[i]-x3[i];
    combdist->Fill(temp);
  }
}

void generate(float *x1, int lambda,long N,float range)
{
  stringstream function;
  function << "(TMath::Power(" << lambda << ",x)*TMath::Exp(-" << lambda << "))/(TMath::Gamma(x+1))";
  TF1 *f1 = new TF1("f1",function.str().c_str(),0,range);
  for(int i=0;i<N;i++)
  {
    if(lambda>0) x1[i]=f1->GetRandom();
    else x1[i]=0; //if we want to do this for lambda=0 then all we can provide is zero ... 
  }//end of for
}//end of generate_x1

void megagen(float a, float b, float c, float d, float e, float f, float g, int N,float range,TH1 *combineddist) {
  stringstream function;
  function << "(TMath::Power(" << a << ",x)*TMath::Exp(-" << a << "))/(TMath::Gamma(x+1))";
  TF1 *f1 = new TF1("f1",function.str().c_str(),0,range);
  
  stringstream function2;
  function2 << "(TMath::Power(" << b << ",x)*TMath::Exp(-" << b << "))/(TMath::Gamma(x+1))";
  TF1 *f2 = new TF1("f2",function2.str().c_str(),0,range);

  stringstream function3;
  function3 << "(TMath::Power(" << c << ",x)*TMath::Exp(-" << c << "))/(TMath::Gamma(x+1))";
  TF1 *f3 = new TF1("f3",function3.str().c_str(),0,range);

  stringstream function4;
  function4 << "(TMath::Power(" << d << ",x)*TMath::Exp(-" << d << "))/(TMath::Gamma(x+1))";
  TF1 *f4 = new TF1("f4",function4.str().c_str(),0,range);

  stringstream function5;
  function5 << "(TMath::Power(" << e << ",x)*TMath::Exp(-" << e << "))/(TMath::Gamma(x+1))";
  TF1 *f5 = new TF1("f5",function5.str().c_str(),0,range);

  stringstream function6;
  function6 << "(TMath::Power(" << f << ",x)*TMath::Exp(-" << f << "))/(TMath::Gamma(x+1))";
  TF1 *f6 = new TF1("f6",function6.str().c_str(),0,range);

  stringstream function7;
  function7 << "(TMath::Power(" << g << ",x)*TMath::Exp(-" << g << "))/(TMath::Gamma(x+1))";
  TF1 *f7 = new TF1("f7",function7.str().c_str(),0,range);
  
  float fillval;
  for(int i=0;i<N;i++) {
        flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
    if(PlottingSetup::RestrictToMassPeak) fillval=f1->GetRandom()+(1.0/3)*(f2->GetRandom()-f3->GetRandom())+(1.0/3)*(f4->GetRandom()-f5->GetRandom())+(1.0/3)*(f6->GetRandom()-f7->GetRandom());
    else fillval=f1->GetRandom()+(f2->GetRandom()-f3->GetRandom());
    combineddist->Fill(fillval);
  }
}
  
  

int poisson_canvas_counter=-1;

void ComputePoissonError(float one, float two, float three,float &maximum, float &errordown, float &errorup, string title="no title has been passed") {
  maximum=one+two-three;
  if(one+two+three>0.01) {
  long N=500000;
  float range=(one+two-three)*2;
  if(range<3) range=3;
  float x1[N];
  float x2[N];
  float x3[N];
  
  TCanvas *c1 = new TCanvas("c1","c1");
  TH1F *combineddist = new TH1F("combineddist","",500,0,range);
  generate(x1,one,N,range);
  generate(x2,two,N,range);
  generate(x3,three,N,range);
  combine_dists(combineddist,x1,x2,x3,N);
  find68(combineddist,N,1,maximum,errordown,errorup);
  
  //from here on we only do "cosmetics" :-)
  combineddist->Draw();
  combineddist->GetYaxis()->SetRangeUser(0,1.1*combineddist->GetMaximum());
  TLine *central = new TLine(maximum,0,maximum,combineddist->GetMaximum());
  central->SetLineColor(kBlue);
  TH1F *drawarea = compute_area(combineddist,maximum-errordown,maximum+errorup);
  //blublu : need to implement the compute_area function which gives the integration area in a nice color :-) 
  drawarea->Draw("same");
  combineddist->Draw("same");
  central->Draw();
  TText *ttitle = write_title(title);
  ttitle->Draw();
  stringstream result1;
  result1 << "Pred: "<<one<<" + " << two << " - " << three << " = " << one+two-three;
  TText *res1=write_text(0.10,0.05,result1.str().c_str());
  res1->SetTextSize(0.03);
  res1->SetTextAlign(11);
  stringstream result2;
  result2 << "Error band: [" << maximum-errordown << " , " << maximum+errorup << "]";
  TText *res2=write_text(0.10,0.01,result2.str().c_str());
  res2->SetTextSize(0.03);
  res2->SetTextAlign(11);
  res1->Draw();
  res2->Draw();
  stringstream result3;
  result3 << "Result: " << maximum << " + " << errorup << " - " << errordown;
  TText *res3=write_text(0.95,0.03,result3.str().c_str());
  res3->SetTextColor(kBlue);
  res3->SetTextSize(0.03);
  res3->SetTextAlign(31);
  res3->Draw();
  poisson_canvas_counter++;
  stringstream savename;
  savename << "Poisson/PoissonStatisticsCanvas_" << poisson_canvas_counter;
  CompleteSave(c1,savename.str());
  
  delete combineddist;
  
  }
  else {
    if(VERBOSITY>0) dout << "Entries are too small. Not much to be done." << endl;
    maximum=one+two-three;errordown=0;errorup=0;
  }
}


void advanced_poisson(float a, float b, float c, float d, float e, float f, float g,float &errordown,float &errorup)
{
  float maximum=a;
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) maximum+=(1.0/3)*(b-c)+(1.0/3)*(d-e)+(1.0/3)*(f-g);
  else maximum+=(b-c);
  float parametersum=a+b+c;
  if(PlottingSetup::RestrictToMassPeak) parametersum+=d+e+f+g;
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(parametersum>0.01) {
  long N=500000;
  float range=a;
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) range+=(1.0/3)*(b-c)+(1.0/3)*(d-e)+(1.0/3)*(f-g);
  else range+=(b-c);
  range*=2;
  if(range<3) range=3;
  
  TCanvas *c1 = new TCanvas("c1","c1");
  TH1F *combineddist = new TH1F("combineddist","",500,0,range);
  megagen(a,b,c,d,e,f,g,N,range,combineddist);
  find68(combineddist,N,1,maximum,errordown,errorup);
  
  //from here on we only do "cosmetics" :-)
  combineddist->Draw();
  combineddist->GetYaxis()->SetRangeUser(0,1.1*combineddist->GetMaximum());
  TLine *central = new TLine(maximum,0,maximum,combineddist->GetMaximum());
  central->SetLineColor(kBlue);
  TH1F *drawarea = compute_area(combineddist,maximum-errordown,maximum+errorup);
  //blublu : need to implement the compute_area function which gives the integration area in a nice color :-) 
  drawarea->Draw("same");
  combineddist->Draw("same");
  central->Draw();
  stringstream result1;
  flag_this_change(__FUNCTION__,__LINE__,false);//PlottingSetup::RestrictToMassPeak
  if(PlottingSetup::RestrictToMassPeak) result1 << "Pred: "<<a<<" + (1/3)*(" << b<<"-"<<c<<")+ (1/3)*(" << d<<"-"<<e<<")+ (1/3)*(" << f<<"-"<<g<<") = " << maximum;
  else result1 << "Pred: "<<a<<" + (" << b<<"-"<<c<<") = " << maximum;
  TText *res1=write_text(0.10,0.05,result1.str().c_str());
  res1->SetTextSize(0.03);
  res1->SetTextAlign(11);
  stringstream result2;
  result2 << "Error band: [" << maximum-errordown << " , " << maximum+errorup << "]";
  TText *res2=write_text(0.10,0.01,result2.str().c_str());
  res2->SetTextSize(0.03);
  res2->SetTextAlign(11);
  res1->Draw();
  res2->Draw();
  stringstream result3;
  result3 << "Result: " << maximum << " + " << errorup << " - " << errordown;
  TText *res3=write_text(0.95,0.03,result3.str().c_str());
  res3->SetTextColor(kBlue);
  res3->SetTextSize(0.03);
  res3->SetTextAlign(31);
  res3->Draw();
  poisson_canvas_counter++;
  stringstream savename;
  savename << "Poisson/Result_PoissonStatisticsCanvas_" << poisson_canvas_counter;
  CompleteSave(c1,savename.str());
  
  delete combineddist;
  
  }
  else {
    if(VERBOSITY>0) dout << "Entries are too small. Not much to be done." << endl;
    errordown=0;errorup=0;
  }
}


/***
 * 
 * SAMPLE USAGE BELOW !!!!
 * 
 * 

int main()
{
  dout << "hello!" << endl;
  float max,down,up;
  ComputePoissonError(2,6,0.001,max,down,up);
  dout << "RESULTS: " << max << " + " << up << " - " << down << endl;
  return 0;
}
*/