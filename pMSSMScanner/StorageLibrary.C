#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

void StorePoint(float pMSSMpars[],
                int SLHAValid=0, // first quick check (tachyons, charged LSP and so on)
                float HiggsMass=-1, // second quick check (is Higgs mass usable? here the Higgs mass, rejected or not, is provided)
                int Satisfies_bsgamma=-1,int Satisfies_gm2=-1,int Satisfies_Bsmumu=-1,int Satisfies_Higgs=-1,int Satisfies_SUSY=-1,int Satisfies_LSP=-1,int Satisfies_Top=-1,int Satisfies_Mbmb=-1,int Satisfies_Alphas=-1,int Satisfies_RBtaunu=-1, // extensive constraint check
                float Compatibility=-1) // last test
{
    std::ofstream PointLog;
    PointLog.open("pointlog.txt",std::ios::app);
    
    for(int i=0;i<20;i++) PointLog << pMSSMpars[i] << " ";
    PointLog << SLHAValid << " ";
    PointLog << HiggsMass << " ";
    PointLog << Satisfies_bsgamma << " ";
    PointLog << Satisfies_gm2 << " ";
    PointLog << Satisfies_Bsmumu << " ";
    PointLog << Satisfies_Higgs << " ";
    PointLog << Satisfies_SUSY << " ";
    PointLog << Satisfies_LSP << " ";
    PointLog << Satisfies_Top << " ";
    PointLog << Satisfies_Mbmb << " ";
    PointLog << Satisfies_Alphas << " ";
    PointLog << Satisfies_RBtaunu << " ";
    
    PointLog << Compatibility << " ";
    
    PointLog << endl;
}
