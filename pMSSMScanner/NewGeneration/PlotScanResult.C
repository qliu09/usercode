#include <iostream>
#include <sstream>
#include <math.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TColor.h>

#include "/shome/buchmann/KillerKoala/CBAF/Development/Plotting/Modules/setTDRStyle.C"

using namespace std;

int NModels=-1;
int NValid=-1;

stringstream CompleteCut;
TTree* tpmssm;

int FullFills(string cut) {
    stringstream scut;
    scut << "(" << cut << ">0&&SLHAValid && abs(HiggsMass-125.4)<2.5)";
    int Result = tpmssm->Draw("HiggsMass",scut.str().c_str(),"goff");
    return Result;
    
}

string RoundNicely(int nmod) {
    int exp=0;
    for(int i=1;i<19;i++) {
        if(nmod/10.0>1) {
            nmod/=10;
            exp++;
        } else {
            break;
        }
    }
    stringstream res;
    res << nmod << " #times 10^{" << exp << "}";
    return res.str();
}

float ThreeDigits(float num) {
    int res = 1000*num;
    return res/1000.0;
}


int ComputeCut(string NewCut) {
    if(NewCut!="") CompleteCut << " && " << NewCut;
    int Result = tpmssm->Draw("HiggsMass",CompleteCut.str().c_str());
    cout << "Evaluated cut " << CompleteCut.str().c_str() << " and obtained " << Result << endl;
    return Result;
}


void DrawpMSSMLabel() {
    float rho_g = ThreeDigits(pow(NModels,1.0/19));
    float rho_v = ThreeDigits(pow(NValid,1.0/19));
    stringstream slabel;
    slabel << "Private pMSSM scan (N_{models}>" << RoundNicely(NModels) << ", #rho_{g} #approx " << rho_g << ", #rho_{v} #approx " << rho_v << ")";
    TLatex *label = new TLatex(0.5,0.96,slabel.str().c_str());
    label->SetTextSize(0.03);
    label->SetTextAlign(22);
    label->SetNDC();
    label->Draw();
}

