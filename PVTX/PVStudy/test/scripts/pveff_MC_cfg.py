import FWCore.ParameterSet.Config as cms

process = cms.Process("pvtxana")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.Geometry_cff")

process.MessageLogger.cerr.FwkReport.reportEvery = 1000
# For the associators
process.load("Validation.RecoTrack.TrackValidation_cff")
#process.load("SimTracker.TrackAssociation.TrackAssociatorByHits_cfi")

#======================================
# Global Tag
#======================================
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag ='GR_R_35X_V7A::All'

#======================================
# Input
#======================================
process.load("UserCode.PVStudy.MinBias_Commissioning10_Apr20ReReco_v1_cfi")
process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(1000))

#======================================
# Trigger Filter
#======================================
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
from HLTrigger.HLTfilters.hltLevel1GTSeed_cfi import hltLevel1GTSeed
process.L1MinBias = hltLevel1GTSeed.clone(
    L1TechTriggerSeeding = cms.bool(True),
    L1SeedsLogicalExpression = cms.string('(40 OR 41) AND NOT (36 OR 37 OR 38 OR 39) AND NOT ((42 AND NOT 43) OR (43 AND NOT 42))')
    )

#======================================
# HLT
#======================================
process.HLTMinBias = cms.EDFilter("HLTHighLevel",
                                  TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
                                  HLTPaths = cms.vstring('HLT_L1_BscMinBiasOR_BptxPlusORMinus'), 
                                  eventSetupPathsKey = cms.string(''), 
                                  andOr = cms.bool(True), # how to deal with multiple triggers : True (OR) accept if ANY is true, False (AND) accept if ALL are true
                                  throw = cms.bool(False)    # throw exception on unknown path names
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
                                           filter = cms.bool(True),
                                           # otherwise it won't filter the events, just produce an empty vertex collection.
                                           )

process.GOODCOLL = cms.Sequence(process.L1MinBias*process.HLTMinBias*(process.primaryVertexFilter+process.scrapingVeto))

#====================================
# VtxTrackSplitterProducer
#====================================
process.load("UserCode.PVStudy.VtxTrackSplitterProducer_cff")
process.VtxTrackSplitterProducer.beamSpot = cms.InputTag("offlineBeamSpot")
process.VtxTrackSplitterProducer.nTrkPerBlock = cms.int32(3) # split 1:2

#====================================
# PVT for SplittedTracks1
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
# PVT for SplittedTracks2
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
# PVT Analyzer for SplittedTracks1
#==================================== 
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pveffstudy_cfi")
process.pveffana.verbose = False
process.pveffana.realData = False
process.pveffana.useAssociator = True
process.pveffana.analyze = False
process.pveffana.reqCluster = True
process.pveffana.ptcut = 0.0
process.pveffana.zsigncut = 5.0
process.pveffana.histoFileName = cms.untracked.string('histo.root')
process.pveffana.nTrkMin = cms.untracked.int32(2)
process.pveffana.nTrkMax = cms.untracked.int32(50)

#======================================
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False),
                                      SkipEvent = cms.untracked.vstring('ProductNotFound')
                                      )

process.allPath=cms.Path(process.GOODCOLL*process.VtxTrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.pveffana)
