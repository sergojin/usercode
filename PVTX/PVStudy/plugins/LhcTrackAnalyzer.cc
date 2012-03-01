// -*- C++ -*-
//
// Package:    LhcTrackAnalyzer
// Class:      LhcTrackAnalyzer
// 
/**\class LhcTrackAnalyzer LhcTrackAnalyzer.cc MySub/LhcTrackAnalyzer/src/LhcTrackAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//

// updated to 25/2/2009 5.30 pm

//
//

// system include files
#include <memory>


// user include files
#include "UserCode/PVStudy/interface/LhcTrackAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "SimDataFormats/EncodedEventId/interface/EncodedEventId.h"
#include "FWCore/Framework/interface/Event.h"
#include <FWCore/Framework/interface/ESHandle.h>
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include <SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h>
#include <SimDataFormats/TrackingHit/interface/PSimHit.h>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TROOT.h"
#include "TChain.h"
#include "TNtuple.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include <Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h>
#include <DataFormats/GeometrySurface/interface/Surface.h>
#include <DataFormats/GeometrySurface/interface/GloballyPositioned.h>
#include <Geometry/CommonDetUnit/interface/GeomDet.h>
#include "MagneticField/Engine/interface/MagneticField.h" 
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h" 

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

// Constructor

LhcTrackAnalyzer::LhcTrackAnalyzer(const edm::ParameterSet& iConfig)

{
  //now do what ever initialization is needed
  debug_    = iConfig.getParameter<bool>       ("Debug");  
  TrackCollectionTag_      = iConfig.getParameter<edm::InputTag>("TrackCollectionTag");  
  filename_ = iConfig.getParameter<std::string>("OutputFileName");
}
   
// Destructor
LhcTrackAnalyzer::~LhcTrackAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
LhcTrackAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace reco;
  using namespace std;

  //=======================================================
  // Initialize Root-tuple variables
  //=======================================================

  SetVarToZero();
  
  //=======================================================
  // Retrieve the Track information
  //=======================================================
  
  Handle< TrackCollection>  trackCollectionHandle;
  iEvent.getByLabel(TrackCollectionTag_, trackCollectionHandle);
  
  if(debug_)
    cout<<"LhcTrackAnalyzer::analyze() looping over "<< trackCollectionHandle->size()<< "tracks." << endl;       
  unsigned int i = 0;   
  for(TrackCollection::const_iterator track = trackCollectionHandle->begin(); track!= trackCollectionHandle->end(); ++track, ++i)
    {
      if ( nTracks_ >= nMaxtracks_ ) {
	std::cout << " LhcTrackAnalyzer::analyze() : Warning - Number of tracks: " << nTracks_ << " , greater than " << nMaxtracks_ << std::endl;
	continue;
      }
      pt_[nTracks_]       = track->pt();
      eta_[nTracks_]      = track->eta();
      phi_[nTracks_]      = track->phi();
      chi2_[nTracks_]     = track->chi2();
      chi2ndof_[nTracks_] = track->normalizedChi2();
      charge_[nTracks_]   = track->charge();
      qoverp_[nTracks_]   = track->qoverp();
      dz_[nTracks_]       = track->dz();
      dxy_[nTracks_]      = track->dxy();
      xPCA_[nTracks_]     = track->vertex().x();
      yPCA_[nTracks_]     = track->vertex().y();
      zPCA_[nTracks_]     = track->vertex().z(); 
      ++nTracks_;
      
      if(debug_)
	cout<< "Track "<<i<<" : pT = "<<track->pt()<<endl;
    }
  rootTree_->Fill();
} 


// ------------ method called once each job before begining the event loop  ------------
void LhcTrackAnalyzer::beginJob(const edm::EventSetup&)
{
  edm::LogInfo("beginJob") << "Begin Job" << std::endl;
  // Define TTree for output
  rootFile_ = new TFile(filename_.c_str(),"recreate");
  rootTree_ = new TTree("tree","Lhc Track tree");
  
  // Track Paramters 
  rootTree_->Branch("nTracks",&nTracks_,"nTracks/I");
  rootTree_->Branch("pt",&pt_,"pt[nTracks]/D");
  rootTree_->Branch("eta",&eta_,"eta[nTracks]/D");
  rootTree_->Branch("phi",&phi_,"phi[nTracks]/D");
  rootTree_->Branch("chi2",&chi2_,"chi2[nTracks]/D");
  rootTree_->Branch("chi2ndof",&chi2ndof_,"chi2ndof[nTracks]/D");
  rootTree_->Branch("charge",&charge_,"charge[nTracks]/I");
  rootTree_->Branch("qoverp",&qoverp_,"qoverp[nTracks]/D");
  rootTree_->Branch("dz",&dz_,"dz[nTracks]/D");
  rootTree_->Branch("dxy",&dxy_,"dxy[nTracks]/D");
  rootTree_->Branch("xPCA",&xPCA_,"xPCA[nTracks]/D");
  rootTree_->Branch("yPCA",&yPCA_,"yPCA[nTracks]/D");
  rootTree_->Branch("zPCA",&zPCA_,"zPCA[nTracks]/D");
  
}

// ------------ method called once each job just after ending the event loop  ------------
void LhcTrackAnalyzer::endJob() 
{
   if ( rootFile_ ) {
     rootFile_->Write();
     rootFile_->Close();
   }



}

void LhcTrackAnalyzer::SetVarToZero() {
  
  nTracks_ = 0;
  for ( int i=0; i<nMaxtracks_; ++i ) {
    pt_[i]        = 0;
    eta_[i]       = 0;
    phi_[i]       = 0;
    chi2_[i]      = 0;
    chi2ndof_[i]  = 0;
    charge_[i]    = 0;
    qoverp_[i]    = 0;
    dz_[i]        = 0;
    dxy_[i]       = 0;
    xPCA_[i]        = 0;
    yPCA_[i]        = 0;
    zPCA_[i]        = 0;
  }


}

//define this as a plug-in
//DEFINE_FWK_MODULE(LhcTrackAnalyzer);
