#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "../StorageLibrary.C"


void StoreThisPoint(float compatibility) {
  std::cout << "Storing this point with a compatibility of " << compatibility << std::endl;
  std::ifstream QuickConfig;
  QuickConfig.open("LastPoint.cfg");
  float pMSSMpars[20];
  float HiggsMass;
  while(QuickConfig >> pMSSMpars[0] >> pMSSMpars[1] >> pMSSMpars[2] >> pMSSMpars[3] >> pMSSMpars[4] >> pMSSMpars[5] >> pMSSMpars[6] >> pMSSMpars[7] >> pMSSMpars[8] >> pMSSMpars[9] >> pMSSMpars[10] >> pMSSMpars[11] >> pMSSMpars[12] >> pMSSMpars[13] >> pMSSMpars[14] >> pMSSMpars[15] >> pMSSMpars[16] >> pMSSMpars[17] >> pMSSMpars[18] >> pMSSMpars[19] >> HiggsMass) {
    StorePoint(pMSSMpars,1,HiggsMass,1,1,1,1,1,1,1,1,1,1,compatibility);
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
  if(argc<2) {
    std::cout << "you're supposed to give me the compatibility so i can store it. try again. " << std::endl;
    return -1;
  }
  
  StoreThisPoint(atof(argv[1]));
  StoreBestPoint(atof(argv[1]));
}