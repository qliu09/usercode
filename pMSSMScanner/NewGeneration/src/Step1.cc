#include <iostream>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <TRandom3.h>
#include <TSystem.h>



#include "Step1.h"
#include "MicroInput.h"
#include "DeformedNDimensionalSphere.h"
#include "StorageLibrary.h"



TRandom3 *randgen;

float pMSSMpars[20];

bool ExistsBestConfig=false;

void HaveFun() {
   std::cout << " " << std::endl;
   std::cout << "             ___.-~\"~-._   __....__" << std::endl;
   std::cout << "           .'    `    \\ ~\"~        ``-." << std::endl;
   std::cout << "          /` _      )  `\\              `\\" << std::endl;
   std::cout << "         /`  a)    /     |               `\\" << std::endl;
   std::cout << "        :`        /      |                 \\" << std::endl;
   std::cout << "   <`-._|`  .-.  (      /   .            `;\\          _.-~\"~-." << std::endl;
   std::cout << "    `-. `--'_.'-.;\\___/'   .      .       | \\       ;`a)   )  `\\" << std::endl;
   std::cout << " _     /:--`     |        /     /        .'  \\     /      /    |~=-=--." << std::endl;
   std::cout << "(\"\\   /`/        |       '     '         /    ;`'._/ ,;_  |    /        \"." << std::endl;
   std::cout << "`\'\\_/`/         .\\     /`~`=-.:        /     ``.,__/ ` `| `\"~`           \\" << std::endl;
   std::cout << "  `._.'          /`\\    |      `\\      /(            .--'   /      /      |\\" << std::endl;
   std::cout << "                /  /\\   |        `Y   /  \\          /  ____/\\     |      / `\"" << std::endl;
   std::cout << "               J  /  Y  |         |  /`\\  \\        /__/  |  |`-...-\\    |" << std::endl;
   std::cout << "              /  |   |  |         |  |  |  |       '""   |  |      |`\\  |" << std::endl;
   std::cout << "             \"---\"  /___|        /___|  /__|             /__|     /__/__|" << std::endl;
   std::cout << "                    '\"\"\"         '\"\"\"  '\"\"\"              '\"\"\"    '\"\"\"'\"\"" << std::endl;
   std::cout << "                                                                                         " << std::endl;
   std::cout << "                                                                                         " << std::endl;
   std::cout << "                                                                                         " << std::endl;
   std::cout << "                          starting point finding now ...                                 " << std::endl;
   std::cout << "                                                                                         " << std::endl;
   std::cout << "                                                                                         " << std::endl;
}


void HaveFunAfter() {
std::cout << " " << std::endl;
std::cout << "" << std::endl;
std::cout << "  " << std::endl;
std::cout << "" << std::endl;
std::cout << "                  /^----^\\" << std::endl;
std::cout << "                  | 0  0 |" << std::endl;
std::cout << "    Whoo!!        |  \\/  |" << std::endl;
std::cout << "                  /       \\" << std::endl;
std::cout << "      Whoo!!     |     |;;;|" << std::endl;
std::cout << "                 |     |;;;|          \\   \\" << std::endl;
std::cout << "                 |      \\;;|           \\\\//" << std::endl;
std::cout << "                  \\       \\|           / /" << std::endl;
std::cout << " ------------------(((--(((------------\\ \\----------," << std::endl;
std::cout << "  --  ___  ----  __ ---   ____   ---- _____ -- __ - \\" << std::endl;
std::cout << "  __ --   __ -- _____ --- __  ----  ___  ---- __ -- /" << std::endl;
std::cout << " ---------------/ /---------------\\  \\--------------`" << std::endl;
std::cout << "                \\ \\               / /" << std::endl;
std::cout << "                 //\\             //" << std::endl;
std::cout << "                 \\               \\\\" << std::endl;
std::cout << "                                 /" << std::endl;
std::cout << "Glo Pearl" << std::endl;
}

void FinalSteps() {
//  std::cerr << "Have not yet defined final steps ... " << std::endl;
}

