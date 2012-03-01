import FWCore.ParameterSet.Config as cms

VtxTrackSplitterProducer = cms.EDProducer("VtxTrackSplitterProducer"
                                          ,vertexCollectionTag = cms.InputTag("offlinePrimaryVertices")
                                          ,Debug = cms.bool(False)
                                          ,useHWTrack = cms.bool(False)
                                          ,nTrkPerBlock = cms.int32(2)
                                          ,trackWeightCut = cms.double(1e-10)
                                          )

