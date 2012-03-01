import FWCore.ParameterSet.Config as cms

process = cms.Process("pvtxana")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.Geometry_cff")

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#======================================
# Global Tag
#======================================
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag ='GR09_R_V5::All'

#======================================
# Input
#======================================
process.load("UserCode.PVStudy.MinBias2360GeV_BSCNOBEAMHALO_Dec19thSkim_v1_cff")
process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(-1))

#======================================
# Trigger Filter
#======================================
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
from HLTrigger.HLTfilters.hltLevel1GTSeed_cfi import hltLevel1GTSeed
process.bit40 = hltLevel1GTSeed.clone(
    L1TechTriggerSeeding = cms.bool(True),
    L1SeedsLogicalExpression = cms.string('(40 OR 41) AND NOT (36 OR 37 OR 38 OR 39)')
    )


#====================================
# VtxTrackSplitterProducer
#====================================
process.load("UserCode.PVStudy.VtxTrackSplitterProducer_cff")
#process.VtxTrackSplitterProducer.Debug = cms.bool(True)

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
process.PVProducer1.TkClusParameters.zSeparation = 10


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
process.PVProducer2.TkClusParameters.zSeparation = 10


#====================================
# PVT Analyzer for SplittedTracks1
#==================================== 
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pvstudy_cfi")
#process.ana.verbose = True
process.ana.realData = True
#process.ana.analyzeOnTheFly = True
process.ana.saventuple = True
process.ana.OutputFileName = 'pvtxana_ntuple_MB2360GeV_Dec19thReReco.root'
process.ana.histoFileName = cms.untracked.string('pvtxana_hist_MB2360GeV_Dec19thReReco.root')
process.ana.nTrkMin = cms.untracked.int32(2)
process.ana.nTrkMax = cms.untracked.int32(40)    
process.ana.ntrkdiffcut = cms.untracked.double(0.1)

#======================================
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) ) ## default is false
process.allPath=cms.Path(process.bit40*process.VtxTrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.ana)