void DefineRange(const int ipar, float &lowrange, float &highrange) {
  if(ipar==0) {
    lowrange=1;
    highrange=60;
    return;
  }
  if(ipar==1) {
    lowrange=50;
    highrange=2000;
    return;
  }
  if(ipar==2) { //M1
    lowrange=-2500;
    highrange=2500;
    return;
  }
  if(ipar==3) { //M2
    lowrange=-2500;
    highrange=2500;
    return;
  }
  if(ipar==4) { //M3
    lowrange=50;
    highrange=2500;
    return;
  }
  if(ipar==5) { // Ad,s,b
    lowrange=-2000;
    highrange=2000;
    return;
  }
  if(ipar==6) { // Au,c,t
    lowrange=-2000;
    highrange=2000;
    return;
  }
  if(ipar==7) { // Ae,mu,tau
    lowrange=-2000;
    highrange=2000;
    return;
  }
  if(ipar==8) { // mu
    lowrange=-1000;
    highrange=2000;
    return;
  }
  if(ipar>=9 && ipar<20) {
    lowrange=50;
    highrange=2500;
    return;
  // MebarL=MmubarL
  // MebarR=MmubarR
  // MtaubarL
  // Mq1barL, Mq2barL
  // Mqbar3L
  //MubarR=McbarR
  //MtbarR
  //MdbarR=MsbarR
  //MbbarR
  }
  std::cout << "Received unknown case : " << ipar << std::endl;
  assert(0);
  return;
}

float GetRandomParameter(const int ipar) {
  float lowrange,hirange;
  DefineRange(ipar,lowrange,hirange);
  return randgen->Uniform(lowrange,hirange);
}
  
float VaryAroundPoint(const float point, const int ipar) {
  float lowrange,hirange;
  DefineRange(ipar,lowrange,hirange);
  
  float newpoint=point;
  while(newpoint==point) {
    newpoint=point+randgen->Uniform(0.1*lowrange,0.1*hirange);
    //need to make sure new suggested point is in range!
    if(newpoint>hirange) newpoint=point;
    if(newpoint<lowrange) newpoint=point;
  }
  return newpoint;
}

bool exists(const char *filename)
{
  ifstream ifile(filename);
  return ifile;
}

void LoadBestConfig() {
  if(exists("RecentPointStorage.txt")) {
    ifstream PointStorage;
    PointStorage.open("RecentPointStorage.txt");
    std::string info;
    float compatibility;
    while(PointStorage >> info >> pMSSMpars[0] >> pMSSMpars[1] >> pMSSMpars[2] >> pMSSMpars[3] >> pMSSMpars[4] >> pMSSMpars[5] >> pMSSMpars[6] >> pMSSMpars[7] >> pMSSMpars[8] >> pMSSMpars[9] >> pMSSMpars[10] >> pMSSMpars[11] >> pMSSMpars[12] >> pMSSMpars[13] >> pMSSMpars[14] >> pMSSMpars[15] >> pMSSMpars[16] >> pMSSMpars[17] >> pMSSMpars[18] >> pMSSMpars[19] >> compatibility) {
      std::cout << "Loading line from RecentPointStorage ... " << std::endl;
      if(info == "BestPoint") {
          if(pMSSMpars[0]>-10000) ExistsBestConfig=true;
          else std::cout << "No best point found (is filler ... ) " << std::endl;
          break;
      } 
    }
  } else {
    std::cout << "No last best point " << std::endl;
    ExistsBestConfig=false;
  }
  
  std::cout << "Done loading configuration (ExistsBestConfig=" << ExistsBestConfig<< ")" << std::endl;
  
}

void DefineParameters() {
  LoadBestConfig();
  std::cout << "Is there a best configuration ? " << ExistsBestConfig << std::endl;
  
  if(ExistsBestConfig) {
    // only check in the vicinity which is defined as one parameter being varied by up to 10% of the full range   (so if we're at 50 of [-2000,2000] the new parameter will be in [-150,250] )
    /*
    int ParVary=randgen->Uniform(0,20);
    std::cout << "There is a \"best point\", and we'll vary parameter " << ParVary << " which is currently " << pMSSMpars[ParVary] << " . The new value for it is ";
    pMSSMpars[ParVary]=VaryAroundPoint(pMSSMpars[ParVary],ParVary);
    std::cout << pMSSMpars[ParVary] << std::endl;
    */
    GetPointFromDeformedNDimensionalSphere(pMSSMpars,20, randgen);
  } else {
    // there is no "best point", need to vary all parameters freely.
    for(int i=0;i<20;i++) pMSSMpars[i]=GetRandomParameter(i);
  }
  
  std::cout << "Done defining parameters: " << std::endl;
  for(int i=0;i<20;i++) std::cout << pMSSMpars[i] << ";";
  std::cout << std::endl;
  
}

