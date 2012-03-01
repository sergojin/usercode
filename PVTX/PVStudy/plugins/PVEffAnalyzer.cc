
// -*- C++ -*-
//
// Package:    PVEffAnalyzer
// Class:      PVEffAnalyzer
// 
/**\class PVEffAnalyzer PVEffAnalyzer.cc UserCode/PVEffAnalyzer/plugins/PVEffAnalyzer.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  "Geng-yuan Jeng/UC Riverside"
//                    "Yanyan Gao/Fermilab ygao@fnal.gov"
//         Created:  Thu Aug 20 11:55:40 CDT 2009
// $Id: PVEffAnalyzer.cc,v 1.8 2010/06/04 17:46:09 yygao Exp $
//
//


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
#include "UserCode/PVStudy/interface/PVEffAnalyzer.h"
#include "DataFormats/Common/interface/RefToBaseVector.h"
#include "DataFormats/Common/interface/RefToBase.h"
// Vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

// simulated vertices,..., add <use name=SimDataFormats/Vertex> and <../Track>
#include <SimDataFormats/Vertex/interface/SimVertex.h>
#include <SimDataFormats/Vertex/interface/SimVertexContainer.h>
#include <SimDataFormats/Track/interface/SimTrack.h>
#include <SimDataFormats/Track/interface/SimTrackContainer.h>
// TrackingParticle Associators
#include "SimTracker/TrackAssociation/interface/TrackAssociatorByHits.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
// BeamSpot 
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

// For the clusters
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h" 

//root
#include <TROOT.h>
#include <TF1.h>
#include <TString.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TPad.h>



using namespace std;
using namespace edm;
using namespace reco;

typedef math::XYZTLorentzVectorF LorentzVector;
typedef math::XYZPoint Point;

PVEffAnalyzer::PVEffAnalyzer(const edm::ParameterSet& iConfig)
  :theTrackClusterizer(iConfig.getParameter<edm::ParameterSet>("TkClusParameters")),
   theTrackFilter(iConfig.getParameter<edm::ParameterSet>("TkFilterParameters"))
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
  useTP_                     = iConfig.getUntrackedParameter<bool>("useTP",false);
  useAssociator_             = iConfig.getUntrackedParameter<bool>("useAssociator",false);
  histoFileName_             = iConfig.getUntrackedParameter<std::string> ("histoFileName");
  outputfilename_            = iConfig.getUntrackedParameter<string>("OutputFileName");   
  nTrkMin_                   = iConfig.getUntrackedParameter<int>("nTrkMin");
  nTrkMax_                   = iConfig.getUntrackedParameter<int>("nTrkMax");
  zsigncut_                  = iConfig.getUntrackedParameter<double>("zsigncut");
  ptcut_                     = iConfig.getUntrackedParameter<double>("ptcut");
  analyze_                   = iConfig.getUntrackedParameter<bool>("analyze",false);
  saventuple_                   = iConfig.getUntrackedParameter<bool>("saventuple",false);
  bsSrc = iConfig.getParameter< edm::InputTag >("beamSpot");
  reqCluster_               = iConfig.getUntrackedParameter<bool>("reqCluster",false);
    
  // Specify the data mode vector
  if(realData_) datamode.push_back(0);
  else {
    datamode.push_back(0);
    datamode.push_back(1);
  }

  if(saventuple_) {
    file_ = TFile::Open(outputfilename_.c_str(),"RECREATE");
    // pvtxtree_ analyzing the pvtxs ootb
    pvtxtree_ = new TTree("pvtxtree","pvtxtree");
    if(!realData_) {
      pvtxtree_->Branch("nsimPV",&nsimPV_,"nsimPV/I");
      pvtxtree_->Branch("nsimTrkPV",&nsimTrkPV_,"nsimTrkPV[nsimPV]/I");
      pvtxtree_->Branch("simx",&simx_,"simx[nsimPV]/D");
      pvtxtree_->Branch("simy",&simy_,"simy[nsimPV]/D");
      pvtxtree_->Branch("simz",&simz_,"simz[nsimPV]/D");
      pvtxtree_->Branch("simptsq",&simptsq_,"simptsq[nsimPV]/D");
      pvtxtree_->Branch("simDeltaZ",&simDeltaZ_,"simDeltaZ[nsimPV]/D");
    }
  }
  
  theFile = new TFile(histoFileName_.c_str(), "RECREATE"); 
  theFile->mkdir("Summary"); 
  theFile->cd();


  // Book MC only plots
  if (!realData_) {
    h_gen = new PVEffHistograms();
    h_gen->Init("generator");
  }
  
  h_summary = new PVEffHistograms();
  //Book histograms sensitive to data/mc
  for (vector<int>::const_iterator it= datamode.begin(); it != datamode.end() ; ++it) {
    string suffix; 
    edm::LogInfo("Debug")<<"datamode = "<< *it<<endl;
    switch(*it) {
    case 0: suffix = "";
      break;
    case 1: suffix = "_mct";
      break;
    }
    h_summary->Init("summary",suffix, nTrkMin_, nTrkMax_);
  }

}

PVEffAnalyzer::~PVEffAnalyzer()
{
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  theFile->cd();
  theFile->cd("Summary");
  h_summary->Save();
  if (!realData_) 
    h_gen->Save();
  theFile->Close();
}

//
// member functions
//
std::vector<PVEffAnalyzer::simPrimaryVertex> PVEffAnalyzer::getSimPVs(const Handle<HepMCProduct> & evtMC, std::string suffix, double & ptcut_) 
{
 std::vector<PVEffAnalyzer::simPrimaryVertex> simpv;
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
	    if ( (m.perp()>ptcut_) && (fabs(m.pseudoRapidity())<2.5) && isCharged( *daughter ) ){
	      vp->nGenTrk++;
	    }
	  }
	}
      }//loop MC vertices daughters
    }//loop MC vertices
  }
  return simpv;
}


// get sim pv from TrackingParticles/TrackingVertex 
std::vector<PVEffAnalyzer::simPrimaryVertex> PVEffAnalyzer::getSimPVs( const edm::Handle<TrackingVertexCollection> & tVC )
{
  std::vector<simPrimaryVertex> simpv;
  //std::cout <<"number of vertices " << tVC->size() << std::endl;

  if(verbose_){std::cout << "getSimPVs TrackingVertexCollection " << std::endl;}

  for (TrackingVertexCollection::const_iterator v = tVC -> begin(); v != tVC -> end(); ++v) {

    if(verbose_){
      std::cout << (v->eventId()).event() << v -> position() << v->g4Vertices().size() <<" "  <<v->genVertices().size() <<  "   t=" <<v->position().t()*1.e12 <<"    ==0:" <<(v->position().t()>0) << std::endl;
      for( TrackingVertex::g4v_iterator gv=v->g4Vertices_begin(); gv!=v->g4Vertices_end(); gv++){
	std::cout << *gv << std::endl;
      }
      std::cout << "----------" << std::endl;
    }
 
    //    bool hasMotherVertex=false;
    if ((unsigned int) v->eventId().event()<simpv.size()) continue;
    //if (v->position().t()>0) { continue;} // skip secondary vertices (obsolete + doesn't work)
    if (fabs(v->position().z())>1000) continue;  // skip funny junk vertices
    
    // could be a new vertex, check  all primaries found so far to avoid multiple entries
    const double mm=1.0; // for tracking vertices
    simPrimaryVertex sv(v->position().x()*mm,v->position().y()*mm,v->position().z()*mm);

    for (TrackingParticleRefVector::iterator iTrack = v->daughterTracks_begin(); iTrack != v->daughterTracks_end(); ++iTrack){
      //cout <<((**iTrack).eventId()).event() << " ";  // an iterator of Refs, dereference twice. Cool eyh?
      sv.eventId=(**iTrack).eventId();
      sv.ptsq += pow((**iTrack).pt(),2);
      sv.nGenTrk++;
    }

    simPrimaryVertex *vp=NULL;  // will become non-NULL if a vertex is found and then point to it
    for(std::vector<simPrimaryVertex>::iterator v0=simpv.begin();
	v0!=simpv.end(); v0++){
      if( (sv.eventId==v0->eventId) && (fabs(sv.x-v0->x)<1e-5) && (fabs(sv.y-v0->y)<1e-5) && (fabs(sv.z-v0->z)<1e-5)){
	vp=&(*v0);
	break;
      }
    }
    if(!vp){
      // this is a new vertex
      if(verbose_){std::cout << "this is a new vertex " << sv.eventId.event() << "   "  << sv.x << " " << sv.y << " " << sv.z <<std::endl;}
      simpv.push_back(sv);
      vp=&simpv.back();
    }else{
      if(verbose_){std::cout << "this is not a new vertex"  << sv.x << " " << sv.y << " " << sv.z <<std::endl;}
    }


    // Loop over daughter track(s)
    if(verbose_){
      for (TrackingVertex::tp_iterator iTP = v -> daughterTracks_begin(); iTP != v -> daughterTracks_end(); ++iTP) {
	std::cout << "  Daughter momentum:      " << (*(*iTP)).momentum();
	std::cout << "  Daughter type     " << (*(*iTP)).pdgId();
	std::cout << std::endl;
      }
    }
  }
  if(verbose_){  
    cout << "------- PVEffAnalyzer simPVs from TrackingVertices -------" <<  endl; 
    for(std::vector<simPrimaryVertex>::iterator v0=simpv.begin();
	v0!=simpv.end(); v0++){
      cout << "z=" << v0->z << "  event=" << v0->eventId.event() << endl;
    }
    cout << "-----------------------------------------------" << endl;
  }
  return simpv;
}




// ------------ method called to for each event  ------------
void
PVEffAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace reco;

  edm::ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB); 

  //========================================================================
  // Step 0: Prepare root variables and get information from the Event
  //========================================================================
  
  edm::LogInfo("Debug")<<"[PVEffAnalyzer]"<<endl;

  // ====== TrackCollection
  edm::Handle<edm::View<reco::Track> > trackCollectionHandle;
  iEvent.getByLabel(trackCollectionTag_, trackCollectionHandle);
  
  // ====== splitTrackCollection1
  edm::Handle<edm::View<reco::Track> > splitTrackCollection1Handle;
  iEvent.getByLabel(splitTrackCollection1Tag_, splitTrackCollection1Handle);

  // ====== splitTrackCollection2
  edm::Handle<edm::View<reco::Track> > splitTrackCollection2Handle;
  iEvent.getByLabel(splitTrackCollection2Tag_, splitTrackCollection2Handle);
  
  // ======= PrimaryVertexCollection
  static const reco::VertexCollection s_empty_vertexColl;
  const reco::VertexCollection *vertexColl = &s_empty_vertexColl;
  edm::Handle<reco::VertexCollection> vertexCollectionHandle;
  iEvent.getByLabel(vertexCollectionTag_, vertexCollectionHandle);
  if( iEvent.getByLabel(vertexCollectionTag_, vertexCollectionHandle)) {
    vertexColl = vertexCollectionHandle.product();
  } else {
   edm::LogInfo("Debug") << "[PVEffAnalyzer] vertexCollection cannot be found -> using empty collection of same type." <<endl;
  }
  // ====== splitVertexCollection1
  static const reco::VertexCollection s_empty_splitVertexColl1;
  const reco::VertexCollection *splitVertexColl1 = &s_empty_splitVertexColl1;
  edm::Handle<reco::VertexCollection> splitVertexCollection1Handle;
  iEvent.getByLabel(splitVertexCollection1Tag_, splitVertexCollection1Handle);
  if( iEvent.getByLabel(splitVertexCollection1Tag_, splitVertexCollection1Handle)) {
    splitVertexColl1 = splitVertexCollection1Handle.product();
  } else {
    edm::LogInfo("Debug") << "[PVEffAnalyzer] splitVertexCollection1 cannot be found -> using empty collection of same type." <<endl;
  }
  // ====== splitVertexCollection2
  static const reco::VertexCollection s_empty_splitVertexColl2;
  const reco::VertexCollection *splitVertexColl2 = &s_empty_splitVertexColl2;
  edm::Handle<reco::VertexCollection> splitVertexCollection2Handle;
  iEvent.getByLabel(splitVertexCollection2Tag_, splitVertexCollection2Handle);
  if( iEvent.getByLabel(splitVertexCollection2Tag_, splitVertexCollection2Handle)) {
    splitVertexColl2 = splitVertexCollection2Handle.product();
  } else {
    edm::LogInfo("Debug") << "[PVEffAnalyzer] splitVertexCollection2 cannot be found -> using empty collection of same type." <<endl;
  }

  
  // ======== BeamSpot accessors
  edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
  iEvent.getByLabel(bsSrc,recoBeamSpotHandle);
  reco::BeamSpot bs = *recoBeamSpotHandle;     
  const Point beamSpot = recoBeamSpotHandle.isValid() ? Point(recoBeamSpotHandle->x0(), recoBeamSpotHandle->y0(), recoBeamSpotHandle->z0()) : Point(0, 0, 0);
  
  edm::LogInfo("Debug")<<"[PVEffAnalyzer] End accessing the track, beamSpot, primary vertex collections"<<endl;
  
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
    edm::LogInfo("Debug") << "[PVEffAnalyzer] Some problem occurred with the particle data table. This may not work !." <<endl;
  }
  
  //setUpVectors(RealData, nTrkMin_, nTrkMax_ ) ;
  
  // ======= Analyze MC efficiency
  if (!realData_) { 
    
    bool MC=false; // the flag to see if the MC level information is complete

    Handle<HepMCProduct> evtMC;
    iEvent.getByLabel("generator",evtMC);

    edm::Handle<TrackingParticleCollection>  TPCollectionH ;
    bool gotTP=iEvent.getByLabel("mergedtruth","MergedTrackTruth",TPCollectionH);

    edm::Handle<TrackingVertexCollection>    TVCollectionH ;
    bool gotTV=iEvent.getByLabel("mergedtruth","MergedTrackTruth",TVCollectionH);

    std::vector<simPrimaryVertex> simpv;
    
    if(gotTV){
      MC=true;
      if(verbose_){
	cout << "Found Tracking Vertices " << endl;
      }
      simpv=getSimPVs(TVCollectionH);
    }
    else if (evtMC.isValid()) {
      MC=true;
      edm::LogInfo("Debug") << "[PVEffAnalyzer] generator HepMCProduct found"<< endl;
    } else {
      edm::LogInfo("Debug") << "[PVEffAnalyzer] no HepMCProduct found"<< endl;
      MC=false;
    }
   

    if(MC){
      TString suffix = "_mct";
      // make a list of primary vertices:
      h_gen->Fill1d("nsimPV", simpv.size());
      
      int isimpv = 0;
      nsimPV_ = simpv.size();
   
      for(int isimpv = 0; isimpv<int(simpv.size());isimpv++) {
	simPrimaryVertex vsim = simpv.at(isimpv);
	nsimTrkPV_[isimpv]  =vsim.nGenTrk;
	simx_[isimpv] = vsim.x;
	simy_[isimpv] = vsim.y;
	simz_[isimpv] = vsim.z;
	simptsq_[isimpv] = vsim.ptsq;
	if(simpv.size() > 1 )
	  if(verbose_)
	    std::cout<<"Simulated Vertex # " << isimpv << ": ptsq = " << vsim.ptsq<<std::endl;
	if(isimpv >=1 )
	  simDeltaZ_[isimpv-1] = vsim.z-simz_[0]; 
      }
      
      // Just analyze the first vertex in the collection 
      simPrimaryVertex vsim = *simpv.begin();
      int ntracks(0);
      if( useTP_ )
	ntracks = vsim.nGenTrk;
      else {
	reco::RecoToSimCollection recSimColl; 
	if (useAssociator_) {
	  // get TrackingParticle
	  edm::Handle<TrackingParticleCollection>  TPCollectionHandle ;
	  iEvent.getByLabel("mergedtruth","MergedTrackTruth",TPCollectionHandle);
	  // get associators
	  edm::ESHandle<TrackAssociatorBase> theAssociator;
	  iSetup.get<TrackAssociatorRecord>().get("TrackAssociatorByHitsRecoDenom",theAssociator);
	  recSimColl = (theAssociator.product())->associateRecoToSim( trackCollectionHandle,
								    TPCollectionHandle,
								    & iEvent);
	}

	// == use the nubmer of tracks that are filtered and clusterized
	// copy code from CMSSW/RecoVertex/PrimaryVertexProducer/src/PrimaryVertexProducerAlgorithm.cc
	std::vector<reco::TransientTrack> seltks;
	for(unsigned int i=0; i<trackCollectionHandle->size(); ++i){ 
	  edm::RefToBase<reco::Track> track(trackCollectionHandle, i);
	  if(useAssociator_) {
	    bool isSimMatched(false);
	    std::vector<std::pair<TrackingParticleRef, double> > tp;
	    if(recSimColl.find(track) != recSimColl.end()){
	      tp = recSimColl[track];
	      if (tp.size()!=0) 
		isSimMatched = true;
	    }
	    if(!isSimMatched) continue;
	  }
	  TransientTrack t_track = (*theB).build(*track);
	  t_track.setBeamSpot(bs);
	  if (theTrackFilter(t_track)) seltks.push_back(t_track);
	}
	std::vector< std::vector<reco::TransientTrack> > clusters =  theTrackClusterizer.clusterize(seltks);
	if (clusters.size() == 1 && (clusters.begin()->size()) >1 ) {
	  for(unsigned idx_tt = 0; idx_tt < clusters.begin()->size(); idx_tt++) {
	    reco::Track track = (*(clusters.begin())).at(idx_tt).track();
	    if(track.pt()>ptcut_) // && TMath::Abs(track.eta())<2.5) // count only good tracks
	      ntracks++;
	  }
	}
      } // == end of getting the ntracks
      
      // == Fill Efficiency Histogram
      if(ntracks>1) {
	h_summary->Fill1d(TString("eff_denom_ntrack"+suffix), ntracks);
	if(!vertexColl->begin()->isFake())
	  h_summary->Fill1d(TString("deltazSign"+suffix), vsim.z - vertexColl->begin()->z());
	if ( isAssoVertex(vsim, *vertexColl->begin(), zsigncut_)  )
	  h_summary->Fill1d(TString("eff_numer_ntrack"+suffix), ntracks);
      }

      // == Fill FakeRate Histogram
      if(!vertexColl->begin()->isFake ()) {
	h_summary->Fill1d(TString("fakerate_denom_ntrack"+suffix), int(vertexColl->begin()->tracksSize()));
	if (!isAssoVertex(vsim, *vertexColl->begin(), zsigncut_)  )
	  h_summary->Fill1d(TString("fakerate_numer_ntrack"+suffix), int(vertexColl->begin()->tracksSize()));
      }
    }  // end of MC analyzing
  } // end of MC access
  
  // == Start of Split-Method
  // Event selections
  if ( !isGoodSplitEvent( *vertexColl->begin())) return;
  for(reco::Vertex::trackRef_iterator t = vertexColl->begin()->tracks_begin(); 
      t!=vertexColl->begin()->tracks_end(); t++) {
    h_summary->Fill1d("trackweight",vertexColl->begin()->trackWeight(*t));
  }

  // == Start of FakeRate Analyzing
  
  if(!splitVertexColl1->begin()->isFake()) {
    h_summary->Fill1d("fakerate_denom_ntrack", int(splitVertexColl1->begin()->tracksSize()));
    if ( !isGoodProbeVertex( *splitVertexColl1->begin(), *vertexColl->begin(), zsigncut_) ) 
      h_summary->Fill1d("fakerate_numer_ntrack", int(splitVertexColl1->begin()->tracksSize()));
  }

  // == End of FakeRate Analyzing
  // == Start of Efficiency Analyzing
  std::vector< std::vector<reco::TransientTrack> > clusters1;
  std::vector< std::vector<reco::TransientTrack> > clusters2;
  if(splitTrackCollection1Handle.isValid())
    clusters1 =  theTrackClusterizer.clusterize((*theB).build(splitTrackCollection1Handle));
  if(splitTrackCollection2Handle.isValid())
    clusters2 =  theTrackClusterizer.clusterize((*theB).build(splitTrackCollection2Handle));
  
  // clustering requirement
  if( !goodCluster (clusters1) || !goodCluster (clusters2) ) return;

  // TagVertex Requirement
  if ( isGoodTagVertex( *(splitVertexColl2->begin()), *(vertexColl->begin()), zsigncut_) ) {
    if(verbose_) {
      std::cout<<"splitTrackCollection1Handle->size() = " << int(splitTrackCollection1Handle->size()) << std::endl;
      std::cout<<"splitTrackCollection2Handle->size() = " << int(splitTrackCollection2Handle->size()) << std::endl;
    }
    int nGoodTracks = 0;
    for(unsigned int i = 0; i < splitTrackCollection1Handle->size(); i++ ) {
      edm::RefToBase<reco::Track> track(splitTrackCollection1Handle, i);
      if(track->pt() > ptcut_) // && abs(track->eta()) < 2.5 )
	nGoodTracks++;
    }
    
    h_summary->Fill1d("eff_denom_ntrack", nGoodTracks);
    // ProbeVertex Requirement
    if ( isGoodProbeVertex( *splitVertexColl1->begin(), *vertexColl->begin(), zsigncut_) ) {
      h_summary->Fill1d("eff_numer_ntrack", nGoodTracks);
      h_summary->Fill1d("avgWeight_orgvtx_eff",avgWeight(*vertexColl->begin()) );
      h_summary->Fill1d("avgWeight_tagvtx_eff",avgWeight(*splitVertexColl2->begin()) );
      h_summary->Fill1d("avgWeight_probevtx_eff",avgWeight(*splitVertexColl1->begin()) );
    }
    else
      {
	if (nGoodTracks >= 2) {
	  h_summary->Fill1d("avgWeight_orgvtx_ineff",avgWeight(*vertexColl->begin()) );
	  h_summary->Fill1d("avgWeight_tagvtx_ineff",avgWeight(*splitVertexColl2->begin()) );
	  h_summary->Fill1d("avgWeight_probevtx_ineff",avgWeight(*splitVertexColl1->begin()) );

	  std::cout<<"**********Inefficiency Found************** " <<std::endl;
	  std::cout<<"==== Original Vertex: " <<std::endl;
	  //printRecVtx(*vertexColl->begin());
	  printRecVtxs(vertexCollectionHandle);
	  std::cout<<"==== Probe Vertex: " <<std::endl;
	  //printRecVtx(*splitVertexColl1->begin());
	  printRecVtxs(splitVertexCollection1Handle);
	  std::cout<<"==== Tag Vertex: " <<std::endl;
	  //printRecVtx(*splitVertexColl2->begin());
	  printRecVtxs(splitVertexCollection2Handle);
	  std::cout<<"==== Probe cluster information "<< std::endl;
	  printCluster(clusters1, beamSpot);
	  std::cout<<"==== Tag cluster information "<< std::endl;
	  printCluster(clusters2, beamSpot);
	  std::cout<<"**********Inefficiency Found************** " <<std::endl;
	}
      }
  }
  // == End of Efficiency Analyzing
  // == Save Ntuple
  if(saventuple_) {
    pvtxtree_->Fill();
  }

}

  
// ------------ method called once each job just before starting event loop  ------------
void 
PVEffAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PVEffAnalyzer::endJob() {
  edm::LogInfo("Analysis") << "[endJob] Analyzing PV info" << endl;
 
  if(saventuple_) {
    file_->cd();
    pvtxtree_->Write();
    file_->Close();
  }


  for (vector<int>::const_iterator it= datamode.begin(); it != datamode.end() ; ++it) {
    string suffix; 
    edm::LogInfo("Debug")<<"datamode = "<< *it<<endl;
    switch(*it) {
    case 0: suffix = "";
      break;
    case 1: suffix = "_mct";
      break;
    }
    if (analyze_) {
      MakeEff(h_summary->ReadHisto1D(TString("eff_numer_ntrack"+suffix)), h_summary->ReadHisto1D(TString("eff_denom_ntrack"+suffix)), h_summary->ReadHisto1D(TString("eff_ntrack"+suffix)), false, 1);
      MakeEff(h_summary->ReadHisto1D(TString("fakerate_numer_ntrack"+suffix)), h_summary->ReadHisto1D(TString("fakerate_denom_ntrack"+suffix)), h_summary->ReadHisto1D(TString("fakerate_ntrack"+suffix)), false, 1);
    }
  }    
}



bool PVEffAnalyzer::isResonance(const HepMC::GenParticle * p){
  double ctau=(pdt->particle( abs(p->pdg_id()) ))->lifetime();
  edm::LogInfo("Debug") << "[isResonance] isResonance   " << p->pdg_id() << " " << ctau << endl;
  return  ctau >0 && ctau <1e-6;
}

bool PVEffAnalyzer::isFinalstateParticle(const HepMC::GenParticle * p){
  return ( !p->end_vertex() && p->status()==1 );
}

bool PVEffAnalyzer::isCharged(const HepMC::GenParticle * p){
  const ParticleData * part = pdt->particle( p->pdg_id() );
  if (part){
    return part->charge()!=0;
  }else{
    // the new/improved particle table doesn't know anti-particles
    return  pdt->particle( -p->pdg_id() )!=0;
  }
}

void PVEffAnalyzer::printSimVtxs(const Handle<SimVertexContainer> simVtxs){
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

void PVEffAnalyzer::printRecVtxs(const Handle<reco::VertexCollection> recVtxs){
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

void PVEffAnalyzer::printRecVtx(const reco::Vertex & v){

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

void PVEffAnalyzer::printSimTrks(const Handle<SimTrackContainer> simTrks){
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

// Select based on the highest SumPtSq Vertex 
// Require at least 6 tracks with track weight > ~  0.8
// vtx.ndof = 2*Sum(trackWeight) - 3 

bool PVEffAnalyzer::isGoodSplitEvent( const reco::Vertex & org_vtx) 
{
  if (  org_vtx.tracksSize() >= 6 && org_vtx.ndof() >= 6 && avgWeight(org_vtx) > 0.75  ) return true;
  //if (  org_vtx.tracksSize() >= 6 && org_vtx.ndof() >= 6 ) return true;
  else 
    return false;
}

// Comparing the tag vertex with the unsplit vertex position in z
bool PVEffAnalyzer::isGoodTagVertex( const reco::Vertex & tag_vtx, const reco::Vertex & org_vtx, double & zsign_cut) 
{
  if(tag_vtx.isValid () && !tag_vtx.isFake()) {
    float zsign = TMath::Abs(tag_vtx.z() - org_vtx.z() ) / TMath::Max(tag_vtx.zError(), org_vtx.zError() );
    //float zsign = TMath::Abs(tag_vtx.z() - org_vtx.z() ) / sqrt(pow(tag_vtx.zError(),2)+pow(org_vtx.zError(),2) );
    return (zsign < zsign_cut) ;
  }
  else
    return false;
}

// Comparing the probe vertex with the unsplit vertex position in z
bool PVEffAnalyzer::isGoodProbeVertex( const reco::Vertex & probe_vtx, const reco::Vertex & org_vtx, double & zsign_cut) 
{
  if(probe_vtx.isValid () && !probe_vtx.isFake()) {
    float zsign = TMath::Abs(probe_vtx.z() - org_vtx.z() ) / TMath::Max(probe_vtx.zError(), org_vtx.zError() );
    //float zsign = TMath::Abs(probe_vtx.z() - org_vtx.z() ) / sqrt(pow(probe_vtx.zError(),2)+pow(org_vtx.zError(),2) );
    return (zsign < zsign_cut) ;
  }
  else
    return false;
}


bool PVEffAnalyzer::isAssoVertex(const PVEffAnalyzer::simPrimaryVertex & vsim, 
				 const reco::Vertex & vrec, double & zsign_cut)
{
  if(vrec.isValid() && !vrec.isFake() )
    return (TMath::Abs(vsim.z-vrec.z()) < zsign_cut * vrec.zError() );
  else 
    return false;
}



void PVEffAnalyzer::MakeEff(TH1D* numer, TH1D* denom, TH1D* eff,  //TGraphAsymmErrors* & gr_eff,
			    const bool rebin, const Float_t n ) {
  eff->Divide( numer, denom, 1,1,"B" );
  
  if( rebin ) {
    std::vector<Double_t> binEdges;
    Int_t bin = denom->GetNbinsX() + 1;
    binEdges.push_back(denom->GetBinLowEdge(bin));
    Float_t nEntries = 0;
    while (bin > 1) {
      bin --;
      nEntries = denom->GetBinContent(bin);
      while (nEntries < n && bin > 1) {
	bin --;
	nEntries += denom->GetBinContent(bin);
      }
      binEdges.push_back(denom->GetBinLowEdge(bin));
    }
    
    Double_t *array = new Double_t[binEdges.size()];
    
    Int_t j = 0;
    for (Int_t i = binEdges.size(); i > 0; --i) {
      array[j] = binEdges[i - 1];
      ++j;
    }
  }
  
  for (int i = 1; i<eff->GetNbinsX()+1; i++) {
    if(numer->GetBinContent(i) == 0 || denom->GetBinContent(i) == 0 ) continue;
    float error = sqrt(eff->GetBinContent(i)*(1-eff->GetBinContent(i))/denom->GetBinContent(i)); // Binominal-Error
    eff->SetBinError(i, error);
  }
}

bool PVEffAnalyzer::goodCluster(std::vector< std::vector<reco::TransientTrack> > & clusters)
{
  if( clusters.size() == 1 && ( clusters.begin()->size() > 1 )) return true;
  else
    return false;
}

void PVEffAnalyzer::printCluster(std::vector< std::vector<reco::TransientTrack> > & clusters, const Point & beamSpot) 
{
  //std::cout<<"theTrackClusterizer.zSeparation()"<<theTrackClusterizer.zSeparation()<<std::endl;
  cout << "#clusters " << std::setw(3) << clusters.size() << endl;
  int idx_cluster = 0;
  for (vector< vector<reco::TransientTrack> >::const_iterator iclus = clusters.begin(); iclus != clusters.end(); iclus++, idx_cluster++) {
    if((*iclus).size() == 0) continue;
    cout << "=== Cluster # " << std::setw(3) << idx_cluster << std::setw(3) << " nTracks" << std::setw(3) << (*iclus).size() <<endl;
    for (int idx_track = 0; idx_track < int((*iclus).size()) ; idx_track++) {
      reco::Track t = (*iclus).at(idx_track).track();
      cout << "== Trk #  " << std::setw(4) << idx_track
	   << " nHit     " << std::setw(4) << t.hitPattern().numberOfValidHits() <<endl
	   << " pt       " << std::setw(9) << std::fixed << std::setprecision(4) << t.pt()
	   << " dpt      " << std::setw(9) << t.ptError() << endl 
	   << " dxy(BS)  " << std::setw(9) << t.dxy(beamSpot) 
	   << " sigmaDxy " << std::setw(9) << t.dxyError() << endl 
	   << " dz(BS)   " << std::setw(9) << t.dz(beamSpot) 
	   << " sigmaDz  " << std::setw(9) << t.dzError() << endl 
	   << " xPCA     " << std::setw(9) << t.vertex().x()
	   << " yPCA     " << std::setw(9) << t.vertex().y()
	   << " zPCA     " << std::setw(9) << t.vertex().z() 
	   << endl;
    }
  }
  
}
double PVEffAnalyzer::avgWeight(const reco::Vertex & vtx)
{
  if(!vtx.isValid() || vtx.isFake() )
    return 0;
  else
    return (vtx.ndof()+3.0)/2.0/double(vtx.tracksSize());
}


void PVEffAnalyzer::SetVarToZero() {
  nsimPV_ = 0;
  //simpv 
  for(int i = 0; i < nMaxPVs_; i++) {
    nsimTrkPV_[i] = 0; 
    simx_[i] = 0;
    simy_[i] = 0;
    simz_[i] = 0;
    simptsq_[i] = 0;
    if(i>=1)
      simDeltaZ_[i-1] = 0;
  }
}
