#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>

#include <TSystem.h>
#include <TRandom3.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>

#include "MicroInput.C"
#include "StorageLibrary.C"

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

void RunMicroMegas() {
//  gSystem->Exec("cp genPoint.dat micromegas_2.4.5/MSSM/ && cd micromegas_2.4.5/MSSM/ && ./pmssmz1lsp genPoint.dat");
//  gSystem->Exec("cp micromegas_2.4.5/MSSM/LesH* .");
  gSystem->Exec("/shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/micromegas_2.4.5/MSSM/pmssmz1lsp genPoint.dat > micrOMEGAs_Log.txt");
  
    ifstream MicroErr;
  MicroErr.open("microErrors.txt");
  std::string micro_line;
  bool RejectPoints=false;
  cout << "____________________________________________________________" << endl;
  cout << "MICROMEGAS RUNTIME ERRORS : " << endl;
   while(std::getline(MicroErr, micro_line))
   {
     cout << micro_line << endl;
     RejectPoints=true;
   }
   
   cout << "Base 0 test : " << RejectPoints << endl;
   assert(!RejectPoints);
  cout << "____________________________________________________________" << endl;
  
  MicroErr.close();
  
}

  
int ValidateSLHA() {
  ifstream susyout;
    susyout.open("LesHout");
  std::string suspect_line;
  bool showlines=false;
  bool RejectPoints=false;
  cout << "____________________________________________________________" << endl;
  cout << "MICROMEGAS ERROR SUMMARY : " << endl;
   while(std::getline(susyout, suspect_line))
   {
     if((int)suspect_line.find("Declining")>=0) {
       cout << suspect_line << endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("invalid")>=0) {
       cout << suspect_line << endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("Warning")>=0 && (int)suspect_line.find("LSP")>=0) { // bad LSP
       cout << suspect_line << endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("WARNING")>=0) { // other warning (e.g. chargino below LEP limits)
       cout << suspect_line << endl;
       RejectPoints=true;
     }
   }
  cout << "____________________________________________________________" << endl;
  
  //also check log file (may contain problems as well, such as LEP limits)
  ifstream miLog;
  miLog.open("micrOMEGAs_Log.txt");
  showlines=false;
  cout << "____________________________________________________________" << endl;
  cout << "MICROMEGAS LOG ERRORS : " << endl;
   while(std::getline(susyout, suspect_line))
   {
     if((int)suspect_line.find("Declining")>=0) {
       cout << suspect_line << endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("invalid")>=0) {
       cout << suspect_line << endl;
       RejectPoints=true;
     }
     if((int)suspect_line.find("WARNING")>=0) { // other warning (e.g. chargino below LEP limits)
       cout << suspect_line << endl;
       RejectPoints=true;
     }
   }
  cout << "____________________________________________________________" << endl;
  
  
  return !RejectPoints;
}

int CreateSLHAFile() {
  cout << "About to create SuSpect intput file " << endl;
  CreateMicroMegasInput(pMSSMpars);
  cout << "Done with that. Now going to run micrOMEGAs!" << endl;
  RunMicroMegas();
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
  gSystem->Exec("mv LesHout slhaspectrum.in");
  gSystem->Exec("cp /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/susyhit0812/susyhit.in .");
  gSystem->Exec("/shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/susyhit0812/run > /dev/null");
//  gSystem->Exec("cp suspect2_lha.out SusyHit/slhaspectrum.in");
//  gSystem->Exec("cd SusyHit && ./run && cp susyhit_slha.out ../ && cd ../");
  return true;
}