void WriteToStorage() {
  float BestpMSSMpars[20];
  float compatibility=-1;
  for(int i=0;i<20;i++) BestpMSSMpars[i]=-1111111;
  if(exists("RecentPointStorage.txt")) {
    ifstream PointStorage;
    PointStorage.open("RecentPointStorage.txt");
    std::string info;
    while(PointStorage >> info >> BestpMSSMpars[0] >> BestpMSSMpars[1] >> BestpMSSMpars[2] >> BestpMSSMpars[3] >> BestpMSSMpars[4] >> BestpMSSMpars[5] >> BestpMSSMpars[6] >> BestpMSSMpars[7] >> BestpMSSMpars[8] >> BestpMSSMpars[9] >> BestpMSSMpars[10] >> BestpMSSMpars[11] >> BestpMSSMpars[12] >> BestpMSSMpars[13] >> BestpMSSMpars[14] >> BestpMSSMpars[15] >> BestpMSSMpars[16] >> BestpMSSMpars[17] >> BestpMSSMpars[18] >> BestpMSSMpars[19] >> compatibility) {
      if(info == "BestPoint") break;
    }
    
    PointStorage.close();
  }
  
  ofstream Storage;
  Storage.open("RecentPointStorage.txt");
  Storage << "CurrentPoint ";
  for(int i=0;i<20;i++) Storage << pMSSMpars[i] << " ";
  Storage << " -1 " << std::endl << "BestPoint ";
  for(int i=0;i<20;i++) Storage << BestpMSSMpars[i] << " ";
  Storage << compatibility << std::endl;
  Storage.close();
}

void RunMicroMegas() {
  gSystem->Exec("/shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/micromegas_2.4.5/MSSM/pmssmz1lsp genPoint.dat > micrOMEGAs_Log.txt");
  
  ifstream MicroErr;
  MicroErr.open("microErrors.txt");
  std::string micro_line;
  bool RejectPoints=false;
  std::cout << "____________________________________________________________" << std::endl;
  std::cout << "MICROMEGAS RUNTIME ERRORS : " << std::endl;
  while(std::getline(MicroErr, micro_line)) {
    std::cout << micro_line << std::endl;
    RejectPoints=true;
  }
  std::cout << "Base 0 test : " << RejectPoints << std::endl;
  assert(!RejectPoints);
  std::cout << "____________________________________________________________" << std::endl;
  
  MicroErr.close();
}



int ValidateSLHA() {
  ifstream susyout;
  susyout.open("LesHout");
  std::string suspect_line;
  bool RejectPoints=false;
  std::cout << "____________________________________________________________" << std::endl;
  std::cout << "MICROMEGAS ERROR SUMMARY : " << std::endl;
   while(std::getline(susyout, suspect_line))
   {
     if((int)suspect_line.find("Declining")>=0) {
       std::cout << suspect_line << std::endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("invalid")>=0) {
       std::cout << suspect_line << std::endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("Warning")>=0 && (int)suspect_line.find("LSP")>=0) { // bad LSP
       std::cout << suspect_line << std::endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("WARNING")>=0) { // other warning (e.g. chargino below LEP limits)
       std::cout << suspect_line << std::endl;
       RejectPoints=true;
     }
   }
  std::cout << "____________________________________________________________" << std::endl;
  
  //also check log file (may contain problems as well, such as LEP limits)
  ifstream miLog;
  miLog.open("micrOMEGAs_Log.txt");
  std::cout << "____________________________________________________________" << std::endl;
  std::cout << "MICROMEGAS LOG ERRORS : " << std::endl;
   while(std::getline(susyout, suspect_line))
   {
     if((int)suspect_line.find("Declining")>=0) {
       std::cout << suspect_line << std::endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("invalid")>=0) {
       std::cout << suspect_line << std::endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("WARNING")>=0) { // other warning (e.g. chargino below LEP limits)
       std::cout << suspect_line << std::endl;
       RejectPoints=true;
     }
   }
  std::cout << "____________________________________________________________" << std::endl;
  
  return !RejectPoints;
}

