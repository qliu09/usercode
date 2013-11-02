#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <TFile.h>
#include <TTree.h>

int SLHAValid,Satisfies_bsgamma,Satisfies_gm2,Satisfies_Bsmumu,Satisfies_Higgs,Satisfies_SUSY,Satisfies_LSP,Satisfies_Top,Satisfies_Mbmb,Satisfies_Alphas;
int Satisfies_RBtaunu;
float HiggsMass,Compatibility;
bool Problematic=false;
float pMSSMpars[20];

float KSP_LM,KSP_HM,KSP_MET,KSP_BTAG,eff,LM_Integral,FullStats;

unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    int pos = txt.find( ch );
    unsigned int initialPos = 0;
    strs.clear();
    
    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos + 1 ) );
        initialPos = pos + 1;
        
        pos = (int)txt.find( ch, initialPos );
    }
    
    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( (int)pos, (int)txt.size() ) - initialPos + 1 ) );
    
    return strs.size();
}

float Cond(std::vector<std::string> v, int index){
    if(v.size()>index) return atof(v[index].c_str());
    else return -1;
}

void InterpretLine(std::string line) {
    
    std::vector<std::string> v;
    
    split( line, v, ' ' );
    
    Problematic=false;
    
    for(int i=0;i<20;i++) {
        if(v.size()>i) {
	  pMSSMpars[i]=atof(v[i].c_str());
	}
	if(pMSSMpars[i]==-1 || !(v.size()>i)) {
	  pMSSMpars[i]=-1;
	  Problematic=true;
	  std::cout << "Found a problematic line:" << std::endl;
	  std::cout << "         " << line << std::endl;
	}
    }
    
    SLHAValid=Cond(v,20);
    HiggsMass=Cond(v,21);
    Satisfies_bsgamma=Cond(v,22);
    Satisfies_gm2=Cond(v,23);
    Satisfies_Bsmumu=Cond(v,24);
    Satisfies_Higgs=Cond(v,25);
    Satisfies_SUSY=Cond(v,26);
    Satisfies_LSP=Cond(v,27);
    Satisfies_Top=Cond(v,28);
    Satisfies_Mbmb=Cond(v,29);
    Satisfies_Alphas=Cond(v,30);
    Satisfies_RBtaunu=Cond(v,31);
    Compatibility=Cond(v,32);
    KSP_LM=Cond(v,33);
    KSP_HM=Cond(v,34);
    KSP_MET=Cond(v,35);
    KSP_BTAG=Cond(v,36);
    eff=Cond(v,37);
    LM_Integral=Cond(v,38);
    FullStats=Cond(v,39);
}

int main() {
  
  std::cout << "Going to turn log into root tree" << std::endl;
  

  
  
  TFile *fout = new TFile("pMSSMpoints.root","RECREATE");
  TTree *tree = new TTree("pmssm","pmssm");
  tree->Branch("mu",&pMSSMpars[8],"mu/F");
  tree->Branch("MG1",&pMSSMpars[2],"MG1/F");
  tree->Branch("MG2",&pMSSMpars[3],"MG2/F");
  tree->Branch("MG3",&pMSSMpars[4],"MG3/F");
  tree->Branch("Ml1",&pMSSMpars[9],"Ml1/F");
  tree->Branch("Ml2",&pMSSMpars[9],"Ml2/F");
  tree->Branch("Ml3",&pMSSMpars[11],"Ml3/F");
  tree->Branch("Mr1",&pMSSMpars[10],"Mr1/F");
  tree->Branch("Mr2",&pMSSMpars[10],"Mr2/F");
  tree->Branch("Mr3",&pMSSMpars[12],"Mr3/F");
  tree->Branch("Mq1",&pMSSMpars[13],"Mq1/F");
  tree->Branch("Mq2",&pMSSMpars[13],"Mq2/F");
  tree->Branch("Mq3",&pMSSMpars[14],"Mq3/F");
  tree->Branch("Mu1",&pMSSMpars[15],"Mu1/F");
  tree->Branch("Mu2",&pMSSMpars[15],"Mu2/F");
  tree->Branch("Mu3",&pMSSMpars[16],"Mu3/F");
  tree->Branch("Md1",&pMSSMpars[17],"Md1/F");
  tree->Branch("Md2",&pMSSMpars[17],"Md2/F");
  tree->Branch("Md3",&pMSSMpars[18],"Md3/F");
  tree->Branch("At",&pMSSMpars[6],"At/F");
  tree->Branch("Ab",&pMSSMpars[5],"Ab/F");
  tree->Branch("Al",&pMSSMpars[7],"Al/F");
  tree->Branch("MH3",&pMSSMpars[1],"MH3/F");
  tree->Branch("tb",&pMSSMpars[0],"tb/F");



  tree->Branch("SLHAValid",&SLHAValid,"SLHAValid/I");
  tree->Branch("HiggsMass",&HiggsMass,"HiggsMass/F");

  tree->Branch("Satisfies_bsgamma",&Satisfies_bsgamma ,"Satisfies_bsgamma/I");
  tree->Branch("Satisfies_gm2",&Satisfies_gm2 ,"Satisfies_gm2/I");
  tree->Branch("Satisfies_Bsmumu",&Satisfies_Bsmumu,"Satisfies_Bsmumu/I");
  tree->Branch("Satisfies_Higgs",&Satisfies_Higgs,"Satisfies_Higgs/I");
  tree->Branch("Satisfies_SUSY",&Satisfies_SUSY,"Satisfies_SUSY/I");
  tree->Branch("Satisfies_LSP",&Satisfies_LSP,"Satisfies_LSP/I");
  tree->Branch("Satisfies_Top",&Satisfies_Top,"Satisfies_Top/I");
    
  tree->Branch("Satisfies_Mbmb",&Satisfies_Mbmb,"Satisfies_Mbmb/I");
  tree->Branch("Satisfies_Alphas",&Satisfies_Alphas,"Satisfies_Alphas/I");
  tree->Branch("Satisfies_RBtaunu",&Satisfies_RBtaunu,"Satisfies_RBtaunu/I");
  tree->Branch("Compatibility",&Compatibility,"Compatibility/F");
  
  tree->Branch("KSP_LM",&KSP_LM,"KSP_LM/F");
  tree->Branch("KSP_HM",&KSP_HM,"KSP_HM/F");
  tree->Branch("KSP_MET",&KSP_MET,"KSP_MET/F");
  tree->Branch("KSP_BTAG",&KSP_BTAG,"KSP_BTAG/F");
  tree->Branch("eff",&eff,"eff/F");
  tree->Branch("FullStats",&FullStats,"FullStats/F");
  tree->Branch("LM_Integral",&LM_Integral,"LM_Integral/F");

  ifstream LogIn;
  LogIn.open("pointlog.txt");
    
  std::string line;
  int entries=0;

  if (LogIn.is_open()) {
      while ( LogIn.good() ) {
          Problematic=false;
          getline (LogIn,line);
          InterpretLine(line);
          entries++;
//          if(entries%100==0) std::cout << "Working on entry " << entries << std::endl;
          if(Problematic) continue;
          tree->Fill();
      }
  }
  
  std::cout << " Whole file has " << entries << " entries" << std::endl;
  std::cout << "Final tree has " << tree->GetEntries() << " entries." << std::endl;
  
  tree->Write();  
  fout->Close();


return 0;
}
