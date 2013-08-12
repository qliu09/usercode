#include <iostream>
#include <fstream>


#ifndef __StorageLibrary_h__INCLUDED
#define __StorageLibrary_h__INCLUDED

void StorePoint(float pMSSMpars[],
                int SLHAValid=0, // first quick check (tachyons, charged LSP and so on)
                float HiggsMass=-1, // second quick check (is Higgs mass usable? here the Higgs mass, rejected or not, is provided)
                int Satisfies_bsgamma=-1,int Satisfies_gm2=-1,int Satisfies_Bsmumu=-1,int Satisfies_Higgs=-1,int Satisfies_SUSY=-1,int Satisfies_LSP=-1,int Satisfies_Top=-1,int Satisfies_Mbmb=-1,int Satisfies_Alphas=-1,int Satisfies_RBtaunu=-1, // extensive constraint check
                float Compatibility=-1) // last test
;


#endif