#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "src/StorageLibrary.cc"


void StoreThisPoint(float compatibility, float KSP_LM, float KSP_HM, float KSP_MET, float KSP_BTag, float eff ) {
  std::cout << "Storing this point with a compatibility of " << compatibility << std::endl;
  std::ifstream QuickConfig;
  QuickConfig.open("LastPoint.cfg");
  float pMSSMpars[20];
  float HiggsMass;
  while(QuickConfig >> pMSSMpars[0] >> pMSSMpars[1] >> pMSSMpars[2] >> pMSSMpars[3] >> pMSSMpars[4] >> pMSSMpars[5] >> pMSSMpars[6] >> pMSSMpars[7] >> pMSSMpars[8] >> pMSSMpars[9] >> pMSSMpars[10] >> pMSSMpars[11] >> pMSSMpars[12] >> pMSSMpars[13] >> pMSSMpars[14] >> pMSSMpars[15] >> pMSSMpars[16] >> pMSSMpars[17] >> pMSSMpars[18] >> pMSSMpars[19] >> HiggsMass) {
    StorePoint(pMSSMpars,1,HiggsMass,1,1,1,1,1,1,1,1,1,1,compatibility,KSP_LM,KSP_HM,KSP_MET,KSP_BTag,eff);
    break;
  }
}

void StoreBestPoint(float compatibility) {
  std::cout << "Storing this point with a compatibility of " << compatibility << std::endl;
  std::ifstream QuickConfig;
  QuickConfig.open("LastPoint.cfg");
  float pMSSMpars[20];
  float HiggsMass;
  while(QuickConfig >> pMSSMpars[0] >> pMSSMpars[1] >> pMSSMpars[2] >> pMSSMpars[3] >> pMSSMpars[4] >> pMSSMpars[5] >> pMSSMpars[6] >> pMSSMpars[7] >> pMSSMpars[8] >> pMSSMpars[9] >> pMSSMpars[10] >> pMSSMpars[11] >> pMSSMpars[12] >> pMSSMpars[13] >> pMSSMpars[14] >> pMSSMpars[15] >> pMSSMpars[16] >> pMSSMpars[17] >> pMSSMpars[18] >> pMSSMpars[19] >> HiggsMass) {
    break;
  }
  
  std::ofstream Storage;
  Storage.open("RecentPointStorage.txt");
  Storage << "CurrentPoint ";
  for(int i=0;i<20;i++) Storage << pMSSMpars[i] << " ";
  Storage << " -1 " << std::endl << "BestPoint ";
  for(int i=0;i<20;i++) Storage << pMSSMpars[i] << " ";
  Storage << compatibility << std::endl;
  Storage.close();
  
}


int main(int argc, char** argv) {
  if(argc<3) {
    std::cout << "you're supposed to give me the compatibility so i can store it (the second argument is the path to the summary file). try again. " << std::endl;
    return -1;
  }
  
  std::cout << "Storing point now ... " << std::endl;
  std::ifstream KS;
    KS.open(argv[2]);
    float KSP_LM,KSP_HM,KSP_MET,KSP_BTag,eff;
    while(KS >> KSP_LM >> KSP_HM >> KSP_MET >> KSP_BTag >> eff) {
        std::cout << "Loaded values " << KSP_LM << " : " << KSP_HM << " : " << KSP_MET << " : " << KSP_BTag << " : " << eff << std::endl;
    }

    
  StoreThisPoint(atof(argv[1]),KSP_LM,KSP_HM,KSP_MET,KSP_BTag,eff);
  StoreBestPoint(atof(argv[1]));
}