#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>

#include <TFile.h>
#include <TTree.h>




using namespace std;


int main(int argc, char** argv) {
    if(argc!=3) {
        cerr << "You need to provide 2 arguments: ROOT file, row number" << endl;
        return -1;
    }
    
    string filename=argv[1];
    int row=atof(argv[2]);
    
    cout << "Going to extract point " << row << " from " << filename << endl;
    
    
    
    float mu,MG1,MG2,MG3,Ml1,Ml2,Ml3,Mr1,Mr2,Mr3,Mq1,Mq2,Mq3,Mu1,Mu2,Mu3,Md1,Md2,Md3,At,Ab,Al,MH3,tb;
    
    TFile *file = new TFile(filename.c_str());
    
    if(file->IsZombie()) {
        cerr << "File invalid?" << endl;
        return -1;
    }
    
    TTree *pmssm = (TTree*)file->Get("pmssm");
    if(!pmssm) {
        cerr << "Tree invalid?" << endl;
        return -1;
    }
    
    pmssm->SetBranchAddress("mu",&mu);
    pmssm->SetBranchAddress("MG1",&MG1);
    pmssm->SetBranchAddress("MG2",&MG2);
    pmssm->SetBranchAddress("MG3",&MG3);
    pmssm->SetBranchAddress("Ml1",&Ml1);
    pmssm->SetBranchAddress("Ml2",&Ml2);
    pmssm->SetBranchAddress("Ml3",&Ml3);
    pmssm->SetBranchAddress("Mr1",&Mr1);
    pmssm->SetBranchAddress("Mr2",&Mr2);
    pmssm->SetBranchAddress("Mr3",&Mr3);
    pmssm->SetBranchAddress("Mq1",&Mq1);
    pmssm->SetBranchAddress("Mq2",&Mq2);
    pmssm->SetBranchAddress("Mq3",&Mq3);
    pmssm->SetBranchAddress("Mu1",&Mu1);
    pmssm->SetBranchAddress("Mu2",&Mu2);
    pmssm->SetBranchAddress("Mu3",&Mu3);
    pmssm->SetBranchAddress("Md1",&Md1);
    pmssm->SetBranchAddress("Md2",&Md2);
    pmssm->SetBranchAddress("Md3",&Md3);
    pmssm->SetBranchAddress("At",&At);
    pmssm->SetBranchAddress("Ab",&Ab);
    pmssm->SetBranchAddress("Al",&Al);
    pmssm->SetBranchAddress("MH3",&MH3);
    pmssm->SetBranchAddress("tb",&tb);
    
    pmssm->GetEntry(row);
    
    ofstream GenOut;
    stringstream GenOutName;
    GenOutName << "genPoint_Row" << row << ".dat";
    GenOut.open(GenOutName.str().c_str());
    
    GenOut << "alfEMZ  0.007818061" << endl;
    GenOut << "alfSMZ  0.118245" << endl;
    GenOut << "MZ      91.1876" << endl;
    GenOut << "Mtp     172.426" << endl;
    GenOut << "MbMb    4.21428" << endl;
    GenOut << "mu      " << mu << endl;
    GenOut << "MG1     " << MG1 << endl;
    GenOut << "MG2     " << MG2 << endl;
    GenOut << "MG3     " << MG3 << endl;
    GenOut << "Ml1     " << Ml1 << endl;
    GenOut << "Ml2     " << Ml2 << endl;
    GenOut << "Ml3     " << Ml3 << endl;
    GenOut << "Mr1     " << Mr1 << endl;
    GenOut << "Mr2     " << Mr2 << endl;
    GenOut << "Mr3     " << Mr3 << endl;
    GenOut << "Mq1     " << Mq1 << endl;
    GenOut << "Mq2     " << Mq2 << endl;
    GenOut << "Mq3     " << Mq3 << endl;
    GenOut << "Mu1     " << Mu1 << endl;
    GenOut << "Mu2     " << Mu2 << endl;
    GenOut << "Mu3     " << Mu3 << endl;
    GenOut << "Md1     " << Md1 << endl;
    GenOut << "Md2     " << Md2 << endl;
    GenOut << "Md3     " << Md3 << endl;
    GenOut << "At      " << At << endl;
    GenOut << "Ab      " << Ab << endl;
    GenOut << "Al      " << Al << endl;
    GenOut << "MH3     " << MH3 << endl;
    GenOut << "tb      " << tb << endl;
    GenOut << "Am      0" << endl;

    GenOut.close();
    
    cout << "Have produced gen point called " << GenOutName.str() << endl;
    
    delete pmssm;
    delete file;
                    
    return 0;
}


