#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TText.h>
#include <TGraph.h>
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <THStack.h>
#include <TColor.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TError.h>
#include <TVirtualPad.h>
#include <TGraphAsymmErrors.h>
#include <TPaveText.h>
#include <TRandom.h>
#ifndef Verbosity
#define Verbosity 0
#endif

/*
#ifndef SampleClassLoaded
#include "SampleClass.C"
#endif
*/
#define GeneralToolBoxLoaded

using namespace std;

namespace PlottingSetup {
    string cbafbasedir="";
    string basedirectory="";
}

bool dopng=false;
bool doC=false;
bool doeps=false;
bool dopdf=false;
bool doroot=false;
float generaltoolboxlumi;

TLegend* make_legend(string title,float posx,float posy, bool drawleg);
TText* write_title(bool, string);
TText* write_title_low(string title);

TText* write_text(float xpos,float ypos,string title);
float computeRatioError(float a, float da, float b, float db);
float computeProductError(float a, float da, float b, float db);
TGraphAsymmErrors *histRatio(TH1F *h1,TH1F *h2, int id, vector<float>binning);
void setlumi(float l);
void DrawPrelim(float writelumi);
void CompleteSave(TCanvas *can, string filename, bool feedback);
void CompleteSave(TVirtualPad *can, string filename, bool feedback);
void write_warning(string funcname, string text);
void write_error(string funcname, string text);
void write_info(string funcname, string text);
string get_directory();
bool Contains(string wholestring, string findme);
//-------------------------------------------------------------------------------------

template<typename U>
inline bool isanyinf(U value)
{
return !(value >= std::numeric_limits<U>::min() && value <=
std::numeric_limits<U>::max());
}

stringstream warningsummary;
stringstream infosummary;
stringstream errorsummary;

template<class A>
string any2string(const A& a){
    ostringstream out;
    out << a;
    return out.str();
}

void do_png(bool s) { dopng=s;}
void do_eps(bool s) { doeps=s;}
void do_C(bool s)   { doC=s;}
void do_pdf(bool s) { dopdf=s;}
void do_root(bool s){ doroot=s;}

string topdir(string child) {
    string tempdirectory=child;
    if(tempdirectory.substr(tempdirectory.length()-1,1)=="/") tempdirectory=tempdirectory.substr(0,tempdirectory.length());
    //we now have a directory without the trailing slash so we can just look for the last non-slash character :-) 
    for(int ichar=tempdirectory.length()-1;ichar>=0;ichar--) {
      if(tempdirectory.substr(ichar,1)=="/") {
	return tempdirectory.substr(0,ichar);
      }
    }
}

template < typename CHAR_TYPE,
           typename TRAITS_TYPE = std::char_traits<CHAR_TYPE> >

struct basic_teebuf : public std::basic_streambuf< CHAR_TYPE, TRAITS_TYPE >
{
    typedef std::basic_streambuf< CHAR_TYPE, TRAITS_TYPE > streambuf_type ;
    typedef typename TRAITS_TYPE::int_type int_type ;

    basic_teebuf( streambuf_type* buff_a, streambuf_type* buff_b )
            : first(buff_a), second(buff_b) {}

    protected:
        virtual int_type overflow( int_type c )
        {
            const int_type eof = TRAITS_TYPE::eof() ;
            if( TRAITS_TYPE::eq_int_type( c, eof ) )
                return TRAITS_TYPE::not_eof(c) ;
            else
            {
                const CHAR_TYPE ch = TRAITS_TYPE::to_char_type(c) ;
                if( TRAITS_TYPE::eq_int_type( first->sputc(ch), eof ) ||
                    TRAITS_TYPE::eq_int_type( second->sputc(ch), eof ) )
                        return eof ;
                else return c ;
            }
        }

        virtual int sync()
        { return !first->pubsync() && !second->pubsync() ? 0 : -1 ; }

    private:
        streambuf_type* first ;
        streambuf_type* second ;
};

template < typename CHAR_TYPE,
           typename TRAITS_TYPE = std::char_traits<CHAR_TYPE> >
struct basic_teestream : public std::basic_ostream< CHAR_TYPE, TRAITS_TYPE >
{
    typedef std::basic_ostream< CHAR_TYPE, TRAITS_TYPE > stream_type ;
    typedef basic_teebuf< CHAR_TYPE, TRAITS_TYPE > streambuff_type ;

    basic_teestream( stream_type& first, stream_type& second )
         : stream_type( &stmbuf), stmbuf( first.rdbuf(), second.rdbuf() ) {}

    ~basic_teestream() { stmbuf.pubsync() ; }

    private: streambuff_type stmbuf ;
};

typedef basic_teebuf<char> teebuf ;
typedef basic_teestream<char> teestream ;

std::ofstream file("LOG.txt",ios::app) ;
std::ofstream texfile("Tex.txt") ;
std::ofstream efile("LOGerr.txt",ios::app) ;
teestream dout( file, std::cout ) ; // double out
teestream eout( efile, std::cout ) ; // double out (errors)

template < typename CHAR_TYPE,
           typename TRAITS_TYPE = std::char_traits<CHAR_TYPE> >

struct basic_tripbuf : public std::basic_streambuf< CHAR_TYPE, TRAITS_TYPE >
{
    typedef std::basic_streambuf< CHAR_TYPE, TRAITS_TYPE > streambuf_type ;
    typedef typename TRAITS_TYPE::int_type int_type ;

    basic_tripbuf( streambuf_type* buff_a, streambuf_type* buff_b, streambuf_type* buff_c )
            : first(buff_a), second(buff_b), third(buff_c) {}

    protected:
        virtual int_type overflow( int_type d )
        {
            const int_type eof = TRAITS_TYPE::eof() ;
            if( TRAITS_TYPE::eq_int_type( d, eof ) )
                return TRAITS_TYPE::not_eof(d) ;
            else
            {
                const CHAR_TYPE ch = TRAITS_TYPE::to_char_type(d) ;
                if( TRAITS_TYPE::eq_int_type( first->sputc(ch), eof ) ||
                    TRAITS_TYPE::eq_int_type( second->sputc(ch), eof )||
                    TRAITS_TYPE::eq_int_type( third->sputc(ch), eof ) )
                        return eof ;
                else return d ;
            }
        }

        virtual int sync()
        { return !first->pubsync() && !second->pubsync() && !third->pubsync() ? 0 : -1 ; }

    private:
        streambuf_type* first ;
        streambuf_type* second ;
        streambuf_type* third ;
};

template < typename CHAR_TYPE,
           typename TRAITS_TYPE = std::char_traits<CHAR_TYPE> >
