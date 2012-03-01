// -*- C++ -*-
//
// Package:    VtxTrackSplitterProducer
// Class:      VtxTrackSplitterProducer
// 
/**\class VtxTrackSplitterProducer VtxTrackSplitterProducer.cc MySub/VtxTrackSplitterProducer/src/VtxTrackSplitterProducer.cc

 Description: <one line class summary>

This module splits the tracks used in the leading primaryvertex 
 Implementation:
     <Notes on implementation>
*/
//

// updated to 19/9/2009 8.30 pm

//
//

// system include files
#include <memory>

// user include files
#include "UserCode/PVStudy/interface/VtxTrackSplitterProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TROOT.h"
#include "TChain.h"
#include "TNtuple.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "UserCode/PVStudy/interface/TrackSorter.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

VtxTrackSplitterProducer::VtxTrackSplitterProducer(const edm::ParameterSet & iConfig)
{
  //now do what ever initialization is needed
  useHWTrack_    = iConfig.getParameter<bool>       ("useHWTrack");  
  debug_    = iConfig.getParameter<bool>       ("Debug");  
  vertexCollectionTag_      = iConfig.getParameter<edm::InputTag>("vertexCollectionTag");  
  nTrkPerBlock_             = iConfig.getParameter<int>("nTrkPerBlock");
  trackWeightCut_             = iConfig.getParameter<double>("trackWeightCut");
  
  produces<TrackCollection>("SplittedTracks1");
  produces<TrackCollection>("SplittedTracks2"); 
}


void VtxTrackSplitterProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace reco;
  using namespace std;
  
  // ==== Step 0: Get Primary vertexCollection
  static const reco::VertexCollection s_empty_vertexColl;
  const reco::VertexCollection *vertexColl = &s_empty_vertexColl;
  edm::Handle<reco::VertexCollection> vertexCollectionHandle;
  iEvent.getByLabel(vertexCollectionTag_, vertexCollectionHandle);
  if( iEvent.getByLabel(vertexCollectionTag_, vertexCollectionHandle)) {
    vertexColl = vertexCollectionHandle.product();
  } else {
    edm::LogInfo("Debug") << "[VtxTrackSplitterProducer] vertexCollection cannot be found -> using empty collection of same type." <<endl;
  }
  // select only events with at least one real pvtx reconstructed
  if(vertexCollectionHandle->size() == 0) return;
  if(vertexColl->begin()->isFake() || (!vertexColl->begin()->isValid())) return;
    
  // ==== Step 1: Get the tracks in the leading pvtx clustered by the given vertex
  std::vector<reco::Track> vertexTrackColl;
  unsigned int j=0;
  for(reco::Vertex::trackRef_iterator t = vertexColl->begin()->tracks_begin(); 
      t!=vertexColl->begin()->tracks_end(); t++,j++) {
    if(useHWTrack_) {
      if (vertexColl->begin()->trackWeight(*t) > trackWeightCut_) {
	vertexTrackColl.push_back(reco::Track(**t));  
      }
    }
    else {
      vertexTrackColl.push_back(reco::Track(**t));
    }
    if(debug_) {
      cout<<"Original Track #"<<j<<": pT = "<<(*t)->pt()<<"; Weight = "<< vertexColl->begin()->trackWeight(*t)<<endl;
    }
  }
  // make sure that each trackCollection have to be greater than 2
  if(vertexTrackColl.size() == 0) return;
  // ==== Step 2: Sort the Tracks in the order of descending pT
  TrackSorter trksorter; 
  TrackCollection sortedTrackColl = trksorter.sortedList(vertexTrackColl);
 
  // ==== Step 3: split the tracks into two trackCollection   
  
  // ==== Splitting Alogrithm ==============
  // 1. Divide the total tracks into nBlocks = nTracks / nTrkPerBlock_
  // 2. Loop over each block. 
  //    At each block, randomly generate an iterger iSelect = rand() % nTrkPerBlock_
  // 3. Loop over nTrkPerBlock_ tracks,
  //    Push back the track with index iSelect to splittedTrackColl1
  //    and the rest go to splittedtrackColl2
  // =======================================

  splittedTColl1_ = std::auto_ptr<TrackCollection> (new TrackCollection );
  splittedTColl2_ = std::auto_ptr<TrackCollection> (new TrackCollection );

  int nBlocks = sortedTrackColl.size()/nTrkPerBlock_;
  if(debug_)
    cout<<"[VtxTrackSplitterProducer] looping over sortedTrackColl with "
	<< sortedTrackColl.size()<< "tracks in "<<nBlocks<<" Blocks."<< endl;
  
  for(int iBlock = 0; iBlock < nBlocks ; iBlock++) {
    // Generate a random integer between 0 and nTrkPerBlock_-1
    int  irand =  rand()  % nTrkPerBlock_; 
    // Loop over the tracks in Block iBlock
    for (int itrack_block = 0; itrack_block < nTrkPerBlock_; itrack_block++) {
      unsigned track_idx = iBlock * nTrkPerBlock_ + itrack_block; 
      reco::Track itrack = sortedTrackColl[track_idx]; 
      if(debug_) cout<<"Track # "<<track_idx<<"(iBlock "<<iBlock<<", irand = "<<irand <<"): pT = "<<itrack.pt()<<endl;
      if( itrack_block == irand ) 
	splittedTColl1_->push_back(itrack);
      else 
	splittedTColl2_->push_back(itrack);
    }
  }

  if(debug_) {
    //Iterate over splittedTColl1 
    cout<<"VtxTrackSplitterProducer::splittedTColl1->size() = " << splittedTColl1_->size()<<endl;
    if (splittedTColl1_->size() != 0) {
      cout<<"TrackSplitterProducer Looping over splittedTColl1: " <<endl;
      for(unsigned int i = 0; i< splittedTColl1_->size(); i++)
	cout<< "Track "<<i<<" : pT = "<< splittedTColl1_->at(i).pt()<<endl;
    }
    //Iterate over splittedTColl2
    cout<<"VtxTrackSplitterProducer::splittedTColl2->size() = " << splittedTColl2_->size()<<endl;
    if(splittedTColl2_->size() != 0) {
      cout<<"TrackSplitterProducer Looping over splittedTColl2: " <<endl;
      for(unsigned int i = 0; i< splittedTColl2_->size(); i++)
	cout<< "Track "<<i<<" : pT = "<< splittedTColl2_->at(i).pt()<<endl;
    }
  } // end of debug
  
  // ==== Step 4: Put the Splitted TrackCollections into Event
  iEvent.put(splittedTColl1_, "SplittedTracks1");
  iEvent.put(splittedTColl2_, "SplittedTracks2");
}

//DEFINE_FWK_MODULE(VtxTrackSplitterProducer);
