/* 

g++ ShapeFit.C -o ShapeFit.exec `root-config --glibs --cflags` && ./ShapeFit.exec

*/

#include <iostream>
#include <vector>
#include <fstream>

#include <TH1.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TF1.h>
#include <TError.h>
#include <TFractionFitter.h>
#include <TMath.h>
#include <TROOT.h>
#include <THStack.h>
#include <TLine.h>
#include <TColor.h>

#ifndef ValueClassLoaded
#include "ValueClass.C"
#endif
#ifndef GeneralToolBoxLoaded
#include "GeneralToolBox.C"
#endif
using namespace std;

void check_compatibility_with_zero(Value sigresult, ofstream &file) {
  dout << "Checking compatibility with zero for f=" << sigresult << endl;
  dout << "   The result is compatible with zero to " << 1-(TMath::Erf((sigresult.getValue()/(TMath::Sqrt(2)*sigresult.getError())))) << endl;
  file << "Checking compatibility with zero for f=" << sigresult << endl;
  file << "   The result is compatible with zero to " << 1-(TMath::Erf((sigresult.getValue()/(TMath::Sqrt(2)*sigresult.getError())))) << endl;
  
}

/*
vector<float> compute_sigmas(TH1 *signal, float weight) {
  TH1F *histo = (TH1F*)signal->Clone();
  histo->Scale(weight);
  float integral[7];
  for(int i=0;i<4;i++) integral[i]=0;
  for(int i=0;i<=histo->GetNbinsX()+1;i++) {
    integral[0]+=histo->GetBinContent(i);
    integral[1]+=histo->GetBinContent(i)+histo->GetBinError(i);
    integral[2]+=histo->GetBinContent(i)+2*histo->GetBinError(i);
    integral[3]+=histo->GetBinContent(i)+3*histo->GetBinError(i);
    integral[4]+=histo->GetBinContent(i)+4*histo->GetBinError(i);
    integral[5]+=histo->GetBinContent(i)+5*histo->GetBinError(i);
    integral[6]+=histo->GetBinContent(i)+6*histo->GetBinError(i);
  }
  float meanintegral = signal->Integral();
  vector<float> results;
  results.push_back(integral[0]/meanintegral);
  results.push_back(integral[1]/meanintegral);
  results.push_back(integral[2]/meanintegral);
  results.push_back(integral[3]/meanintegral);
  results.push_back(integral[4]/meanintegral);
  results.push_back(integral[5]/meanintegral);
  results.push_back(integral[6]/meanintegral);
  return results;	
}
*/
void present_limit_fitting_results(TH1 *result,TH1 *data, TH1 *background, TH1 *signal, float bgw, float siw,string filename) {
  TH1F *bg = (TH1F*)background->Clone();
  bg->Scale(bgw);
  TH1F *si = (TH1F*)signal->Clone();
  si->Scale(siw);
//  TCanvas *c1 = new TCanvas("c1","c1",500,900);
  TCanvas *c1 = new TCanvas("c1","c1",1500,500);
  c1->Divide(3,1);
//  c1->Divide(1,3);
  
  c1->cd(1);
  data->SetTitle("");
  data->SetStats(0);
  background->SetLineColor(kBlue);
  data->SetLineColor(kBlack);
  signal->SetLineColor(kRed);
//  background->SetFillColor(TColor::GetColor("#5882FA"));
  background->SetMarkerSize(0);
//  background->SetFillColor(TColor::GetColor("#5882FA"));
//  signal->SetFillColor(TColor::GetColor("#81F781"));
  signal->SetMarkerSize(0);
//  signal->SetFillColor(TColor::GetColor("#FE2E2E"));
  TLegend *leg = new TLegend(0.7,0.7,0.89,0.89);
  leg->AddEntry(data,"Data","p");
  leg->AddEntry(bg,"Z+Jets","f");
  leg->AddEntry(si,"signal","f");
  leg->SetLineColor(kWhite);
  leg->SetFillColor(kWhite);
  data->Draw();
  background->Draw("histo,same");
  signal->Draw("histo,same");
  data->Draw("same");
  leg->Draw("same");
  TText* title = write_title("Before fitting");
  title->Draw();
  
  c1->cd(2);
  c1->cd(2)->SetLogy(1);
  TH1F *datac = (TH1F*)data->Clone();
  bg->SetFillColor(TColor::GetColor("#5882FA"));
  bg->SetLineColor(kBlue);
//  si->SetLineColor(kGreen);
//  si->SetFillColor(TColor::GetColor("#81F781"));
  si->SetLineColor(kRed);
  si->SetFillColor(TColor::GetColor("#FE2E2E"));
  THStack fresult("fresult","");
  fresult.Add(bg);
  fresult.Add(si);
  fresult.Draw("histo");
  datac->SetMinimum(0.4);
  datac->SetMaximum(data->GetMaximum()*10);
  datac->Draw();
  fresult.Draw("histo,same");
  datac->Draw("same");
  leg->Draw("same");
  TText* title2 = write_title("After fitting (composition)");
  title2->Draw();
  
  c1->cd(3);
  TH1F *ratio = (TH1F*)result->Clone();
  ratio->Divide(data);  
  ratio->GetYaxis()->SetRangeUser(0,2);
  TLine *oneline = new TLine(data->GetBinLowEdge(1),1,data->GetBinLowEdge(data->GetNbinsX())+data->GetBinWidth(data->GetNbinsX()),1);
  oneline->SetLineColor(kBlue);
  ratio->SetFillColor(TColor::GetColor("#FA8258"));
  ratio->SetTitle("");
  ratio->SetStats(0);
  ratio->GetYaxis()->SetTitle("(Fit result)/data");
  ratio->GetYaxis()->CenterTitle();
  ratio->SetMarkerSize(0);
  ratio->Draw("E5");
  oneline->Draw();
  TText* title3 = write_title("Ratio");
  title3->Draw();
  CompleteSave(c1,("upper_limit/"+filename).c_str());
  CompleteSave(c1->cd(1),("upper_limit/"+filename+"_cd1").c_str());
  CompleteSave(c1->cd(2),("upper_limit/"+filename+"_cd2").c_str());
  CompleteSave(c1->cd(3),("upper_limit/"+filename+"_cd3").c_str());
  
}
  
