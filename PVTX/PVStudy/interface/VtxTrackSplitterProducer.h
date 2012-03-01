#ifndef VtxTrackSplitterProducer_h
#define VtxTrackSplitterProducer_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// related to reco::Track
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

using namespace edm;
using namespace std;
using namespace reco;

class VtxTrackSplitterProducer : public edm::EDProducer
{

public:
    explicit VtxTrackSplitterProducer( const edm::ParameterSet & );   
    virtual void produce(edm::Event&, const edm::EventSetup&);
    //~VtxTrackSplitterProducer();

private:
    // ----------member data ---------------------------
    edm::InputTag  vertexCollectionTag_;
    bool debug_; 
    bool useHWTrack_; 
    int nTrkPerBlock_;
    int trackWeightCut_;
    std::auto_ptr<reco::TrackCollection> splittedTColl1_;
    std::auto_ptr<reco::TrackCollection> splittedTColl2_;
};


#endif
