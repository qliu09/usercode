#include <iostream>
#include <sstream>
#include <math.h>
#include <assert.h>

#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>

#include "Step1.h"
#include "DeformedNDimensionalSphere.h"


float maxRadius=0.6;

float GetNewPoint(float initial, int iPar, float weight,TRandom3 *randgen) {
  float lowrange,highrange;
  DefineRange(iPar, lowrange, highrange);
  float newpoint=initial;
  bool IsNewPointOk=false;
  while(!IsNewPointOk) {
      if(initial<lowrange||initial>highrange) {
          std::cerr << " The initial point (" << initial << ") for parameter " << iPar << " is outside the allowed range [" << lowrange << "," << highrange << "] . Will reset initial point to lower boundary of range (" << lowrange << ")" << std::endl;
          initial=lowrange;
      }
    float addon=randgen->Uniform(lowrange,highrange);
    int sign=1;
    if(randgen->Uniform(0,1)>0.5) sign=-1;
    newpoint=initial+maxRadius*weight*addon*sign;
    IsNewPointOk=true;
    if(newpoint>highrange) IsNewPointOk=false;
    if(newpoint<lowrange) IsNewPointOk=false;
  }
  
  return newpoint;
}

void GeneratePointAround(const int nDim, float *InitialPars,TRandom3 *randgen) {
  float weights[nDim];
  float weightsum=0;
  for(int i=0;i<nDim;i++) {
    weights[i]=randgen->Uniform(0,1);
    weightsum+=weights[i]*weights[i];
  }
  
  weightsum=sqrt(weightsum);
  
  for(int i=0;i<nDim;i++) {
    weights[i]=weights[i]/(weightsum);
  }
  
  for(int i=0;i<nDim;i++) {
    InitialPars[i]=GetNewPoint(InitialPars[i],i,weights[i],randgen);
  }
}

void GetPointFromDeformedNDimensionalSphere(float *pars, const int nDimensions, TRandom3 *randgen) {
  GeneratePointAround(nDimensions,pars,randgen);
}
