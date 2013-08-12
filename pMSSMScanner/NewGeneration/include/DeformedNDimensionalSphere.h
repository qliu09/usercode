#include <iostream>
#include <sstream>
#include <math.h>
#include <assert.h>

#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>

#ifndef __DeformedNDimensionalSphere_h__INCLUDED
#define __DeformedNDimensionalSphere_h__INCLUDED

float GetNewPoint(float initial, int iPar, float weight,TRandom3 *randgen);
void GeneratePointAround(const int nDim, float *InitialPars, TRandom3 *randgen);
void GetPointFromDeformedNDimensionalSphere(float *pars, const int nDimensions, TRandom3 *randgen);

#endif