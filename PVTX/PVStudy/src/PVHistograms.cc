/**_________________________________________________________________
   class:   PVHistograms.cc

   author: Geng-yuan Jeng, UC Riverside (Geng-yuan.Jeng@cern.ch)
           Yanyan Gao, Fermilab (ygao@fnal.gov)
           Sergo Jindariani, Fermilab (ygao@fnal.gov)


   ________________________________________________________________**/


#include "UserCode/PVStudy/interface/PVHistograms.h"
#include <iostream>

//_______________________________________________________________
PVHistograms::PVHistograms() {

}

//_______________________________________________________________
PVHistograms::~PVHistograms() {

  this->DeleteHisto();
	
}

//_______________________________________________________________
void PVHistograms::Init(TString type, TString suffix1, TString suffix2) {

  TString methodName = suffix1;
  if (suffix2 != "") suffix1 += "_" + suffix2;

  if (type == "pvTrk") {
    // Histograms of track properties
    h1["nTrk"+suffix1]     = new TH1D("nTrk"+suffix1, "Num of rec tracks"+suffix1, 1000,0,1000);
    h1["trkPt"+suffix1]    = new TH1D("trkPt"+suffix1, "Pt of rec tracks "+suffix1, 800,0,100);
    h1["trkEta"+suffix1]   = new TH1D("trkEta"+suffix1, "#eta of rec tracks "+suffix1, 100,-3,3);
    h1["trkPhi"+suffix1]   = new TH1D("trkPhi"+suffix1, "#phi of rec tracks "+suffix1, 100,-3.2,3.2);
    h1["trkDxy"+suffix1]   = new TH1D("trkDxy"+suffix1, "Dxy of rec tracks "+suffix1, 100,-0.5,0.5);
    h1["trkDxyCorr"+suffix1] = new TH1D("trkDxyCorr"+suffix1, TString("BS Corrected Dxy of rec tracks "+suffix1),100,-0.5,0.5); 
    h1["trkDz"+suffix1]    = new TH1D("trkDz"+suffix1, "Dz of rec tracks "+suffix1, 300,-50,50);

    h1["nTrkPV"+suffix1]   = new TH1D("nTrkPV"+suffix1, "Num of rec tracks in PV"+suffix1, 1000,0,1000);
    h1["nHWTrkPV"+suffix1]   = new TH1D("nHWTrkPV"+suffix1, "Num of rec tracks with over 0.5 weight in PV"+suffix1, 1000,0,1000);
    h1["ndofPV"+suffix1]   = new TH1D("ndofPV"+suffix1, "PV ndof"+suffix1, 300,0,100);
    h1["trkPtPV"+suffix1]  = new TH1D("trkPtPV"+suffix1, "Pt of rec tracks in "+suffix1 ,800,0,100);
    h1["trkEtaPV"+suffix1] = new TH1D("trkEtaPV"+suffix1, "#eta of rec tracks in PV"+suffix1, 100,-3,3);
    h1["trkPhiPV"+suffix1] = new TH1D("trkPhiPV"+suffix1, "#phi of rec tracks in PV"+suffix1, 100,-3.2,3.2);
    h1["trkDxyPV"+suffix1] = new TH1D("trkDxyPV"+suffix1, "Dxy of rec tracks in PV"+suffix1, 100,-0.5,0.5);
    h1["trkDxyCorrPV"+suffix1] = new TH1D("trkDxyCorrPV"+suffix1, "BS corrected Dxy of rec tracks in PV"+suffix1, 100,-0.5,0.5);
    h1["trkDzPV"+suffix1]  = new TH1D("trkDzPV"+suffix1, "Dz of rec tracks "+suffix1, 300,-50,50);
    h1["nrecPV"+suffix1]   = new TH1D("nrecPV"+suffix1, "Num of rec pvtx"+suffix1, 50,0,50);
  }
  else if (type == "generator") {
    h1["genPart_T"]      = new TH1D("genPart_T","t component of gen particles",300,-0.5,0.5);
    h1["genPart_T"]->GetXaxis()->SetTitle("t (nanosecond)");
    h1["genPart_cT"]     = new TH1D("genPart_cT","ct component of gen particles",300,-150.,150.);
    h1["genPart_cT"]->GetXaxis()->SetTitle("ct (mm)");    
    h1["nsimPV"]         = new TH1D("nsimPV","Num of sim PV",51,-0.5,50.5);  
  }
  else if (type == "misc") {
    h1["nrecPVDiff"]     = new TH1D("nrecPVDiff","nrecPV1-nRecPV2",21,-10.5,10.5);
    h1["nTrkPVDiff"]     = new TH1D("nTrkPVDiff","nTrkPV1-nTrkPV2",41,-20.5,20.5);
    h1["nTrkPVRelDiff"]  = new TH1D("nTrkPVRelDiff","(nTrkPV1-nTrkPV2)/(nTrkPV1+nTrkPV2)",100,-1,1);  
    h1["ndofPVDiff"]     = new TH1D("ndofPVDiff","ndofPV1-ndofPV2",100,-10,10);
    h1["ndofPVRelDiff"]  = new TH1D("ndofPVRelDiff","(ndofPV1-ndofPV2)/(ndofPV1+ndofPV2)",100,-1,1);  
    h1["twovtxzsign"]     = new TH1D("twovtxzsign", "two vtx z signficance",300,-20,20);
    // Histograms on comparing the multi-vertices
    // Difference in reconstructed vtx position
    h1["min_xsep"]       = new TH1D("min_xsep", "min x diff of primary and secondary pvtx",300,0,0.1);
    h1["min_xsep_sign"]  = new TH1D("min_xsep_sign", "min x diff in signf of primary and secondary pvtx",300,0,5);
    h1["min_ysep"]       = new TH1D("min_ysep", "min y diff of primary and secondary pvtx",300,0,0.1);
    h1["min_ysep_sign"]  = new TH1D("min_ysep_sign", "min y diff in signf of primary and secondary pvtx",300,0,5);
    h1["min_zsep"]       = new TH1D("min_zsep", "min z diff of primary and secondary pvtx",300,0,5);
    h1["min_zsep_sign"]  = new TH1D("min_zsep_sign", "min z diff in signf of primary and secondary pvtx",300,0,200);
    // Difference in reconstructed vtx position
    h1["min_ntrksep"]    = new TH1D("min_ntrksep", "min nTrk diff of primary and secondary pvtx",201,-50.5,150.5);
    h1["min_sumpt2sep"]  = new TH1D("min_sumpt2sep", "min sumpt2 diff of primary and secondary pvtx",300,0,10000);

  }
  else if (type == "summary") {
    h1["deltax"+suffix1] = new TH1D("deltax"+suffix1, "x-residual pvtx"+suffix1, 800,-0.1,0.1);
    h1["deltay"+suffix1] = new TH1D("deltay"+suffix1, "y-residual pvtx"+suffix1, 800,-0.1,0.1);
    h1["deltaz"+suffix1] = new TH1D("deltaz"+suffix1, "z-residual pvtx"+suffix1, 800,-0.1,0.1);
    h1["pullx"+suffix1]  = new TH1D("pullx"+suffix1, "x-pull pvtx"+suffix1, 800,-5,5);
    h1["pully"+suffix1]  = new TH1D("pully"+suffix1, "y-pull pvtx"+suffix1, 800,-5,5);
    h1["pullz"+suffix1]  = new TH1D("pullz"+suffix1, "z-pull pvtx"+suffix1, 800,-5,5);
    h1["errPVx"+suffix1] = new TH1D("errPVx"+suffix1, "X"+suffix1+" vertex error", 200,0.,0.1);
    h1["errPVy"+suffix1] = new TH1D("errPVy"+suffix1, "Y"+suffix1+" vertex error", 200,0.,0.1);
    h1["errPVz"+suffix1] = new TH1D("errPVz"+suffix1, "Z"+suffix1+" vertex error", 200,0.,0.1);
  }
  else if (type == "others") {
    h1["deltax"+suffix1] = new TH1D("deltax"+suffix1, "x-residual of pvtx"+methodName, 200,-0.1,0.1);
    h1["deltax"+suffix1]->GetXaxis()->SetTitle("cm");
    h1["deltay"+suffix1] = new TH1D("deltay"+suffix1, "y-residual of pvtx"+methodName, 200,-0.1,0.1);
    h1["deltay"+suffix1]->GetXaxis()->SetTitle("cm");
    h1["deltaz"+suffix1] = new TH1D("deltaz"+suffix1, "z-residual of pvtx"+methodName, 200,-0.1,0.1);
    h1["deltaz"+suffix1]->GetXaxis()->SetTitle("cm");
    h1["pullx"+suffix1]  = new TH1D("pullx"+suffix1, "x-pull of pvtx"+methodName, 200,-5.,5.);
    h1["pully"+suffix1]  = new TH1D("pully"+suffix1, "y-pull of pvtx"+methodName, 200,-5.,5.);
    h1["pullz"+suffix1]  = new TH1D("pullz"+suffix1, "z-pull of pvtx"+methodName, 200,-5.,5.);
    h1["errPVx"+suffix1] = new TH1D("errPVx"+suffix1, "X"+methodName+" vertex error", 200,0.,0.1);
    h1["errPVy"+suffix1] = new TH1D("errPVy"+suffix1, "Y"+methodName+" vertex error", 200,0.,0.1);
    h1["errPVz"+suffix1] = new TH1D("errPVz"+suffix1, "Z"+methodName+" vertex error", 200,0.,0.1);
  }

}

