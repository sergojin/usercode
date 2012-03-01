#ifndef PVStudy_h
#define PVStudy_h

// -*- C++ -*-
//
// Package:    PVStudy
// Class:      PVStudy
// 
/**\class PVStudy PVStudy.h UserCode/PVStudy/interface/PVStudy.h

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  "Geng-yuan Jeng/UC Riverside"
//                   "Yanyan Gao/Fermilab ygao@fnal.gov"
//                   "Sergo Jindariani/Fermilab sergo@fnal.gov"
//
//


// system include files
#include <memory>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "DataFormats/Math/interface/Point3D.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "UserCode/PVStudy/interface/PVHistograms.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

//needed for MessageLogger
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//generator level
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
 
// vertex stuff
#include <DataFormats/VertexReco/interface/VertexFwd.h>
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

// simulated vertices,..., add <use name=SimDataFormats/Vertex> and <../Track>
#include <SimDataFormats/Vertex/interface/SimVertex.h>
#include <SimDataFormats/Vertex/interface/SimVertexContainer.h>
#include <SimDataFormats/Track/interface/SimTrack.h>
#include <SimDataFormats/Track/interface/SimTrackContainer.h>
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

// jet info
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/Common/interface/RefToBase.h" 

//Root
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TProfile.h>
//
// class decleration
//
using namespace edm;
using namespace std;
typedef math::XYZTLorentzVectorF LorentzVector;
typedef math::XYZPoint Point;

class PVStudy : public edm::EDAnalyzer {

  typedef math::XYZPoint res;
  typedef math::XYZPoint pull;
  typedef math::XYZPoint error;

  // auxiliary class holding simulated primary vertices
  class simPrimaryVertex {
  public:
    simPrimaryVertex(double x1, double y1, double z1): x(x1), y(y1), z(z1), ptsq(0), nGenTrk(0) {};
      double x, y, z;
      HepMC::FourVector ptot;
      //HepLorentzVector ptot;
      double ptsq;
      int nGenTrk;
      std::vector<int> finalstateParticles;
      std::vector<int> simTrackIndex;
      std::vector<int> genVertex;
      const reco::Vertex *recVtx;
  };
  
  class PVInfo {
  public:
    PVInfo(res r, error er, int n):
      res_(math::XYZPoint(0.,0.,0.)), pvErr_(math::XYZPoint(0.,0.,0.)),
      nTrk_(0) {
      res_ = r;
      pvErr_ = er;
      nTrk_ = n;
    };
      res res_;
      error pvErr_;
      int nTrk_;
  };

  class PVAnaInfo {
  public:
    PVAnaInfo(res r, error er, pull p, error ep, int n):
      res_(math::XYZPoint(0.,0.,0.)), errRes_(math::XYZPoint(0.,0.,0.)),
      pull_(math::XYZPoint(0.,0.,0.)), errPull_(math::XYZPoint(0.,0.,0.)),
      nTrk_(0) {
      res_ = r;
      errRes_ = er;
      pull_ = p;
      errPull_ = ep;
      nTrk_ = n;
    };
      res res_;
      error errRes_;
      pull pull_;
      error errPull_;
      int nTrk_;
  };
  
 public:
  explicit PVStudy(const edm::ParameterSet&);
  ~PVStudy();


 private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
   
  void ApplyJetStuff(const edm::Event&, const edm::EventSetup&);
  //  bool mysorter (TLorentzVector i, TLorentzVector j); 

  void printRecVtxs(const edm::Handle<reco::VertexCollection> recVtxs);
  void printSimVtxs(const edm::Handle<edm::SimVertexContainer> simVtxs);
  void printSimTrks(const edm::Handle<edm::SimTrackContainer> simVtrks);
  void printRecVtx(const reco::Vertex&);

  bool isResonance(const HepMC::GenParticle * p);
  bool isFinalstateParticle(const HepMC::GenParticle * p);
  bool isCharged(const HepMC::GenParticle * p);
  bool matchVertex(const simPrimaryVertex&, const reco::Vertex&);
  bool matchVertex(const reco::VertexRef&, const reco::VertexRef&, int);
  double sumPtSquared(const reco::Vertex& );
  double avgPtRecVtx(const reco::Vertex& );
  int nHWTrkRecVtx(const reco::Vertex& );


  std::vector<simPrimaryVertex> getSimPVs(const Handle<HepMCProduct> evtMC, std::string suffix);
  std::vector<simPrimaryVertex> getSimPVs(const Handle<HepMCProduct> evtMC,
					  const Handle<SimVertexContainer> simVtxs,
					  const Handle<SimTrackContainer> simTrks);
  PVAnaInfo GetPVAnaInfo(int, int);
  void fillHisto(res, error, int, int);
  void fit(TH1*, double []);
  void setRootStyle();
  void fillTrackHisto(const reco::TrackCollection *, int, const Point &) ;
  void fillTrackHistoInPV(const reco::VertexCollection *, int, const Point &); 
  void fillMCHisto(std::vector<simPrimaryVertex>::iterator, int, const reco::VertexCollection *, int, double, double);  
  
  // ----------member data ---------------------------
  edm::InputTag simG4_;
  edm::InputTag trackCollectionTag_;
  edm::InputTag splitTrackCollection1Tag_;
  edm::InputTag splitTrackCollection2Tag_;
  edm::InputTag vertexCollectionTag_;
  edm::InputTag splitVertexCollection1Tag_;
  edm::InputTag splitVertexCollection2Tag_;
  edm::InputTag bsSrc;

  edm::Handle<edm::TriggerResults> triggerResultsH_;


  std::string histoFileName_;
  bool verbose_;
  bool realData_;
  bool analyze_;
  bool saventuple_;
  int zsigncut_;
  int nTrkMin_;
  int nTrkMax_;
  double ntrkdiffcut_; 
  double avgTrkPtInPVMin_;
  double avgTrkPtInPVMax_;
  bool useHWTrk_;
  TString menu_;
  
  ESHandle < ParticleDataTable > pdt;

  std::vector<PVInfo> pvinfo;
  std::vector<int> datamode;

  std::string outputfilename_;
  TFile* file_;
  TTree* ftree_;
  TTree* pvtxtree_;
  
  // Root-Tuple variables :
  //=======================
  void SetVarToZero();  
  int glob_runno_;
  int glob_evtno_;
  int glob_ls_;
  int glob_bx_;

  static const int nMaxTrg_ = 400;
  int glob_ntrg_;
  int glob_trg_idx_[nMaxTrg_];
  std::vector<std::string> glob_trg_name_; 
  bool HLTtrigger_pass_;
  bool filterHLT_;
  std::vector<std::string> filterExpressions_;

  
  // 2-vertices method
  double fres_[3];  // == pvtx residual (x,y,z)
  double ferror_[3]; // == pvtx error (x,y,z)
  int fntrk_;  // == Number of tracks in the pvtx
  

  int nrecPV_;
  int nrecPV1_spl_;
  int nrecPV2_spl_;
  // Mininum separation between secondary vtxes and leading vtx
  double min_zsep_;
  double min_ntrksep_;
  double min_sumpt2sep_;

  static const int nMaxPVs_ = 50;
  //Information related to twovtx
  int nrecPV_twovtx_; 
  int nTrkPV_twovtx_[nMaxPVs_];
  double deltax_twovtx_[nMaxPVs_];
  double deltay_twovtx_[nMaxPVs_];
  double deltaz_twovtx_[nMaxPVs_];
  double errx_twovtx_[nMaxPVs_];
  double erry_twovtx_[nMaxPVs_];
  double errz_twovtx_[nMaxPVs_];
  double pullx_twovtx_[nMaxPVs_];
  double pully_twovtx_[nMaxPVs_];
  double pullz_twovtx_[nMaxPVs_];

  int nTrkPV1_spl_twovtx_[nMaxPVs_];
  double ndofPV1_spl_twovtx_[nMaxPVs_];
  double normchi2PV1_spl_twovtx_[nMaxPVs_];
  double avgPtPV1_spl_twovtx_[nMaxPVs_];
  double errx1_spl_twovtx_[nMaxPVs_];
  double erry1_spl_twovtx_[nMaxPVs_];
  double errz1_spl_twovtx_[nMaxPVs_];



  int nTrkPV2_spl_twovtx_[nMaxPVs_];
  double ndofPV2_spl_twovtx_[nMaxPVs_];
  double normchi2PV2_spl_twovtx_[nMaxPVs_];
  double avgPtPV2_spl_twovtx_[nMaxPVs_];
  double errx2_spl_twovtx_[nMaxPVs_];
  double erry2_spl_twovtx_[nMaxPVs_];
  double errz2_spl_twovtx_[nMaxPVs_];



  //MC variables requiring the matching to simVertex 
  int nsimPV_;
  int nsimTrkPV_[nMaxPVs_]; 
  double simx_[nMaxPVs_];
  double simy_[nMaxPVs_];
  double simz_[nMaxPVs_];
  double simptsq_[nMaxPVs_];
  
  //for pvtx with all tracks 
  int nrecPV_mct_;  
  int nTrkPV_mct_[nMaxPVs_]; 
  double ndofPV_mct_[nMaxPVs_];  
  double normchi2PV_mct_[nMaxPVs_]; 
  double avgPtPV_mct_[nMaxPVs_]; 

  double deltax_mct_[nMaxPVs_];
  double deltay_mct_[nMaxPVs_];
  double deltaz_mct_[nMaxPVs_];
  double pullx_mct_[nMaxPVs_];
  double pully_mct_[nMaxPVs_];
  double pullz_mct_[nMaxPVs_];
  double errx_mct_[nMaxPVs_];
  double erry_mct_[nMaxPVs_];
  double errz_mct_[nMaxPVs_];


  //for pvtx with splittracks1  
  int nrecPV_spl1_mct_; 
  int nTrkPV_spl1_mct_[nMaxPVs_];   
  double ndofPV_spl1_mct_[nMaxPVs_];  
  double normchi2PV_spl1_mct_[nMaxPVs_]; 
  double avgPtPV_spl1_mct_[nMaxPVs_]; 
  double deltax_spl1_mct_[nMaxPVs_];
  double deltay_spl1_mct_[nMaxPVs_];
  double deltaz_spl1_mct_[nMaxPVs_];
  double pullx_spl1_mct_[nMaxPVs_];
  double pully_spl1_mct_[nMaxPVs_];
  double pullz_spl1_mct_[nMaxPVs_]; 
  double errx_spl1_mct_[nMaxPVs_];
  double erry_spl1_mct_[nMaxPVs_];
  double errz_spl1_mct_[nMaxPVs_];

  //for pvtx with splittracks2  
  int nrecPV_spl2_mct_; 
  int nTrkPV_spl2_mct_[nMaxPVs_];   
  double ndofPV_spl2_mct_[nMaxPVs_];  
  double normchi2PV_spl2_mct_[nMaxPVs_]; 
  double avgPtPV_spl2_mct_[nMaxPVs_]; 
  double deltax_spl2_mct_[nMaxPVs_];
  double deltay_spl2_mct_[nMaxPVs_];
  double deltaz_spl2_mct_[nMaxPVs_];
  double pullx_spl2_mct_[nMaxPVs_];
  double pully_spl2_mct_[nMaxPVs_];
  double pullz_spl2_mct_[nMaxPVs_];
  double errx_spl2_mct_[nMaxPVs_];
  double erry_spl2_mct_[nMaxPVs_];
  double errz_spl2_mct_[nMaxPVs_];

  static const int nMaxJets_ = 100;

  int nCaloJets_;
  double CaloJet_px_[nMaxJets_];
  double CaloJet_py_[nMaxJets_];
  double CaloJet_pz_[nMaxJets_];
  TLorentzVector CaloJet_[nMaxJets_];
  double DijetBalance_;
    
  edm::InputTag uncorJetsInputTag_;
  std::string CaloJetCorrectorL2L3_;
  std::string CaloJetCorrectorL1L2L3_;

  // Histos:
  /*   std::map<std::string, TH1*> h; */
  /*   std::map<std::string, TH2*> h2; */
  PVHistograms * h_pvtrk;
  PVHistograms * h_gen;
  PVHistograms * h_misc;
  PVHistograms * h_summary;
  PVHistograms * h_others;
  PVHistograms * h_ana;

  // File:
  TFile * theFile;
  //
  // constants, enums and typedefs
  //

  //
  // static data member definitions
  //

};

#endif
