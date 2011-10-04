#include <string>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

string inttoprocess(int id) {
    if(id==1) return "ng";
    if(id==2) return "ns";
    if(id==3) return "nn";
    if(id==4) return "ll";
    if(id==5) return "sb";
    if(id==6) return "ss";
    if(id==7) return "tb";
    if(id==8) return "bb";
    if(id==9) return "gg";
    if(id==10) return "sg";
    return "ERROR";
}

map <  pair<float, float>, map<string, float>  > getXsec(string xsecfile) {

map <  pair<float, float>, map<string, float>  > xsec;
std::ifstream inFile(xsecfile.c_str());
    if(!inFile) {
        write_error(__FUNCTION__,"SPECIFIED CROSS SECTION FILE WAS NOT FOUND!");
        return xsec;
    }
std::string sLine;
while(std::getline(inFile, sLine))
{
	int init_m0 = sLine.find("m0", 0);
	if(init_m0 == string::npos) continue;
	
	//file structure
	//|(scale=1.0) m0=1000, m1/2=100, tanbeta=3, A0=0, sign(mu)=+ve| 0.224611 | 0.0101205 | 53.1719 | 3.5998e-06 | 0.00592 | 0.0386 | 0.024632 | 0.001203 | 66.1 | 2.3 |
	//| Sub-processes | ng | ns | nn | ll | sb | ss | tb | bb | gg | sg |
    
	int m0=-1, m12=-1, tanb=-1;
	float ng=-1,ns=-1,nn=-1,ll=-1,sb=-1,ss=-1,tb=-1,bb=-1,gg=-1,sg=-1,tngb=-1,a0=-1; 
	sscanf(sLine.c_str(), " |(scale=1.0) m0=%*d, m1/2=%*d, tanbeta=%f, A0=%f, sign(mu)=+ve| %f | %e | %e | %e | %e | %e | %e | %e | %e | %e |",
           &tngb,&a0,&ng,&ns,&nn,&ll,&sb,&ss,&tb,&bb,&gg,&sg);
	sscanf(sLine.c_str(), " |(scale=1.0) m0=%d, m1/2=%d, %*s |",
	       &m0, &m12);
	
	pair< float, float > susyPoint;
	map< string, float > subXsec;
	susyPoint.first = m0;         susyPoint.second = m12;
	subXsec["ng"] = ng;
	subXsec["ns"] = ns;
    subXsec["nn"] = nn;
    subXsec["ll"] = ll;
    subXsec["sb"] = sb;
    subXsec["ss"] = ss;
    subXsec["tb"] = tb;
    subXsec["bb"] = bb;
    subXsec["gg"] = gg;
    subXsec["sg"] = sg;
	xsec[ susyPoint ] = subXsec;
}
inFile.close();
    return xsec;
}

float GetXSecForPointAndChannel(float m0, float m12, map <  pair<float, float>, map<string, float>  > &xsec, string channel) {
    pair< float, float > susyPoint; susyPoint.first = m0; susyPoint.second = m12;
    map< string, float > subXsec = xsec[susyPoint];
    return subXsec[channel];
}

float GetXSecForPointAndChannel(float m0, float m12, map <  pair<float, float>, map<string, float>  > &xsec, int channel) {
    return  GetXSecForPointAndChannel(m0, m12, xsec, inttoprocess(channel));
}



/*
 //This is how you use this implementation (a little example) :

int main() {
    cout << "This is an illustration of how this thing works :-)" << endl;
    map <  pair<float, float>, map<string, float>  >  xsec = getXsec("/scratch/buchmann/C/scale_xsection_nlo1.0_m0_m12_10_0_1v1.txt_INVALID");
    float tester = GetXSecForPointAndChannel(1920,500,xsec,"sg");
    cout << tester << endl;
    return 0;
}

*/