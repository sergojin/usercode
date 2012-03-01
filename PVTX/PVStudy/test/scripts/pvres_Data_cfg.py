import FWCore.ParameterSet.Config as cms

process = cms.Process("pvtxana")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#======================================
# Global Tag
#======================================
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag ='START3X_V26A::All'

#======================================
# Input
#======================================
process.source = cms.Source("PoolSource",
                            skipEvents = cms.untracked.uint32(0),
                            fileNames = cms.untracked.vstring(
    '/store/data/Commissioning10/MinimumBias/RECO/Apr20ReReco-v1/0168/F0493EBC-654D-DF11-9B79-00261894385A.root',
    ),
                            )
process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(500000))
 
#======================================
# Trigger Filter
#======================================
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
from HLTrigger.HLTfilters.hltLevel1GTSeed_cfi import hltLevel1GTSeed
process.L1MinBias = hltLevel1GTSeed.clone(
    L1TechTriggerSeeding = cms.bool(True),
    L1SeedsLogicalExpression = cms.string('(40 OR 41) AND NOT (36 OR 37 OR 38 OR 39) AND NOT ((42 AND NOT 43) OR (43 AND NOT 42))')
    )

# require scraping filter
process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                    applyfilter = cms.untracked.bool(True),
                                    debugOn = cms.untracked.bool(False),
                                    numtrack = cms.untracked.uint32(10),
                                    thresh = cms.untracked.double(0.25)
                                    )

process.primaryVertexFilter = cms.EDFilter("VertexSelector",
                                           src = cms.InputTag("offlinePrimaryVertices"),
                                           cut = cms.string("!isFake && ndof > 0 && abs(z) <= 15 && position.Rho <= 2"),
                                           filter = cms.bool(True)
                                           )

process.GOODCOLL = cms.Sequence(process.L1MinBias*(process.primaryVertexFilter+process.scrapingVeto))

#====================================
# VtxTrackSplitterProducer
#====================================
process.load("UserCode.PVStudy.VtxTrackSplitterProducer_cff")

#====================================
# PVProducer based on SplittedTracks1
#====================================
process.load("RecoVertex.Configuration.RecoVertex_cff")
process.PVProducer1 = process.offlinePrimaryVertices.clone()
process.PVProducer1.TrackLabel = cms.InputTag("VtxTrackSplitterProducer","SplittedTracks1")
process.PVProducer1.beamSpotLabel = cms.InputTag("offlineBeamSpot")
process.PVProducer1.PVSelParameters.maxDistanceToBeam = 2
process.PVProducer1.TkFilterParameters.maxNormalizedChi2 = 20
process.PVProducer1.TkFilterParameters.minSiliconHits = 6
process.PVProducer1.TkFilterParameters.maxD0Significance = 100
process.PVProducer1.TkFilterParameters.minPixelHits = 1
process.PVProducer1.TkClusParameters.zSeparation = 1


#====================================
# PVProducer based on SplittedTracks2
#====================================
process.PVProducer2 = process.offlinePrimaryVertices.clone()
process.PVProducer2.TrackLabel = cms.InputTag("VtxTrackSplitterProducer","SplittedTracks2")
process.PVProducer2.beamSpotLabel = cms.InputTag("offlineBeamSpot") 
process.PVProducer2.PVSelParameters.maxDistanceToBeam = 2
process.PVProducer2.TkFilterParameters.maxNormalizedChi2 = 20
process.PVProducer2.TkFilterParameters.minSiliconHits = 6
process.PVProducer2.TkFilterParameters.maxD0Significance = 100
process.PVProducer2.TkFilterParameters.minPixelHits = 1
process.PVProducer2.TkClusParameters.zSeparation = 1

#====================================
# PVT Analyzer 
#==================================== 
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pvstudy_cfi")
process.ana.realData = True
process.ana.histoFileName = 'histo.root'
process.ana.nTrkMin = 2
process.ana.nTrkMax = 50
process.ana.ntrkdiffcut = cms.untracked.double(0.1)
process.ana.avgTrkPtInPVMin = cms.untracked.double(PTMIN)
process.ana.avgTrkPtInPVMax = cms.untracked.double(PTMAX)

#======================================
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False),
                                      SkipEvent = cms.untracked.vstring('ProductNotFound')
                                      )
process.allPath=cms.Path(process.GOODCOLL*process.VtxTrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.ana)