struct basic_tripstream : public std::basic_ostream< CHAR_TYPE, TRAITS_TYPE >
{
    typedef std::basic_ostream< CHAR_TYPE, TRAITS_TYPE > stream_type ;
    typedef basic_tripbuf< CHAR_TYPE, TRAITS_TYPE > streambuff_type ;

    basic_tripstream( stream_type& first, stream_type& second, stream_type& third )
         : stream_type( &stmbuf), stmbuf( first.rdbuf(), second.rdbuf(), third.rdbuf() ) {}

    ~basic_tripstream() { stmbuf.pubsync() ; }

    private: streambuff_type stmbuf ;
};

//typedef basic_tripbuf<char> teebuf ;
typedef basic_tripstream<char> tripplestream ;

tripplestream tout( file, texfile , std::cout ) ; // tripple out

void ensure_directory_exists(string thisdirectory) {
  struct stat st;
  if(stat(thisdirectory.c_str(),&st) == 0) {
    if(Verbosity>0) dout << "Directory " << thisdirectory << " exists!" << endl;
  }
  else {
    if(Verbosity>0) dout << "Directory " << thisdirectory << " does not exist. Need to create it!" << endl;
    ensure_directory_exists(topdir(thisdirectory));
    if (mkdir(thisdirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            if(Verbosity>0) dout << "Created the directory " << thisdirectory << endl;
  }
}

void initialize_log() {
  dout << "____________________________________________________________" << endl;
  dout << endl;
  dout << "                                                            " << endl;
  dout << "          JJJJJJJJJJJZZZZZZZZZZZZZZZZZZZBBBBBBBBBBBBBBBBB   " << endl;
  dout << "          J:::::::::JZ:::::::::::::::::ZB::::::::::::::::B  " << endl;
  dout << "          J:::::::::JZ:::::::::::::::::ZB::::::BBBBBB:::::B " << endl;
  dout << "          JJ:::::::JJZ:::ZZZZZZZZ:::::Z BB:::::B     B:::::B" << endl;
  dout << "            J:::::J  ZZZZZ     Z:::::Z    B::::B     B:::::B" << endl;
  dout << "            J:::::J          Z:::::Z      B::::B     B:::::B" << endl;
  dout << "            J:::::J         Z:::::Z       B::::BBBBBB:::::B " << endl;
  dout << "            J:::::j        Z:::::Z        B:::::::::::::BB  " << endl;
  dout << "            J:::::J       Z:::::Z         B::::BBBBBB:::::B " << endl;
  dout << "JJJJJJJ     J:::::J      Z:::::Z          B::::B     B:::::B" << endl;
  dout << "J:::::J     J:::::J     Z:::::Z           B::::B     B:::::B" << endl;
  dout << "J::::::J   J::::::J  ZZZ:::::Z     ZZZZZ  B::::B     B:::::B" << endl;
  dout << "J:::::::JJJ:::::::J  Z::::::ZZZZZZZZ:::ZBB:::::BBBBBB::::::B" << endl;
  dout << " JJ:::::::::::::JJ   Z:::::::::::::::::ZB:::::::::::::::::B " << endl;
  dout << "   JJ:::::::::JJ     Z:::::::::::::::::ZB::::::::::::::::B  " << endl;
  dout << "     JJJJJJJJJ       ZZZZZZZZZZZZZZZZZZZBBBBBBBBBBBBBBBBB   " << endl;
  dout << "                                                            " << endl;
  dout << endl << endl;
  dout << "____________________________________________________________" << endl;
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  dout << "       Analysis run on " << asctime (localtime ( &rawtime ));
  dout << "____________________________________________________________" << endl;
  dout << " Results saved in : " << get_directory() << endl << endl;
}

void extract_cbaf_dir(string curpath) {
  int position=curpath.find("/Plotting");
  if(position<0) position=curpath.find("/DistributedModelCalculations");
  if(position<0) position=curpath.find("/various_assignments");
  PlottingSetup::cbafbasedir=curpath.substr(0,position);
}

void set_directory(string basedir="") {
  if(basedir.substr(0,1)=="/") basedir=basedir.substr(1,basedir.length()-1);
  if(basedir.substr(basedir.length()-1,1)!="/") basedir+="/";
  char currentpath[1024];
  char *path = getcwd(currentpath,1024);
  PlottingSetup::basedirectory=(string)currentpath+"/Plots/"+basedir;
  ensure_directory_exists(PlottingSetup::basedirectory);
  initialize_log();
  extract_cbaf_dir(currentpath);
}

string get_directory() {
  return PlottingSetup::basedirectory;
}

string extract_directory(string savethis) {
  bool foundslash=false;
  int position=savethis.length();
  while(!foundslash&&position>0) {
    position--;
    if(savethis.substr(position,1)=="/") foundslash=true;
  }
  if(position>0) return savethis.substr(0,position+1);
  else return "";
}

string extract_root_dir(string name) {
    int position = -1;
    if(name.substr(0,1)=="/") name=name.substr(1,name.length()-1);
    for(int ipos=0;ipos<name.length();ipos++) {
        if(name.substr(ipos,1)=="/") position=ipos;
    }
    if(position==-1) return "";
    return name.substr(0,position);
}

string extract_root_filename(string name) {
    int position = -1;
    if(name.substr(0,1)=="/") name=name.substr(1,name.length()-1);
    for(int ipos=0;ipos<name.length();ipos++) {
        if(name.substr(ipos,1)=="/") position=ipos;
    }
    return name.substr(position+1,name.length()-position-1);
}

void SaveToRoot(TCanvas *can, string name) {
    TFile *fout = new TFile((TString(PlottingSetup::basedirectory)+TString("allplots.root")),"UPDATE");
    fout->cd();
    string directory=extract_root_dir(name);
    string filename=extract_root_filename(name);
    if(directory!="") {
      if(fout->GetDirectory(directory.c_str())) {
        fout->cd(directory.c_str());
        can->Write(filename.c_str());
      }else {
        fout->mkdir(directory.c_str());
        fout->cd(directory.c_str());
        can->Write(filename.c_str());
      }
    } else {
      can->Write(filename.c_str());
    }
    fout->cd();
    fout->Close();
}

void SaveToRootPlaceHolder(TCanvas *can, string name) {
  write_warning(__FUNCTION__,"Saving to root file has been deactivated (it works but when opening ROOT complains a bit about filenames which is unelegant)");
  //if you want to activate this feature anyway, you can rename this to SaveToRoot_Placeholder and rename SaveToRoot_REAL to SaveToRoot.
}

void CompleteSave(TCanvas *can, string filename, bool feedback=false, bool redraw=true) {
  //any change you make here should also be done below in the CompleteSave function for virtual pads
  Int_t currlevel=gErrorIgnoreLevel;
  if(!feedback) gErrorIgnoreLevel=1001;
  if(redraw) can->RedrawAxis();
  ensure_directory_exists(extract_directory(PlottingSetup::basedirectory+filename));
  if(dopng) can->SaveAs((PlottingSetup::basedirectory+filename+".png").c_str());
  if(doeps) can->SaveAs((PlottingSetup::basedirectory+filename+".eps").c_str());
  if(dopdf) can->SaveAs((PlottingSetup::basedirectory+filename+".pdf").c_str());
  if(doC) can->SaveAs((PlottingSetup::basedirectory+filename+".C").c_str());
  if(doroot) SaveToRoot(can,filename);
  gErrorIgnoreLevel=currlevel;
  dout << "Saved " << filename << " in all requested formats" << endl;
}

void CompleteSave(TVirtualPad *can, string filename, bool feedback=false, bool redraw=true) {
  Int_t currlevel=gErrorIgnoreLevel;
  if(!feedback) gErrorIgnoreLevel=1001;
  if(redraw) can->RedrawAxis();
  ensure_directory_exists(extract_directory(PlottingSetup::basedirectory+filename));
  if(dopng) can->SaveAs((PlottingSetup::basedirectory+filename+".png").c_str());
  if(doeps) can->SaveAs((PlottingSetup::basedirectory+filename+".eps").c_str());
  if(doC) can->SaveAs((PlottingSetup::basedirectory+filename+".C").c_str());
  gErrorIgnoreLevel=currlevel;
  dout << "Saved " << filename << " in all requested formats" << endl;
}


void setlumi(float l) {
generaltoolboxlumi=l;
}

int write_first_line(vector<vector<string> > &entries) {
  if(entries.size()>0) {
    vector<string> firstline = entries[0];
    int ncolumns=firstline.size();
    int ndividers=ncolumns+1;
    int cellwidth=(int)(((float)(60-ndividers))/(ncolumns));
    dout << " |";
    for(int idiv=0;idiv<ncolumns;idiv++) {
      for(int isig=0;isig<cellwidth;isig++) dout << "-";
      dout << "|";
    }
    dout << endl;
    return ncolumns;
  } else {
    return 0;
  }
}

void write_entry(string entry,int width,int iline=0,int ientry=0) {
  int currwidth=entry.size();
  while(currwidth<width) {
    entry=" "+entry;
    if(entry.size()<width) entry=entry+" ";
    currwidth=entry.size();
  }
  bool do_special=false;
  if(iline==1&&ientry==1) {  dout << "\033[1;32m" << entry << "\033[0m|";do_special=true;}//observed
  if(iline==1&&ientry==2) {  dout << "\033[1;34m" << entry << "\033[0m|";do_special=true;}//predicted (1)
  if(iline==2&&ientry==1) {  dout << "\033[1;34m" << entry << "\033[0m|";do_special=true;}//predicted (1)
  if(iline==2&&ientry==2) {  dout << "\033[1;34m" << entry << "\033[0m|";do_special=true;}//predicted (1)
  if(!do_special) dout << entry << "|";
}

void make_nice_table(vector<vector <string> > &entries) {
  int ncolumns=write_first_line(entries);
  int cellwidth=(int)(((float)(60-(ncolumns+1)))/(ncolumns));
  for(int iline=0;iline<entries.size();iline++) {
    vector<string> currline = entries[iline];
    dout << " |";
    for(int ientry=0;ientry<currline.size();ientry++) {
      write_entry(currline[ientry],cellwidth);
    }
    dout << endl;
    if(iline==0)   write_first_line(entries);
  }
  write_first_line(entries);
}

void make_nice_jzb_table(vector<vector <string> > &entries) {
  int ncolumns=write_first_line(entries);
  int cellwidth=(int)(((float)(60-(ncolumns+1)))/(ncolumns));
  for(int iline=0;iline<entries.size();iline++) {
    vector<string> currline = entries[iline];
    dout << " |";
    for(int ientry=0;ientry<currline.size();ientry++) {
      write_entry(currline[ientry],cellwidth,iline,ientry);
    }
    dout << endl;
    if(iline==0)   write_first_line(entries);
  }
  write_first_line(entries);
}


void write_warning(string funcname, string text) {
  string colid="[1;35m";
  char hostname[1023];
  gethostname(hostname,1023);
  if(!Contains((string)hostname,"t3")) colid="[1;33m";
  eout << endl << endl;
  eout << "\033"<<colid<<"" << "                          _             " << endl;
  eout << "\033"<<colid<<"" << "                         (_)            " << endl;
  eout << "\033"<<colid<<"" << "__      ____ _ _ __ _ __  _ _ __   __ _ " << endl;
  eout << "\033"<<colid<<"" << "\\ \\ /\\ / / _` | '__| '_ \\| | '_ \\ / _` |" << endl;
  eout << "\033"<<colid<<"" << " \\ V  V / (_| | |  | | | | | | | | (_| |" << endl;
  eout << "\033"<<colid<<"" << "  \\_/\\_/ \\__,_|_|  |_| |_|_|_| |_|\\__, |" << endl;
  eout << "\033"<<colid<<"" << "                                   __/ |" << endl;
  eout << "\033"<<colid<<"" << "                                  |___/ " << endl;
  eout << endl;
  eout << "\033"<<colid<<" [" << funcname << "] " << text << " \033[0m" << endl;
  eout << endl << endl;
  warningsummary << "[" << funcname << "] " << text << endl;
}
void write_error(string funcname, string text) {
  eout << endl << endl;
  eout << "\033[1;31m  ___ _ __ _ __ ___  _ __ " << endl;
  eout << "\033[1;31m / _ \\  __|  __/ _ \\| '__|" << endl;
  eout << "\033[1;31m|  __/ |  | | | (_) | |   " << endl;
  eout << "\033[1;31m \\___|_|  |_|  \\___/|_|   " << endl;
  eout << endl;
  eout << "\033[1;31m [" << funcname << "] " << text << " \033[0m" << endl;
  eout << endl << endl;
  errorsummary << "[" << funcname << "] " << text << endl;
}

void write_info(string funcname, string text) {
  dout << endl << endl;
  dout << "\033[1;34m   _____        __        " << endl;
  dout << "\033[1;34m  |_   _|      / _|      " << endl;
  dout << "\033[1;34m    | |  _ __ | |_ ___   " << endl;
  dout << "\033[1;34m    | | | '_ \\|  _/ _ \\  " << endl;
  dout << "\033[1;34m   _| |_| | | | || (_) | " << endl;
  dout << "\033[1;34m  |_____|_| |_|_| \\___/  " << endl;
  dout << endl;
  dout << "\033[1;34m [" << funcname << "] " << text << " \033[0m" << endl;
  dout << endl << endl;
  infosummary << "[" << funcname << "] " << text << endl;
}

TText* write_text(float xpos,float ypos,string title)
{
   TLatex* titlebox = new TLatex (xpos,ypos,title.c_str());
   titlebox->SetNDC(true);
   titlebox->SetTextFont(42);
   titlebox->SetTextSize(0.04);
   titlebox->SetTextAlign(21);
   return titlebox;
}

TText* write_title(string title)
{
   TText* titlebox = write_text(0.5,0.945,title);
   return titlebox;
}

TText* write_cut_on_canvas(string cut) {
//  TLatex *normbox = new TLatex(0.96,0.5,cut.c_str());
  TLatex *normbox = new TLatex(0.96,0.5,"");//currently deactivated
  normbox->SetNDC(true);
  normbox->SetTextFont(42);
  normbox->SetTextSize(0.01);
  normbox->SetTextAlign(21);
  normbox->SetTextAngle(270);
  return normbox;
}

TText* write_title_low(string title)
{
   TText* titlebox = write_text(0.5,0.94,title);
   return titlebox;
}

void DrawPrelim(float writelumi=generaltoolboxlumi,bool isMC=false) {
  string barn="pb";
  if(writelumi>=1000)
  {
    writelumi/=1000;
    barn="fb";
  }
  
  stringstream prelimtext;
  //prelimtext << "CMS Preliminary 2011 , #sqrt{s}= 7 TeV, L= O(1) fb^{-1}"; //temporary replacement
  if(isMC) prelimtext << "CMS MC Simulation , #sqrt{s} = 7 TeV, L_{int} = " << std::setprecision(2) <<writelumi<<" "<<barn<<"^{-1}";
  else prelimtext << "CMS Preliminary, #sqrt{s} = 7 TeV, L_{int} = " << std::setprecision(2) <<writelumi<<" "<<barn<<"^{-1}";
  TPaveText *eventSelectionPaveText = new TPaveText(0.27, 0.93,0.77, 1.0,"blNDC");
  eventSelectionPaveText->SetFillStyle(4000);
  eventSelectionPaveText->SetBorderSize(0.1);
  eventSelectionPaveText->SetFillColor(kWhite);
  eventSelectionPaveText->SetTextFont(42);
  eventSelectionPaveText->SetTextSize(0.042);
  eventSelectionPaveText->AddText(prelimtext.str().c_str());
  eventSelectionPaveText->Draw();
}

void DrawMCPrelim(float writelumi=generaltoolboxlumi) {
  DrawPrelim(writelumi,true);
}

TLegend* make_legend(string title="", float posx=0.6, float posy=0.55, bool drawleg=true)
{
  gStyle->SetTextFont(42);
  TLegend *leg = new TLegend(posx,posy,0.89,0.89);
  if(title!="") leg->SetHeader(title.c_str());
  leg->SetTextFont(42);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);
  leg->SetBorderSize(0);
  leg->SetLineColor(kWhite);
  if(drawleg) DrawPrelim();
  return leg;
}

TLegend* make_legend(bool drawleg, string title) {
  return make_legend(title,0.6,0.55,drawleg);
}

TGraph* make_nice_ratio(int nbins,float binning[],TH1F* histo)
{
  float errorsquared[nbins];
  float errors[nbins];
  float bincontent[nbins];
  for (int i=0;i<nbins;i++) {
    errorsquared[i]=0;
    bincontent[i]=0;
    errors[i]=0;
  }
  float currlimit=binning[0];
  int currtoplim=1;
  for(int ibin=1;ibin<=histo->GetNbinsX();ibin++)
  {
    if(binning[currtoplim]<histo->GetBinCenter(ibin)) currtoplim++;
    dout << "Bin i=" << ibin << " with bin center " << histo->GetBinCenter(ibin) << " contains " << histo->GetBinContent(ibin) << " is within " << binning[currtoplim-1] << " and " << binning[currtoplim] << endl;
    
  }
    
  return 0;
}
  
float statErrorN(float x){return x - 0.5*TMath::ChisquareQuantile(0.3173/2,2*x);}
float statErrorP(float x){return 0.5*TMath::ChisquareQuantile(1-0.3173/2,2*(x+1))-x;}
float lowLimit(float a, float x){return 0.5*TMath::ChisquareQuantile(a,2*x);}
float highLimit(float a,float x){return 0.5*TMath::ChisquareQuantile(1-a,2*(x+1));}

float computeRatioError(float a, float da, float b, float db)
{
    float val=0.;
    float errorSquare = (a/b)*(a/b)*( (da/a)*(da/a) + (db/b)*(db/b));
    val = TMath::Sqrt(errorSquare);
    return val;

}
float computeProductError(float a, float da, float b, float db)
{
    float val=0.;
    float errorSquare = (a*b)*(a*b)*( (da/a)*(da/a) + (db/b)*(db/b));
    val = TMath::Sqrt(errorSquare);
    return val;
}

TGraphAsymmErrors *histRatio(TH1F *h1,TH1F *h2, int id, vector<float>binning, bool precise=false)
{
	int absJZBbinsNumber = binning.size()-1;
	TGraphAsymmErrors* graph = new TGraphAsymmErrors(absJZBbinsNumber);
	
	for(unsigned int i=0;i<absJZBbinsNumber;i++)
	{
		float xCenter=h1->GetBinCenter(i+1);
		float xWidth=(h1->GetBinWidth(i+1))*0.5;
		float nominatorError = h1->GetBinError(i+1);
		float nominator=h1->GetBinContent(i+1);
		float denominatorError=h2->GetBinError(i+1);
		float denominator=h2->GetBinContent(i+1);
		float errorN = 0;
		float errorP = computeRatioError(nominator,nominatorError,denominator,denominatorError);
		if(id==1) // (is data)
		{
			if(!precise) errorP = computeRatioError(nominator,statErrorP(nominator),denominator,statErrorP(denominator));
			else         errorP = computeRatioError(nominator,nominatorError,denominator,denominatorError);
			errorN = errorP; // symmetrize using statErrorP
		} else {
			errorN = computeRatioError(nominator,nominatorError,denominator,denominatorError);
			errorP = errorN;
		}
		if(denominator!=0) {
			graph->SetPoint(i, xCenter, nominator/denominator);
			graph->SetPointError(i,xWidth,xWidth,errorN,errorP);
		}
		else {
			graph->SetPoint(i, xCenter, -999);
			graph->SetPointError(i,xWidth,xWidth,errorN,errorP);
		}
	}	
	return graph;
}

string print_range(float cent, float down, float up) {//note that up&down can be flipped, we don't care, but the central value needs to come 1st!
 float uperr=0,downerr=0;
 if(down>up&&down>cent) uperr=down-cent;
 if(up>down&&up>cent) uperr=up-cent;
 if(down<cent&&down<up) downerr=cent-down;
 if(up<cent&&up<down) downerr=cent-up;
 if(cent>up&&cent>down&&(up!=0&&down!=0)) write_error("print_range"," WATCH OUT: THE CENTRAL VALUE SEEMS TO BE LARGER THAN BOTH UP&DOWN!");
 if(cent<up&&cent<down&&(up!=0&&down!=0)) write_error("print_range"," WATCH OUT: THE CENTRAL VALUE SEEMS TO BE SMALLER THAN BOTH UP&DOWN!");
 stringstream result;
 result << cent << " + " << uperr << " - " << downerr;
 return result.str();
}

void bubbleSort ( int arr [ ], int size, int order [ ]) // nice way to sort an array (called arr) which is currently in a random order (indices in (order")
{
   int last = size - 2;
   int isChanged = 1;

   while ( last >= 0 && isChanged )
   {
           isChanged = 0;
           for ( int k = 0; k <= last; k++ )
               if ( arr[k] > arr[k+1] )
               {
                   swap ( arr[k], arr[k+1] );
                   isChanged = 1;
                   int bkp=order[k];
		   order[k]=order[k+1];
		   order[k+1]=bkp;
               }
           last--;
   }
}

void swapvec(vector<float> &vec,int j, int k) {
float bkp=vec[j];
vec[j]=vec[k];
vec[k]=bkp;
}

void bubbleSort ( vector<float> &arr , vector<int> &order) // nice way to sort an array (called arr) which is currently in a random order (indices in (order")
{
   int last = arr.size() - 2;
   int isChanged = 1;

   while ( last >= 0 && isChanged )
   {
           isChanged = 0;
           for ( int k = 0; k <= last; k++ )
               if ( arr[k] > arr[k+1] )
               {
                   swapvec (arr,k,k+1);
                   isChanged = 1;
                   int bkp=order[k];
		   order[k]=order[k+1];
		   order[k+1]=bkp;
               }
           last--;
   }
}

int numerichistoname=0;
bool givingnumber=false;
string GetNumericHistoName() {
  while(givingnumber) sleep(1);
  givingnumber=true;
  stringstream b;
  b << "h_" << numerichistoname;
  numerichistoname++;
  givingnumber=false;
  return b.str();
}

//********************** BELOW : CUT INTERPRETATION **************************//
void splitupcut(string incut, vector<string> &partvector)
{
    //idea: go thru the string called incut; if a parantheses is opened, then the cut cannot be split up until the parantheses is closed.
    //ok anyway screw the parantheses.
    int paranthesis_open=0;
    int substr_start=0;
    string currchar="";
    for (int ichar=0;ichar<incut.length();ichar++)
    {
        currchar=incut.substr(ichar,1);
//        if(currchar=="(") paranthesis_open++;
//        if(currchar==")") paranthesis_open--;
        if(currchar=="&"&&incut.substr(ichar+1,1)=="&"&&paranthesis_open==0) {
            partvector.push_back(incut.substr(substr_start,ichar-substr_start));
            substr_start=ichar+2;
        }
    }
    partvector.push_back(incut.substr(substr_start,incut.length()-substr_start));
    if(Verbosity>1) {
        dout << "[ splitupcut() ] : The cut vector now contains the following elements: "<< endl;
        for (int ipart=0;ipart<partvector.size();ipart++)
        {
            dout << "     - " << partvector[ipart] << endl;
        }
    }
}

int atleastvalue(string expression, int &morethanlessthan) // takes in an expression such as ">2" or ">=3" and returns e.g. 3 (in both examples) 
{
  int retval=0;
  if(expression.substr(0,1)==">"&&expression.substr(1,1)=="=") {
//    dout << "The expression " << expression << " is saying that we have at least " << atoi(expression.substr(2,1).c_str()) << " jets" << endl;
    morethanlessthan=1;
    return atoi(expression.substr(2,1).c_str());
  }
  if(expression.substr(0,1)=="="&&expression.substr(1,1)=="=") {
//    dout << "The expression " << expression << " is saying that we have at least " << atoi(expression.substr(1,1).c_str())+1 << " jets" << endl;
    morethanlessthan=0;
    return atoi(expression.substr(1,1).c_str());
  }
  if(expression.substr(0,1)=="<"&&expression.substr(1,1)=="=") {
//    dout << "The expression " << expression << " is saying that we have at least " << atoi(expression.substr(1,1).c_str())+1 << " jets" << endl;
    morethanlessthan=-1;
    return 1+atoi(expression.substr(1,1).c_str());
  }
  if(expression.substr(0,1)==">") {
//    dout << "The expression " << expression << " is saying that we have at least " << atoi(expression.substr(2,1).c_str()) << " jets" << endl;
    morethanlessthan=1;
    return 1+atoi(expression.substr(2,1).c_str());
  }
  if(expression.substr(0,1)=="<"&&expression.substr(1,1)=="=") {
//    dout << "The expression " << expression << " is saying that we have at least " << atoi(expression.substr(2,1).c_str()) << " jets" << endl;
    morethanlessthan=-1;
    return 1+atoi(expression.substr(2,1).c_str());
  }
}

int do_jet_cut(string incut, int *nJets) {
  string expression=(incut.substr(12,incut.size()-12));
  dout << "Going to analyze the jet cut : " << expression << " with 0,1 being " << expression.substr(0,1) << " and 1,1 being " << expression.substr(1,1) << endl;
  if(expression.substr(0,1)=="<"&&expression.substr(1,1)=="=") {
    int nJet=atoi(expression.substr(2,1).c_str());
    for(int i=nJet+1;i<20;i++) nJets[i]=0;
    dout << "Is of type <=" << endl;
    return 0;
  }
  if(expression.substr(0,1)=="="&&expression.substr(1,1)=="=") {
    int nJet=atoi(expression.substr(2,1).c_str());
    for(int i=0;i<20&&i!=nJet;i++) nJets[i]=0;
    dout << "Is of type ==" << endl;
    return 0;
  }
  if(expression.substr(0,1)==">"&&expression.substr(1,1)=="=") {
    int nJet=atoi(expression.substr(2,1).c_str());
    for(int i=0;i<nJet&&i!=nJet;i++) nJets[i]=0;
    dout << "Is of type >=" << endl;
    return 0;
  }
  if(expression.substr(0,1)=="<") {
    int nJet=atoi(expression.substr(1,1).c_str());
    for(int i=nJet;i<20;i++) nJets[i]=0;
    dout << "Is of type <" << endl;
    return 0;
  }
  if(expression.substr(0,1)==">") {
    int nJet=atoi(expression.substr(1,1).c_str());
    for(int i=0;i<nJet+1&&i!=nJet;i++) nJets[i]=0;
    dout << "Is of type >" << endl;
    return 0;
  }
}

string interpret_cut(string incut, bool &isJetCut, int *permittednJets)
{
//    isJetCut=false;nJets=-1;
    if(incut=="()") return "";
    while(incut.substr(0,1)=="(") incut=incut.substr(1,incut.length()-1);
    while(incut.length()>0&&incut.substr(incut.length()-1,1)==")") incut=incut.substr(0,incut.length()-1);
//    if(incut.substr(0,1)=="("&&incut.substr(incut.length()-1,1)==")") incut=incut.substr(1,incut.length()-2); //this is to make (cut) to cut.
   
    if(Verbosity>0) {
        dout << "Now interpreting cut " << incut << endl;
    }
    /*
    if(incut=="ch1*ch2<0") return "OS";
    if(incut=="id1==id2") return "SF";
    if(incut=="id1!=id2") return "OF";
    */
    if(incut=="ch1*ch2<0") return "";
    if(incut=="(mll>55&&mll<70)||(mll>112&&mll<160)") return "SB";
    if(incut=="(mll>61&&mll<70)||(mll>112&&mll<190)") return "SB'";
    if(incut=="id1==id2") return "";
    if(incut=="id1!=id2") return "";
    if(incut=="mll>2") return "";

    if(incut=="mll>0") return ""; // my typical "fake cut"

    if(incut=="passed_triggers||!is_data") return "Triggers";
    if(incut=="pfjzb[0]>-998") return "";

   
    if(incut=="id1==0") return "ee";
    if(incut=="id1==1") return "#mu#mu";
    if(incut=="abs(mll-91.2)<20") return "|m_{l^{+}l^{-}}-m_{Z}|<20";
    if(incut=="pfJetGoodID[0]") return "";
    if(incut=="pfJetGoodID[1]") return "";
    if((int)incut.find("pfJetGoodNum")>-1) {
        //do_jet_cut(incut,permittednJets);
	stringstream result;
        result << "nJets" << incut.substr(12,incut.size()-12);
/*        dout << "Dealing with a jet cut: " << incut << endl;
        stringstream result;
        result << "nJets" << incut.substr(12,incut.size()-12);
	isJetCut=true;
	if(exactjetcut(incut,nJets))
//	nJets=atleastvalue((incut.substr(12,incut.size()-12)),morethanlessthan);
        return result.str();*/
	return result.str();
    }
    return incut;
}

string interpret_nJet_range(int *nJets) {
  for (int i=0;i<20;i++) dout << i << " : " << nJets[i] << endl;
  return "hello";
}

string interpret_cuts(vector<string> &cutparts)
{
    stringstream nicecut;
    int nJets;
    bool isJetCut;
    int finalJetCut=-1;
    int permittednJets[20];
    for(int ijet=0;ijet<20;ijet++) permittednJets[ijet]=1;
    int morethanlessthan=0;//-1: less than, 0: exactly, 1: more than
    for(int icut=0;icut<cutparts.size();icut++)
    {
        if(icut==0) nicecut<<interpret_cut(cutparts[icut],isJetCut,permittednJets);
        else {
          string nice_this_cut = interpret_cut(cutparts[icut],isJetCut,permittednJets);//blublu
          if(nice_this_cut.length()>0&&nicecut.str().length()>0) {
	    if(!isJetCut) nicecut<<"&&"<<nice_this_cut;
	    else {
	      if(nJets>finalJetCut) finalJetCut=nJets;
	  }
	  }
          if(nice_this_cut.length()>0&&nicecut.str().length()==0) {
	    if(!isJetCut) {
	      nicecut<<nice_this_cut;
	    }
	    else {
	      if(nJets>finalJetCut) finalJetCut=nJets;
	    }
	  }
        }
    }
    if(finalJetCut>-1) {
      if(nicecut.str().length()==0) {
	nicecut << "nJets#geq" << finalJetCut;
      }
      else
      {
	nicecut << "&&nJets#geq " << finalJetCut;
      }
    }
    
//    dout << "The nJet allowed range is given by: " << interpret_nJet_range(permittednJets) << endl;
    
    return nicecut.str();
}

string decipher_cut(TCut originalcut,TCut ignorethispart)
{
    string incut=(const char*)originalcut;
    string ignore=(const char*)ignorethispart;
    
    if(ignore.length()>0 && incut.find(ignore)!=string::npos) incut=incut.replace(incut.find(ignore),ignore.length(),"");
    
    vector<string>cutparts;
    splitupcut(incut,cutparts);
    string write_cut=interpret_cuts(cutparts);
    return write_cut;
}

//********************** ABOVE : CUT INTERPRETATION **************************//

Double_t GausRandom(Double_t mu, Double_t sigma) {
  return gRandom->Gaus(mu,sigma);// real deal
  //return mu;//debugging : no smearing.
}

int functionalhistocounter=0;
TH1F * makehistofromfunction(TF1 *f1,TH1F *model) {
  TH1F *histo = (TH1F*)model->Clone();
  functionalhistocounter++;
  stringstream histoname;
  histoname << "histo_based_on_function_" << f1->GetName() << "__"<<functionalhistocounter;
  histo->SetTitle(histoname.str().c_str());
  histo->SetName(histoname.str().c_str());
  int nbins=histo->GetNbinsX();
  float low=histo->GetBinLowEdge(1);
  float hi=histo->GetBinLowEdge(histo->GetNbinsX())+histo->GetBinWidth(histo->GetNbinsX());
  
  for(int i=0;i<=nbins;i++) {
    histo->SetBinContent(i,(f1->Integral(histo->GetBinLowEdge(i),histo->GetBinLowEdge(i)+histo->GetBinWidth(i)))/histo->GetBinWidth(i));
    histo->SetBinError(i,TMath::Sqrt(histo->GetBinContent(i)));
  }

  return histo;
}

float hintegral(TH1 *histo, float low, float high) {
  float sum=0;
  for(int i=1;i<histo->GetNbinsX();i++) {
    if((histo->GetBinLowEdge(i)>=low)&&(histo->GetBinLowEdge(i)+histo->GetBinWidth(i))<=high) sum+=histo->GetBinContent(i);
    //now on to the less clear cases!
    if(histo->GetBinLowEdge(i)<low&&(histo->GetBinLowEdge(i)+histo->GetBinWidth(i))>low) {
      //need to consider this case still ... the bin is kind of in range but not sooooo much.
    }
    if(histo->GetBinLowEdge(i)<high&&(histo->GetBinLowEdge(i)+histo->GetBinWidth(i))>high) {
      //need to consider this case still ... the bin is kind of in range but not sooooo much.
    }
    
  }
  return sum;
}

string newjzbexpression(string oldexpression,float shift) {
  stringstream ss;
  if(shift>0) ss<<"("<<oldexpression<<"+"<<shift<<")";
  if(shift<0) ss<<"("<<oldexpression<<shift<<")";
  if(shift==0) ss<<oldexpression;
  return ss.str();
}
  
float Round(float num, unsigned int dig)
{
    num *= pow(10, dig);
    if (num >= 0)
        num = floor(num + 0.5);
    else
        num = ceil(num - 0.5);
    num/= pow(10, dig);
    return num;
}

float SigDig(float num, int digits) {
  //produces a number with only the given number of significant digits
  
}

// The two functions below are for distributed processing

int get_job_number(float ipoint, float Npoints,float Njobs) {
  float pointposition=(ipoint/Npoints);
  int njob=floor(pointposition*Njobs);
  if(njob>=Njobs) njob--;
//  cout << "Looking at point " << ipoint << " out of " << Npoints << " which is at position " << pointposition << " corresponding to  " << pointposition*Njobs << " --> JOB " << njob << endl;
  return njob;
}
  

bool do_this_point(int ipoint, int Npoints, int jobnumber, int Njobs) {
  if(get_job_number(ipoint,Npoints,Njobs)==jobnumber) return true;
  return false;
}

Double_t  DoIntegral(TH1F *histo, Int_t binx1, Int_t binx2, Int_t biny1, Int_t biny2, Int_t binz1, Int_t binz2, Double_t & error ,
                          Option_t *option, Bool_t doError)
{
   // internal function compute integral and optionally the error  between the limits
   // specified by the bin number values working for all histograms (1D, 2D and 3D)

   Int_t nbinsx = histo->GetNbinsX();
   if (binx1 < 0) binx1 = 0;
   if (binx2 > nbinsx+1 || binx2 < binx1) binx2 = nbinsx+1;
   if (histo->GetDimension() > 1) {
      Int_t nbinsy = histo->GetNbinsY();
      if (biny1 < 0) biny1 = 0;
      if (biny2 > nbinsy+1 || biny2 < biny1) biny2 = nbinsy+1;
   } else {
      biny1 = 0; biny2 = 0;
   }
   if (histo->GetDimension() > 2) {
      Int_t nbinsz = histo->GetNbinsZ();
      if (binz1 < 0) binz1 = 0;
      if (binz2 > nbinsz+1 || binz2 < binz1) binz2 = nbinsz+1;
   } else {
      binz1 = 0; binz2 = 0;
   }

   //   - Loop on bins in specified range
   TString opt = option;
   opt.ToLower();
   Bool_t width   = kFALSE;
   if (opt.Contains("width")) width = kTRUE;


   Double_t dx = 1.;
   Double_t dy = 1.;
   Double_t dz = 1.;
   Double_t integral = 0;
   Double_t igerr2 = 0;
   for (Int_t binx = binx1; binx <= binx2; ++binx) {
      if (width) dx = histo->GetXaxis()->GetBinWidth(binx);
      for (Int_t biny = biny1; biny <= biny2; ++biny) {
         if (width) dy = histo->GetYaxis()->GetBinWidth(biny);
         for (Int_t binz = binz1; binz <= binz2; ++binz) {
            if (width) dz = histo->GetZaxis()->GetBinWidth(binz);
            Int_t bin  = histo->GetBin(binx, biny, binz);
            if (width) integral += histo->GetBinContent(bin)*dx*dy*dz;
            else       integral += histo->GetBinContent(bin);
            if (doError) {
               if (width)  igerr2 += histo->GetBinError(bin)*histo->GetBinError(bin)*dx*dx*dy*dy*dz*dz;
               else        igerr2 += histo->GetBinError(bin)*histo->GetBinError(bin);
            }
         }
      }
   }

   if (doError) error = TMath::Sqrt(igerr2);
   return integral;
}

Double_t IntegralAndError(TH1F *histo, Int_t binx1, Int_t binx2, Double_t & error, Option_t *option)
{
   //Return integral of bin contents in range [binx1,binx2] and its error
   // By default the integral is computed as the sum of bin contents in the range.
   // if option "width" is specified, the integral is the sum of
   // the bin contents multiplied by the bin width in x.
   // the error is computed using error propagation from the bin errors assumming that
   // all the bins are uncorrelated
   return DoIntegral(histo,binx1,binx2,0,-1,0,-1,error,option,kTRUE);
}

void print_usage() {
  cout << "Some distributed model calculations call Create_All_Plots.exec with the argument \"1\" to calculate some basic quantities, such as the peak position in MC and data, observed and predicted, and so on. If you want to test this, you can just run this program with argument 1 yourself :-) " << endl;
}


string format_number( int value )
{
    if( value == 0 ) return "00";
    if( value < 10 ) return "0"+any2string(value);
    return any2string(value);
}

string seconds_to_time(int seconds) {
    const static unsigned int SECONDS_IN_AN_HOUR    =	3600;
    const static unsigned int SECONDS_IN_A_MINUTE   =	60;
    stringstream answer;
    if( seconds > 0 )
    {
	answer << format_number( (unsigned int)(seconds / SECONDS_IN_AN_HOUR) ) << ":";
	answer << format_number( (unsigned int)((seconds % SECONDS_IN_AN_HOUR) / SECONDS_IN_A_MINUTE) ) << ":";
	answer << format_number( (unsigned int)((seconds % SECONDS_IN_AN_HOUR) % (SECONDS_IN_A_MINUTE)) );
    }
    else
    {
	answer << "00:00:00";
    }
    return answer.str();
}
  
bool Contains(string wholestring, string findme) {
  if((int)wholestring.find(findme)>-1) return true;
  else return false;
}

//////////////////////////////////////////////////////////////////////////////
//
// http://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-run-time-in-c
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0

/* usage: 
double vm2, rss2;
process_mem_usage(vm2, rss2);
cout << "Memory usage: VM: " << vm << "; RSS: " << rss << endl;
*/

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}

