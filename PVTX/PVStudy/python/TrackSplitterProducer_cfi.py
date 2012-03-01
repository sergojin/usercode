import FWCore.ParameterSet.Config as cms
from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *

TrackSplitterProducer = cms.EDProducer("TrackSplitterProducer",
                                       TrackCollectionTag = cms.InputTag("generalTracks"),
                                       Debug = cms.bool(False),
                                       beamSpot = cms.InputTag("offlineBeamSpot"),                                       
                                       TkFilterParameters = cms.PSet(maxNormalizedChi2 = cms.double(20.0),
                                                                     minSiliconHits = cms.int32(6), ## hits > 7
                                                                     maxD0Significance = cms.double(100.0), ## keep most primary tracks
                                                                     minPt = cms.double(0.0), ## better for softish events
                                                                     minPixelHits = cms.int32(1) ## hits > 2
                                                                     )
                                       )