void PVHistograms::InitA(TString type, TString suffix1, TString suffix2, int nTrkMin_, int nTrkMax_) {

  TString methodName = suffix1;
  if (suffix2 != "") suffix1 += "_" + suffix2;

  if (type == "analysis") {
    h2["resx"+suffix1]= new TH2D("resx"+suffix1, "x-resolution vs number of tracks in pvtx"+methodName, nTrkMax_-nTrkMin_,nTrkMin_,nTrkMax_,400,0.,500);
    h2["resx"+suffix1]->SetMarkerStyle(21);
    h2["resx"+suffix1]->SetMarkerColor(4);
    h2["resx"+suffix1]->GetXaxis()->SetTitle("Num of tracks");
    h2["resx"+suffix1]->GetYaxis()->SetTitle("#mum");
    h2["resy"+suffix1]= new TH2D("resy"+suffix1, "y-resolution vs number of tracks in pvtx"+methodName, nTrkMax_-nTrkMin_,nTrkMin_,nTrkMax_,400,0.,500);
    h2["resy"+suffix1]->SetMarkerStyle(21);
    h2["resy"+suffix1]->SetMarkerColor(4);
    h2["resy"+suffix1]->GetXaxis()->SetTitle("Num of tracks");
    h2["resy"+suffix1]->GetYaxis()->SetTitle("#mum");
    h2["resz"+suffix1]= new TH2D("resz"+suffix1, "z-resolution vs number of tracks in pvtx"+methodName, nTrkMax_-nTrkMin_,nTrkMin_,nTrkMax_,400,0.,500);
    h2["resz"+suffix1]->SetMarkerStyle(21);
    h2["resz"+suffix1]->SetMarkerColor(4);
    h2["resz"+suffix1]->GetXaxis()->SetTitle("Num of tracks");
    h2["resz"+suffix1]->GetYaxis()->SetTitle("#mum");
    h2["pullx"+suffix1]= new TH2D("pullx"+suffix1, "x-pull vs number of tracks"+methodName, nTrkMax_-nTrkMin_,nTrkMin_,nTrkMax_,100,0.,2.);
    h2["pullx"+suffix1]->SetMarkerStyle(21);
    h2["pullx"+suffix1]->SetMarkerColor(4);
    h2["pullx"+suffix1]->SetBit(TH1::kCanRebin);
    h2["pullx"+suffix1]->GetXaxis()->SetTitle("Num of tracks");
    h2["pully"+suffix1]= new TH2D("pully"+suffix1, "y-pull vs number of tracks"+methodName, nTrkMax_-nTrkMin_,nTrkMin_,nTrkMax_,100,0.,2.);
    h2["pully"+suffix1]->SetMarkerStyle(21);
    h2["pully"+suffix1]->SetMarkerColor(4);
    h2["pully"+suffix1]->SetBit(TH1::kCanRebin);
    h2["pully"+suffix1]->GetXaxis()->SetTitle("Num of tracks");
    h2["pullz"+suffix1]= new TH2D("pullz"+suffix1, "z-pull vs number of tracks"+methodName, nTrkMax_-nTrkMin_,nTrkMin_,nTrkMax_,100,0.,2.);
    h2["pullz"+suffix1]->SetMarkerStyle(21);
    h2["pullz"+suffix1]->SetMarkerColor(4);
    h2["pullz"+suffix1]->SetBit(TH1::kCanRebin);
    h2["pullz"+suffix1]->GetXaxis()->SetTitle("Num of tracks");

  }

}

