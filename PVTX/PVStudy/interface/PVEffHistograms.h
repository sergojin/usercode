#ifndef PVEffHistograms_h
#define PVEffHistograms_h

/**_________________________________________________________________
   class:   PVEffHistograms.h

   author: Geng-yuan Jeng, UC Riverside (Geng-yuan.Jeng@cern.ch)
           Yanyan Gao, Fermilab (ygao@fnal.gov)

   version $Id: PVEffHistograms.h,v 1.1 2010/04/19 16:07:44 yygao Exp $

   ________________________________________________________________**/


#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include <map>

class PVEffHistograms {

 public:

  PVEffHistograms();
  ~PVEffHistograms();

  void Init(TString type, TString suffix="", int nTrkMin_ = 2, int nTrkMax_ = 50);
  void Fill1d(TString name, Double_t x, Double_t weight = 1.);
  void Fill2d(TString name, Double_t x, Double_t y, Double_t weight=1.);
  void Print(TString extension="png", TString tag="");
  void Save();
  void SaveToFile(TString filename="pveffstudy_histo.root");
  void Fit(TString name, Double_t mean);
  void DeleteHisto() {
    for(std::map<TString,TH1D* >::const_iterator ih=h1.begin(); ih!=h1.end(); ++ih){
      TH1 *htemp = ih->second;
      delete htemp;
    }
    for(std::map<TString,TH2D* >::const_iterator ih=h2.begin(); ih!=h2.end(); ++ih){
      TH2 *htemp = ih->second;
      delete htemp;
    }
  };

  TH1D * ReadHisto1D(TString name) {
    return h1[name];
  };

 private:

  std::map<TString, TCanvas*> cv_map;
  std::map<TString, TH1D*> h1;
  std::map<TString, TH2D*> h2;

  TFile *foutfile;		
	
};

#endif