float compute_upper_limit_based_on_fit(Value signalresult, Value originalfactor, int nsigma) {
  float stretchby=signalresult.getValue()*(signalresult.getValue()+nsigma*signalresult.getError())/(signalresult.getValue());
  stretchby/=originalfactor.getValue();
  return stretchby;
}


vector<float> establish_upper_limits(TH1 *data, TH1 *background, TH1 *signal,string filename, ofstream &file) {
  TObjArray *allmc = new TObjArray(2);
  allmc->Add(background);
  allmc->Add(signal);
  
  TCanvas *c1 = new TCanvas("c1","c1");
  background->SetMarkerColor(kBlue);
  background->Draw();
  signal->SetMarkerColor(kGreen);
  signal->Draw("same");
  data->Draw("same");
  c1->SaveAs("Input_for_establish_upper_limits.png");
  
  TFractionFitter* fit = new TFractionFitter(data, allmc);

  fit->Constrain(1,1.0,1.0);
  fit->Constrain(2,0.0,1.1);
  //fit->Constrain(1,0.0,10.0);
  TFile *rawinput = new TFile("rawtfitterinput.root","RECREATE");
  data->Write();
  signal->Write();
  background->Write();
  rawinput->Close();
  
  Int_t status = fit->Fit();
  vector<float> results;
  if(status!=0) return results;
  
  Double_t mean[2],error[2];
  fit->GetResult(0,mean[0],error[0]);
  Value SMresult(mean[0],error[0]);
  fit->GetResult(1,mean[1],error[1]);
  Value sigresult(mean[1],error[1]);
  dout << "Parameters: " << mean[0] << " , " << mean[1] << endl;
  if(mean[0]<0||mean[1]<0) {
    dout << "Fitting failed, one of the parameters is negative !!!!" << endl;
    return results;
  }
  
  Value vdata(data->Integral(),TMath::Sqrt(data->Integral()));
  Value vsig(signal->Integral(),TMath::Sqrt(signal->Integral()));
  Value vbg(background->Integral(),TMath::Sqrt(background->Integral()));
  float originalfactor=(signal->Integral())/(data->Integral());
  float originalfactorSM=(background->Integral())/(data->Integral());
  
  Value ofac   = vsig/vdata;
  Value ofacsm = vbg /vdata;
  
  Value stretchsm  = SMresult/ofacsm;
  Value stretchsig = sigresult/ofac;
  dout << "Data consists of: " << endl;
  dout << "   SM : " << SMresult << endl;
  dout << "   signal: " << sigresult << endl;
  dout << "This means that the components have been scaled by: " << endl;
  dout << "   SM: has been stretched by " << (mean[0])/originalfactorSM << " "<< stretchsm<<endl;
  dout << "   Signal: has been stretched by " << (mean[1])/originalfactor <<  " "<< stretchsig<<endl;
  
  file << "------------------------------------------"<<endl;
  file << "Now considering: Everything pertaining to " << filename << endl;
  file << "Data consists of: " << endl;
  file << "   SM : " << SMresult << endl;
  file << "   signal: " << sigresult << endl;
  file << "This means that the components have been scaled by: " << endl;
  file << "   SM: has been stretched by " << stretchsm<<endl;
  file << "   Signal: has been stretched by " << stretchsig<<endl;
  
  present_limit_fitting_results((TH1F*)fit->GetPlot(),data,background,signal,(mean[0])/originalfactorSM,(mean[1])/originalfactor,filename);
  
  check_compatibility_with_zero(sigresult,file);
  //vector<float> results = compute_sigmas(signal,(mean[1])/originalfactor);
  
  dout << "\\Chi^2 / ndf = " << fit->GetChisquare() << "/" << fit->GetNDF() << endl;
  
  results.push_back(compute_upper_limit_based_on_fit(sigresult,ofac,0));
  results.push_back(compute_upper_limit_based_on_fit(sigresult,ofac,1));
  results.push_back(compute_upper_limit_based_on_fit(sigresult,ofac,2));
  results.push_back(compute_upper_limit_based_on_fit(sigresult,ofac,3));
  
  dout << "Limits: (in terms of x prediction) " << endl;
  file << "Limits: (in terms of x prediction) " << endl;
  for (int i=0;i<=3;i++) {
    dout << i << " sigma : " << results[i] << endl;
    file << i << " sigma : " << results[i] << endl;
  }
    
  
  dout << endl << endl;
  file << endl << endl;
  
  if(results[3]<1) {
    dout << "The point has been excluded, since the upper limit corresponding to 3 sigma, " << results[3] << " is < 1" << endl;
    file << "The point has been excluded, since the upper limit corresponding to 3 sigma, " << results[3] << " is < 1" << endl;
    //the ANSII code below will make an X for the excluded point (to celebrate)
    dout << "          __   __" << endl;
    dout << "          \\ \\ / /" << endl;
    dout << "           \\ V / " << endl;
    dout << "           /   \\ " << endl;
    dout << "          / /^\\ \\" << endl;
    dout << "          \\/   \\/" << endl;
    
    file << "          __   __" << endl;
    file << "          \\ \\ / /" << endl;
    file << "           \\ V / " << endl;
    file << "           /   \\ " << endl;
    file << "          / /^\\ \\" << endl;
    file << "          \\/   \\/" << endl;


  }
  else {
    dout << "The 3 sigma limit on this point is " << results[3] << " x prediction" << endl;
    file << "The point has been excluded, since " << results[3] << " x prediction" << endl;
  }
    
    
  return results;
}

