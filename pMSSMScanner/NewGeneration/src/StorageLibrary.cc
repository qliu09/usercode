#include <iostream>
#include <fstream>

#include "StorageLibrary.h"

void StorePoint(float pMSSMpars[],
                int SLHAValid, // first quick check (tachyons, charged LSP and so on)
                float HiggsMass, // second quick check (is Higgs mass usable? here the Higgs mass, rejected or not, is provided)
                int Satisfies_bsgamma,int Satisfies_gm2,int Satisfies_Bsmumu,int Satisfies_Higgs,int Satisfies_SUSY,int Satisfies_LSP,int Satisfies_Top,int Satisfies_Mbmb,int Satisfies_Alphas,int Satisfies_RBtaunu, // extensive constraint check
                float Compatibility, // overall compatibility
                float KSP_LM,  // Kolmogorov-Smirnov probability for low mass test
                float KSP_HM,  // Kolmogorov-Smirnov probability for high mass test
                float KSP_MET,  // Kolmogorov-Smirnov probability for MET test
                float KSP_BTag,  // Kolmogorov-Smirnov probability for btag test
                float eff, // efficiency (selected in low mass / full stats)
                float LMintegral, // low mass integral
                float FullStats) // Full Stats
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
    
    PointLog << KSP_LM << " ";
    PointLog << KSP_HM << " ";
    PointLog << KSP_MET << " ";
    PointLog << KSP_BTag << " ";
    PointLog << eff << " ";
    PointLog << LMintegral << " ";
    PointLog << FullStats << " ";
    PointLog << std::endl;
}
