#ifndef PVEffAnalyzer_h
#define PVEffAnalyzer_h

// -*- C++ -*-
//
// Package:    PVEffAnalyzer
// Class:      PVEffAnalyzer
// 
/**\class PVEffAnalyzer PVEffAnalyzer.h UserCode/PVEffAnalyzer/interface/PVEffAnalyzer.h

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  "Geng-yuan Jeng/UC Riverside"
//                   "Yanyan Gao/Fermilab ygao@fnal.gov"
//         Created:  Thu Aug 20 11:55:40 CDT 2009
// $Id: PVEffAnalyzer.h,v 1.7 2011/08/18 16:36:09 yygao Exp $
//
//


// system include files
#include <memory>
#include <string>
#include <vector>
#include "DataFormats/Math/interface/Point3D.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "UserCode/PVStudy/interface/PVEffHistograms.h"

//needed for MessageLogger
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//generator level
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/EncodedEventId/interface/EncodedEventId.h"
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
#include "RecoVertex/PrimaryVertexProducer/interface/TrackClusterizerInZ.h"
#include "RecoVertex/PrimaryVertexProducer/interface/GapClusterizerInZ.h"
#include "RecoVertex/PrimaryVertexProducer/interface/DAClusterizerInZ.h"

#include "RecoVertex/PrimaryVertexProducer/interface/TrackFilterForPVFinding.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"   
#include "SimGeneral/TrackingAnalysis/interface/TrackingTruthProducer.h"  

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

class PVEffAnalyzer : public edm::EDAnalyzer {

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
      EncodedEventId eventId;
  };
    
 public:
  explicit PVEffAnalyzer(const edm::ParameterSet&);
  ~PVEffAnalyzer();


 private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  void printRecVtxs(const edm::Handle<reco::VertexCollection> recVtxs);
  void printSimVtxs(const edm::Handle<edm::SimVertexContainer> simVtxs);
  void printSimTrks(const edm::Handle<edm::SimTrackContainer> simVtrks);
  void printRecVtx(const reco::Vertex&);
  void printCluster(std::vector< std::vector<reco::TransientTrack> > &, const Point &);
  bool goodCluster(std::vector< std::vector<reco::TransientTrack> > &) ;
  bool isResonance(const HepMC::GenParticle * p);
  bool isFinalstateParticle(const HepMC::GenParticle * p);
  bool isCharged(const HepMC::GenParticle * p);

  std::vector<simPrimaryVertex> getSimPVs(const Handle<HepMCProduct> &, std::string, double &);
  std::vector<simPrimaryVertex> getSimPVs(const edm::Handle<TrackingVertexCollection> &);

  bool isGoodSplitEvent( const reco::Vertex & ); 
  bool isGoodTagVertex( const reco::Vertex &, const reco::Vertex &, double & );
  bool isGoodProbeVertex( const reco::Vertex &, const reco::Vertex &, double & );
  bool isAssoVertex(const simPrimaryVertex&, const reco::Vertex &, double & );
  void MakeEff(TH1D* numer, TH1D* denom, TH1D* eff,  //TGraphAsymmErrors* & gr_eff,
	       const bool rebin, const Float_t n );
  double avgWeight(const reco::Vertex & ); 
  void SetVarToZero();  
  
  // ----------member data ---------------------------
  edm::InputTag simG4_;
  edm::InputTag trackCollectionTag_;
  edm::InputTag splitTrackCollection1Tag_;
  edm::InputTag splitTrackCollection2Tag_;
  edm::InputTag vertexCollectionTag_;
  edm::InputTag splitVertexCollection1Tag_;
  edm::InputTag splitVertexCollection2Tag_;
  edm::InputTag bsSrc;

  std::string outputfilename_;
  TFile* file_;
  TTree* pvtxtree_;
  
  // ----------Root-Tuple variables 
  // ------- MC variables requiring the matching to simVertex 
  static const int nMaxPVs_ = 20;
  int nsimPV_;
  int nsimTrkPV_[nMaxPVs_]; 
  double simx_[nMaxPVs_];
  double simy_[nMaxPVs_];
  double simz_[nMaxPVs_];
  double simptsq_[nMaxPVs_];
  double simDeltaZ_[nMaxPVs_];
  bool saventuple_;
  
  std::string histoFileName_;
  bool verbose_;
  bool realData_;
  bool useTP_;
  bool useAssociator_;
  bool analyze_;
  bool reqCluster_;
  double zsigncut_;
  double ptcut_;
  int nTrkMin_;
  int nTrkMax_;
  TrackFilterForPVFinding theTrackFilter; 
  //TrackClusterizerInZ theTrackClusterizer;
  
  GapClusterizerInZ theTrackClusterizer;
  
  ESHandle < ParticleDataTable > pdt;

  std::vector<int> datamode;
    
  // Histograms
  PVEffHistograms * h_gen;
  PVEffHistograms * h_summary;
  
  
  // File:
  TFile * theFile;



};

#endif
