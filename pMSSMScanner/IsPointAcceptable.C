#include <iostream>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <assert.h>

#include "StorageLibrary.C"

bool IsKilled=false;

float x0_bsg = 3.55e-4;
float dx_bsg = 0.34e-4;
float x0_amu = 26.1*1e-10;
float dx_amu = 12.8*1e-10;
float x0_RBtaunu = 1.63;
float dx_RBtaunu = 0.54;
float x0_mt = 173.3;
float dx_mt = 1.4;
float x0_mbmb = 4.19;
float dxp_mbmb = 0.18;
float dxm_mbmb = 0.06;
float x0_alphas = 0.1184;
float dx_alphas = 0.0007;
float x0_Bsmumu = 3.2e-9;
float dxp_Bsmumu = 1.5e-9;
float dxm_Bsmumu = 1.2e-9;


int Satisfies_bsgamma=-1;
int Satisfies_gm2=-1;
int Satisfies_Bsmumu=-1;
int Satisfies_Higgs=-1;
int Satisfies_SUSY=-1;
int Satisfies_LSP=-1;
int Satisfies_Top=-1;
int Satisfies_Mbmb=-1;
int Satisfies_Alphas=-1;
int Satisfies_RBtaunu=-1;




std::vector<std::string> StringSplit(std::string str, std::string delim) {
	std::vector<std::string> results;
	int cutAt;
	while( (cutAt = str.find_first_of(delim)) != (int)str.npos ) {
		if(cutAt > 0) results.push_back(str.substr(0,cutAt));
		str = str.substr(cutAt+1);
	}
	
	if(str.length() > 0) results.push_back(str);
	return results;
}

void Assign(std::string line, float value) {  
  if((int)line.find("BR(b->s gamma)")>=0) {
    float bsgamma=value;
    if(!(bsgamma > (x0_bsg-2*dx_bsg) &&  bsgamma < (x0_bsg+2*dx_bsg))) {
      std::cout << "xxxxxxxx Point contradicts BR(b->s gamma) constraint!" << std::endl;
      IsKilled=true;
      Satisfies_bsgamma=false;
    } else {
      std::cout << "         BR(b->s gamma) constraint satisfied" << std::endl;
      Satisfies_bsgamma=1;
    }
  }
  if((int)line.find("a_muon")>=0) {
    float muon_gm2=value;
    if(!((muon_gm2 > x0_amu-2*dx_amu && muon_gm2 < x0_amu+2*dx_amu))) {
      std::cout << "xxxxxxxx Point contradicts anomalous magnetic moment of muon " << std::endl;
      IsKilled=true;
      Satisfies_gm2=false;
    } else {
      std::cout << "         constraint from anomalous magnetic moment of muon satisfied" << std::endl;
      Satisfies_gm2=1;
    }
  }
  if((int)line.find("BR(Bs->mu mu)")>=0) {
    float Bsmumu=value;
    if(!(Bsmumu > x0_Bsmumu-2*dxm_Bsmumu && Bsmumu < x0_Bsmumu+2*dxp_Bsmumu)) {
      std::cout << "xxxxxxxx Point contradicts constraint from BR(Bs->mu mu)" << std::endl;
      IsKilled=true;
      Satisfies_Bsmumu=false;
    } else {
      std::cout << "         BR(Bs->mu mu) constraint satisfied" << std::endl;
      Satisfies_Bsmumu=1;
    }
  }
  if((int)line.find("excluded_Higgs_mass")>=0) {
    if(value==1) {
      std::cout << "xxxxxxxx Higgs mass has already been excluded ... " << std::endl;
      IsKilled=true;
      Satisfies_Higgs=false;
    } else {
      std::cout << "         Higgs constraint satisfied" << std::endl;
      Satisfies_Higgs=1;
    }
  }
  if((int)line.find("excluded_SUSY_mass")>=0) {
    if(value==1) {
      std::cout << "xxxxxxxx SUSY masses already excluded ... " << std::endl;
      IsKilled=true;
      Satisfies_SUSY=false;
    } else {
      std::cout << "         SUSY  mass constraints satisfied" << std::endl;
      Satisfies_SUSY=1;
    }
  }
  if((int)line.find("charged_LSP")>=0) {
    if(value==1) {
      std::cout << "xxxxxxxx LSP is charged ... " << std::endl;
      Satisfies_LSP=false;
      IsKilled=true;
    } else {
      std::cout << "         LSP not charged" << std::endl;
      Satisfies_LSP=1;
    }
  }
}

void ProcessLine(std::string line) {
  std::vector<std::string> Parts = StringSplit(line,"\t");
  Assign(line,atof(Parts[Parts.size()-1].c_str()));
}

