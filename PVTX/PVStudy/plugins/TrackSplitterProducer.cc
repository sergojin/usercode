
// -*- C++ -*-
//
// Package:    TrackSplitterProducer
// Class:      TrackSplitterProducer
// 
/**\class TrackSplitterProducer TrackSplitterProducer.cc MySub/TrackSplitterProducer/src/TrackSplitterProducer.cc

 Description: <one line class summary>

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
#include "UserCode/PVStudy/interface/TrackSplitterProducer.h"
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

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"                                                                             
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"                                                                            
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"                                                                         

TrackSplitterProducer::TrackSplitterProducer(const edm::ParameterSet & iConfig)
  : theTrackFilter(iConfig.getParameter<edm::ParameterSet>("TkFilterParameters")) 
{
  //now do what ever initialization is needed
  debug_    = iConfig.getParameter<bool>       ("Debug");  
  TrackCollectionTag_      = iConfig.getParameter<edm::InputTag>("TrackCollectionTag");  
  bsSrc = iConfig.getParameter< edm::InputTag >("beamSpot");

  produces<TrackCollection>("SplittedTracks1");
  produces<TrackCollection>("SplittedTracks2"); 
}


void TrackSplitterProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace reco;
  using namespace std;

  Handle< TrackCollection>  trackCollectionHandle;
  iEvent.getByLabel(TrackCollectionTag_, trackCollectionHandle);

  edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
  iEvent.getByLabel(bsSrc,recoBeamSpotHandle);
  reco::BeamSpot bs = *recoBeamSpotHandle;     
  
  if(trackCollectionHandle->size() == 0 ) return;
 
  // Sort the Tracks in the order of descending pT
  TrackSorter trksorter; 
  TrackCollection sortedTrackColl = trksorter.sortedList(*(trackCollectionHandle.product()));


  // Filter the track for PV
  std::vector<reco::Track> filtedTrackColl;
  edm::ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB); 


  for(TrackCollection::const_iterator track = sortedTrackColl.begin(); track!= sortedTrackColl.end(); ++track)
    {      
      TransientTrack t_track = (*theB).build(*track);
      t_track.setBeamSpot(bs);
      if (theTrackFilter(t_track)) filtedTrackColl.push_back(Track (*track));
    }
      
  // create the two splitted trackCollections
  splittedTColl1_ = std::auto_ptr<TrackCollection> (new TrackCollection );
  splittedTColl2_ = std::auto_ptr<TrackCollection> (new TrackCollection );

  unsigned int i = 0;   
  unsigned int irand = 0; 
   
  if(debug_)
    cout<<"TrackSplitterProducer looping over "<< filtedTrackColl.size()<< "tracks." << endl;  

  //for(TrackCollection::const_iterator track = sortedTrackColl.begin(); track!= sortedTrackColl.end(); ++track, ++i)
  for(TrackCollection::const_iterator track =  filtedTrackColl.begin(); track!=  filtedTrackColl.end(); ++track, ++i)
    {
      if(debug_)
	cout<< "Track "<<i<<" : pT = "<<track->pt()<<endl;
 
      //Iterate every other track
      if (track != (filtedTrackColl.end() -1) ) {
	// iterate every other track
	irand =  rand()  % 2; // A random number from [0,1]
	if( irand == 0 )
	  {
	    splittedTColl1_->push_back( Track( *track ) );
	    splittedTColl2_->push_back( Track( *(++track) ) );	
	    ++i;
	  }
	if( irand == 1 )
	  {
	    splittedTColl2_->push_back( Track( *track ) );
	    splittedTColl1_->push_back( Track( *(++track) ) );
	    ++i;
	  }	  
	if(debug_)
	  cout<< "Track "<<i<<" : pT = "<<track->pt()<<endl;
      }
    }   
  
  //Iterate over splittedTColl1 
  if(debug_)
    {
      unsigned int j = 0;
      cout<<"TrackSplitterProducer Looping over splittedTColl1: " <<endl;
      for(TrackCollection::const_iterator track = splittedTColl1_->begin(); track!= splittedTColl1_->end(); ++track, ++j){
	cout<< "Track "<<j<<" : pT = "<<track->pt()<<endl;
      }
      j = 0;
      cout<<"TrackSplitterProducer Looping over splittedTColl2: " <<endl;
      for(TrackCollection::const_iterator track = splittedTColl2_->begin(); track!= splittedTColl2_->end(); ++track, ++j){
	cout<< "Track "<<j<<" : pT = "<<track->pt()<<endl;
      }
    }// End  if(debug_)
  
  // Put Splitted TrackCollection into Event
  iEvent.put(splittedTColl1_, "SplittedTracks1");
  iEvent.put(splittedTColl2_, "SplittedTracks2");
}

//DEFINE_FWK_MODULE(TrackSplitterProducer);