//_______________________________________________________________
void PVHistograms::Fill1d(TString name, Double_t x, Double_t weight) {

  h1[name]->Fill(x,weight);
}

//_______________________________________________________________
void PVHistograms::Fill2d(TString name, Double_t x, Double_t y, Double_t weight) {

  h2[name]->Fill(x,y,weight);
}

//_______________________________________________________________
void PVHistograms::Print(TString extension, TString tag) {

  if ( tag != "" ) tag = "_"+tag;
                
  for(std::map<TString,TCanvas*>::const_iterator icv=cv_map.begin(); icv!=cv_map.end(); ++icv){

    TString tmpname = icv->first;
    TCanvas *acv = icv->second;
    acv->Print(TString(tmpname+tag+"."+extension));
  }

}

//_______________________________________________________________
void PVHistograms::Save() {
	
  for(std::map<TString,TH1* >::const_iterator ih=h1.begin(); ih!=h1.end(); ++ih){
    TH1D *htemp = (TH1D*) ih->second;
    if (htemp->GetEntries() > 0 ) htemp->Write();
  }
  for(std::map<TString,TH2* >::const_iterator ih=h2.begin(); ih!=h2.end(); ++ih){
    TH2D *htemp = (TH2D*) ih->second;
    if (htemp->GetEntries() > 0 ) htemp->Write();
  }
}

//_______________________________________________________________
void PVHistograms::SaveToFile(TString filename) {

  foutfile = new TFile(filename,"RECREATE");
  for(std::map<TString,TH1* >::const_iterator ih=h1.begin(); ih!=h1.end(); ++ih){
    TH1D *htemp = (TH1D*) ih->second;
    htemp->Write();
  }
  for(std::map<TString,TH2* >::const_iterator ih=h2.begin(); ih!=h2.end(); ++ih){
    TH2D *htemp = (TH2D*) ih->second;
    htemp->Write();
  }
  
  foutfile->Write();
  foutfile->Close();
  
}