void ProcessResLine(std::string line) {
  std::vector<std::string> Infos=StringSplit(line," ");
  
  int size=Infos.size();
  
  float mt,mbmb,alphas,mh,mlim,omg,bsgamma,RBtaunu,Bsmumu,muon_gm2;
  
  if(size>19) {
    mt = atof(Infos[19].c_str());
    if(!(mt > x0_mt-2*dx_mt && mt < x0_mt+2*dx_mt)) {
      std::cout << "xxxxxxxx top mass is off : " << mt << std::endl;
      IsKilled=true;
      Satisfies_Top=false;
    } else { 
      std::cout << "         top mass ok (" << mt << ")" << std::endl;
      Satisfies_Top=1;
    }
  }
  
  if(size>20)  {
    mbmb = atof(Infos[20].c_str());
    if(!((mbmb > x0_mbmb-2*dxm_mbmb && mbmb < x0_mbmb+2*dxp_mbmb))) {
      std::cout << "xxxxxxxx mbmb is off : " << mbmb << std::endl;
      IsKilled=true;
      Satisfies_Mbmb=false;
    } else { 
      std::cout << "         mbmb ok (" << mbmb << ")" << std::endl;
      Satisfies_Mbmb=1;
    }
  }

  if(size>21) {
    alphas = atof(Infos[21].c_str());
    if(!(alphas > x0_alphas-2*dx_alphas && alphas < x0_alphas+2*dx_alphas)) {
      std::cout << "xxxxxxxx alphas is off : " << alphas << std::endl;
      IsKilled=true;
      Satisfies_Alphas=false;
    } else { 
      std::cout << "         alphas mass ok (" << alphas << ")" << std::endl;
      Satisfies_Alphas=1;
    }
  }
/*    
  if(size>44) {//superfluous since we reject any point not fulfilling this early on
    mh = atof(Infos[44].c_str());
    if(!(mh > 123 && mh < 128)) {
      std::cout << "xxxxxxxx Higgs mass is off : " << mh << std::endl;
      IsKilled=true;
    } else { 
      std::cout << "         Higgs mass ok (" << mh << ")" << std::endl;
    }
  }
    
/*  if(size>53) {
    mlim = atof(Infos[53].c_str());
    if(!(mlim==0)) {
      std::cout << "xxxxxxxx mlim is off : " << mlim << std::endl;
      IsKilled=true;
    } else { 
      std::cout << "         mlim ok (" << mlim << ")" << std::endl;
    }
  }*/
  /*
  if(size>55) {
    omg = atof(Infos[55].c_str());
    if(!(omg>0.094 && omg<0.129)) {
      std::cout << "xxxxxxxx relic density is off : " << omg << std::endl;
      IsKilled=true;
    } else { 
      std::cout << "         relic density ok (" << omg << ")" << std::endl;
    }
  }*/
  
  if(size>59) {
    bsgamma = atof(Infos[59].c_str());
    if(!((bsgamma > (x0_bsg-2*dx_bsg) && bsgamma < (x0_bsg+2*dx_bsg)))) {
      std::cout << "xxxxxxxx bsgamma is off : " << bsgamma << std::endl;
      IsKilled=true;
      Satisfies_bsgamma=false;
    } else { 
      std::cout << "         bsgamma ok (" << bsgamma << ")" << std::endl;
      Satisfies_bsgamma=1;
    }
  }
  
  if(size>61) {
    RBtaunu = atof(Infos[61].c_str());
    if(!(RBtaunu > x0_RBtaunu-2*dx_RBtaunu and RBtaunu < x0_RBtaunu+2*dx_RBtaunu)) {
      std::cout << "xxxxxxxx RBtaunu is off : " << RBtaunu << std::endl;
      IsKilled=true;
      Satisfies_RBtaunu=false;
    } else { 
      std::cout << "         RBtaunu ok (" << RBtaunu << ")" << std::endl;
      Satisfies_RBtaunu=1;
    }
  }
  
  if(size>66) {
    Bsmumu = atof(Infos[66].c_str());
    if(!((Bsmumu > x0_Bsmumu-2*dxm_Bsmumu and Bsmumu < x0_Bsmumu+2*dxp_Bsmumu))) {
      std::cout << "xxxxxxxx BsMuMu is off : " << Bsmumu << std::endl;
      IsKilled=true;
      Satisfies_Bsmumu=false;
    } else { 
      std::cout << "         BsMuMu is ok (" << Bsmumu << ")" << std::endl;
      Satisfies_Bsmumu=1;
    }
  }

  if(size>70) {
    muon_gm2 = atof(Infos[70].c_str());
    if(!(muon_gm2 > x0_amu-2*dx_amu and muon_gm2 < x0_amu+2*dx_amu)) {
      std::cout << "xxxxxxxx g2 is off : " << muon_gm2 << std::endl;
      IsKilled=true;
      Satisfies_gm2=false;
    } else { 
      std::cout << "         g2 ok (" << muon_gm2 << ")" << std::endl;
      Satisfies_gm2=1;
    }
  }
}

