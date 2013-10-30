#include <iostream>
#include <fstream>
#include <stdlib.h>

using std::endl;
using std::cout;

void CreateMicroMegasInput(float pMSSMParameters[20]) {
  std::ofstream SusPectCard;
  SusPectCard.open("genPoint.dat");  
  
  SusPectCard << "alfEMZ 0.007818061" << endl;
  SusPectCard << "alfSMZ 0.118245" << endl;
  SusPectCard << "MZ 91.1876" << endl;
  SusPectCard << "Mtp 172.426" << endl;
  SusPectCard << "MbMb 4.21428" << endl;
  SusPectCard << "mu " << pMSSMParameters[8] << endl;
  SusPectCard << "MG1 " << pMSSMParameters[2] << endl;
  SusPectCard << "MG2 " << pMSSMParameters[3] << endl;
  SusPectCard << "MG3 " << pMSSMParameters[4] << endl;
  SusPectCard << "Ml1 " << pMSSMParameters[9] << endl;
  SusPectCard << "Ml2 " << pMSSMParameters[9] << endl;
  SusPectCard << "Ml3 " << pMSSMParameters[11] << endl;
  SusPectCard << "Mr1 " << pMSSMParameters[10] << endl;
  SusPectCard << "Mr2 " << pMSSMParameters[10] << endl;
  SusPectCard << "Mr3 " << pMSSMParameters[12] << endl;
  SusPectCard << "Mq1 " << pMSSMParameters[13] << endl;
  SusPectCard << "Mq2 " << pMSSMParameters[13] << endl;
  SusPectCard << "Mq3 " << pMSSMParameters[14] << endl;
  SusPectCard << "Mu1 " << pMSSMParameters[15] << endl;
  SusPectCard << "Mu2 " << pMSSMParameters[15] << endl;
  SusPectCard << "Mu3 " << pMSSMParameters[16] << endl;
  SusPectCard << "Md1 " << pMSSMParameters[17] << endl;
  SusPectCard << "Md2 " << pMSSMParameters[17] << endl;
  SusPectCard << "Md3 " << pMSSMParameters[18] << endl;
  SusPectCard << "At " << pMSSMParameters[6] << endl;
  SusPectCard << "Ab " << pMSSMParameters[5] << endl;
  SusPectCard << "Al " << pMSSMParameters[7] << endl;
  SusPectCard << "MH3 " << pMSSMParameters[1] << endl;
  SusPectCard << "tb " << pMSSMParameters[0] << endl;
  SusPectCard << "Am 0"  << endl;
  SusPectCard.close();
}