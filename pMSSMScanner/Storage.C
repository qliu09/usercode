#include <iostream>

#include "StorageLibrary.C"

int main() {
    std::cout << "Going to store a point " << endl;
    float point[20]={40.6043,925.422,-2479.47,278.597,939.517,1963.77,-1421.78,347.058,1900.83,140.533,1812.52,1940.4,1287.41,509.565,1579.93,1919.38,138.664,1589.35,1347.89,2464.97};
    
    int isSLHAValid=1;
    float HiggsMass=123;
    
    int Satisfies_bsgamma=1;
    int Satisfies_gm2=1;
    int Satisfies_Bsmumu=1;
    int Satisfies_Higgs=1;
    int Satisfies_SUSY=1;
    int Satisfies_LSP=1;
    int Satisfies_Top=1;
    int Satisfies_Mbmb=1;
    int Satisfies_Alphas=1;
    int Satisfies_BsGamma=1;
    int Satisfies_RBtaunu=1;
    int Satisfies_BsMuMu=1;
    
    float Compatibility=0.0274;
    
    
    
    
    StorePoint(point,isSLHAValid,HiggsMass,Satisfies_bsgamma,Satisfies_gm2,Satisfies_Bsmumu,Satisfies_Higgs,Satisfies_SUSY,Satisfies_LSP,Satisfies_Top,Satisfies_Mbmb,Satisfies_Alphas,Satisfies_BsGamma,Satisfies_RBtaunu,Satisfies_BsMuMu,Compatibility);
}
