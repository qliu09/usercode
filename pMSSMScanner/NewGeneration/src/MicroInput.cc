#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "MicroInput.h"

void CreateMicroMegasInput(float pMSSMParameters[20]) {
  std::ofstream SusPectCard;
  SusPectCard.open("genPoint.dat");  
  
  SusPectCard << "alfEMZ 0.007818061" << std::endl;
  SusPectCard << "alfSMZ 0.118245" << std::endl;
  SusPectCard << "MZ 91.1876" << std::endl;
  SusPectCard << "Mtp 172.426" << std::endl;
  SusPectCard << "MbMb 4.21428" << std::endl;
  SusPectCard << "mu " << pMSSMParameters[8] << std::endl;
  SusPectCard << "MG1 " << pMSSMParameters[2] << std::endl;
  SusPectCard << "MG2 " << pMSSMParameters[3] << std::endl;
  SusPectCard << "MG3 " << pMSSMParameters[4] << std::endl;
  SusPectCard << "Ml1 " << pMSSMParameters[9] << std::endl;
  SusPectCard << "Ml2 " << pMSSMParameters[9] << std::endl;
  SusPectCard << "Ml3 " << pMSSMParameters[11] << std::endl;
  SusPectCard << "Mr1 " << pMSSMParameters[10] << std::endl;
  SusPectCard << "Mr2 " << pMSSMParameters[10] << std::endl;
  SusPectCard << "Mr3 " << pMSSMParameters[12] << std::endl;
  SusPectCard << "Mq1 " << pMSSMParameters[13] << std::endl;
  SusPectCard << "Mq2 " << pMSSMParameters[13] << std::endl;
  SusPectCard << "Mq3 " << pMSSMParameters[14] << std::endl;
  SusPectCard << "Mu1 " << pMSSMParameters[15] << std::endl;
  SusPectCard << "Mu2 " << pMSSMParameters[15] << std::endl;
  SusPectCard << "Mu3 " << pMSSMParameters[16] << std::endl;
  SusPectCard << "Md1 " << pMSSMParameters[17] << std::endl;
  SusPectCard << "Md2 " << pMSSMParameters[17] << std::endl;
  SusPectCard << "Md3 " << pMSSMParameters[18] << std::endl;
  SusPectCard << "At " << pMSSMParameters[6] << std::endl;
  SusPectCard << "Ab " << pMSSMParameters[5] << std::endl;
  SusPectCard << "Al " << pMSSMParameters[7] << std::endl;
  SusPectCard << "MH3 " << pMSSMParameters[1] << std::endl;
  SusPectCard << "tb " << pMSSMParameters[0] << std::endl;
  SusPectCard << "Am 0"  << std::endl;
  SusPectCard.close();
}