#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

class LimitDroplet{
public:
    std::string LimitName;
    float JZB;
    float upper68;
    float lower68;
    float expected;
    float observed;
    float upper95;
    float lower95;
    void saveDroplet(std::string filename);//save droplet to file
    void readDroplet(std::string filename);//get droplet from file
    void fill_limit_vector(std::string line, std::vector<float> &res);
    LimitDroplet();
};

LimitDroplet::LimitDroplet() {
    JZB=-1;upper68=-1;lower68=-1;expected=-1;observed=-1;upper95=-1;lower95=-1;
}

std::ostream &operator<<(std::ostream &ostr, LimitDroplet v)
{
  return ostr << "JZB>" << v.JZB << std::endl << " 68 % interval is: [ " << v.lower68 << " : " << v.upper68 << " ]" << std::endl << " 95 % interval is: [" << v.lower95 << " : " << v.upper95 << " ]" << std::endl << " Expected limit is " << v.expected << std::endl << " Observed limit is " << v.observed << std::endl;
}

void LimitDroplet::saveDroplet(std::string filename) {
    std::ofstream myreport(filename.c_str());
    myreport<<JZB<<";"<<this->upper68<<";"<<this->lower68<<";"<<this->expected<<";"<<this->observed<<";"<<this->upper95<<";"<<this->lower95<<";";
    myreport.close();
}

void LimitDroplet::fill_limit_vector(std::string line, std::vector<float> &res) {
    if(line=="") return;
    while((int)line.find(";")>-1) {
        int pos=(int)line.find(";");
        std::string thisentry=line.substr(0,pos);
        res.push_back(atof(thisentry.c_str()));
        line=line.substr(pos+1,line.size()-pos);
    }
}
void LimitDroplet::readDroplet(std::string filename) {
    std::ifstream myreport;
    myreport.open(filename.c_str());
    if(!myreport) {
        std::cerr << "Watch out, cannot fill limit droplet using " << filename << std::endl;
    } else {
        std::vector<float> lresults;
        char c[255];
        while(myreport) {
            myreport.getline(c,255);
            this->fill_limit_vector((std::string)c,lresults);
        }
        if(lresults.size()<7) std::cerr << "Watch out, stored limit droplet from " << filename << " does not contain sufficient information" << std::endl;
        else {
            this->JZB=lresults[0];
            this->upper68=lresults[1];
            this->lower68=lresults[2];
            this->expected=lresults[3];
            this->observed=lresults[4];
            this->upper95=lresults[5];
            this->lower95=lresults[6];
        }//result vector is of right size
    }//report exists
    myreport.close();
}

/*int main() {
    std::cout << "Limit Droplet Class !" << std::endl;
    LimitDroplet alpha;
    alpha.fillDroplet("report_999741748.txt");
    return 0;
}
*/