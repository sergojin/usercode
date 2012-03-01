//
// This is a script to analyze the histograms from the pvstudy
// You need to specify the root files for data ( and/or for MC) 
// Usage:  root -l analyzePVData.C+ 
// 

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>

#include <TMath.h>
#include <TROOT.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TF1.h>
#include <TAxis.h>
#include <TGraphErrors.h>
#include <TPaveStats.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <TLegend.h>
#include <TNamed.h>
#include <TLatex.h>
#include "TLorentzVector.h"

using namespace std;
bool verbose_ = true;

bool mysorter (TLorentzVector i, TLorentzVector j) 
{
  return (i.Pt()>j.Pt());
}



map<string, TFile*> files;
map<string, TH1*> h;
map<string, TH2*> hSum;
vector<string> pvNames;

double cm2um = 10000;
int nTrkMin_ = 5;
int nTrkMax_ = 70;
int nEntries = nTrkMax_ - nTrkMin_ + 1;




void Ntuple2HistCOnverter(TString filename, Float_t minPtHat=0.0, Float_t maxPtHat=1000.0, Float_t maxDijetBalance=0.0)
{
  //gROOT->SetStyle("CMS");
  gStyle->SetOptFit(1111);
  gStyle->SetFillColor(0);

  std::map<TString, TH1*> h1;

  //  TFile ifile("/uscms_data/d3/sergo/crab_MB7TEV_Aug05_v3/res/ntuple_merged_Run2011A_Express.root");

  TFile ifile(filename);

  h1["nTrkPV"] = new TH1D("nTrkPV", "Number of Tracks in PV", 500, -0.5, 499.5);

  h1["JetEt"] = new TH1D("JetEt", "Jet Et", 1000, 0.0, 1000.0);
  h1["DijetEtBal"] = new TH1D("DijetEtBal", "Dijet Et Balance", 100, 0.0, 1.0);



  for (int ntrk=nTrkMin_; ntrk<=nTrkMax_;ntrk++) {
  
    stringstream ss;
    TString suffix1;

    ss << "_"<<ntrk;
    suffix1 = ss.str(); 

    h1["deltax"+suffix1] = new TH1D("deltax"+suffix1, "x-residual of pvtx", 200,-0.1,0.1);
    h1["deltax"+suffix1]->GetXaxis()->SetTitle("cm");
    h1["deltay"+suffix1] = new TH1D("deltay"+suffix1, "y-residual of pvtx", 200,-0.1,0.1);
    h1["deltay"+suffix1]->GetXaxis()->SetTitle("cm");
    h1["deltaz"+suffix1] = new TH1D("deltaz"+suffix1, "z-residual of pvtx", 200,-0.1,0.1);
    h1["deltaz"+suffix1]->GetXaxis()->SetTitle("cm");
    h1["pullx"+suffix1]  = new TH1D("pullx"+suffix1, "x-pull of pvtx", 200,-5.,5.);
    h1["pully"+suffix1]  = new TH1D("pully"+suffix1, "y-pull of pvtx", 200,-5.,5.);
    h1["pullz"+suffix1]  = new TH1D("pullz"+suffix1, "z-pull of pvtx", 200,-5.,5.);
    h1["errPVx"+suffix1] = new TH1D("errPVx"+suffix1, "X vertex error", 200,0.,0.1);
    h1["errPVy"+suffix1] = new TH1D("errPVy"+suffix1, "Y vertex error", 200,0.,0.1);
    h1["errPVz"+suffix1] = new TH1D("errPVz"+suffix1, "Z vertex error", 200,0.,0.1);

  }


  TTree* t = (TTree*)  ifile.Get(TString("pvtxtree"));

  
  Int_t       nrecPV_twovtx;
  Int_t       nTrkPV_twovtx[50];

  Double_t    deltax_twovtx[50];
  Double_t    deltay_twovtx[50];
  Double_t    deltaz_twovtx[50];

  Double_t    errx_twovtx[50];
  Double_t    erry_twovtx[50];
  Double_t    errz_twovtx[50];

  Double_t    pullx_twovtx[50];
  Double_t    pully_twovtx[50];
  Double_t    pullz_twovtx[50];

  Double_t    avgPtPV1_spl_twovtx[50];

  Int_t           nCaloJet;
  Double_t        CaloJet_px[100];     
  Double_t        CaloJet_py[100];     
  Double_t        CaloJet_pz[100];     

  vector<TLorentzVector>   CaloJets;
  TLorentzVector jet;

  
  TBranch        *b_nrecPV_twovtx;
  
  TBranch        *b_nTrkPV_twovtx;
  TBranch        *b_deltax_twovtx;
  TBranch        *b_deltay_twovtx;
  TBranch        *b_deltaz_twovtx;

  TBranch        *b_errx_twovtx;
  TBranch        *b_erry_twovtx;
  TBranch        *b_errz_twovtx;

  TBranch        *b_pullx_twovtx;
  TBranch        *b_pully_twovtx;
  TBranch        *b_pullz_twovtx;

  TBranch        *b_avgPtPV1_spl_twovtx;

  TBranch        *b_ncalojet;
  TBranch        *b_calojetpx;
  TBranch        *b_calojetpy;
  TBranch        *b_calojetpz;


  Double_t       DijetEtBal;


  t->SetBranchAddress("nrecPV_twovtx", &nrecPV_twovtx,&b_nrecPV_twovtx); 
  
  t->SetBranchAddress("nTrkPV_twovtx", nTrkPV_twovtx, &b_nTrkPV_twovtx); 

  t->SetBranchAddress("deltax_twovtx", deltax_twovtx, &b_deltax_twovtx); 
  t->SetBranchAddress("deltay_twovtx", deltay_twovtx, &b_deltay_twovtx); 
  t->SetBranchAddress("deltaz_twovtx", deltaz_twovtx, &b_deltaz_twovtx); 

  t->SetBranchAddress("errx_twovtx", errx_twovtx, &b_errx_twovtx);
  t->SetBranchAddress("erry_twovtx", erry_twovtx, &b_erry_twovtx);
  t->SetBranchAddress("errz_twovtx", errz_twovtx, &b_errz_twovtx);

  t->SetBranchAddress("pullx_twovtx", pullx_twovtx, &b_pullx_twovtx);
  t->SetBranchAddress("pully_twovtx", pully_twovtx, &b_pully_twovtx);
  t->SetBranchAddress("pullz_twovtx", pullz_twovtx, &b_pullz_twovtx);

  t->SetBranchAddress("avgPtPV1_spl_twovtx", avgPtPV1_spl_twovtx, &b_avgPtPV1_spl_twovtx); 

  t->SetBranchAddress("nCaloJets",&nCaloJet, &b_ncalojet); 

  t->SetBranchAddress("CaloJet_px",CaloJet_px, &b_calojetpx); 
  t->SetBranchAddress("CaloJet_py",CaloJet_py, &b_calojetpy); 
  t->SetBranchAddress("CaloJet_pz",CaloJet_pz, &b_calojetpz); 



  for (Int_t ev = 0; ev < t->GetEntries(); ev++) {
    
    CaloJets.clear();

    t->GetEntry(ev);
        
    //    cout<<nCaloJet<<"\n";

  
    /*
    for (int ij=0; ij<nCaloJet; ++ij){  

      jet.SetXYZM(  CaloJet_px[ij], CaloJet_py[ij],  CaloJet_pz[ij], 0.0);
      CaloJets.push_back(jet);
      //      cout<<jet.Pt()<<"   "<<jet.Eta()<<"\n";
      h1["JetEt"]->Fill(jet.Et());
    }
    

    sort (CaloJets.begin(), CaloJets.end(), mysorter);
    
    if (CaloJets.size()>=2)
      DijetEtBal = fabs( CaloJets[0].Pt()- CaloJets[1].Pt() )/( CaloJets[0].Pt() + CaloJets[1].Pt() );
    else
      DijetEtBal=1000.0;
    h1["DijetEtBal"]->Fill(DijetEtBal);

    */

    TString suffix2;
                                                                                                                                                     
    for (int iv=0; iv<nrecPV_twovtx; ++iv){  

      stringstream strNtrk;
      strNtrk << "_"<<nTrkPV_twovtx[iv];
      suffix2 = strNtrk.str();
      
      if ((nTrkPV_twovtx[iv]>=nTrkMin_)&&(nTrkPV_twovtx[iv]<=nTrkMax_)
	  //	  && ( CaloJets[0].Pt()>250 )
	  && (DijetEtBal <= maxDijetBalance)
	  &&( avgPtPV1_spl_twovtx[iv] >minPtHat)
	  && ( avgPtPV1_spl_twovtx[iv] <maxPtHat))
	
	{

      h1["nTrkPV"]->Fill(nTrkPV_twovtx[iv]);

      h1["deltax"+suffix2]->Fill( deltax_twovtx[iv]);
      h1["deltay"+suffix2]->Fill( deltay_twovtx[iv]);
      h1["deltaz"+suffix2]->Fill( deltaz_twovtx[iv]);

      h1["errPVx"+suffix2]->Fill( errx_twovtx[iv]);
      h1["errPVy"+suffix2]->Fill( erry_twovtx[iv]);
      h1["errPVz"+suffix2]->Fill( errz_twovtx[iv]);

      h1["pullx"+suffix2]->Fill( pullx_twovtx[iv]);
      h1["pully"+suffix2]->Fill( pully_twovtx[iv]);
      h1["pullz"+suffix2]->Fill( pullz_twovtx[iv]);

	  }
   
    }

    
  }
  
  
  stringstream outFileName;
  outFileName << filename <<"_"<<minPtHat<<"_"<<maxPtHat<<".root";
  TString fN = outFileName.str();

  TFile *rootFile_ = new TFile(fN,"RECREATE");
  rootFile_->cd();
  rootFile_->mkdir("Others");
  rootFile_->cd("Others");
  
  for(std::map<TString,TH1* >::const_iterator ih=h1.begin(); ih!=h1.end(); ++ih){                                                     
      TH1 *htemp = ih->second;                                                                                                          
      htemp->Write();                                                                                                                     
    }      
  rootFile_->Write();
  rootFile_->Close();
  
}

