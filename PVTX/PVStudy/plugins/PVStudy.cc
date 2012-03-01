// -*- C++ -*-
//
// Package:    PVStudy
// Class:      PVStudy
// 
//\class PVStudy PVStudy.cc UserCode/PVStudy/plugins/PVStudy.cc

// system include files
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "UserCode/PVStudy/interface/PVStudy.h"
//
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

// simulated vertices,..., add <use name=SimDataFormats/Vertex> and <../Track>
#include <SimDataFormats/Vertex/interface/SimVertex.h>
#include <SimDataFormats/Vertex/interface/SimVertexContainer.h>
#include <SimDataFormats/Track/interface/SimTrack.h>
#include <SimDataFormats/Track/interface/SimTrackContainer.h>
// BeamSpot 
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

//Jets
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/JetID.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

//root
#include <TROOT.h>
#include <TF1.h>
#include <TString.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TLorentzVector.h>
#include <TPad.h>

using namespace std;
typedef math::XYZTLorentzVectorF LorentzVector;
typedef math::XYZPoint Point;

bool mysorter (TLorentzVector i, TLorentzVector j) 
{
  return (i.Pt()>j.Pt());
}


PVStudy::PVStudy(const edm::ParameterSet& iConfig)
{
  //=======================================================================
  // Get configuration for TrackTupleMaker
  //=======================================================================
  simG4_                     = iConfig.getParameter<edm::InputTag>( "simG4" );
  trackCollectionTag_        = iConfig.getUntrackedParameter<edm::InputTag>("trackCollection");  
  splitTrackCollection1Tag_  = iConfig.getUntrackedParameter<edm::InputTag>("splitTrackCollection1");
  splitTrackCollection2Tag_  = iConfig.getUntrackedParameter<edm::InputTag>("splitTrackCollection2");
  vertexCollectionTag_       = iConfig.getUntrackedParameter<edm::InputTag>("vertexCollection");
  splitVertexCollection1Tag_ = iConfig.getUntrackedParameter<edm::InputTag>("splitVertexCollection1");
  splitVertexCollection2Tag_ = iConfig.getUntrackedParameter<edm::InputTag>("splitVertexCollection2");
  verbose_                   = iConfig.getUntrackedParameter<bool>("verbose",false);
  realData_                  = iConfig.getUntrackedParameter<bool>("realData",false);
  analyze_                   = iConfig.getUntrackedParameter<bool>("analyzeOnTheFly",false);
  saventuple_                = iConfig.getUntrackedParameter<bool>("saventuple",false);  
  outputfilename_            = iConfig.getUntrackedParameter<string>("OutputFileName");   
  histoFileName_             = iConfig.getUntrackedParameter<std::string> ("histoFileName");
  ntrkdiffcut_               = iConfig.getUntrackedParameter<double>("ntrkdiffcut");
  nTrkMin_                   = iConfig.getUntrackedParameter<int>("nTrkMin");
  nTrkMax_                   = iConfig.getUntrackedParameter<int>("nTrkMax");
  zsigncut_                  = iConfig.getUntrackedParameter<int>("zsigncut");
  useHWTrk_                  = iConfig.getUntrackedParameter<bool>("useHWTrk",false);
  avgTrkPtInPVMin_                  = iConfig.getUntrackedParameter<double>("avgTrkPtInPVMin");
  avgTrkPtInPVMax_                  = iConfig.getUntrackedParameter<double>("avgTrkPtInPVMax");
  bsSrc = iConfig.getParameter< edm::InputTag >("beamSpot");
  filterHLT_                 = iConfig.getUntrackedParameter<bool>("filterHLT",false);
  filterExpressions_         = iConfig.getParameter<std::vector<std::string> >("filterExpressions");
  menu_                      = TString(iConfig.getParameter<std::string>("Menu"));
  menu_.ToLower();

  uncorJetsInputTag_ = iConfig.getParameter<edm::InputTag>  ("uncorJetsInputTag");
  CaloJetCorrectorL2L3_= iConfig.getParameter<std::string>("CaloJetCorrectorL2L3");
  CaloJetCorrectorL1L2L3_= iConfig.getParameter<std::string>("CaloJetCorrectorL1L2L3");
 
  
  //now do what ever initialization is needed
  pvinfo.clear();

  // Specify the data mode vector
  if(realData_) datamode.push_back(0);
  else {
    datamode.push_back(0);
    datamode.push_back(1);
    datamode.push_back(2);
    datamode.push_back(3);
  }
 // Create ntuple files if needed
  if(saventuple_) {
    file_ = TFile::Open(outputfilename_.c_str(),"RECREATE");
    ftree_ = new TTree("mytree","mytree");
    ftree_->AutoSave();
    ftree_->Branch("residual",&fres_,"fres_[3]/D");
    ftree_->Branch("error",&ferror_,"ferror_[3]/D");
    ftree_->Branch("nTrkPV",&fntrk_,"fntrk_/I");

    // pvtxtree_ analyzing the pvtxs ootb
    pvtxtree_ = new TTree("pvtxtree","pvtxtree");

    // Event information for the data
    pvtxtree_->Branch("glob_runno",&glob_runno_,"glob_runno/I");
    pvtxtree_->Branch("glob_evtno",&glob_evtno_,"glob_evtno/I");
    pvtxtree_->Branch("glob_ls",&glob_ls_,"glob_ls/I");
    pvtxtree_->Branch("glob_bx",&glob_bx_,"glob_bx/I");  
    pvtxtree_->Branch("glob_ntrg",&glob_ntrg_,"glob_ntrg/I");  
    pvtxtree_->Branch("glob_trg_idx",&glob_trg_idx_,"glob_trg_idx[glob_ntrg]/I");  
    pvtxtree_->Branch("glob_trg_name",&glob_trg_name_);  
    

    pvtxtree_->Branch("nrecPV",&nrecPV_,"nrecPV/I");
    pvtxtree_->Branch("nTrkPV",&fntrk_,"fntrk_/I");
    pvtxtree_->Branch("nrecPV1_spl",&nrecPV1_spl_,"nrecPV1_spl/I");
    pvtxtree_->Branch("nrecPV2_spl",&nrecPV2_spl_,"nrecPV2_spl/I");
    // Event level information
    pvtxtree_->Branch("min_zsep",&min_zsep_,"min_zsep/D");
    pvtxtree_->Branch("min_ntrksep",&min_ntrksep_,"min_ntrksep/D");
    pvtxtree_->Branch("min_sumpt2sep",&min_sumpt2sep_,"min_sumpt2sep/D");

    //Fill the variables in the twovtx pair (recvtx1, recvtx2)   
    // Information related to the analyzing the two-vertex method

    pvtxtree_->Branch("nrecPV_twovtx",&nrecPV_twovtx_,"nrecPV_twovtx/I");
    pvtxtree_->Branch("nTrkPV_twovtx",&nTrkPV_twovtx_,"nTrkPV_twovtx[nrecPV_twovtx]/I");
    pvtxtree_->Branch("deltax_twovtx",&deltax_twovtx_,"deltax_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("deltay_twovtx",&deltay_twovtx_,"deltay_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("deltaz_twovtx",&deltaz_twovtx_,"deltaz_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("errx_twovtx",&errx_twovtx_,"errx_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("erry_twovtx",&erry_twovtx_,"erry_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("errz_twovtx",&errz_twovtx_,"errz_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("pullx_twovtx",&pullx_twovtx_,"pullx_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("pully_twovtx",&pully_twovtx_,"pully_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("pullz_twovtx",&pullz_twovtx_,"pullz_twovtx[nrecPV_twovtx]/D");

    // Information for the splitVertexColl1
    pvtxtree_->Branch("nTrkPV1_spl_twovtx",&nTrkPV1_spl_twovtx_,"nTrkPV1_spl_twovtx[nrecPV_twovtx]/I");
    pvtxtree_->Branch("ndofPV1_spl_twovtx",&ndofPV1_spl_twovtx_,"ndofPV1_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("normchi2PV1_spl_twovtx",&normchi2PV1_spl_twovtx_,"normchi2PV1_spl_twovtx[nrecPV_twovtx]/D"); 
    pvtxtree_->Branch("avgPtPV1_spl_twovtx",&avgPtPV1_spl_twovtx_,"avgPtPV1_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("errx1_spl_twovtx",&errx1_spl_twovtx_,"errx1_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("erry1_spl_twovtx",&erry1_spl_twovtx_,"erry1_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("errz1_spl_twovtx",&errz1_spl_twovtx_,"errz1_spl_twovtx[nrecPV_twovtx]/D");
  
    
    // Information for the splitVertexColl2
    pvtxtree_->Branch("nTrkPV2_spl_twovtx",&nTrkPV2_spl_twovtx_,"nTrkPV2_spl_twovtx[nrecPV_twovtx]/I");
    pvtxtree_->Branch("ndofPV2_spl_twovtx",&ndofPV2_spl_twovtx_,"ndofPV2_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("normchi2PV2_spl_twovtx",&normchi2PV2_spl_twovtx_,"normchi2PV2_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("avgPtPV2_spl_twovtx",&avgPtPV2_spl_twovtx_,"avgPtPV2_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("errx2_spl_twovtx",&errx2_spl_twovtx_,"errx2_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("erry2_spl_twovtx",&erry2_spl_twovtx_,"erry2_spl_twovtx[nrecPV_twovtx]/D");
    pvtxtree_->Branch("errz2_spl_twovtx",&errz2_spl_twovtx_,"errz2_spl_twovtx[nrecPV_twovtx]/D");


    pvtxtree_->Branch("nCaloJets",&nCaloJets_,"nCaloJets/I");
    pvtxtree_->Branch("CaloJet_px",&CaloJet_px_,"CaloJet_px[nCaloJets]/D");
    pvtxtree_->Branch("CaloJet_py",&CaloJet_py_,"CaloJet_py[nCaloJets]/D");
    pvtxtree_->Branch("CaloJet_pz",&CaloJet_pz_,"CaloJet_pz[nCaloJets]/D");
    pvtxtree_->Branch("DijetBalance",&DijetBalance_,"DijetBalance/D");
   
    
    // MC variables
    if(!realData_) {  
      pvtxtree_->Branch("nsimPV",&nsimPV_,"nsimPV/I");
      pvtxtree_->Branch("nsimTrkPV",&nsimTrkPV_,"nsimTrkPV[nsimPV]/I");
      pvtxtree_->Branch("simx",&simx_,"simx[nsimPV]/D");
      pvtxtree_->Branch("simy",&simy_,"simy[nsimPV]/D");
      pvtxtree_->Branch("simz",&simz_,"simz[nsimPV]/D");
      pvtxtree_->Branch("simptsq",&simptsq_,"simptsq[nsimPV]/D");

      // For pvtxs with all the tracks 
      pvtxtree_->Branch("nrecPV_mct",&nrecPV_mct_,"nrecPV_mct/I");
      pvtxtree_->Branch("deltax_mct",&deltax_mct_,"deltax_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("deltay_mct",&deltay_mct_,"deltay_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("deltaz_mct",&deltaz_mct_,"deltaz_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("pullx_mct",&pullx_mct_,"pullx_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("pully_mct",&pully_mct_,"pully_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("pullz_mct",&pullz_mct_,"pullz_mct[nrecPV_mct]/D");     
      pvtxtree_->Branch("errx_mct",&errx_mct_,"errx_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("erry_mct",&erry_mct_,"erry_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("errz_mct",&errz_mct_,"errz_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("nTrkPV_mct",&nTrkPV_mct_,"nTrkPV_mct[nrecPV_mct]/I");
      pvtxtree_->Branch("ndofPV_mct",&ndofPV_mct_,"ndofPV_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("normchi2PV_mct",&normchi2PV_mct_,"normchi2PV_mct[nrecPV_mct]/D");
      pvtxtree_->Branch("avgPtPV_mct",&avgPtPV_mct_,"avgPtPV_mct[nrecPV_mct]/D");
 
      // For pvtxs with splittracks1
      pvtxtree_->Branch("nrecPV_spl1_mct",&nrecPV_spl1_mct_,"nrecPV_spl1_mct/I");
      pvtxtree_->Branch("deltax_spl1_mct",&deltax_spl1_mct_,"deltax_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("deltay_spl1_mct",&deltay_spl1_mct_,"deltay_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("deltaz_spl1_mct",&deltaz_spl1_mct_,"deltaz_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("pullx_spl1_mct",&pullx_spl1_mct_,"pullx_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("pully_spl1_mct",&pully_spl1_mct_,"pully_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("pullz_spl1_mct",&pullz_spl1_mct_,"pullz_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("errx_spl1_mct",&errx_spl1_mct_,"errx_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("erry_spl1_mct",&erry_spl1_mct_,"erry_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("errz_spl1_mct",&errz_spl1_mct_,"errz_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("nTrkPV_spl1_mct",&nTrkPV_spl1_mct_,"nTrkPV_spl1_mct[nrecPV_spl1_mct]/I");
      pvtxtree_->Branch("ndofPV_spl1_mct",&ndofPV_spl1_mct_,"ndofPV_spl1_mct[nrecPV_spl1_mct]/D");
      pvtxtree_->Branch("normchi2PV_spl1_mct",&normchi2PV_spl1_mct_,"normchi2PV_spl1_mct[nrecPV_spl1_mct]/D"); 
      pvtxtree_->Branch("avgPtPV_spl1_mct",&avgPtPV_spl1_mct_,"avgPtPV_spl1_mct[nrecPV_spl1_mct]/D");
        
      // For pvtxs with splittracks1
      pvtxtree_->Branch("nrecPV_spl2_mct",&nrecPV_spl2_mct_,"nrecPV_spl2_mct/I");
      pvtxtree_->Branch("deltax_spl2_mct",&deltax_spl2_mct_,"deltax_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("deltay_spl2_mct",&deltay_spl2_mct_,"deltay_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("deltaz_spl2_mct",&deltaz_spl2_mct_,"deltaz_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("pullx_spl2_mct",&pullx_spl2_mct_,"pullx_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("pully_spl2_mct",&pully_spl2_mct_,"pully_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("pullz_spl2_mct",&pullz_spl2_mct_,"pullz_spl2_mct[nrecPV_spl2_mct]/D");   
      pvtxtree_->Branch("errx_spl2_mct",&errx_spl2_mct_,"errx_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("erry_spl2_mct",&erry_spl2_mct_,"erry_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("errz_spl2_mct",&errz_spl2_mct_,"errz_spl2_mct[nrecPV_spl2_mct]/D");   
      pvtxtree_->Branch("nTrkPV_spl2_mct",&nTrkPV_spl2_mct_,"nTrkPV_spl2_mct[nrecPV_spl2_mct]/I");
      pvtxtree_->Branch("ndofPV_spl2_mct",&ndofPV_spl2_mct_,"ndofPV_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("normchi2PV_spl2_mct",&normchi2PV_spl2_mct_,"normchi2PV_spl2_mct[nrecPV_spl2_mct]/D");
      pvtxtree_->Branch("avgPtPV_spl2_mct",&avgPtPV_spl2_mct_,"avgPtPV_spl2_mct[nrecPV_spl2_mct]/D");
    }
  }
  
  setRootStyle();

  //========================================
  // Booking histograms
  //========================================
  
  // Create a root file for histograms
  theFile = new TFile(histoFileName_.c_str(), "RECREATE");
  // make diretories
  theFile->mkdir("Summary");
  theFile->cd();
  theFile->mkdir("Others");
  theFile->cd();
  if (analyze_) {
    theFile->mkdir("Results");
    theFile->cd();
  }

  // Book Histograms:
  h_pvtrk   = new PVHistograms();
  h_misc    = new PVHistograms();
  h_summary = new PVHistograms();
  h_others  = new PVHistograms();

  for(int i=0;i<3;i++) {  
    if(i == 0) h_pvtrk->Init("pvTrk");
    else {
      stringstream ss;
      ss << i;
      h_pvtrk->Init("pvTrk", ss.str(),"spl");
    }
  }
  h_misc->Init("misc");

  // Book MC only plots
  if (!realData_) {
    h_gen = new PVHistograms();
    h_gen->Init("generator");
  }
  if (analyze_) h_ana = new PVHistograms();

  //Book histograms sensitive to data/mc
  for (vector<int>::const_iterator it= datamode.begin(); it != datamode.end() ; ++it) {
    string suffix; 
    edm::LogInfo("Debug")<<"datamode = "<< *it<<endl;
    switch(*it) {
    case 0: suffix = "";
      break;
    case 1: suffix = "_spl1_mct";
      break;
    case 2: suffix = "_spl2_mct";	
      break;
    case 3: suffix = "_mct";
      break;
    }
    h_summary->Init("summary",suffix);
    
    // Book residual, error and pull histograms for each nTrk bin
    for (int ntrk=nTrkMin_;ntrk<=nTrkMax_;++ntrk) {
      stringstream ss;
      ss << ntrk;
      h_others->Init("others",suffix,ss.str());
    }

    // Book residual and pull histograms only when analyzeOntheFly is enabled
    if(analyze_) h_ana->InitA("analysis",suffix,"nTrk",nTrkMin_,nTrkMax_);
    suffix.clear();
  } // End of Book histograms sensitive to data/mc    
  

}

PVStudy::~PVStudy()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  theFile->cd();
  theFile->cd("Summary");
  h_pvtrk->Save();
  h_misc->Save();
  h_summary->Save();
  if (!realData_) 
    h_gen->Save();
  if (analyze_) {
    theFile->cd();
    theFile->cd("Results");
    h_ana->Save();
  }
  theFile->cd();
  theFile->cd("Others");
  h_others->Save();

  theFile->Close();
}

void PVStudy::setRootStyle() {
  //
  gROOT->SetStyle("Plain");
  gStyle->SetFillColor(1);
  gStyle->SetOptDate();
  //   gStyle->SetOptStat(1111110); 
  //   gStyle->SetOptFit(0111);
  //   gStyle->SetPadTickX(1);
  //   gStyle->SetPadTickY(1);
  gStyle->SetMarkerSize(0.5);
  gStyle->SetMarkerStyle(8);
  gStyle->SetGridStyle(3);
  //gStyle->SetPadGridX(1);
  //gStyle->SetPadGridY(1);
  gStyle->SetPaperSize(TStyle::kA4);
  gStyle->SetStatW(0.25);             // width of statistics box; default is 0.19
  //   gStyle->SetStatH(0.10);             // height of statistics box; default is 0.1
  gStyle->SetStatFormat("6.4g");      // leave default format for now
  gStyle->SetTitleSize(0.055, "");    // size for pad title; default is 0.02
  gStyle->SetLabelSize(0.03, "XYZ");  // size for axis labels; default is 0.04
  gStyle->SetStatFontSize(0.08);      // size for stat. box
  gStyle->SetTitleFont(42, "XYZ");    // times-bold-italic font (p. 153) for axes
  gStyle->SetTitleFont(42, "");       // same for pad title
  gStyle->SetLabelFont(42, "XYZ");    // same for axis labels
  gStyle->SetStatFont(42);            // same for stat. box
  gStyle->SetLabelOffset(0.006, "Y"); // default is 0.005
  //
  return;
}
//
// member functions
//
std::vector<PVStudy::simPrimaryVertex> PVStudy::getSimPVs(const Handle<HepMCProduct> evtMC, std::string suffix="")
{
 std::vector<PVStudy::simPrimaryVertex> simpv;
  const HepMC::GenEvent* evt=evtMC->GetEvent();
  if (evt) {
    edm::LogInfo("SimPVs") << "[getSimPVs] process id " << evt->signal_process_id()<<endl;
    edm::LogInfo("SimPVs") << "[getSimPVs] signal process vertex " << ( evt->signal_process_vertex() ?
									evt->signal_process_vertex()->barcode() : 0 ) <<endl;
    edm::LogInfo("SimPVs") << "[getSimPVs] number of vertices " << evt->vertices_size() << endl;

    int idx=0; int npv=0;
    for(HepMC::GenEvent::vertex_const_iterator vitr= evt->vertices_begin();
	vitr != evt->vertices_end(); ++vitr ) { // loop for vertex ...
      HepMC::FourVector pos = (*vitr)->position();
      //HepLorentzVector pos = (*vitr)->position();

      // t component of PV:
      for ( HepMC::GenVertex::particle_iterator mother  = (*vitr)->particles_begin(HepMC::parents);
	    mother != (*vitr)->particles_end(HepMC::parents); ++mother ) {
	// 	  edm::LogInfo("SimPVs") << "Status = " << (*mother)->status() << endl;
	HepMC::GenVertex * mv=(*mother)->production_vertex();
	if( ((*mother)->status() == 3) && (!mv)) {
	  // 	    edm::LogInfo("SimPVs") << "npv= " << npv << endl;
	  if (npv == 0) {
	    h_gen->Fill1d("genPart_cT", pos.t()); // mm
	    h_gen->Fill1d("genPart_T", pos.t()/299.792458); // ns
	  }
	  npv++;
	}
      }
      //       if (pos.t()>0) { continue;} // for 22X when t of PV was not smeared
	
      bool hasMotherVertex=false;
      if (verbose_) cout << "[getSimPVs] mothers of vertex[" << ++idx << "]: " << endl;
      for ( HepMC::GenVertex::particle_iterator mother  = (*vitr)->particles_begin(HepMC::parents);
	    mother != (*vitr)->particles_end(HepMC::parents); ++mother ) {
	HepMC::GenVertex * mv=(*mother)->production_vertex();
	// if (verbose_) cout << "Status = " << (*mother)->status() << endl;
	if (mv) {
	  hasMotherVertex=true;
	  if(!verbose_) break; //if verbose_, print all particles of gen vertices
	}
	if(verbose_) {
	  cout << "\t";
	  (*mother)->print();
	}
      }

      if(hasMotherVertex) continue;

      // could be a new vertex, check  all primaries found so far to avoid multiple entries
      const double mm2cm=0.1;
      simPrimaryVertex sv(pos.x()*mm2cm,pos.y()*mm2cm,pos.z()*mm2cm); // sim unit mm, rec unit cm
      simPrimaryVertex *vp=NULL;  // will become non-NULL if a vertex is found and then point to it
      for(vector<simPrimaryVertex>::iterator v0=simpv.begin();
	  v0!=simpv.end(); v0++){
	if( (fabs(sv.x-v0->x)<1e-5) && (fabs(sv.y-v0->y)<1e-5) && (fabs(sv.z-v0->z)<1e-5)){
	  vp=&(*v0);
	  break;
	}
      }

      if(!vp){
	// this is a new vertex
	edm::LogInfo("SimPVs") << "[getSimPVs] this is a new vertex " << sv.x << " " << sv.y << " " << sv.z << endl;
	simpv.push_back(sv);
	vp=&simpv.back();
      }else{
	edm::LogInfo("SimPVs") << "[getSimPVs] this is not a new vertex " << sv.x << " " << sv.y << " " << sv.z << endl;
      }
      vp->genVertex.push_back((*vitr)->barcode());
      // collect final state descendants
      for ( HepMC::GenVertex::particle_iterator daughter  = (*vitr)->particles_begin(HepMC::descendants);
	    daughter != (*vitr)->particles_end(HepMC::descendants);
	    ++daughter ) {
	if (isFinalstateParticle(*daughter)){ 
	  if ( find(vp->finalstateParticles.begin(), vp->finalstateParticles.end(),(*daughter)->barcode())
	       == vp->finalstateParticles.end()){
	    vp->finalstateParticles.push_back((*daughter)->barcode());
	    HepMC::FourVector m=(*daughter)->momentum();
	    // the next four lines used to be "vp->ptot+=m;" in the days of CLHEP::HepLorentzVector
	    // but adding FourVectors seems not to be foreseen
	    vp->ptot.setPx(vp->ptot.px()+m.px());
	    vp->ptot.setPy(vp->ptot.py()+m.py());
	    vp->ptot.setPz(vp->ptot.pz()+m.pz());
	    vp->ptot.setE(vp->ptot.e()+m.e());
	    vp->ptsq+=(m.perp())*(m.perp());
	    if ( (m.perp()>0.8) && (fabs(m.pseudoRapidity())<2.5) && isCharged( *daughter ) ){
	      vp->nGenTrk++;
	    }
	  }
	}
      }//loop MC vertices daughters
    }//loop MC vertices
  }
  return simpv;
}

std::vector<PVStudy::simPrimaryVertex> PVStudy::getSimPVs(const Handle<HepMCProduct> evtMC, 
							  const Handle<SimVertexContainer> simVtxs, 
							  const Handle<SimTrackContainer> simTrks)
{
  // simvertices don't have enough information to decide, 
  // genVertices don't have the simulated coordinates  ( with VtxSmeared they might)
  // go through simtracks to get the link between simulated and generated vertices
  std::vector<PVStudy::simPrimaryVertex> simpv;
  int idx=0;
  for(SimTrackContainer::const_iterator t=simTrks->begin();
      t!=simTrks->end(); ++t){
    if ( !(t->noVertex()) && !(t->type()==-99) ){
      double ptsq=0;
      bool primary=false;   // something coming directly from the primary vertex
      bool resonance=false; // resonance
      bool track=false;     // undecayed, charged particle
      HepMC::GenParticle* gp=evtMC->GetEvent()->barcode_to_particle( (*t).genpartIndex() );
      if (gp) {
	HepMC::GenVertex * gv=gp->production_vertex();
	if (gv) {
	  for ( HepMC::GenVertex::particle_iterator 
		  daughter =  gv->particles_begin(HepMC::descendants);
		daughter != gv->particles_end(HepMC::descendants);
		++daughter ) {
	    if (isFinalstateParticle(*daughter)){
	      ptsq+=(*daughter)->momentum().perp()*(*daughter)->momentum().perp();
	    }
	  }
	  //primary =  ( gv->position().t()==0 );
	  primary = true;
	  h_gen->Fill1d("genPart_cT", gv->position().t()); // mm
	  h_gen->Fill1d("genPart_T", gv->position().t()/299.792458); // ns

	  //resonance= ( gp->mother() && isResonance(gp->mother()));  // in CLHEP/HepMC days
	  // no more mother pointer in the improved HepMC GenParticle
	  resonance= ( isResonance(*(gp->production_vertex()->particles_in_const_begin())));
	  if (gp->status()==1){
	    //track=((pdt->particle(gp->pdg_id()))->charge() != 0);
	    track=not isCharged(gp);
	  }
	}
      }

      const HepMC::FourVector & v=(*simVtxs)[t->vertIndex()].position();
      //const HepLorentzVector & v=(*simVtxs)[t->vertIndex()].position();
      if(primary or resonance){
	{
	  // check all primaries found so far to avoid multiple entries
	  bool newVertex=true;
	  for(std::vector<PVStudy::simPrimaryVertex>::iterator v0=simpv.begin();
	      v0!=simpv.end(); v0++){
	    if( (fabs(v0->x-v.x())<0.001) && (fabs(v0->y-v.y())<0.001) && (fabs(v0->z-v.z())<0.001) ){
	      if (track) {
		v0->simTrackIndex.push_back(idx);
		if (ptsq>(*v0).ptsq){(*v0).ptsq=ptsq;}
	      }
	      newVertex=false;
	    }
	  }
	  if(newVertex && !resonance){
	    PVStudy::simPrimaryVertex anotherVertex(v.x(),v.y(),v.z());
	    if (track) anotherVertex.simTrackIndex.push_back(idx);
	    anotherVertex.ptsq=ptsq;
	    simpv.push_back(anotherVertex);
	  }
	}// 
      }
       
    }// simtrack has vertex and valid type
    idx++;
  }//simTrack loop
  return simpv;
}

// ------------ method called to for each event  ------------
void
PVStudy::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace reco;
  
  //========================================================================
  // Step 0: Prepare root variables and get information from the Event
  //========================================================================
  
  edm::LogInfo("Debug")<<"[PVStudy]"<<endl;
  // Initialize Root-tuple variables if needed
  SetVarToZero();
  
  // ====== TrackCollection
  static const reco::TrackCollection s_empty_trackColl;
  const reco::TrackCollection *trackColl = &s_empty_trackColl;
  edm::Handle<reco::TrackCollection> trackCollectionHandle;
  iEvent.getByLabel(trackCollectionTag_, trackCollectionHandle);
  if( iEvent.getByLabel(trackCollectionTag_, trackCollectionHandle)) {
    trackColl = trackCollectionHandle.product();
  } else {
    edm::LogInfo("Debug") << "[PVStudy] trackCollection cannot be found -> using empty collection of same type." <<endl;
  }
  // ====== splitTrackCollection1
  static const reco::TrackCollection s_empty_splitTrackColl1;
  const reco::TrackCollection *splitTrackColl1 = &s_empty_splitTrackColl1;
  edm::Handle<reco::TrackCollection> splitTrackCollection1Handle;
  iEvent.getByLabel(splitTrackCollection1Tag_, splitTrackCollection1Handle);
  if( iEvent.getByLabel(splitTrackCollection1Tag_, splitTrackCollection1Handle)) {
    splitTrackColl1 = splitTrackCollection1Handle.product();
  } else {
    edm::LogInfo("Debug") << "[PVStudy] splitTrackCollection1 cannot be found -> using empty collection of same type." <<endl;
  }
  // ====== splitTrackCollection2
  static const reco::TrackCollection s_empty_splitTrackColl2;
  const reco::TrackCollection *splitTrackColl2 = &s_empty_splitTrackColl2;
  edm::Handle<reco::TrackCollection> splitTrackCollection2Handle;
  iEvent.getByLabel(splitTrackCollection2Tag_, splitTrackCollection2Handle);
  if( iEvent.getByLabel(splitTrackCollection2Tag_, splitTrackCollection2Handle)) {
    splitTrackColl2 = splitTrackCollection2Handle.product();
  } else {
   edm::LogInfo("Debug") << "[PVStudy] splitTrackCollection2 cannot be found -> using empty collection of same type." <<endl;
  }
  
  // ======= PrimaryVertexCollection
  static const reco::VertexCollection s_empty_vertexColl;
  const reco::VertexCollection *vertexColl = &s_empty_vertexColl;
  edm::Handle<reco::VertexCollection> vertexCollectionHandle;
  iEvent.getByLabel(vertexCollectionTag_, vertexCollectionHandle);
  if( iEvent.getByLabel(vertexCollectionTag_, vertexCollectionHandle)) {
    vertexColl = vertexCollectionHandle.product();
  } else {
   edm::LogInfo("Debug") << "[PVStudy] vertexCollection cannot be found -> using empty collection of same type." <<endl;
  }
  // ====== splitVertexCollection1
  static const reco::VertexCollection s_empty_splitVertexColl1;
  const reco::VertexCollection *splitVertexColl1 = &s_empty_splitVertexColl1;
  edm::Handle<reco::VertexCollection> splitVertexCollection1Handle;
  iEvent.getByLabel(splitVertexCollection1Tag_, splitVertexCollection1Handle);
  if( iEvent.getByLabel(splitVertexCollection1Tag_, splitVertexCollection1Handle)) {
    splitVertexColl1 = splitVertexCollection1Handle.product();
  } else {
    edm::LogInfo("Debug") << "[PVStudy] splitVertexCollection1 cannot be found -> using empty collection of same type." <<endl;
  }
  // ====== splitVertexCollection2
  static const reco::VertexCollection s_empty_splitVertexColl2;
  const reco::VertexCollection *splitVertexColl2 = &s_empty_splitVertexColl2;
  edm::Handle<reco::VertexCollection> splitVertexCollection2Handle;
  iEvent.getByLabel(splitVertexCollection2Tag_, splitVertexCollection2Handle);
  if( iEvent.getByLabel(splitVertexCollection2Tag_, splitVertexCollection2Handle)) {
    splitVertexColl2 = splitVertexCollection2Handle.product();
  } else {
    edm::LogInfo("Debug") << "[PVStudy] splitVertexCollection2 cannot be found -> using empty collection of same type." <<endl;
  }

  
  // ======== BeamSpot accessors
  edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
  iEvent.getByLabel(bsSrc,recoBeamSpotHandle);
  reco::BeamSpot bs = *recoBeamSpotHandle;     
  const Point beamSpot = recoBeamSpotHandle.isValid() ? Point(recoBeamSpotHandle->x0(), recoBeamSpotHandle->y0(), recoBeamSpotHandle->z0()) : Point(0, 0, 0);
  
  edm::LogInfo("Debug")<<"[PVStudy] End accessing the track, beamSpot, primary vertex collections"<<endl;
  
  // ========== MC simvtx accessor
  if (!realData_) {
    edm::Handle<SimVertexContainer> simVtxs;
    iEvent.getByLabel( simG4_, simVtxs);
    
    edm::Handle<SimTrackContainer> simTrks;
    iEvent.getByLabel( simG4_, simTrks);
  }

  // ========== GET PDT
  try{
    iSetup.getData(pdt);
  }catch(const Exception&){
    edm::LogInfo("Debug") << "[PVStudy] Some problem occurred with the particle data table. This may not work !." <<endl;
  }
  
  // ======= Get MC information if needed 
  bool MC=false;  
  Handle<HepMCProduct> evtMC;
  if (!realData_) {
    iEvent.getByLabel("generator",evtMC);
    if (!evtMC.isValid()) {
      MC=false;
      edm::LogInfo("Debug") << "[PVStudy] no HepMCProduct found"<< endl;
    } else {
      edm::LogInfo("Debug") << "[PVStudy] generator HepMCProduct found"<< endl;
      MC=true;
    }
  }

  //========================================================================
  // Step 1:  Apply event cleaning for data and MC 
  //          WARNING: event selection cut are hard coded!!
  //========================================================================

  glob_runno_ = iEvent.id().run();
  glob_evtno_ = iEvent.id().event();
  glob_ls_   = iEvent.luminosityBlock();
  glob_bx_  = iEvent.bunchCrossing();   
  
  //========================================================================                                               
  // Fill trigger information

  iEvent.getByLabel(edm::InputTag("TriggerResults","","HLT"), triggerResultsH_);

  const edm::TriggerNames& trigNames = iEvent.triggerNames(*triggerResultsH_);   
 
  unsigned int nTriggers = triggerResultsH_->size();
  //  cout<<"N Triggers="<<nTriggers<<"\n";
  
  glob_trg_name_.clear();  
  glob_ntrg_=0;
  HLTtrigger_pass_=false;
  if (!realData_) HLTtrigger_pass_=true;
  if (!filterHLT_) HLTtrigger_pass_=true;

  if (realData_) {
  for(unsigned int i = 0; i < nTriggers; ++i)
    {
      if (triggerResultsH_->accept(i)) {
	glob_trg_idx_[glob_ntrg_]=i;
	glob_trg_name_.push_back(trigNames.triggerName(i));
	glob_ntrg_++;
	//	cout<<"Event passed trigger bit = "<<trigNames.triggerName(i)<<"\n";
      }
    }

  for ( vector<std::string>::const_iterator i = filterExpressions_.begin(), end = filterExpressions_.end(); i != end; ++i) 
    for ( vector<std::string>::const_iterator j = glob_trg_name_.begin(); j != glob_trg_name_.end(); ++j) 
      if ( (*i).find(*j)!=string::npos)  HLTtrigger_pass_=true; 

  //  if ((glob_ntrg_==2)&&(glob_trg_name_[0].find("Pixel")!=string::npos))  HLTtrigger_pass_=false;
  //  if ((glob_ntrg_==3)&&(glob_trg_name_[0].find("Pixel")!=string::npos)&&(glob_trg_name_[1].find("Pixel")!=string::npos))  HLTtrigger_pass_=false;
  }

  if (HLTtrigger_pass_) {

  //========================================================================
  // Step 2: Fill histograms for the splitting consistency checks
  //======================================================================== 
  
  // === Fill trackparameters of the input tracks to pvtx fitter
  edm::LogInfo("Debug")<<"[PVStudy] Start filling track parameters of the input tracks to pvtx fitter."<<endl;
  //fillTrackHisto(const reco::TrackCollection *trackColl, int datatype, const Point & bs) 
  // datatype: unsplittracks (0); splittracks1 (1);  splittracks2 (2);
  fillTrackHisto(trackColl, 0, beamSpot); 
  fillTrackHisto(splitTrackColl1, 1, beamSpot);
  fillTrackHisto(splitTrackColl2, 2, beamSpot); 
  edm::LogInfo("Debug")<<"[PVStudy] End filling track parameters of the input tracks to pvtx fitter."<<endl;
  
  
  // ==== Fill number of reconstructed vertices
  edm::LogInfo("Debug")<<"[PVStudy] Printing vertexCollection: "<<endl;
  edm::LogInfo("Debug")<<"[PVStudy] Printing splitVertexCollection1: "<<endl;
  edm::LogInfo("Debug")<<"[PVStudy] Printing splitVertexCollection2: "<<endl;
  edm::LogInfo("Debug")<<"[PVStudy] Start filling pvtx parameters."<<endl; 
  if (verbose_) {
    printRecVtxs(vertexCollectionHandle);
    printRecVtxs(splitVertexCollection1Handle);
    printRecVtxs(splitVertexCollection2Handle);
  }

  nrecPV_ = int (vertexColl->size());
  nrecPV1_spl_ = int (splitVertexColl1->size());
  nrecPV2_spl_ = int (splitVertexColl2->size());

  h_pvtrk->Fill1d("nrecPV", nrecPV_);
  h_pvtrk->Fill1d("nrecPV1_spl", nrecPV1_spl_);
  h_pvtrk->Fill1d("nrecPV2_spl", nrecPV2_spl_);
  h_misc->Fill1d("nrecPVDiff", double(nrecPV1_spl_)-double(nrecPV2_spl_));
  
  
  // ======= Fill track parameter ntuple/hist for tracks used in recoVertices
  //fillTrackHistoInPV(const reco::VertexCollection *vertexColl, int datatype, const Point & bs) {
  if(vertexColl->size() > 0 && vertexColl->begin()->isValid() && !(vertexColl->begin()->isFake())) 
    fillTrackHistoInPV(vertexColl, 0, beamSpot);
  
  // ======= Fill secondary/primary min separations for multi-vertices 
  if(vertexColl->size()>1 && vertexColl->begin()->isValid() && !(vertexColl->begin()->isFake()) ) {  
    double min_xsep = 9999.0; 
    double min_ysep = 9999.0; 
    double min_zsep = 9999.0;    
    double min_xsep_sign = 9999.0; 
    double min_ysep_sign = 9999.0; 
    double min_zsep_sign = 9999.0; 
    double min_ntrksep = 9999.0; 
    double min_sumpt2sep = 9999999.0; 
    
    edm::LogInfo("Debug")<<"[PVStudy] leading pvtx z = "<< vertexColl->begin()->z() <<endl;

    // Looping through the secondary vertices to find the mininum separation to the primary
    for(reco::VertexCollection::const_iterator v=vertexColl->begin() + 1 ; 
	v!=vertexColl->end(); ++v) {  
      if(v->isValid() && ! v->isFake()) {
	if(fabs(v->x()- vertexColl->begin()->x())<min_xsep) 
	  min_xsep = fabs(v->x()- vertexColl->begin()->x());
	if(fabs(v->y()- vertexColl->begin()->y())<min_ysep) 
	  min_ysep = fabs(v->y()- vertexColl->begin()->y());
	if(fabs(v->z()- vertexColl->begin()->z())<min_zsep) 
	  min_zsep = fabs(v->z()- vertexColl->begin()->z());
	double xsep_sign = fabs(v->x()- vertexColl->begin()->x())/max(v->xError(), vertexColl->begin()->xError()); 
	if(xsep_sign < min_xsep_sign) 
	  min_xsep_sign = xsep_sign; 
	double ysep_sign = fabs(v->y()- vertexColl->begin()->y())/max(v->yError(), vertexColl->begin()->yError()); 
	if(ysep_sign < min_ysep_sign) 
	  min_ysep_sign = ysep_sign; 
	double zsep_sign = fabs(v->z()- vertexColl->begin()->z())/max(v->zError(), vertexColl->begin()->zError()); 
	if(zsep_sign < min_zsep_sign) 
	  min_zsep_sign = zsep_sign; 
	if( (double(vertexColl->begin()->tracksSize()) - double(v->tracksSize())) < min_ntrksep)
	  min_ntrksep = double(vertexColl->begin()->tracksSize()) - double(v->tracksSize()); 
	if(fabs(sumPtSquared(*v) - sumPtSquared(*(vertexColl->begin()))) < min_sumpt2sep) 
	  min_sumpt2sep = fabs(sumPtSquared(*v) - sumPtSquared(*(vertexColl->begin())));
      }
    }
    h_misc->Fill1d("min_xsep", min_xsep); 
    h_misc->Fill1d("min_ysep", min_ysep); 
    h_misc->Fill1d("min_zsep", min_zsep); 
    h_misc->Fill1d("min_xsep_sign", min_xsep_sign); 
    h_misc->Fill1d("min_ysep_sign", min_ysep_sign); 
    h_misc->Fill1d("min_zsep_sign", min_zsep_sign); 
    h_misc->Fill1d("min_ntrksep", min_ntrksep); 
    h_misc->Fill1d("min_sumpt2sep", min_sumpt2sep); 

    min_zsep_ = min_zsep;
    min_ntrksep_ = min_ntrksep;
    min_sumpt2sep_ = min_sumpt2sep;
  } // End of  if(vertexColl->size()>1) { 
  
  edm::LogInfo("Debug")<<"[PVStudy] End filling pvtx parameters."<<endl;

  //========================================================================
  // Step 3:  PrimaryVertex Matching
  //   1. In z  |z1-z2|< zsigncut * max(sigmaz1, sigmaz2)
  //   2. |(nTrkPV1 - nTrkPV2)/(nTrkPV1+nTrkPV2)|<ntrkdiffcut_
  //   3. The first match is the primary vertex, which has largest sum(pT^2) 
  //========================================================================
  
  edm::LogInfo("Debug")<<"[PVStudy] matching pvtxs "<<endl;
  reco::VertexCollection s_empty_matchedVertexColl1;
  reco::VertexCollection *matchedVertexColl1 = &s_empty_matchedVertexColl1; 
  matchedVertexColl1->reserve(splitVertexColl1->size()); 
  reco::VertexCollection s_empty_matchedVertexColl2;
  reco::VertexCollection *matchedVertexColl2 = &s_empty_matchedVertexColl2; 
  matchedVertexColl2->reserve(splitVertexColl2->size());
  
  bool stopmatching = false; 
  for (size_t ivtx = 0; ivtx<splitVertexCollection1Handle->size() && !stopmatching; ++ivtx) {
    reco::VertexRef recvtx1(splitVertexCollection1Handle, ivtx);
    if( !(recvtx1->isValid()) || recvtx1->isFake()) break;
    for (size_t jvtx = ivtx; jvtx < splitVertexCollection2Handle->size(); ++jvtx) {    
      //------------------------------------------------------------------------
      //== If only considering matching the first vertex of the splitVertexColl
      //------------------------------------------------------------------------
      //      if (ivtx!=0 || jvtx!=0) { stopmatching = true; break; } 
      reco::VertexRef recvtx2(splitVertexCollection2Handle, jvtx);
      if( !(recvtx2->isValid()) || recvtx2->isFake()) break;
      edm::LogInfo("Debug")<<"[PVStudy] Matching splitVertexColl1: #  "<< ivtx<< " and splitVertexColl1: # "<<jvtx<<endl;
      edm::LogInfo("Debug")<<"[PVStudy] recvtx1->z() = " << recvtx1->z() << "+/- "<< recvtx1->zError() << "." << endl;
      edm::LogInfo("Debug")<<"[PVStudy] recvtx2->z() = " << recvtx2->z() << "+/- "<< recvtx2->zError() << "." << endl;
      
      double twovtxsig = (recvtx1->z()-recvtx2->z())/max(recvtx1->zError(), recvtx2->zError());
      h_misc->Fill1d("twovtxzsign", twovtxsig);
      if(matchVertex(recvtx1, recvtx2, zsigncut_)) {
	edm::LogInfo("Debug")<<"[PVStudy] The two splitted vertices match in Z. "<<endl;

	int nTrkPV1, nTrkPV2;
	if(useHWTrk_) {
	  nTrkPV1 = nHWTrkRecVtx(*recvtx1);
	  nTrkPV2 = nHWTrkRecVtx(*recvtx2);
	}
	else {
	  nTrkPV1 = recvtx1->tracksSize();
	  nTrkPV2 = recvtx2->tracksSize(); 
	}
	double ntrkreldiff = double(nTrkPV1-nTrkPV2)/double(nTrkPV1+nTrkPV2);
	h_misc->Fill1d("nTrkPVDiff", nTrkPV1-nTrkPV2);
	h_misc->Fill1d("nTrkPVRelDiff", ntrkreldiff);
	if(fabs(ntrkreldiff)<ntrkdiffcut_) {	
	  edm::LogInfo("Debug")<<"[PVStudy] (nTrkPV1-nTrkPV2)/(nTrkPV1+nTrkPV2) = " << ntrkreldiff<<endl;

	  matchedVertexColl1->push_back(*recvtx1);
	  matchedVertexColl2->push_back(*recvtx2);
	  // stopmatching = true; // stop the matching when the first match is found!
	  //	  break;
	}
	else 
	  edm::LogInfo("Debug")<<"WARNING: (nTrkPV1-nTrkPV2)/(nTrkPV1+nTrkPV2) = " << ntrkreldiff<<", exceeding cut "<<ntrkdiffcut_<<endl; 
      }
      else {
	edm::LogInfo("Debug")<<"WARNING: The two reconstructed vertices do not match in z: "<<endl;
	edm::LogInfo("Debug")<<"recvtx1->z() = " << recvtx1->z() << "+/- "<< recvtx1->zError() << "." << endl;
	edm::LogInfo("Debug")<<"recvtx2->z() = " << recvtx2->z() << "+/- "<< recvtx2->zError() << "." << endl;	
      }
    }
  } 
  edm::LogInfo("Debug")<<"[PVStudy] End matching pvtxs"<<endl;

  //========================================================================
  // Step 4: Analyze matchedVertexColls
  //========================================================================
  edm::LogInfo("Debug")<<"[PVStudy] Begin analyzing the matchedVertexColl with size = "<< matchedVertexColl1->size()<< endl;
  
  // ==== If it is MC, analyze the res/pull of the unsplit vertexColl first
  if(MC){
    // make a list of primary vertices:
    std::vector<simPrimaryVertex> simpv;
    simpv=getSimPVs(evtMC,"");
    //     simpv=getSimPVs(evtMC, simVtxs, simTrks);
    h_gen->Fill1d("nsimPV", simpv.size());
    nsimPV_ = simpv.size();
    int isimpv = 0; 
    for(std::vector<simPrimaryVertex>::iterator vsim=simpv.begin();
	vsim!=simpv.end(); vsim++, isimpv++){
      nsimTrkPV_[isimpv]  =vsim->nGenTrk;
      simx_[isimpv] = vsim->x;
      simy_[isimpv] = vsim->y;
      simz_[isimpv] = vsim->z;
      simptsq_[isimpv] = vsim->ptsq;
      fillMCHisto(vsim, isimpv, vertexColl, 3, avgTrkPtInPVMin_, avgTrkPtInPVMax_);
    }
  } 
  
  // Compare the reconstructed vertex position and calculate resolution/pulls
  if(matchedVertexColl1->size() != 0 && matchedVertexColl2->size() != 0) {  
    // ==== Analyze the splitted vtx using MC method
    if(MC){
      // make a list of primary vertices:
      std::vector<simPrimaryVertex> simpv;
      simpv=getSimPVs(evtMC,"");
      //     simpv=getSimPVs(evtMC, simVtxs, simTrks);
      int isimpv = 0; 
      for(std::vector<simPrimaryVertex>::iterator vsim=simpv.begin();
	  vsim!=simpv.end(); vsim++, isimpv++){
	fillMCHisto(vsim, isimpv, matchedVertexColl1, 1, avgTrkPtInPVMin_, avgTrkPtInPVMax_);
	fillMCHisto(vsim, isimpv, matchedVertexColl2, 2, avgTrkPtInPVMin_, avgTrkPtInPVMax_);
      } 
    } 

    // ==== Analyze res/pull two-vertex method
    //fillPvtxHisto(const reco::VertexCollection *vertexColl, int datatype)
    fillTrackHistoInPV(matchedVertexColl1, 1, beamSpot);
    fillTrackHistoInPV(matchedVertexColl2, 2, beamSpot);

    reco::VertexCollection::const_iterator v1; 
    reco::VertexCollection::const_iterator v2; 

    nrecPV_twovtx_ = 0;
    for(v1 = matchedVertexColl1->begin(), v2 = matchedVertexColl2->begin(); 
	v1!=matchedVertexColl1->end(), v2 != matchedVertexColl2->end(); 
	++v1, ++v2) {
      
      // ==================================================================
      // With the option to fill the histograms at a given average pT range
      // ==================================================================
      if (avgPtRecVtx(*v1) < avgTrkPtInPVMin_ || avgPtRecVtx(*v1) > avgTrkPtInPVMax_ ) continue;
      if (avgPtRecVtx(*v2) < avgTrkPtInPVMin_ || avgPtRecVtx(*v2) > avgTrkPtInPVMax_ ) continue;

      h_misc->Fill1d("ndofPVDiff", v1->ndof() - v2->ndof());
      h_misc->Fill1d("ndofPVRelDiff", (v1->ndof()-v2->ndof())/(v1->ndof()+v2->ndof()));
      fres_[0] = (v1->x()-v2->x())/sqrt(2.0);
      fres_[1] = (v1->y()-v2->y())/sqrt(2.0);
      fres_[2] = (v1->z()-v2->z())/sqrt(2.0);
      ferror_[0] = sqrt(pow(v1->xError(),2)+pow(v2->xError(),2))/sqrt(2);
      ferror_[1] = sqrt(pow(v1->yError(),2)+pow(v2->yError(),2))/sqrt(2);
      ferror_[2] = sqrt(pow(v1->zError(),2)+pow(v2->zError(),2))/sqrt(2);
      
      int nTrkPV1, nTrkPV2;
      if(useHWTrk_) {
	nTrkPV1 = nHWTrkRecVtx(*v1);
	nTrkPV2 = nHWTrkRecVtx(*v2); 
      }
      else {
	nTrkPV1 = v1->tracksSize();
	nTrkPV2 = v2->tracksSize();
      }
      
      fntrk_ = (nTrkPV1 + nTrkPV2)/2;
      
      h_summary->Fill1d("deltax", fres_[0] );
      h_summary->Fill1d("deltay", fres_[1] );
      h_summary->Fill1d("deltaz", fres_[2] );
      h_summary->Fill1d("pullx", fres_[0]/ferror_[0]);
      h_summary->Fill1d("pully", fres_[1]/ferror_[1]);
      h_summary->Fill1d("pullz", fres_[2]/ferror_[2]);
      h_summary->Fill1d("errPVx", ferror_[0] );
      h_summary->Fill1d("errPVy", ferror_[1] );
      h_summary->Fill1d("errPVz", ferror_[2] );
      pvinfo.push_back(PVStudy::PVInfo(res(fres_[0], fres_[1], fres_[2]),
				       error(ferror_[0], ferror_[1], ferror_[2]),
				       fntrk_));
      // Fill histo according to its track multiplicity, set datamode = 0 for pvtx using all tracks

      fillHisto(res(fres_[0], fres_[1], fres_[2]),
		error(ferror_[0], ferror_[1], ferror_[2]),
		fntrk_,0);  

      // Fill the ntuple variables
      nTrkPV_twovtx_[nrecPV_twovtx_] = fntrk_;
      deltax_twovtx_[nrecPV_twovtx_] = fres_[0];
      deltay_twovtx_[nrecPV_twovtx_] = fres_[1];
      deltaz_twovtx_[nrecPV_twovtx_] = fres_[2];
      errx_twovtx_[nrecPV_twovtx_] = ferror_[0];
      erry_twovtx_[nrecPV_twovtx_] = ferror_[1];
      errz_twovtx_[nrecPV_twovtx_] = ferror_[2];
      pullx_twovtx_[nrecPV_twovtx_] = fres_[0]/ferror_[0];
      pully_twovtx_[nrecPV_twovtx_] = fres_[1]/ferror_[1]; 
      pullz_twovtx_[nrecPV_twovtx_] = fres_[2]/ferror_[2];     


      //SplittedVertex
      nTrkPV1_spl_twovtx_[nrecPV_twovtx_] = nTrkPV1;
      ndofPV1_spl_twovtx_[nrecPV_twovtx_] = v1->ndof(); 
      normchi2PV1_spl_twovtx_[nrecPV_twovtx_] = v1->normalizedChi2();
      avgPtPV1_spl_twovtx_[nrecPV_twovtx_] = avgPtRecVtx(*v1); 
      errx1_spl_twovtx_[nrecPV_twovtx_] = v1->xError(); 
      erry1_spl_twovtx_[nrecPV_twovtx_] = v1->yError(); 
      errz1_spl_twovtx_[nrecPV_twovtx_] = v1->zError(); 
      
      nTrkPV2_spl_twovtx_[nrecPV_twovtx_] = nTrkPV2;
      ndofPV2_spl_twovtx_[nrecPV_twovtx_] = v2->ndof(); 
      normchi2PV2_spl_twovtx_[nrecPV_twovtx_] = v2->normalizedChi2();
      avgPtPV2_spl_twovtx_[nrecPV_twovtx_] = avgPtRecVtx(*v2); 
      errx2_spl_twovtx_[nrecPV_twovtx_] = v2->xError(); 
      erry2_spl_twovtx_[nrecPV_twovtx_] = v2->yError(); 
      errz2_spl_twovtx_[nrecPV_twovtx_] = v2->zError(); 
  
      nrecPV_twovtx_++;
      
      // Print some information of the two tracks events
      if(verbose_ && fntrk_ < 4) {
	cout<<"Printing matchedVertexColl1 for low ntrack bins"<<endl;
	printRecVtx(*v1);
	cout<<"Printing matchedVertexColl1 for low ntrack bins"<<endl;
	printRecVtx(*v2);
      }
    } // End of analyzing res/pull
  } // End of  if(matchedVertexColl1->size() == 1 && matchedVertexColl2->size() == 1 ) {
  else 
    edm::LogInfo("Debug")<<"[PVStudy] WARNING: Cannot find matching pvtxs"<<endl;
  
  edm::LogInfo("Debug")<<"[PVStudy] End analyzing the matchedVertexColl"<<endl;


  // Fill Jet stuff
  //  ApplyJetStuff(iEvent,iSetup); 

  //========================================================================
  // Step 5: Fill ntuple if saventuple_ is on
  //========================================================================
  if(saventuple_) {
    ftree_->Fill();
    pvtxtree_->Fill();
  }


  }   
}


  
// ------------ method called once each job just before starting event loop  ------------
void 
PVStudy::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PVStudy::endJob() {
  edm::LogInfo("Analysis") << "[endJob] Analyzing PV info" << endl;
  // Looping through the datamodes available
  for (vector<int>::const_iterator it= datamode.begin(); it != datamode.end() ; ++it) {  
    string suffix; 
    edm::LogInfo("Analysis")<<"[endJob] datamode = "<< *it<<endl;
    switch(*it) {
    case 0: suffix = "";
      break;
    case 1: suffix = "_spl1_mct";
      break;
    case 2: suffix = "_spl2_mct";	
      break;
    case 3: suffix = "_mct";
      break;
    }
    suffix += "_nTrk";
    if(analyze_) {
      for (int ntrk=nTrkMin_;ntrk<=nTrkMax_;++ntrk) {
	edm::LogInfo("Analysis") << "[endJob] ntrk = " << ntrk << endl;
	PVStudy::PVAnaInfo ipv = GetPVAnaInfo(ntrk,*it);
 	if ( ipv.res_.x() > 0 ) h_ana->Fill2d("resx"+suffix, ntrk,ipv.res_.x());
	if ( ipv.res_.y() > 0 ) h_ana->Fill2d("resy"+suffix, ntrk,ipv.res_.y());
	if ( ipv.res_.z() > 0 ) h_ana->Fill2d("resz"+suffix, ntrk,ipv.res_.z());
	if ( ipv.pull_.x() > 0 ) h_ana->Fill2d("pullx"+suffix, ntrk,ipv.pull_.x());
	if ( ipv.pull_.y() > 0 ) h_ana->Fill2d("pully"+suffix, ntrk,ipv.pull_.y());
	if ( ipv.pull_.z() > 0 ) h_ana->Fill2d("pullz"+suffix, ntrk,ipv.pull_.z());
      }
    }
    suffix.clear();
  }
  if(saventuple_) {
    file_->cd();
    ftree_->Write();
    pvtxtree_->Write();
    file_->Close();
  }
  

}

// Match a recovertex with a simvertex
// ? Should the cut be parameter dependent? 
// cut can be within n sigma of the vertex.zerror()
bool PVStudy::matchVertex(const PVStudy::simPrimaryVertex & vsim, 
			  const reco::Vertex & vrec)
{
  if(vrec.isFake() || !vrec.isValid()) return false;
  else
    return true;
    //return (fabs(vsim.z-vrec.z())<0.0500); // =500um // remove this hard cut
}

// Match two reconstructed vertices
bool PVStudy::matchVertex( const reco::VertexRef & vrec1, 
			   const reco::VertexRef & vrec2, 
			   int zsigncut)
{
  double cut = zsigncut*max(vrec1->zError(), vrec2->zError()); 
  edm::LogInfo("Debug")<<"[matchVertex] The matching criteria in z is "<<cut<<endl;
  return (fabs(vrec1->z()-vrec2->z())<cut);
}


bool PVStudy::isResonance(const HepMC::GenParticle * p){
  double ctau=(pdt->particle( abs(p->pdg_id()) ))->lifetime();
  edm::LogInfo("Debug") << "[isResonance] isResonance   " << p->pdg_id() << " " << ctau << endl;
  return  ctau >0 && ctau <1e-6;
}

bool PVStudy::isFinalstateParticle(const HepMC::GenParticle * p){
  return ( !p->end_vertex() && p->status()==1 );
}

bool PVStudy::isCharged(const HepMC::GenParticle * p){
  const ParticleData * part = pdt->particle( p->pdg_id() );
  if (part){
    return part->charge()!=0;
  }else{
    // the new/improved particle table doesn't know anti-particles
    return  pdt->particle( -p->pdg_id() )!=0;
  }
}

void PVStudy::printSimVtxs(const Handle<SimVertexContainer> simVtxs){
  int i=0;
  for(SimVertexContainer::const_iterator vsim=simVtxs->begin();
      vsim!=simVtxs->end(); ++vsim){
    cout << i++ << ")" << scientific
	 << " evtid=" << vsim->eventId().event() 
	 << " sim x=" << vsim->position().x()
	 << " sim y=" << vsim->position().y()
	 << " sim z=" << vsim->position().z()
	 << " sim t=" << vsim->position().t()
	 << " parent=" << vsim->parentIndex() 
	 << endl;
  }
}

void PVStudy::printRecVtxs(const Handle<reco::VertexCollection> recVtxs){
  int ivtx=0;
  for(reco::VertexCollection::const_iterator v=recVtxs->begin(); 
      v!=recVtxs->end(); ++v){
    cout << "Recvtx "<< std::setw(3) << std::setfill(' ')<<ivtx++
	 << "#trk " << std::setw(3) << v->tracksSize() 
	 << " chi2 " << std::setw(4) << v->chi2() 
	 << " ndof " << std::setw(3) << v->ndof() << endl 
	 << " x "  << std::setw(8) <<std::fixed << std::setprecision(4) << v->x() 
	 << " dx " << std::setw(8) << v->xError()<< endl
	 << " y "  << std::setw(8) << v->y() 
	 << " dy " << std::setw(8) << v->yError()<< endl
	 << " z "  << std::setw(8) << v->z() 
	 << " dz " << std::setw(8) << v->zError()
	 << endl;
  }
}

void PVStudy::printRecVtx(const reco::Vertex & v){

  cout << "#trk " << std::setw(3) << v.tracksSize() 
	 << " chi2 " << std::setw(4) << v.chi2() 
	 << " ndof " << std::setw(3) << v.ndof() << endl 
	 << " x "  << std::setw(8) <<std::fixed << std::setprecision(4) << v.x() 
	 << " dx " << std::setw(8) << v.xError()<< endl
	 << " y "  << std::setw(8) << v.y() 
	 << " dy " << std::setw(8) << v.yError()<< endl
	 << " z "  << std::setw(8) << v.z() 
	 << " dz " << std::setw(8) << v.zError()
	 << endl;
}

void PVStudy::printSimTrks(const Handle<SimTrackContainer> simTrks){
  cout <<  " simTrks   type, (momentum), vertIndex, genpartIndex"  << endl;
  int i=1;
  for(SimTrackContainer::const_iterator t=simTrks->begin();
      t!=simTrks->end(); ++t){
    //HepMC::GenParticle* gp=evtMC->GetEvent()->particle( (*t).genpartIndex() );
    cout << i++ << ")" 
	 << (*t)
	 << " index="
	 << (*t).genpartIndex();
    //if (gp) {
    //  HepMC::GenVertex *gv=gp->production_vertex();
    //  cout  <<  " genvertex =" << (*gv);
    //}
    cout << endl;
  }
}

void PVStudy::fillHisto(res r, error er, int ntk, int datamode) {
  stringstream ss;
  ss << ntk;
  string suffix;
  if(datamode == 0 )  suffix = "_" + ss.str();
  if(datamode == 1 )  suffix = "_spl1_mct_" + ss.str();
  if(datamode == 2 )  suffix = "_spl2_mct_" + ss.str();
  if(datamode == 3 )  suffix = "_mct_" + ss.str();
  
  if (ntk < nTrkMin_ || ntk > nTrkMax_ ) return;
  // Fill histograms of res and pull of ntk
  h_others->Fill1d("deltax"+suffix, r.x());
  h_others->Fill1d("deltay"+suffix, r.y());
  h_others->Fill1d("deltaz"+suffix, r.z()); 
  h_others->Fill1d("pullx"+suffix, r.x()/er.x());
  h_others->Fill1d("pully"+suffix, r.y()/er.y());
  h_others->Fill1d("pullz"+suffix, r.z()/er.z());  
  h_others->Fill1d("errPVx"+suffix, er.x());
  h_others->Fill1d("errPVy"+suffix, er.y());
  h_others->Fill1d("errPVz"+suffix, er.z());
}


PVStudy::PVAnaInfo PVStudy::GetPVAnaInfo(int ntk, int datamode) {
  map<int,double> data;
  data.clear();
  double fpar[2] = {-999,-999};
  double cm2um = 10000;
  stringstream ss;
  ss << ntk;
  string suffix = ss.str(); 
  if(datamode == 0 )  suffix = "_" + suffix;
  if(datamode == 1 )  suffix = "_spl1_mct_" + suffix;
  if(datamode == 2 )  suffix = "_spl2_mct_" + suffix;
  if(datamode == 3 )  suffix = "_mct_" + suffix;
  
  if(analyze_) {
    for ( int i = 0; i < 6; ++i) {
      switch (i) {
      case 0:
	if (!h_others->ReadHisto1D("deltax"+suffix)) break;
	fit(h_others->ReadHisto1D("deltax"+suffix), fpar);
	data[i] = fpar[0]*cm2um;
	break;
      case 1:
	if (!h_others->ReadHisto1D("deltay"+suffix)) break;
	fit(h_others->ReadHisto1D("deltay"+suffix), fpar);
	data[i] = fpar[0]*cm2um;
	break;
      case 2:
	if (!h_others->ReadHisto1D("deltaz"+suffix)) break;
	fit(h_others->ReadHisto1D("deltaz"+suffix), fpar);
	data[i] = fpar[0]*cm2um;
	break;
      case 3:
	if (!h_others->ReadHisto1D("pullx"+suffix)) break;
	fit(h_others->ReadHisto1D("pullx"+suffix), fpar);
 	data[i] = fpar[0];
	break;
      case 4:
	if (!h_others->ReadHisto1D("pully"+suffix)) break;
	fit(h_others->ReadHisto1D("pully"+suffix), fpar);
	data[i] = fpar[0];
	break;
      case 5:
	if (!h_others->ReadHisto1D("pullz"+suffix)) break;
	fit(h_others->ReadHisto1D("pullz"+suffix), fpar);
 	data[i] = fpar[0];
	break;
      }
      if (data[i] > 0) edm::LogInfo("Analysis") << "[Analysis] data[" << i << "] = " << data[i] << (i<3?" micro m":" ") << endl;
    }
  }
  else
    for ( int i = 0; i < 6; ++i) {
      data[i] = -999;
    }

  return PVStudy::PVAnaInfo(res(data[0], data[1], data[2]),
			    error(0.,0.,0.),
			    pull(data[3], data[4], data[5]),
			    error(0.,0.,0.),
			    ntk);
}


void PVStudy::fit(TH1 *hdist, double fitPars[]){
  TAxis *axis0 = hdist->GetXaxis();
  int nbin = axis0->GetLast();
  double nOF = hdist->GetBinContent(nbin+1);
  double nUF = hdist->GetBinContent(0);
  //   double fitRange = axis0->GetBinUpEdge(nbin);
  // double fitRange = axis0->GetXmax();
  double fitRange = 2*hdist->GetRMS();
  double sigMax[2] = {0.,0.};

  edm::LogInfo("Analysis") << "[Fit] Last bin = " << nbin
			   << "; hdist: Overflow Entries = " << nOF
			   << "; Underflow Entries = " << nUF
			   << "; hdist->GetEntries() = " << hdist->GetEntries()
			   << "; fitting range = " << -fitRange << " to " << fitRange << endl;

  if (hdist->GetEntries() - nOF - nUF >= 10) { // FIXME: for reasonable Gaussian fit
    for (int bi = 0; bi < nbin; bi++) {
      if ( (axis0->GetBinLowEdge(bi) < 0) && (hdist->GetBinContent(bi) > 0) ) {
	sigMax[0] = axis0->GetBinLowEdge(bi);
	if ( abs(sigMax[0]) > abs(sigMax[1]) ) sigMax[1] = abs(sigMax[0]);
      }
      if ( (axis0->GetBinLowEdge(bi) >= 0) && (hdist->GetBinContent(bi) > 0) ) {
	sigMax[0] = axis0->GetBinUpEdge(bi);
	if ( abs(sigMax[0]) > abs(sigMax[1]) ) sigMax[1] = abs(sigMax[0]);
      }
    }
    //edm::LogInfo("Analysis") << "[Fit] Fit sigMax = " << sqrt(2.)*sigMax[1] << endl;

    TF1 *fgaus = new TF1("fgaus","gaus",-fitRange, fitRange);
    fgaus->SetParameter(1, 0.);
    fgaus->SetParLimits(1, -fitRange/10., fitRange/10.);
    fgaus->SetParLimits(2, 0., sqrt(2.)*sigMax[1]);
    fgaus->SetLineColor(4);
    hdist->Fit(fgaus,"QLRM");
    gPad->Update();
    TPaveStats *s = (TPaveStats*) hdist->GetListOfFunctions()->FindObject("stats");
    s->SetOptStat(1111111);
    s->SetOptFit(0111);
    gPad->Update();
    //edm::LogInfo("Analysis") << "[Fit] got function" << endl;
    fitPars[0] = ((fgaus->GetParameter(2))?fgaus->GetParameter(2):-999);
  }
  else 
    fitPars[0] = -999;
}


void PVStudy::fillTrackHisto(const reco::TrackCollection *trackColl, int datatype, const Point & bs) {
  string suffix;
  suffix = ""; // for unsplit tracks
  if(datatype == 1) suffix = "1_spl"; // for splittracks 1
  if(datatype == 2) suffix = "2_spl"; // for splittracks 2
  
  h_pvtrk->Fill1d("nTrk"+suffix, trackColl->size());
  for(reco::TrackCollection::const_iterator itTrack = trackColl->begin();
      itTrack != trackColl->end();                      
      ++itTrack) {
    h_pvtrk->Fill1d("trkPt"+suffix, itTrack->pt());
    h_pvtrk->Fill1d("trkDxy"+suffix, itTrack->dxy());
    h_pvtrk->Fill1d("trkDxyCorr"+suffix, itTrack->dxy(bs));
    h_pvtrk->Fill1d("trkDz"+suffix, itTrack->dz());
    h_pvtrk->Fill1d("trkEta"+suffix, itTrack->eta());
    h_pvtrk->Fill1d("trkPhi"+suffix, itTrack->phi());
  }
}

void PVStudy::fillTrackHistoInPV(const reco::VertexCollection *vertexColl, int datatype, const Point & bs) {
  string suffix;
  suffix = ""; // for unsplit tracks
  if(datatype == 1) suffix = "1_spl"; // for splittracks 1
  if(datatype == 2) suffix = "2_spl"; // for splittracks 2
  int ivtx = 0; 
  for(reco::VertexCollection::const_iterator v=vertexColl->begin(); 
      v!=vertexColl->end(); ++v, ++ivtx) {  
    if(!v->isFake()) {
      h_pvtrk->Fill1d("nTrkPV"+suffix, vertexColl->begin()->tracksSize());
      h_pvtrk->Fill1d("ndofPV"+suffix, vertexColl->begin()->ndof());   
      h_pvtrk->Fill1d("nHWTrkPV"+suffix, nHWTrkRecVtx(*v));
      try {
	for(reco::Vertex::trackRef_iterator t = vertexColl->begin()->tracks_begin(); 
	    t!=vertexColl->begin()->tracks_end(); t++) {
	  // illegal charge
	  if ( (**t).charge() < -1 || (**t).charge() > 1 ) {
	    // 	  h_pvtrk->Fill1d("trkPtPV", 0.);
	  }
	  else {
	    h_pvtrk->Fill1d("trkPtPV"+suffix, (**t).pt());
	    h_pvtrk->Fill1d("trkDxyPV"+suffix, (**t).dxy());  
	    h_pvtrk->Fill1d("trkDxyCorrPV"+suffix, (**t).dxy(bs));
	    h_pvtrk->Fill1d("trkDzPV"+suffix, (**t).dz());
	    h_pvtrk->Fill1d("trkEtaPV"+suffix, (**t).eta());
	    h_pvtrk->Fill1d("trkPhiPV"+suffix, (**t).phi());
	  }
	}
      }
      catch (...) {
	// exception thrown when trying to use linked track
	// 	  h_pvtrk->Fill1d("trkPtPV", 0.);
      }
    }
  }
}

void PVStudy::fillMCHisto(std::vector<simPrimaryVertex>::iterator vsim, int isimpv, const reco::VertexCollection *vtxColl, 
			  int datatype, double avgTrkPtInPVMin_, double avgTrkPtInPVMax_) 
{
  std::string suffix;
  // Set the vertexColl and histogram suffix according to datamode
  if (datatype == 1) {
    suffix = "_spl1_mct";
    nrecPV_spl1_mct_ = 0;   
  }
  if (datatype == 2) {
    suffix = "_spl2_mct";
    nrecPV_spl2_mct_ = 0;     
  } 
  if (datatype == 3) {
    suffix = "_mct"; 
    nrecPV_mct_ = 0;   
  }

  //========================================================
  //  For each simulated vertex, look for a match in the vertexColl
  //  If more than 1 recVtx is found, use the one with closest in Z
  //========================================================   
     
  // === look for a matching reconstructed vertex in vertexColl
  for(reco::VertexCollection::const_iterator vrec=vtxColl->begin(); 
      vrec!=vtxColl->end(); ++vrec){
    vsim->recVtx=NULL;  
    edm::LogInfo("Debug") << "[fillMCHisto] sim primary vertex x = " << vsim->x << "; y = " << vsim->y << "; z = " << vsim->z <<  endl;
    edm::LogInfo("Debug") << "[fillMCHisto] Is matched? " << (matchVertex(*vsim,*vrec)?"Yes":"No") << endl;
    
    if ( matchVertex(*vsim,*vrec)  ) {
      vsim->recVtx=&(*vrec);
      //if the matching critera are fulfilled, accept the rec-vertex that is closest in z
      //if(    ((vsim->recVtx) && (fabs(vsim->recVtx->position().z()-vsim->z)>fabs(vrec->z()-vsim->z)))
      //|| (!vsim->recVtx) )
      //vsim->recVtx=&(*vrec);
    }
  }
  
  // === If match found fill the residual and pulls
  if(vsim->recVtx) {
    edm::LogInfo("Debug") <<"[fillMCHisto] primary matched in vertexColl" << vsim->x << " " << vsim->y << " " << vsim->z <<  endl;
    double avgPtPV = avgPtRecVtx(*(vsim->recVtx));
    if(avgPtPV>avgTrkPtInPVMax_ || avgPtPV<avgTrkPtInPVMin_) return;

    // if study the resolution/pull for all tracks in the PVtx
    int nrectrk; 
    if(useHWTrk_)
      nrectrk=nHWTrkRecVtx(*(vsim->recVtx));
    else
      nrectrk =  int(vsim->recVtx->tracksSize()); 

    double fres_mct[3];
    double ferror_mct[3];
    
    double ndofPV = vsim->recVtx->ndof();
    double normchi2PV =  vsim->recVtx->normalizedChi2();

    fres_mct[0] = vsim->recVtx->x()-vsim->x;
    fres_mct[1] = vsim->recVtx->y()-vsim->y;
    fres_mct[2] = vsim->recVtx->z()-vsim->z;
    ferror_mct[0] = vsim->recVtx->xError();
    ferror_mct[1] = vsim->recVtx->yError();
    ferror_mct[2] = vsim->recVtx->zError();
    
    h_summary->Fill1d("deltax"+suffix, fres_mct[0] );
    h_summary->Fill1d("deltay"+suffix, fres_mct[1] );
    h_summary->Fill1d("deltaz"+suffix, fres_mct[2] );
    h_summary->Fill1d("pullx"+suffix, fres_mct[0]/ferror_mct[0] );
    h_summary->Fill1d("pully"+suffix, fres_mct[1]/ferror_mct[1] );
    h_summary->Fill1d("pullz"+suffix, fres_mct[2]/ferror_mct[2] );
    h_summary->Fill1d("errPVx"+suffix, ferror_mct[0] );
    h_summary->Fill1d("errPVy"+suffix, ferror_mct[1] );
    h_summary->Fill1d("errPVz"+suffix, ferror_mct[2] );
    pvinfo.push_back(PVStudy::PVInfo(res(fres_mct[0], fres_mct[1], fres_mct[2]),
				     error(ferror_mct[0], ferror_mct[1], ferror_mct[2]),
				     nrectrk));
    // Fill histo according to its track multiplicity
    fillHisto(res(fres_mct[0], fres_mct[1], fres_mct[2]),
	      error(ferror_mct[0], ferror_mct[1], ferror_mct[2]),
	      nrectrk, datatype);
    
    if(saventuple_) {
      //Fill the values for variables in pvtxtree_   
      if(datatype == 1) { 
	nTrkPV_spl1_mct_[nrecPV_spl1_mct_] =   nrectrk;
	ndofPV_spl1_mct_[nrecPV_spl1_mct_] =  ndofPV;
	normchi2PV_spl1_mct_[nrecPV_spl1_mct_] =  normchi2PV;
	avgPtPV_spl1_mct_[nrecPV_spl1_mct_] = avgPtPV;
	deltax_spl1_mct_[nrecPV_spl1_mct_] =  fres_mct[0]; 
	deltay_spl1_mct_[nrecPV_spl1_mct_] =  fres_mct[1]; 
	deltaz_spl1_mct_[nrecPV_spl1_mct_] =  fres_mct[2]; 
	pullx_spl1_mct_[nrecPV_spl1_mct_] = fres_mct[0]/ferror_mct[0];  
	pully_spl1_mct_[nrecPV_spl1_mct_] = fres_mct[1]/ferror_mct[1];
	pullz_spl1_mct_[nrecPV_spl1_mct_] =  fres_mct[2]/ferror_mct[2]; 
	errx_spl1_mct_[nrecPV_spl1_mct_] =  ferror_mct[0]; 
	erry_spl1_mct_[nrecPV_spl1_mct_] =  ferror_mct[1]; 
	errz_spl1_mct_[nrecPV_spl1_mct_] =  ferror_mct[2];
	nrecPV_spl1_mct_++;
      }
      if(datatype == 2) { 
	nTrkPV_spl2_mct_[nrecPV_spl2_mct_] = nrectrk;	    
	ndofPV_spl2_mct_[nrecPV_spl2_mct_] =  ndofPV;
	normchi2PV_spl2_mct_[nrecPV_spl2_mct_] =  normchi2PV;
	avgPtPV_spl2_mct_[nrecPV_spl2_mct_] = avgPtPV;
	deltax_spl2_mct_[nrecPV_spl2_mct_] =  fres_mct[0]; 
	deltay_spl2_mct_[nrecPV_spl2_mct_] =  fres_mct[1]; 
	deltaz_spl2_mct_[nrecPV_spl2_mct_] =  fres_mct[2]; 
	pullx_spl2_mct_[nrecPV_spl2_mct_] = fres_mct[0]/ferror_mct[0];  
	pully_spl2_mct_[nrecPV_spl2_mct_] = fres_mct[1]/ferror_mct[1];
	pullz_spl2_mct_[nrecPV_spl2_mct_] =  fres_mct[2]/ferror_mct[2]; 	 
	errx_spl2_mct_[nrecPV_spl2_mct_] =  ferror_mct[0]; 
	erry_spl2_mct_[nrecPV_spl2_mct_] =  ferror_mct[1]; 
	errz_spl2_mct_[nrecPV_spl2_mct_] =  ferror_mct[2];
	nrecPV_spl2_mct_++;
      }
      if(datatype == 3) {	
	nTrkPV_mct_[nrecPV_mct_] = nrectrk;
	ndofPV_mct_[nrecPV_mct_] =  ndofPV;
	normchi2PV_mct_[nrecPV_mct_] =  normchi2PV;
	avgPtPV_mct_[nrecPV_mct_] = avgPtPV;
	deltax_mct_[nrecPV_mct_] =  fres_mct[0]; 
	deltay_mct_[nrecPV_mct_] =  fres_mct[0]; 
	deltaz_mct_[nrecPV_mct_] =  fres_mct[0]; 
	pullx_mct_[nrecPV_mct_] = fres_mct[0]/ferror_mct[0];  
	pully_mct_[nrecPV_mct_] = fres_mct[1]/ferror_mct[1];
	pullz_mct_[nrecPV_mct_] =  fres_mct[2]/ferror_mct[2]; 
	errx_mct_[nrecPV_mct_] =  ferror_mct[0]; 
	erry_mct_[nrecPV_mct_] =  ferror_mct[1]; 
	errz_mct_[nrecPV_mct_] =  ferror_mct[2];	  
	nrecPV_mct_++;
      }
    } // End of  if(saventuple_) {
  } //  if ( matchVertex(*vsim,*vrec) ) {
  else {  // no rec vertex found for this simvertex
    edm::LogInfo("Debug") <<"[fillMCHisto] primary not found " << vsim->x << " " << vsim->y << " " << vsim->z << " nGenTrk=" << vsim->nGenTrk << endl;
  }
}

														   
void PVStudy::SetVarToZero() {
  fntrk_ = 0;
  //pvtx position (x,y,z) residual and error
  for(int i = 0; i<3;i++)    {
    fres_[i] = 0;
    ferror_[i] = 0;
  }
  
  // Event level information
  glob_runno_ = 0;
  glob_evtno_ = 0;
  glob_ls_ = 0;
  glob_bx_ = 0;

  glob_ntrg_=0;
  for (int i = 0; i < nMaxTrg_; i++) 
    {
      glob_trg_idx_[i]=0;
    }
  glob_trg_name_.clear();  

  nrecPV_ = 0;
  nrecPV1_spl_ = 0; 
  nrecPV2_spl_ = 0;
  nrecPV_twovtx_ = 0;
  nsimPV_ = 0;

  nrecPV_mct_ = 0;
  nrecPV_spl1_mct_ = 0;
  nrecPV_spl2_mct_ = 0;

  // Mininum separation between the secondary pvtxes and leading pvtx
  min_zsep_ = 9999.0;
  min_ntrksep_ = 9999.0;
  min_sumpt2sep_ = -9999.0;
  
  // Variables filled per Vertex
  for (int i = 0; i < nMaxPVs_; i++) {
    // matched two-vertices 
    nTrkPV1_spl_twovtx_[i] = 0;
    ndofPV1_spl_twovtx_[i] = 0;
    normchi2PV1_spl_twovtx_[i] = 0;
    avgPtPV1_spl_twovtx_[i] = 0;
    errx1_spl_twovtx_[i] = 0;
    erry1_spl_twovtx_[i] = 0;
    errz1_spl_twovtx_[i] = 0;

    nTrkPV2_spl_twovtx_[i] = 0;
    ndofPV2_spl_twovtx_[i] = 0;
    normchi2PV2_spl_twovtx_[i] = 0;
    avgPtPV2_spl_twovtx_[i] = 0;
    errx2_spl_twovtx_[i] = 0;
    erry2_spl_twovtx_[i] = 0;
    errz2_spl_twovtx_[i] = 0;
    
    deltax_twovtx_[i] = 0;
    deltay_twovtx_[i] = 0;
    deltaz_twovtx_[i] = 0;
    errx_twovtx_[i] = 0;
    erry_twovtx_[i] = 0;
    errz_twovtx_[i] = 0;
    pullx_twovtx_[i] = 0;
    pully_twovtx_[i] = 0;
    pullz_twovtx_[i] = 0;
    
    //simpv 
    nsimTrkPV_[i] = 0; 
    simx_[i] = 0;
    simy_[i] = 0;
    simz_[i] = 0;
    simptsq_[i] = 0;
    
    // residual and pulls with mc truth required
    deltax_mct_[i] = 0;
    deltay_mct_[i] = 0;
    deltaz_mct_[i] = 0;
    pullx_mct_[i] = 0;
    pully_mct_[i] = 0;
    pullz_mct_[i] = 0;
    nTrkPV_mct_[i] = 0;
    ndofPV_mct_[i] = 0;
    normchi2PV_mct_[i] = 0;
    avgPtPV_mct_[i] = 0;
    errx_mct_[i] = 0;
    erry_mct_[i] = 0;
    errz_mct_[i] = 0;
    
    deltax_spl1_mct_[i] = 0;
    deltay_spl1_mct_[i] = 0;
    deltaz_spl1_mct_[i] = 0;
    pullx_spl1_mct_[i] = 0;
    pully_spl1_mct_[i] = 0;
    pullz_spl1_mct_[i] = 0;
    nTrkPV_spl1_mct_[i] = 0;
    ndofPV_spl1_mct_[i] = 0;
    normchi2PV_spl1_mct_[i] = 0; 
    avgPtPV_spl1_mct_[i] = 0;
    errx_spl1_mct_[i] = 0;
    erry_spl1_mct_[i] = 0;
    errz_spl1_mct_[i] = 0;

    deltax_spl2_mct_[i] = 0;
    deltay_spl2_mct_[i] = 0;
    deltaz_spl2_mct_[i] = 0;
    pullx_spl2_mct_[i] = 0;
    pully_spl2_mct_[i] = 0;
    pullz_spl2_mct_[i] = 0;
    nTrkPV_spl2_mct_[i] = 0;
    ndofPV_spl2_mct_[i] = 0;
    normchi2PV_spl2_mct_[i] = 0;
    avgPtPV_spl2_mct_[i] = 0;
    errx_spl2_mct_[i] = 0;
    erry_spl2_mct_[i] = 0;
    errz_spl2_mct_[i] = 0;

  }
}

double PVStudy::sumPtSquared(const reco::Vertex & v)  {
  if(v.isFake() || (!v.isValid()) || v.tracksSize() == 0) return 0.0;
  else {
    double sum = 0.;
    double pT;
    for (reco::Vertex::trackRef_iterator it = v.tracks_begin(); it != v.tracks_end(); it++) {
      pT = (**it).pt();
      sum += pT*pT;
    }
    return sum;
  }
}



double PVStudy::avgPtRecVtx(const reco::Vertex & v)  {

  if(v.isFake() || !v.isValid() || v.tracksSize()==0 ) return 0;
  else {
    int nHWTrk = 0;
    double sumpT = 0.;
    for (reco::Vertex::trackRef_iterator it = v.tracks_begin(); it != v.tracks_end(); it++) {
      if(v.trackWeight(*it) > 0.5 ) {
	sumpT +=  (**it).pt();
	nHWTrk++;
      }
  }
    if(nHWTrk > 0)
      return sumpT/double(nHWTrk);
    else 
      return 0;
  }
}

int PVStudy::nHWTrkRecVtx(const reco::Vertex & v)  {
  int nHWTrkPV = 0;
  for (reco::Vertex::trackRef_iterator it = v.tracks_begin(); it != v.tracks_end(); it++) {
    if(v.trackWeight(*it) > 0.5) 
      nHWTrkPV++;
  }
  return nHWTrkPV;
}


void PVStudy::ApplyJetStuff(const edm::Event& iEvent, const edm::EventSetup& iSetup){


  Handle< View<reco::CaloJet> > uncorJetsHandle;
  iEvent.getByLabel(uncorJetsInputTag_, uncorJetsHandle);
  unsigned int  jetN = 0;

  const JetCorrector* correctorL2L3		= JetCorrector::getJetCorrector (CaloJetCorrectorL2L3_          , iSetup);
  const JetCorrector* correctorL1L2L3		= JetCorrector::getJetCorrector (CaloJetCorrectorL1L2L3_	, iSetup);

  vector<TLorentzVector>  CaloJets;
  TLorentzVector jet;

  for (View<reco::CaloJet>::const_iterator it = uncorJetsHandle->begin(); it != uncorJetsHandle->end(); it++) {

    unsigned int idx = it - uncorJetsHandle->begin();
    edm::RefToBase<reco::Jet> jetRef(edm::Ref<View<reco::CaloJet> >(uncorJetsHandle,idx));

    double cor= correctorL2L3      ->correction(*it, jetRef, iEvent, iSetup);
    double corL1L2L3= correctorL1L2L3    ->correction(*it, jetRef, iEvent, iSetup);

    CaloJet_px_[jetN]          =   LorentzVector(it->p4()).Px()*cor;
    CaloJet_py_[jetN]          =   LorentzVector(it->p4()).Py()*cor;
    CaloJet_pz_[jetN]          =   LorentzVector(it->p4()).Pz()*cor;

    jet.SetXYZM(  CaloJet_px_[jetN], CaloJet_py_[jetN],  CaloJet_pz_[jetN], 0.0);
    CaloJets.push_back(jet);

    jetN++;
      
      }

  nCaloJets_=jetN;
  
  sort(CaloJets.begin(), CaloJets.end(), mysorter);
    
  if (CaloJets.size()>=2)
    DijetBalance_  = fabs( CaloJets[0].Pt()- CaloJets[1].Pt() )/( CaloJets[0].Pt() + CaloJets[1].Pt() );
  else
    DijetBalance_  = 1000.0;
  
}