int CreateSLHAFile() {
  std::cout << "About to create SuSpect intput file " << std::endl;
  CreateMicroMegasInput(pMSSMpars);
  std::cout << "Done with that. Now going to run micrOMEGAs!" << std::endl;
  RunMicroMegas();
  return ValidateSLHA();
}

void StoreThisPoint(std::string problem, float HiggsMass) {
  if(problem=="InvalidSpectrum") StorePoint(pMSSMpars,0,HiggsMass);
  else StorePoint(pMSSMpars,1,HiggsMass);
}

bool HasAcceptableHiggsMass(float &hmass) {
  ifstream EarlySLHA;
  EarlySLHA.open("LesHout");
  
  std::string line;

  std::string curline[5];
  while(std::getline(EarlySLHA, line)) {
    if((int)line.find("h0")>=0 && (int)line.find("25")>=0) {
      std::cout << "******* : " << line << std::endl;
      std::stringstream iss(line);
      std::string code,mass,pound,name;
      iss >> code >> mass >> pound >> name;
      std::cout << code << " - " << mass << " - " << pound << " - " << name << std::endl;
      if(atoi(code.c_str())==25 && name=="h0") hmass=atof(mass.c_str());
    }
  }
  
  bool Accepted=(abs(hmass-125.8)<2.5);
  std::cout << "Identified a Higgs mass of " << hmass << "; accepted=" << Accepted << std::endl;
  EarlySLHA.close();
  
  return Accepted;
}

bool RunSUSYHit() {
  gSystem->Exec("mv LesHout slhaspectrum.in");
  gSystem->Exec("cp /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/susyhit0812/susyhit.in .");
  gSystem->Exec("/shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/susyhit0812/run > /dev/null");
//  gSystem->Exec("cp suspect2_lha.out SusyHit/slhaspectrum.in");
//  gSystem->Exec("cd SusyHit && ./run && cp susyhit_slha.out ../ && cd ../");
  return true;
}

bool RunSuperIso() {
  gSystem->Exec("/shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/slha.x /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/susyhit_slha.out | tee Constraints.txt");
  std::cout << " *************************** STILL NEED TO IMPLEMENT XCHECKING SUPERISO *************************** " << std::endl;
  return true;
}

void WriteQuickConfig(float HiggsMass) {
  ofstream QuickConfig;
  QuickConfig.open("LastPoint.cfg");
  for(int i=0;i<20;i++) QuickConfig << pMSSMpars[i] << " ";
  QuickConfig << HiggsMass << " ";
  QuickConfig << std::endl;
  QuickConfig.close();
}


int main() {
  
  HaveFun();
  
  int nAttempts=0;
  
  float mh=0;
  
  bool HaveValidSLHA=false;
  
  randgen = new TRandom3(0);
  
  while(!HaveValidSLHA&&nAttempts<10000) {
    std::cout << "\033[1;34m Attempt " << nAttempts << "\033[0m " << std::endl;

    HaveValidSLHA=false;
    
    DefineParameters();
    
    WriteToStorage();
    
    HaveValidSLHA=CreateSLHAFile();

    std::cout << "After generating the SLHA file, do we have a valid one ? " << HaveValidSLHA << std::endl;
    nAttempts++;
  
    if(!HaveValidSLHA) {
      StoreThisPoint("InvalidSpectrum");
      continue;
    }
    
    HaveValidSLHA=HasAcceptableHiggsMass(mh);
    
    if(!HaveValidSLHA) {
      std::cout << "Higgs killed this one" << std::endl;
      StoreThisPoint("Higgs",mh);
      continue;
    }

    if(HaveValidSLHA) HaveValidSLHA=RunSuperIso();
    
    if(HaveValidSLHA) HaveValidSLHA=RunSUSYHit();
    
    if(!HaveValidSLHA) StoreThisPoint("InvalidSpectrum",mh);
  }
  
  FinalSteps();
  
  std::cout << "Getting a theoretically valid model with acceptable Higgs mass took " << nAttempts << " attempts." << std::endl;
  
  WriteQuickConfig(mh);
  
  return 0;
}
