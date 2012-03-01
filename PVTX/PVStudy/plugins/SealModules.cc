#include "FWCore/Framework/interface/MakerMacros.h"

#include "UserCode/PVStudy/interface/LhcTrackAnalyzer.h"
#include "UserCode/PVStudy/interface/PVStudy.h"
#include "UserCode/PVStudy/interface/PVEffAnalyzer.h"
#include "UserCode/PVStudy/interface/TrackSplitterProducer.h"
#include "UserCode/PVStudy/interface/VtxTrackSplitterProducer.h" 

// DEFINE_SEAL_MODULE();
DEFINE_FWK_MODULE(LhcTrackAnalyzer);
DEFINE_FWK_MODULE(PVStudy);
DEFINE_FWK_MODULE(PVEffAnalyzer);
DEFINE_FWK_MODULE(TrackSplitterProducer);
DEFINE_FWK_MODULE(VtxTrackSplitterProducer);
