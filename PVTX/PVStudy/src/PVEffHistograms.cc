
/**_________________________________________________________________
   class:   PVEffHistograms.cc

   author: Geng-yuan Jeng, UC Riverside (Geng-yuan.Jeng@cern.ch)
           Yanyan Gao, Fermilab (ygao@fnal.gov)

   version $Id: PVEffHistograms.cc,v 1.3 2010/05/13 09:27:45 yygao Exp $
1
   ________________________________________________________________**/


#include "UserCode/PVStudy/interface/PVEffHistograms.h"
#include <iostream>

//_______________________________________________________________
PVEffHistograms::PVEffHistograms() {

}

//_______________________________________________________________
PVEffHistograms::~PVEffHistograms() {

  this->DeleteHisto();
	
}

//_______________________________________________________________
void PVEffHistograms::Init(TString type, TString suffix, int nTrkMin_, int nTrkMax_) {

  TString methodName = suffix;
  
  if (type == "summary") {
    h1[TString("eff_numer_ntrack"+suffix)] = new TH1D(TString("eff_numer_ntrack"+suffix),"eff ntracks numberator",nTrkMax_-nTrkMin_, nTrkMin_-0.5, nTrkMax_-0.5);
    h1[TString("eff_denom_ntrack"+suffix)] = new TH1D(TString("eff_denom_ntrack"+suffix),"eff ntracks numberator",nTrkMax_-nTrkMin_, nTrkMin_-0.5, nTrkMax_-0.5);
    h1[TString("eff_ntrack"+suffix)] = new TH1D(TString("eff_ntrack"+suffix),"efficiency vs #tracks", nTrkMax_-nTrkMin_, nTrkMin_-0.5, nTrkMax_-0.5);
    h1[TString("fakerate_numer_ntrack"+suffix)] = new TH1D(TString("fakerate_numer_ntrack"+suffix),"fakerate ntracks numberator",nTrkMax_-nTrkMin_, nTrkMin_-0.5, nTrkMax_-0.5);
    h1[TString("fakerate_denom_ntrack"+suffix)] = new TH1D(TString("fakerate_denom_ntrack"+suffix),"fakerate ntracks numberator",nTrkMax_-nTrkMin_, nTrkMin_-0.5, nTrkMax_-0.5);
    h1[TString("fakerate_ntrack"+suffix)] = new TH1D(TString("fakerate_ntrack"+suffix),"fakerate vs #tracks", nTrkMax_-nTrkMin_, nTrkMin_-0.5, nTrkMax_-0.5);
    h1[TString("avgWeight_orgvtx_eff")] = new TH1D(TString("avgWeight_orgvtx_eff"),"Average Track Weight", 200, 0.0, 1.0);
    h1[TString("avgWeight_orgvtx_ineff")] = new TH1D(TString("avgWeight_orgvtx_ineff"),"Average Track Weight", 200, 0.0, 1.0);
    h1[TString("avgWeight_tagvtx_eff")] = new TH1D(TString("avgWeight_tagvtx_eff"),"Average Track Weight", 200, 0.0, 1.0);
    h1[TString("avgWeight_tagvtx_ineff")] = new TH1D(TString("avgWeight_tagvtx_ineff"),"Average Track Weight", 200, 0.0, 1.0);
    h1[TString("avgWeight_probevtx_eff")] = new TH1D(TString("avgWeight_probevtx_eff"),"Average Track Weight", 200, 0.0, 1.0);
    h1[TString("avgWeight_probevtx_ineff")] = new TH1D(TString("avgWeight_probevtx_ineff"),"Average Track Weight", 200, 0.0, 1.0);
    h1[TString("trackweight")] = new TH1D(TString("trackweight"),"Track Weight", 200, 0.0, 1.0);
    h1[TString("deltazSign"+suffix)] = new TH1D(TString("deltazSign_mct"),"deltaZ significance", 200, -10, 10);
  }
  else if (type == "generator") {
    h1["genPart_T"]      = new TH1D("genPart_T","t component of gen particles",300,-0.5,0.5);
    h1["genPart_T"]->GetXaxis()->SetTitle("t (nanosecond)");
    h1["genPart_cT"]     = new TH1D("genPart_cT","ct component of gen particles",300,-150.,150.);
    h1["genPart_cT"]->GetXaxis()->SetTitle("ct (mm)");    
    h1["nsimPV"]         = new TH1D("nsimPV","Num of sim PV",51,-0.5,50.5);  
  }
}

//_______________________________________________________________
void PVEffHistograms::Fill1d(TString name, Double_t x, Double_t weight) {

  h1[name]->Fill(x,weight);
}

//_______________________________________________________________
void PVEffHistograms::Fill2d(TString name, Double_t x, Double_t y, Double_t weight) {

  h2[name]->Fill(x,y,weight);
}

//_______________________________________________________________
void PVEffHistograms::Print(TString extension, TString tag) {

  if ( tag != "" ) tag = "_"+tag;
                
  for(std::map<TString,TCanvas*>::const_iterator icv=cv_map.begin(); icv!=cv_map.end(); ++icv){

    TString tmpname = icv->first;
    TCanvas *acv = icv->second;
    acv->Print(TString(tmpname+tag+"."+extension));
  }

}

//_______________________________________________________________
void PVEffHistograms::Save() {
	
  for(std::map<TString,TH1D* >::const_iterator ih=h1.begin(); ih!=h1.end(); ++ih){
    TH1 *htemp = (TH1D*) ih->second;
    if (htemp->GetEntries() > 0 ) htemp->Write();
  }
  for(std::map<TString,TH2D* >::const_iterator ih=h2.begin(); ih!=h2.end(); ++ih){
    TH2D *htemp = (TH2D*) ih->second;
    if (htemp->GetEntries() > 0 ) htemp->Write();
  }
}

//_______________________________________________________________
void PVEffHistograms::SaveToFile(TString filename) {

  foutfile = new TFile(filename,"RECREATE");
  for(std::map<TString,TH1D* >::const_iterator ih=h1.begin(); ih!=h1.end(); ++ih){
    TH1 *htemp = (TH1D*) ih->second;
    htemp->Write();
  }
  for(std::map<TString,TH2D* >::const_iterator ih=h2.begin(); ih!=h2.end(); ++ih){
    TH2D *htemp = (TH2D*) ih->second;
    htemp->Write();
  }
  
  foutfile->Write();
  foutfile->Close();
  
}
