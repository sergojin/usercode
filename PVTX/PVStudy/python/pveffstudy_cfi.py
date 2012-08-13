import FWCore.ParameterSet.Config as cms
from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *

pveffana = cms.EDAnalyzer('PVEffAnalyzer'
                          ,simG4 = cms.InputTag("g4SimHits")
                          ,trackCollection = cms.untracked.InputTag('generalTracks')
                          ,associator = cms.untracked.InputTag("TrackAssociatorByHits")
                          ,splitTrackCollection1 = cms.untracked.InputTag("VtxTrackSplitterProducer","SplittedTracks1")
                          ,splitTrackCollection2 = cms.untracked.InputTag("VtxTrackSplitterProducer","SplittedTracks2")
                          ,vertexCollection = cms.untracked.InputTag('offlinePrimaryVertices')
                          ,splitVertexCollection1 = cms.untracked.InputTag("PVProducer1")
                          ,splitVertexCollection2 = cms.untracked.InputTag("PVProducer2")
                          ,verbose = cms.untracked.bool(False)
                          ,realData = cms.untracked.bool(False)
                          ,useTP = cms.untracked.bool(False)
                          ,useAssociator = cms.untracked.bool(False)
                          ,reqCluster = cms.untracked.bool(True)
                          ,analyze = cms.untracked.bool(False)
                          ,histoFileName = cms.untracked.string('pveff_hist.root')
                          ,OutputFileName = cms.untracked.string('pveff_ntuple.root')
                          ,saventuple = cms.untracked.bool(False)
                          ,zsigncut = cms.untracked.double(5)
                          ,ptcut = cms.untracked.double(0.5) # the cut on goodTracks in vtx
                          ,nTrkMin = cms.untracked.int32(0)
                          ,nTrkMax = cms.untracked.int32(999)
                          ,beamSpot = cms.InputTag("offlineBeamSpot")
                          ,TkClusParameters = cms.PSet(
                               algorithm   = cms.string("DA"),
                               TkDAClusParameters = cms.PSet(
                               coolingFactor = cms.double(0.6),  #  moderate annealing speed
                               Tmin = cms.double(4.),            #  end of annealing
                               vertexSize = cms.double(0.01),    #  ~ resolution / sqrt(Tmin)
                               d0CutOff = cms.double(3.),        # downweight high IP tracks
                               dzCutOff = cms.double(4.)         # outlier rejection after freeze-out (T<Tmin)
                               )
                               )
                          ,TkFilterParameters = cms.PSet(
                                algorithm=cms.string('filter'),
                                maxNormalizedChi2 = cms.double(20.0),
                                minPixelLayersWithHits=cms.int32(2),
                                minSiliconLayersWithHits = cms.int32(5),
                                maxD0Significance = cms.double(5.0), 
                                minPt = cms.double(0.0),
                                trackQuality = cms.string("any")
                                )
                          )
