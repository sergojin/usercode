#ifndef PVHistograms_h
#define PVHistograms_h

/**_________________________________________________________________
   class:   PVHistograms.h

   author: Geng-yuan Jeng, UC Riverside (Geng-yuan.Jeng@cern.ch)
           Yanyan Gao, Fermilab (ygao@fnal.gov)

   version $Id: PVHistograms.h,v 1.2 2009/12/04 20:42:39 jengbou Exp $

   ________________________________________________________________**/


#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include <map>

class PVHistograms {

 public:

  PVHistograms();
  ~PVHistograms();

  void Init(TString type, TString suffix1="", TString suffix2="");
  void InitA(TString type, TString suffix1="", TString suffix2="", int min=20, int max=50);
  void Fill1d(TString name, Double_t x, Double_t weight = 1.);
  void Fill2d(TString name, Double_t x, Double_t y, Double_t weight=1.);
  void Print(TString extension="png", TString tag="");
  void Save();
  void SaveToFile(TString filename="pvstudy_histo.root");
  void Fit(TString name, Double_t mean);
  void DeleteHisto() {
    for(std::map<TString,TH1* >::const_iterator ih=h1.begin(); ih!=h1.end(); ++ih){
      TH1 *htemp = ih->second;
      delete htemp;
    }
    for(std::map<TString,TH2* >::const_iterator ih=h2.begin(); ih!=h2.end(); ++ih){
      TH2 *htemp = ih->second;
      delete htemp;
    }
  };

  TH1 * ReadHisto1D(TString name) {
    return h1[name];
  };

 private:

  std::map<TString, TCanvas*> cv_map;
  std::map<TString, TH1*> h1;
  std::map<TString, TH2*> h2;

  TFile *foutfile;		
	
};

#endif
