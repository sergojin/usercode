import FWCore.ParameterSet.Config as cms

ana = cms.EDAnalyzer('PVStudy'
                     ,simG4 = cms.InputTag("g4SimHits")
                     ,trackCollection = cms.untracked.InputTag('generalTracks')
                     ,splitTrackCollection1 = cms.untracked.InputTag("VtxTrackSplitterProducer","SplittedTracks1")
                     ,splitTrackCollection2 = cms.untracked.InputTag("VtxTrackSplitterProducer","SplittedTracks2")
                     ,vertexCollection = cms.untracked.InputTag('offlinePrimaryVertices')
                     ,splitVertexCollection1 = cms.untracked.InputTag("PVProducer1")
                     ,splitVertexCollection2 = cms.untracked.InputTag("PVProducer2")
                     ,verbose = cms.untracked.bool(False)
                     ,realData = cms.untracked.bool(False)
                     #max sep in z allowed for the two-vertices pair to be analyzed 
                     ,zsigncut = cms.untracked.int32(5)
                     ,analyzeOnTheFly = cms.untracked.bool(False)
                     ,saventuple = cms.untracked.bool(False)
                     ,ntrkdiffcut = cms.untracked.double(1.0)
                     ,avgTrkPtInPVMin = cms.untracked.double(-1.0)
                     ,avgTrkPtInPVMax = cms.untracked.double(999999)
                     ,OutputFileName = cms.untracked.string('ntuple.root')
                     ,nTrkMin = cms.untracked.int32(0)
                     ,nTrkMax = cms.untracked.int32(999)
                     ,beamSpot = cms.InputTag("offlineBeamSpot")
                     ,histoFileName = cms.untracked.string('pvstudy_histo.root')
                     ,Menu = cms.string('HLT')
                     ,filterHLT = cms.untracked.bool(False) 
                     ,filterExpressions = cms.vstring("HLT_ZeroBias_v4",
                                                      "HLT_Physics_v2",
                                                      "HLT_Random_v1")
                     ,useHWTrk = cms.untracked.bool(False)
                     ,uncorJetsInputTag     = cms.InputTag("ak5CaloJets")
                     ,CaloJetCorrectorL2L3  = cms.string('ak5CaloL2L3')
                     ,CaloJetCorrectorL1L2L3 = cms.string('ak5CaloL1L2L3')
)
