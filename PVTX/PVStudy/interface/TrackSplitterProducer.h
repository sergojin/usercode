#ifndef TrackSplitterProducer_h
#define TrackSplitterProducer_h

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
#include <DataFormats/VertexReco/interface/VertexFwd.h>

// Track filter for PV
#include "RecoVertex/PrimaryVertexProducer/interface/TrackFilterForPVFinding.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"   
#include "TrackingTools/Records/interface/TransientTrackRecord.h" 

// vertex stuff
#include <DataFormats/VertexReco/interface/VertexFwd.h>
#include <DataFormats/VertexReco/interface/Vertex.h>
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

using namespace edm;
using namespace std;
using namespace reco;

class TrackSplitterProducer : public edm::EDProducer
{

public:
    explicit TrackSplitterProducer( const edm::ParameterSet & );   
    virtual void produce(edm::Event&, const edm::EventSetup&);
    //~TrackSplitterProducer();

private:
    // ----------member data ---------------------------
    edm::InputTag TrackCollectionTag_; 
    edm::InputTag vertexCollectionTag_;
    edm::InputTag pixelVertexCollectionTag_; 
    edm::InputTag bsSrc;

    double min_pvtx_zsep_; // min zsep between secondary/leading pvtx
    double trkFilterZcut_; //filter tracks based on Z cuts from pixelVertices
    bool debug_;

    std::auto_ptr<reco::TrackCollection> splittedTColl1_;
    std::auto_ptr<reco::TrackCollection> splittedTColl2_;
    TrackFilterForPVFinding theTrackFilter; 

};





#endif
