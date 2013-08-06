#include <iostream>
#include <sstream>
#include <math.h>
#include <assert.h>

#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>


using namespace std;

TRandom3 *randgen;

const int nDimensions=20;
const int nPoints=1000000;

float maxRadius=0.4;

float DeformedSphereParameters[nDimensions];

void GetPointRange(int iPar,float &lowrange, float &highrange) {
    lowrange=(iPar+1)*-10;
    lowrange=0;
    highrange=(iPar+1)*10;
}

float GetNewPoint(float initial, int iPar, float weight) {
  float lowrange,highrange;
  GetPointRange(iPar,lowrange,highrange);
  float newpoint=initial;
  bool IsNewPointOk=false;
  while(!IsNewPointOk) {
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

void GeneratePointAround(float InitialPars[nDimensions]) {
  float weights[nDimensions];
  float weightsum=0;
  for(int i=0;i<nDimensions;i++) {
    weights[i]=randgen->Uniform(0,1);
    weightsum+=weights[i]*weights[i];
  }
  
  weightsum=sqrt(weightsum);
  
  for(int i=0;i<nDimensions;i++) {
    weights[i]=weights[i]/(weightsum);
  }
  
  for(int i=0;i<nDimensions;i++) {
    DeformedSphereParameters[i]=GetNewPoint(InitialPars[i],i,weights[i]);
  }
}
  
int main() {
  cout << "Testing the deformed n-dimensional sphere" << endl;
  
  int FeedBackEvery=100;
  randgen = new TRandom3(0);
  
  float InitialPars[nDimensions];
  for(int i=0;i<nDimensions;i++) InitialPars[i]=i+1.56789;
  
  TFile *f = new TFile("Sphere1.root","RECREATE");
  TTree *sphere = new TTree("sphere","sphere");
  
  for(int i=0;i<nDimensions;i++) {
    stringstream name;
    name << "p" << i;
    sphere->Branch(name.str().c_str(),&DeformedSphereParameters[i],(name.str()+"/F").c_str());
  }
  
  for(int i=0;i<nPoints;i++) {
    GeneratePointAround(InitialPars);
    sphere->Fill();
    if(int(FeedBackEvery*i/((float)nPoints))==float(FeedBackEvery*i/((float)nPoints))) cout << " \rCURRENT STATUS ::::::: " << FeedBackEvery*i/((float)nPoints);
  }
  cout << "\rDone ...                  " << endl;
  
  sphere->Write();
  f->Close();
  
  return 0;
}
