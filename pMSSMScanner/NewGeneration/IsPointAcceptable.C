#include <iostream>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <assert.h>

#include "include/StorageLibrary.h"

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
      std::cout << "xxxxxxxx Point contradicts BR(b->s gamma) constraint!     (value: " << bsgamma << ") range: "<< (x0_bsg-2*dx_bsg) << " , " << (x0_bsg+2*dx_bsg) << std::endl;
      IsKilled=true;
      Satisfies_bsgamma=0;
    } else {
      std::cout << "         BR(b->s gamma) constraint satisfied     (value: " << bsgamma << ") range: "<< (x0_bsg-2*dx_bsg) << " , " << (x0_bsg+2*dx_bsg) << std::endl;
      Satisfies_bsgamma=1;
    }
  }
  if((int)line.find("a_muon")>=0) {
    float muon_gm2=value;
    if(!((muon_gm2 > x0_amu-2*dx_amu && muon_gm2 < x0_amu+2*dx_amu))) {
      std::cout << "xxxxxxxx Point contradicts anomalous magnetic moment of muon!     (value: " << muon_gm2 << ") range: " << x0_amu-2*dx_amu << " , " << x0_amu+2*dx_amu << std::endl;
      IsKilled=true;
      Satisfies_gm2=false;
    } else {
      std::cout << "         constraint from anomalous magnetic moment of muon satisfied     (value: " << muon_gm2 << ") range: " << x0_amu-2*dx_amu << " , " << x0_amu+2*dx_amu << std::endl;
      Satisfies_gm2=1;
    }
  }
  if((int)line.find("BR(Bs->mu mu)")>=0) {
    float Bsmumu=value;
    if(!(Bsmumu > x0_Bsmumu-2*dxm_Bsmumu && Bsmumu < x0_Bsmumu+2*dxp_Bsmumu)) {
      std::cout << "xxxxxxxx Point contradicts constraint from BR(Bs->mu mu)          (value: " << Bsmumu << ") range: " << x0_Bsmumu-2*dxm_Bsmumu << " , " << x0_Bsmumu+2*dxp_Bsmumu << std::endl;
      IsKilled=true;
      Satisfies_Bsmumu=0;
    } else {
      std::cout << "         BR(Bs->mu mu) constraint satisfied          (value: " << Bsmumu << ") range: " << x0_Bsmumu-2*dxm_Bsmumu << " , " << x0_Bsmumu+2*dxp_Bsmumu << std::endl;
      Satisfies_Bsmumu=1;
    }
  }
  if((int)line.find("excluded_Higgs_mass")>=0) {
    if(value==1) {
      std::cout << "xxxxxxxx Higgs mass has already been excluded ... " << std::endl;
      IsKilled=true;
      Satisfies_Higgs=0;
    } else {
      std::cout << "         Higgs constraint satisfied" << std::endl;
      Satisfies_Higgs=1;
    }
  }
  if((int)line.find("excluded_SUSY_mass")>=0) {
    if(value==1) {
      std::cout << "xxxxxxxx SUSY masses already excluded ... " << std::endl;
      IsKilled=true;
      Satisfies_SUSY=0;
    } else {
      std::cout << "         SUSY  mass constraints satisfied" << std::endl;
      Satisfies_SUSY=1;
    }
  }
  if((int)line.find("charged_LSP")>=0) {
    if(value==1) {
      std::cout << "xxxxxxxx LSP is charged ... " << std::endl;
      Satisfies_LSP=0;
      IsKilled=true;
    } else {
      std::cout << "         LSP not charged" << std::endl;
      Satisfies_LSP=1;
    }
  }
  if((int)line.find("R(B->tau nu)")==0) {
    float RBtaunu=value;
    if(! ((x0_RBtaunu-2*dx_RBtaunu)<RBtaunu   && RBtaunu< (x0_RBtaunu+2*dx_RBtaunu)  ) ) {
      std::cout << "xxxxxxxx R(B->tau nu) violated ... (" << RBtaunu << ")" << "      (range : " << (x0_RBtaunu-2*dx_RBtaunu)  << " , " <<  (x0_RBtaunu+2*dx_RBtaunu) << " )" << std::endl;
      Satisfies_RBtaunu=0;
      IsKilled=true;
    } else {
      std::cout << "         R(B->tau nu) ok (" << RBtaunu << ")" << "      (range : " << (x0_RBtaunu-2*dx_RBtaunu)  << " , " <<  (x0_RBtaunu+2*dx_RBtaunu) << " )" << std::endl;
      Satisfies_RBtaunu=1;
    }
  }
}

void ProcessLine(std::string line) {
  std::vector<std::string> Parts = StringSplit(line,"\t");
  Assign(line,atof(Parts[Parts.size()-1].c_str()));
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
  
  std::ofstream answer;
  answer.open("IsAcceptable.txt");
  answer << !IsKilled;
  answer.close();

  std::ofstream ConstraintLog;
  ConstraintLog.open("ConstraintLog.txt",std::ios::app);
  ConstraintLog << Satisfies_bsgamma << "\t" << Satisfies_gm2<< "\t" << Satisfies_Bsmumu << "\t" << Satisfies_Higgs<< "\t" << Satisfies_SUSY<< "\t" << Satisfies_LSP<< "\t" << Satisfies_Top<< "\t" << Satisfies_Mbmb<< "\t" << Satisfies_Alphas<< "\t" << Satisfies_RBtaunu << "\t" << !IsKilled << std::endl;
  ConstraintLog.close();
  
  if(IsKilled) {
    std::cout << "Got killed. Storing. " << std::endl;
    StoreThisPoint();
  }
  
  return 0;
}