void flag_this_change(string function, int line, int checked=0) {
  stringstream peakmodificationwarning;
  if(checked==0) peakmodificationwarning  << "There's been a change on line " << line << " in function " << function << " that affects the functionality you're using. If you've checked that it works well please change the function call to flag_this_change(..,..,true) so this will only be an info instead of a warning :-) ";
  if(checked==1) peakmodificationwarning << "There's been a change on line " << line << " in function " << function << " that affects the functionality you're using. This modification has already been checked. Please produce the corresponding plot manually and then mark this as done (i.e. flag_this_change(..,..,2)";
  if(checked==2) peakmodificationwarning << "Xchecked: There's been a change on line " << line << " in function " << function << " that affects the functionality you're using. This modification has been checked and crosschecked.";
  
  
  if(checked==0) write_warning(function,peakmodificationwarning.str());
//  if(checked==1) write_info(function,peakmodificationwarning.str());
  peakmodificationwarning << " This modification has been checked and the changes have been reproduced. Checks completed.";
  if(checked==2) write_info(function,peakmodificationwarning.str());
}


TCanvas* draw_ratio_on_canvas(TH1F *nominator, TH1F *denominator, TVirtualPad *canvas) {
  
  float bottommargin=gStyle->GetPadBottomMargin();
  string canvasname="anyname";
  float canvas_height=gStyle->GetCanvasDefH();
  float canvas_width=gStyle->GetCanvasDefW();
  float ratiospace=0.25;// space the ratio should take up (relative to original pad)
  
  float ratiobottommargin=0.3;
  float ratiotopmargin=0.1;
  
  float xstretchfactor=((1-ratiospace)*(1-gStyle->GetPadTopMargin()))/((1)*ratiospace);
  
  TH1F *ratio = (TH1F*)nominator->Clone(GetNumericHistoName().c_str());
  ratio->Divide(denominator);
  ratio->SetTitle("");
  ratio->GetYaxis()->SetRangeUser(0.5,1.5);
  ratio->GetXaxis()->SetTitle(nominator->GetXaxis()->GetTitle());
  ratio->GetXaxis()->CenterTitle();
  ratio->GetYaxis()->SetTitle("ratio");
  ratio->GetYaxis()->SetTitleOffset(0.4);
  ratio->GetYaxis()->CenterTitle();
  ratio->SetStats(0);
  ratio->GetXaxis()->SetLabelSize(xstretchfactor*ratio->GetXaxis()->GetLabelSize());
  ratio->GetYaxis()->SetLabelSize(xstretchfactor*ratio->GetYaxis()->GetLabelSize());
  ratio->GetXaxis()->SetTitleSize(xstretchfactor*gStyle->GetTitleSize());
  ratio->GetYaxis()->SetTitleSize(xstretchfactor*gStyle->GetTitleSize());
  ratio->GetYaxis()->SetNdivisions(502,false);
  
  TCanvas *main_canvas = new TCanvas("main_canvas","main_canvas",canvas_width,canvas_height*(1+ratiospace));
  TPad *mainpad = new TPad("mainpad","mainpad",0,1-(1.0/(1+ratiospace)),1,1);//top (main) pad
  TPad *bottompad = new TPad("bottompad", "Ratio Pad",0,0,1,(1-(1-bottommargin)/(1+ratiospace))-0.01); //bottom pad
  
  main_canvas->Range(0,0,1,1);
  main_canvas->SetBorderSize(0);
  main_canvas->SetFrameFillColor(0);
  
  mainpad->Draw();
  mainpad->cd();
  mainpad->Range(0,0,1,1);
  mainpad->SetFillColor(kWhite);
  mainpad->SetBorderSize(0);
  mainpad->SetFrameFillColor(0);
  canvas->Range(0,0,1,1);
  canvas->Draw("same");
  mainpad->Modified();
  main_canvas->cd();
  bottompad->SetTopMargin(ratiotopmargin);
  bottompad->SetBottomMargin(ratiobottommargin);
  bottompad->Draw();
  bottompad->cd();
  bottompad->Range(0,0,1,1);
  bottompad->SetFillColor(kWhite);
//  ratio->SetFillColor(TColor::GetColor("#CEECF5"));
  ratio->SetFillColor(TColor::GetColor("#58D3F7"));
  ratio->SetMarkerSize(0);
  ratio->Draw("e4");
  TLine *oneline = new TLine(ratio->GetXaxis()->GetBinLowEdge(1),1,ratio->GetXaxis()->GetBinLowEdge(ratio->GetNbinsX())+ratio->GetXaxis()->GetBinWidth(ratio->GetNbinsX()),1);
  oneline->SetLineStyle(2);
  oneline->SetLineColor(kBlue);
  oneline->Draw("same");
  main_canvas->cd();
  main_canvas->Modified();
  main_canvas->cd();
  main_canvas->SetSelected(main_canvas);
  
  return main_canvas;
}

