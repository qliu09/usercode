#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>

#include <TSystem.h>
#include <TRandom3.h>

#include "SuSpectInput.C"
using std::cout;
using std::endl;

float pMSSMpars[20];

bool HaveNonZeroCompatibility=false;

TRandom3 *randgen;

float GetRandomParameter(int ipar) {
  if(ipar==0) return randgen->Uniform(1,60); // tan Beta
  if(ipar==1) return randgen->Uniform(50,2000); // MA
  if(ipar==2) return randgen->Uniform(-2500,2500); // M1
  if(ipar==3) return randgen->Uniform(-2500,2500); // M2
  if(ipar==4) return randgen->Uniform(50,2500); // M3
  if(ipar==5) return randgen->Uniform(-2000,2000); // Ad,s,b
  if(ipar==6) return randgen->Uniform(-2000,2000); // Au,c,t
  if(ipar==7) return randgen->Uniform(-2000,2000); // Ae,mu,tau
  if(ipar==8) return randgen->Uniform(-1000,2000); // mu
  if(ipar>=9) return randgen->Uniform(50,2500);
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

void StoreParameters() {
  ofstream ParOut;
  ParOut.open("MyParameters.txt",std::ios::app);
  ParOut << endl;
  for(int i=0;i<20;i++) ParOut << pMSSMpars[i] << ";";
  ParOut.close();
}
void GetParameters(bool IsInitialRun) {
  if(IsInitialRun) {
    for(int i=0;i<20;i++) pMSSMpars[i]=GetRandomParameter(i);
  } else {
    int iVariedParameter=randgen->Uniform(0,20);
    cout << "Going to vary parameter " << iVariedParameter << endl;
    pMSSMpars[iVariedParameter]=GetRandomParameter(iVariedParameter);
  }
    cout << "New suggested model : " << endl;
    for(int i=0;i<20;i++) cout << pMSSMpars[i] << " ";
    cout << endl;
}
  
void LoadBestPreviousParameters() {
  ifstream ParIn;
  ParIn.open("MyParameters.txt");
  float curpar[21]; // the last parameter is the compatibility, which may or may not be in there
  float BestCompatibility=0;
  while (ParIn >> curpar[0] >>  curpar[1] >>  curpar[2] >>  curpar[3] >>  curpar[4] >>  curpar[5] >>  curpar[6] >>  curpar[7] >>  curpar[8] >>  curpar[9] >>  curpar[10] >>  curpar[11] >>  curpar[12] >>  curpar[13] >>  curpar[14] >>  curpar[15] >>  curpar[16] >>  curpar[17] >>  curpar[18] >>  curpar[19] >>  curpar[20]) {
    if(curpar[2]>=0 && curpar[20]<=1 && curpar[20]>BestCompatibility) {
      BestCompatibility=curpar[20];
      for(int i=0;i<20;i++) pMSSMpars[i]=curpar[i];
      cout << "Loaded new favorite (has compatibility of " << BestCompatibility << " ) " << endl;
    }
  }
  if(BestCompatibility>0) {
    cout << "Our initial model for this round is : " << endl;
    for(int i=0;i<20;i++) {
      cout << pMSSMpars[i] << " ";
    }
    cout << "  with compatibility " << BestCompatibility << endl;
    HaveNonZeroCompatibility=true;
  }
}
    
bool exists(const char *filename)
{
  ifstream ifile(filename);
  return ifile;
}

void RunSuSpect() {
  gSystem->Exec("./suspect | tee suspect.log");
}

int ValidateSLHA() {
  ifstream susyout;
  susyout.open("suspect2.out");
  std::string suspect_line;
  bool showlines=false;
   while(std::getline(susyout, suspect_line))
   {
     if((int)suspect_line.find("Warning")>=0) showlines=true;
     if(!showlines) continue;
     
     if((int)suspect_line.find("Warning")>=0) {
       cout << "Skipping this line : " << endl;
       continue;
     }
     if((int)suspect_line.find("-------")>=0) {
       cout << "Skipping this line : " << endl;
       continue;
     }
     cout << "SUSPECT ERROR SUMMARY : " << endl;
     cout << suspect_line << endl;
     if((int)suspect_line.find("1")>=0) {
       cout << "There seems to be an error. will mark this one as invalid. " << endl;
       return 0;
     } else return 1;
   }
   
   // file invalid
   return 0;

}

int CreateSLHAFile() {
  cout << "About to create SuSpect intput file " << endl;
  CreateSuSpectInput(pMSSMpars);
  cout << "Done with that. Now going to run suspect!" << endl;
  RunSuSpect();
  return ValidateSLHA();
}

void WriteCompatibility() {
  ifstream CoPa;
  CoPa.open("KS.txt");
  float currcompt=0;
  while (CoPa >> currcompt) {}
  cout << "Read compatibility : " << currcompt << endl;
  ofstream ParOut;
  ParOut.open("MyParameters.txt",std::ios::app);
  ParOut << currcompt << "\n";
  ParOut.close();
  cout << "Written compatibility " << currcompt << " to file " << endl;
  gSystem->Exec("rm KS.txt");
}
    
  
bool RunSUSYHit() {
  gSystem->Exec("cp suspect2_lha.out SusyHit/slhaspectrum.in");
  gSystem->Exec("cd SusyHit && ./run && cp susyhit_slha.out ../ && cd ../");
  return true;
}

bool RunSuperIso() {
  gSystem->Exec("superiso_v3.3/slha.x susyhit_slha.out");
  cout << " *************************** STILL NEED TO IMPLEMENT XCHECKING SUPERISO *************************** " << endl;
  return true;
}
  
void AppendParameters() {
  ifstream OldSLHA;
  OldSLHA.open("susyhit_slha.out");
 
  ofstream NewSLHA;
  NewSLHA.open("Candidate.slha");
  
  int iline=0;
  
  std::string OldSLHA_line;

  while(std::getline(OldSLHA, OldSLHA_line)) {
     iline++;
     if(iline==4) {
       NewSLHA << "# ";
       for(int i=0;i<20;i++) NewSLHA << pMSSMpars[i] << " ";
       NewSLHA << endl;
     } else {
       NewSLHA << OldSLHA_line << endl;
     }
  }
  
  OldSLHA.close();
  NewSLHA.close();
}

bool HasAcceptableHiggsMass() {
  ifstream EarlySLHA;
  EarlySLHA.open("suspect2_lha.out");
  
  std::string line;

  std::string curline[5];
  float hmass=0;
  while (EarlySLHA >> curline[0] >>  curline[1] ) {
    if(atoi(curline[0].c_str())==25) {
      hmass=atof(curline[1].c_str());
    }
  }
  
  bool Accepted=(abs(hmass-125.8)<2.5);
  cout << "Identified a Higgs mass of " << hmass << "; accepted=" << Accepted << endl;
  EarlySLHA.close();
  
  return Accepted;
}
  
void CleanUp() {
  std::string files[11] = {"suspect2_lha.in","suspect2_lha.out","suspect.log","suspect2.out","susyhit_slha.out","suspect2_lha.out","slhaspectrum.in","KS.txt","Candidate.slha","susyhit_slha.out"};
  for (int i=0;i<10;i++) gSystem->Exec(("rm "+files[i]+" 2>/dev/null").c_str());
}
int main() {
  
/*
These are the different inputs & outputs: 
- MyParameters.txt : contains ALL the tested parameters in the following format: p1;p2;p3;p4;.....;KS
                      where KS is not filled at the time of creation of the SLHA file but we actually update the last line (N-1) in the Nth run
- KS.txt: exists if this is the N>1 run; is used to obtain the previous compatibility and compare it, i.e. to reject or accept it
- SLHA file: is created by program
*/
  
  randgen = new TRandom3(0);
  
  
  cout << "INPUT: read from MyParameters (if available) and also read last compatibility from KS.txt" << endl;
  cout << "OUTPUT: SLHA and write file name to LatestSLHA.txt; store last parameters in MyParameters.txt" << endl;
  
  bool IsInitialRun=true;
  
  if(exists("MyParameters.txt")) {
    IsInitialRun=false;
    cout << "Not the first run, we already have some probabilities available" << endl;
  }
  
  if(!IsInitialRun && exists("KS.txt")) WriteCompatibility(); 
  
  if(!IsInitialRun) LoadBestPreviousParameters();
  
  if(!HaveNonZeroCompatibility) {
    IsInitialRun=true;
    cout << "No run with non-zero compatibility found. treat this as though it was an initial run (all parameters randgenom)" << endl;
  }
  
  bool HaveValidSLHA=false;
  
  int nAttempts=0;
  
  while(!HaveValidSLHA&&nAttempts<100) {
    CleanUp();
    GetParameters(IsInitialRun);
    StoreParameters();
    HaveValidSLHA=CreateSLHAFile();
    cout << "After generating the SLHA file, do we have a valid one ? " << HaveValidSLHA << endl;
    nAttempts++;
    if(HaveValidSLHA) HaveValidSLHA=HasAcceptableHiggsMass();
    if(HaveValidSLHA) HaveValidSLHA=RunSUSYHit();
    if(HaveValidSLHA) HaveValidSLHA=RunSuperIso();
    if(HaveValidSLHA) AppendParameters();
  }
  
  return 0;
}