/*

Please use the function like this :

int main() {
  gROOT->SetStyle("Plain");
  ofstream myfile;
  myfile.open ("ShapeFit_log.txt");
  TH1F *data = new TH1F("data","data",10,0,10);
  data->GetXaxis()->SetTitle("some variable");
  data->GetXaxis()->CenterTitle();
  data->GetYaxis()->SetTitle("events");
  data->GetYaxis()->CenterTitle();
  data->Sumw2();
  TH1F *mc = new TH1F("mc","mc",10,0,10);
  mc->Sumw2();
  TH1F *si = new TH1F("si","signal",10,0,10);
  si->Sumw2();
  TF1 *f1 = new TF1("f1", "[0] +[1]*x +gaus(2)", 0, 5);
  f1->SetParameters(6, -1,8, 0.1, 0.05);
  TF1 *f2 = new TF1("f2", "gaus(2)", 0, 5);
  f2->SetParameters(5, 0.1, 0.05);
  TF1 *f3 = new TF1("f3", "[0] +[1]*x", 0, 5);
  f3->SetParameters(6, -1,8);
  
  data->FillRandom("f1",10000);
  mc->FillRandom("f3",6000);
  si->FillRandom("f2",2000);
  
  establish_upper_limits(data,mc,si,"test2.png",myfile);
  myfile.close();
  return 0;
} 
*/