TH1F* CollapseStack(THStack stack) {
   TH1F *bhist = ((TH1F*)((stack.GetHists())->At(0)));
   TH1F *basehisto = (TH1F*)bhist->Clone("base");
   TIter next(stack.GetHists());
   TH1F *h;
   int counter=0;
   while ((h=(TH1F*)next())) {
     counter++;
     if(counter==1) continue;
     basehisto->Add(h);
   }
   return basehisto;
}

TCanvas* draw_ratio_on_canvas(TH1F *nominator, THStack denominator, TVirtualPad *canvas) {
  return draw_ratio_on_canvas(nominator, CollapseStack(denominator), canvas);
}

stringstream all_bugs;

void bug_tracker(string function, int line, string description) {
  cout << "\033[1;31m               .-. " << endl;
  cout << "            o   \\     .-. " << endl;
  cout << "               .----.'   \\ " << endl;
  cout << "             .'o)  / `.   o " << endl;
  cout << "            /         | " << endl;
  cout << "            \\_)       /-. " << endl;
  cout << "              '_.`    \\  \\ " << endl;
  cout << "               `.      |  \\ " << endl;
  cout << "                |       \\ | " << endl;
  cout << "            .--/`-.     / / " << endl;
  cout << "          .'.-/`-. `.  .\\| " << endl;
  cout << "         /.' /`._ `-    '-. " << endl;
  cout << "    ____(|__/`-..`-   '-._ \\ " << endl;
  cout << "   |`------.'-._ `      ||\\ \\ " << endl;
  cout << "   || #   /-.   `   /   || \\| " << endl;
  cout << "   ||   #/   `--'  /  /_::_|)__ " << endl;
  cout << "   `|____|-._.-`  /  ||`--------` " << endl;
  cout << "         \\-.___.` | / || #      | " << endl;
  cout << "          \\       | | ||   #  # | " << endl;
  cout << "          /`.___.'\\ |.`|________| " << endl;
  cout << "          | /`.__.'|'.` " << endl;
  cout << "        __/ \\    __/ \\ " << endl;
  cout << "       /__.-.)  /__.-.) LGB " << endl;
  cout << "" << endl;
  // bug ascii from : http://www.chris.com/ASCII/index.php?art=animals/insects/other
  cout << "There is a bug in " << function << " on line " << line << endl;
  cout << "The bug description is : " << description << endl;
  all_bugs << "There is a bug in " << function << " on line " << line << endl;
  all_bugs << "The bug description is : " << description << " \033[0m" << endl;
}

//TODO: Write a bug summary at the end.
