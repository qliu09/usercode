#ifndef __Step1_h__INCLUDED
#define __Step1_h__INCLUDED



void HaveFun();
void HaveFunAfter();
void FinalSteps();
void DefineRange(const int ipar, float &lowrange, float &highrange);
float GetRandomParameter(const int ipar);
float VaryAroundPoint(const float point, const int ipar);
bool exists(const char *filename);
void LoadBestConfig();
void DefineParameters();
void WriteToStorage();
void RunMicroMegas();
int ValidateSLHA();
int CreateSLHAFile();
void StoreThisPoint(std::string problem, float HiggsMass=-1);
bool HasAcceptableHiggsMass(float &hmass);
bool RunSUSYHit();
bool RunSuperIso();
void WriteQuickConfig(float HiggsMass);

#endif