void PrintGlasses() {
std::cout << "            _____________________________________________________ " << std::endl;
std::cout << "   _,aad888P\"\"\"\"\"\"\"\"\"\"Y888888888888888888888888888888P\"\"\"\"\"\"\"\"\"\"\"Y888baa,_" << std::endl;
std::cout << "  aP'                     `\"\"Ybaa,         ,aadP\"\"'                     `Ya" << std::endl;
std::cout << " dP                             `\"b,     ,d\"'                             Yb" << std::endl;
std::cout << " 8l                               8l_____8l                                8l" << std::endl;
std::cout << "[8l                               8l\"\"\"\"\"8l                                8l]" << std::endl;
std::cout << " 8l                              d8       8b                               8l" << std::endl;
std::cout << " 8l                             dP/       \\Yb                              8l" << std::endl;
std::cout << " 8l                           ,dP/         \\Yb,                            8l" << std::endl;
std::cout << " 8l                        ,adP'/           \\`Yba,                         8l" << std::endl;
std::cout << " Yb                     ,adP'                   `Yba,                     dP" << std::endl;
std::cout << "  Yb                ,aadP'                         `Ybaa,                dP" << std::endl;
std::cout << "   `Yb          ,aadP'                                 `Ybaa,          dP'" << std::endl;
std::cout << "     `Ybaaaaad8P\"'                                         `\"Y8baaaaadP'" << std::endl;
std::cout << "" << std::endl;
}

void StoreThisPoint() {
  std::ifstream QuickConfig;
  QuickConfig.open("LastPoint.cfg");
  float pMSSMpars[20];
  float HiggsMass;
  while(QuickConfig >> pMSSMpars[0] >> pMSSMpars[1] >> pMSSMpars[2] >> pMSSMpars[3] >> pMSSMpars[4] >> pMSSMpars[5] >> pMSSMpars[6] >> pMSSMpars[7] >> pMSSMpars[8] >> pMSSMpars[9] >> pMSSMpars[10] >> pMSSMpars[11] >> pMSSMpars[12] >> pMSSMpars[13] >> pMSSMpars[14] >> pMSSMpars[15] >> pMSSMpars[16] >> pMSSMpars[17] >> pMSSMpars[18] >> pMSSMpars[19] >> HiggsMass) {
    StorePoint(pMSSMpars,1,HiggsMass,Satisfies_bsgamma,Satisfies_gm2,Satisfies_Bsmumu,Satisfies_Higgs,Satisfies_SUSY,Satisfies_LSP,Satisfies_Top,Satisfies_Mbmb,Satisfies_Alphas,Satisfies_RBtaunu);
    break;
  }
}

int main() {
  PrintGlasses();
  
  std::ifstream information;
  information.open("Constraints.txt");
  std::string information_line;
  bool ObservableList=false;
  while(std::getline(information,information_line)) {
    if(!ObservableList&&(int)information_line.find("Observable")>=0) {
      ObservableList=true;
      continue;
    }
    if(!ObservableList) continue;
    if(information_line.size()<5) continue;
    ProcessLine(information_line);
  }
  
  information.close();
  
  std::ifstream ResInformation;
  ResInformation.open("res_t.tmp");
  while(std::getline(ResInformation,information_line)) {
    ProcessResLine(information_line);
  }
  
  ResInformation.close();
  
  std::ofstream answer;
  answer.open("IsAcceptable.txt");
  answer << !IsKilled;
  answer.close();

  std::ofstream ConstraintLog;
  ConstraintLog.open("ConstraintLog.txt",std::ios::app);
  ConstraintLog << Satisfies_bsgamma << "\t" << Satisfies_gm2<< "\t" << Satisfies_Bsmumu << "\t" << Satisfies_Higgs<< "\t" << Satisfies_SUSY<< "\t" << Satisfies_LSP<< "\t" << Satisfies_Top<< "\t" << Satisfies_Mbmb<< "\t" << Satisfies_Alphas<< "\t" << Satisfies_RBtaunu << "\t" << !IsKilled << std::endl;
  ConstraintLog.close();
  
  if(IsKilled) {
    cout << "Got killed. Storing. " << endl;
    StoreThisPoint();
  }
  
  return 0;
}