int main() {
    cout << "Hello" << endl;
    
    setTDRStyle(false);
    
    TCanvas *can = new TCanvas("can","can",800,800);
    can->SetLogy(1);
    TFile *fin = new TFile("AllCandidates.root");
    tpmssm = (TTree*)fin->Get("pmssm");
    
    CompleteCut << "(HiggsMass>2 || HiggsMass<5)";
    
    NModels=tpmssm->GetEntries();
    NValid = tpmssm->Draw("HiggsMass","Compatibility>-1","goff");
    
    // We want to know :
    // 1) Number of points generated
    // 2) Number of valid SLHA files (no tachyons etc.)
    // 3) Number of points within 125 +/- 2 GeV
    // 4) Higgs mass distribution
    // 5) Diagram of how the points satisfy the experimental constraints
    // 6) All points that survive all the previous cuts, show KSP's
    
    // We can therefore make a cutflow:
    // 1) Number of points generated
    // 2) Number of valid SLHA
    // 3) Number of points within 125 +/- 2 GeV
    // 4-10) Experimental constraints

    TH1F *cutflow = new TH1F("cutflow","",9,0,9);
    cutflow->SetStats(0);
    cutflow->SetLineColor(TColor::GetColor("#3c41f7")); // nice blue
    
    cutflow->SetBinContent(1,ComputeCut(""));
    cutflow->GetXaxis()->SetBinLabel(1,"Generated");
    
    cutflow->SetBinContent(2,ComputeCut("SLHAValid"));
    cutflow->GetXaxis()->SetBinLabel(2,"Theoretically sound");
    
    cutflow->SetBinContent(3,ComputeCut("abs(HiggsMass-125.4)<2.5"));
    cutflow->GetXaxis()->SetBinLabel(3,"E: m_{h}=125.4#pm 2.5 GeV");
    
    
//    cutflow->SetBinContent(4,ComputeCut("Satisfies_Alphas"));
//    cutflow->GetXaxis()->SetBinLabel(4,"E: Passes #alpha_{S}");
//    cutflow->SetBinContent(5,ComputeCut("Satisfies_Mbmb"));
//    cutflow->GetXaxis()->SetBinLabel(5,"E: Passes mbmb");
//    cutflow->SetBinContent(6,ComputeCut("Satisfies_Top"));
//    cutflow->GetXaxis()->SetBinLabel(6,"E: Passes m_{t}");
//    cutflow->SetBinContent(7,ComputeCut("Satisfies_Higgs"));
//    cutflow->GetXaxis()->SetBinLabel(7,"E: Passes m_{h}");
    cutflow->SetBinContent(4,ComputeCut("Satisfies_LSP"));
    cutflow->GetXaxis()->SetBinLabel(4,"E: LSP");
    cutflow->SetBinContent(5,ComputeCut("Satisfies_SUSY"));
    cutflow->GetXaxis()->SetBinLabel(5,"E: SUSY exclusion");
    cutflow->SetBinContent(6,ComputeCut("Satisfies_gm2"));
    cutflow->GetXaxis()->SetBinLabel(6,"E: (g-2)");
    cutflow->SetBinContent(7,ComputeCut("Satisfies_bsgamma"));
    cutflow->GetXaxis()->SetBinLabel(7,"E: BR(b#rightarrow s#gamma)");
    cutflow->SetBinContent(8,ComputeCut("Satisfies_RBtaunu"));
    cutflow->GetXaxis()->SetBinLabel(8,"E: R(B#rightarrow#tau#nu)");
//    cutflow->SetBinContent(13,ComputeCut("Satisfies_BsGamma"));
//    cutflow->GetXaxis()->SetBinLabel(13,"E: Passes b_{s} #gamma");
//    cutflow->SetBinContent(14,ComputeCut("Satisfies_BsMuMu"));
//    cutflow->GetXaxis()->SetBinLabel(14,"E: Passes b_{s} #mu#mu");
    cutflow->SetBinContent(9,ComputeCut("Satisfies_Bsmumu"));
    cutflow->GetXaxis()->SetBinLabel(9,"E: BR(B_{s}#rightarrow#mu#mu)");
    

//    cutflow->GetXaxis()->LabelsOption("v");
    
    
    TH1F *ExpConstraints = new TH1F("ExpConstraints","",7,0,7);
    ExpConstraints->SetStats(0);
    ExpConstraints->SetLineColor(TColor::GetColor("#3c41f7")); // nice blue

    ExpConstraints->SetBinContent(1,FullFills("Satisfies_LSP"));
    ExpConstraints->GetXaxis()->SetBinLabel(1,"LSP ok");
    
    ExpConstraints->SetBinContent(2,FullFills("Satisfies_RBtaunu"));
    ExpConstraints->GetXaxis()->SetBinLabel(2,"R(B#rightarrow#tau#nu)");

    ExpConstraints->SetBinContent(3,FullFills("Satisfies_SUSY"));
    ExpConstraints->GetXaxis()->SetBinLabel(3,"SUSY exclusion");

    ExpConstraints->SetBinContent(4,FullFills("Satisfies_Higgs"));
    ExpConstraints->GetXaxis()->SetBinLabel(4,"m_{h}");

    ExpConstraints->SetBinContent(5,FullFills("Satisfies_Bsmumu"));
    ExpConstraints->GetXaxis()->SetBinLabel(5,"BR(B_{s}#rightarrow#mu#mu)");

    ExpConstraints->SetBinContent(6,FullFills("Satisfies_gm2"));
    ExpConstraints->GetXaxis()->SetBinLabel(6,"(g-2)");

    ExpConstraints->SetBinContent(7,FullFills("Satisfies_bsgamma"));
    ExpConstraints->GetXaxis()->SetBinLabel(7,"BR(b#rightarrows#gamma)");
    
    ExpConstraints->GetYaxis()->SetTitle("N(models) passing exp. constraint");
    ExpConstraints->GetYaxis()->CenterTitle();

    ExpConstraints->Draw();
    DrawpMSSMLabel();
    can->SaveAs("CandidateCutFlow_Experimental.png");
    can->SaveAs("CandidateCutFlow_Experimental.pdf");
    
    
    
    
    
    
    cutflow->Draw("histo");
    DrawpMSSMLabel();
    can->SaveAs("CandidateCutFlow.png");
    can->SaveAs("CandidateCutFlow.pdf");
    
    TH1F *HiggsHisto = new TH1F("HiggsHisto","HiggsHisto",35,100,135);HiggsHisto->Sumw2();
    tpmssm->Draw("HiggsMass>>HiggsHisto","SLHAValid");
    HiggsHisto->GetXaxis()->SetTitle("m_{h} (GeV/c^{2})");
    HiggsHisto->GetXaxis()->CenterTitle();
    HiggsHisto->SetLineColor(TColor::GetColor("#3c41f7")); // nice blue
    HiggsHisto->SetFillColor(TColor::GetColor("#3c41f7")); // nice blue
    HiggsHisto->DrawNormalized("e3");
    DrawpMSSMLabel();
    can->SaveAs("Candidate_HiggsMass_Log.png");
    can->SaveAs("Candidate_HiggsMass_Log.pdf");
    can->SetLogy(0);
    HiggsHisto->DrawNormalized("e3");
    DrawpMSSMLabel();
    can->SaveAs("Candidate_HiggsMass_Linear.png");
    can->SaveAs("Candidate_HiggsMass_Linear.pdf");
    
    
    
    
    delete fin;
    
    
    
    return 0;
}