bool RunSuperIso() {
  gSystem->Exec("/shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/slha.x /shome/buchmann/Year_Of_SUSY_Discovery/NewSimulation/CMSSW_5_2_6/src/UserCode/Scans/LHECruncher/susyhit_slha.out | tee Constraints.txt");
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

bool HasAcceptableHiggsMass(float &hmass) {
  ifstream EarlySLHA;
  EarlySLHA.open("LesHout");
  
  std::string line;

  std::string curline[5];
  while(std::getline(EarlySLHA, line)) {
    if((int)line.find("h0")>=0 && (int)line.find("25")>=0) {
      cout << "******* : " << line << endl;
      std::stringstream iss(line);
      std::string code,mass,pound,name;
      iss >> code >> mass >> pound >> name;
      cout << code << " - " << mass << " - " << pound << " - " << name << endl;
      if(atoi(code.c_str())==25 && name=="h0") hmass=atof(mass.c_str());
    }
/*  while (EarlySLHA >> curline[0] >>  curline[1] >> curline[2]) {
    cout << curline[0] << " ; " << curline[1] << " : " << curline[2] << endl;
    if(atoi(curline[0].c_str())==25 || atoi(curline[1].c_str())==25 || atoi(curline[2].c_str())==25) {
      cout << "****************************************************************" << endl;
    }
    if(atoi(curline[0].c_str())==25) {
      hmass=atof(curline[1].c_str());
    }*/


  }
  
  bool Accepted=(abs(hmass-125.8)<2.5);
  cout << "Identified a Higgs mass of " << hmass << "; accepted=" << Accepted << endl;
  EarlySLHA.close();
  
  return Accepted;
}
  
void CleanUp() {
  const int nFiles=17;
  std::string files[nFiles] = {"suspect2_lha.in","suspect2_lha.out","suspect.log","suspect2.out","susyhit_slha.out","suspect2_lha.out","slhaspectrum.in","KS.txt","Candidate.slha","susyhit_slha.out","res_t.tmp","Constraints.txt","micrOMEGAs_Log.txt","ConstraintLog.txt","IsAcceptable.txt","output.flha","microErrors.txt"};
  for (int i=0;i<nFiles;i++) gSystem->Exec(("rm -f "+files[i]+" 2>/dev/null").c_str());
}

void HaveFun() {
   cout << " " << endl;
   cout << "             ___.-~\"~-._   __....__" << endl;
   cout << "           .'    `    \\ ~\"~        ``-." << endl;
   cout << "          /` _      )  `\\              `\\" << endl;
   cout << "         /`  a)    /     |               `\\" << endl;
   cout << "        :`        /      |                 \\" << endl;
   cout << "   <`-._|`  .-.  (      /   .            `;\\          _.-~\"~-." << endl;
   cout << "    `-. `--'_.'-.;\\___/'   .      .       | \\       ;`a)   )  `\\" << endl;
   cout << " _     /:--`     |        /     /        .'  \\     /      /    |~=-=--." << endl;
   cout << "(\"\\   /`/        |       '     '         /    ;`'._/ ,;_  |    /        \"." << endl;
   cout << "`\'\\_/`/         .\\     /`~`=-.:        /     ``.,__/ ` `| `\"~`           \\" << endl;
   cout << "  `._.'          /`\\    |      `\\      /(            .--'   /      /      |\\" << endl;
   cout << "                /  /\\   |        `Y   /  \\          /  ____/\\     |      / `\"" << endl;
   cout << "               J  /  Y  |         |  /`\\  \\        /__/  |  |`-...-\\    |" << endl;
   cout << "              /  |   |  |         |  |  |  |       '""   |  |      |`\\  |" << endl;
   cout << "             \"---\"  /___|        /___|  /__|             /__|     /__/__|" << endl;
   cout << "                    '\"\"\"         '\"\"\"  '\"\"\"              '\"\"\"    '\"\"\"'\"\"" << endl;
   cout << "                                                                                         " << endl;
   cout << "                                                                                         " << endl;
   cout << "                                                                                         " << endl;
   cout << "                          starting point finding now ...                                 " << endl;
   cout << "                                                                                         " << endl;
   cout << "                                                                                         " << endl;
}

void HaveFunAfter() {
cout << " " << endl;
cout << "" << endl;
cout << "  " << endl;
cout << "" << endl;
cout << "                  /^----^\\" << endl;
cout << "                  | 0  0 |" << endl;
cout << "    Whoo!!        |  \\/  |" << endl;
cout << "                  /       \\" << endl;
cout << "      Whoo!!     |     |;;;|" << endl;
cout << "                 |     |;;;|          \\   \\" << endl;
cout << "                 |      \\;;|           \\\\//" << endl;
cout << "                  \\       \\|           / /" << endl;
cout << " ------------------(((--(((------------\\ \\----------," << endl;
cout << "  --  ___  ----  __ ---   ____   ---- _____ -- __ - \\" << endl;
cout << "  __ --   __ -- _____ --- __  ----  ___  ---- __ -- /" << endl;
cout << " ---------------/ /---------------\\  \\--------------`" << endl;
cout << "                \\ \\               / /" << endl;
cout << "                 //\\             //" << endl;
cout << "                 \\               \\\\" << endl;
cout << "                                 /" << endl;
cout << "Glo Pearl" << endl;
}

void StoreThisPoint(std::string problem, float HiggsMass=-1) {
  if(problem=="InvalidSpectrum") StorePoint(pMSSMpars,0,HiggsMass);
  else StorePoint(pMSSMpars,1,HiggsMass);
}

void RunMCMC() {
/*  cout << "Going to run MCMC " << endl;
  ofstream tmp;
  tmp.open("res_t.tmp",std::ios::app);
  tmp << "0  ";
  tmp.close();
  gSystem->Exec("./MCMC.exec l");
//  gSystem->Exec("cp micromegas_2.4.5/MSSM/res_t.tmp . && ./MCMC.exec l");
*/
}
  
  
void WriteQuickConfig(float HiggsMass) {
  ofstream QuickConfig;
  QuickConfig.open("LastPoint.cfg");
  for(int i=0;i<20;i++) QuickConfig << pMSSMpars[i] << " ";
  QuickConfig << HiggsMass << " ";
  QuickConfig << endl;
  QuickConfig.close();
}
  
int main(int argc, char** argv) {
  
  if(argc<2) {
    cout << "You need to provide the current path, i.e. " << argv[0] << " `pwd` " << endl;
  } else {
    cout << "Current directory : " << argv[1] << endl;
  }
  
  HaveFun();

  randgen = new TRandom3(0);
  
  TFile *f = new TFile("HiggsPoints.root","UPDATE");
  f->cd();
  TH1F *histo = (TH1F*)f->Get("mhisto");
  TTree *tree;
  if(!histo) {
    histo = new TH1F("mhisto","mhisto",400,0,200);
    histo->Sumw2();
    tree = new TTree("pmssm","pmssm");
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
  } else {
    tree = (TTree*)f->Get("pmssm");
    tree->SetBranchAddress("mu",&pMSSMpars[8]);
    tree->SetBranchAddress("MG1",&pMSSMpars[2]);
    tree->SetBranchAddress("MG2",&pMSSMpars[3]);
    tree->SetBranchAddress("MG3",&pMSSMpars[4]);
    tree->SetBranchAddress("Ml1",&pMSSMpars[9]);
    tree->SetBranchAddress("Ml2",&pMSSMpars[9]);
    tree->SetBranchAddress("Ml3",&pMSSMpars[11]);
    tree->SetBranchAddress("Mr1",&pMSSMpars[10]);
    tree->SetBranchAddress("Mr2",&pMSSMpars[10]);
    tree->SetBranchAddress("Mr3",&pMSSMpars[12]);
    tree->SetBranchAddress("Mq1",&pMSSMpars[13]);
    tree->SetBranchAddress("Mq2",&pMSSMpars[13]);
    tree->SetBranchAddress("Mq3",&pMSSMpars[14]);
    tree->SetBranchAddress("Mu1",&pMSSMpars[15]);
    tree->SetBranchAddress("Mu2",&pMSSMpars[15]);
    tree->SetBranchAddress("Mu3",&pMSSMpars[16]);
    tree->SetBranchAddress("Md1",&pMSSMpars[17]);
    tree->SetBranchAddress("Md2",&pMSSMpars[17]);
    tree->SetBranchAddress("Md3",&pMSSMpars[18]);
    tree->SetBranchAddress("At",&pMSSMpars[6]);
    tree->SetBranchAddress("Ab",&pMSSMpars[5]);
    tree->SetBranchAddress("Al",&pMSSMpars[7]);
    tree->SetBranchAddress("MH3",&pMSSMpars[1]);
    tree->SetBranchAddress("tb",&pMSSMpars[0]);
  }

    
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
  
  float mh=0;
  while(!HaveValidSLHA&&nAttempts<10000) {
    CleanUp();
    GetParameters(IsInitialRun);
    StoreParameters();
    HaveValidSLHA=CreateSLHAFile();
    cout << "After generating the SLHA file, do we have a valid one ? " << HaveValidSLHA << endl;
    nAttempts++;
    if(!HaveValidSLHA) {
      StoreThisPoint("InvalidSpectrum");
      continue;
    }
    HaveValidSLHA=HasAcceptableHiggsMass(mh);
    histo->Fill(mh);
    if(!HaveValidSLHA) {
      cout << "Higgs killed this one" << endl;
      StoreThisPoint("Higgs",mh);
      continue;
    }
    if(HaveValidSLHA) HaveValidSLHA=RunSuperIso();
    if(HaveValidSLHA) RunMCMC();
    
    if(HaveValidSLHA) HaveValidSLHA=RunSUSYHit();
    /*
    if(HaveValidSLHA) HaveValidSLHA=RunSuperIso();
    if(HaveValidSLHA) AppendParameters();
    */
    if(!HaveValidSLHA) StoreThisPoint("InvalidSpectrum",mh);
  }
  
  histo->Write();
  tree->Fill();
  tree->Write();
  f->Close();
  
  HaveFunAfter();
  
  cout << "Getting a valid model took " << nAttempts << " attempts." << endl;
  WriteQuickConfig(mh);
  return 0;
}