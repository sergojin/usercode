import FWCore.ParameterSet.Config as cms

process = cms.Process("pvtxana")
process.load('Configuration/StandardSequences/Services_cff')
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load('Configuration/StandardSequences/GeometryExtended_cff')
process.load('Configuration/StandardSequences/MagneticField_AutoFromDBCurrent_cff')

process.MessageLogger.cerr.FwkReport.reportEvery = 1000
#process.MessageLogger.categories = ['Analysis']

#======================================
# Global Tag
#======================================
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'GR09_R_34X_V2::All'

#======================================
# Input
#======================================
process.load("INPUT_FILE")                                                 
process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(-1) )
process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange('LUMIRANGE')

#====================================
# Filters
#====================================
# HLT Filter
process.load("HLTrigger.special.HLTTriggerTypeFilter_cfi")
# 0=random, 1=physics, 2=calibration, 3=technical
process.hltTriggerTypeFilter.SelectedTriggerType = 1

# L1 Trigger Bit Selection (bit 40 and 41 for BSC trigger)
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')
process.hltLevel1GTSeed.L1TechTriggerSeeding = cms.bool(True)
process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0 AND (40 OR 41) AND NOT (36 OR 37 OR 38 OR 39)')

#====================================
# TrackSplitterProducer
#====================================
process.load("UserCode.PVStudy.TrackSplitterProducer_cff")
#process.TrackSplitterProducer.Debug = cms.bool(True)
process.TrackSplitterProducer.beamSpot = cms.InputTag("offlineBeamSpot")

#====================================
# PVT for SplittedTracks1
#====================================
process.load("RecoVertex.Configuration.RecoVertex_cff")
process.PVProducer1 = process.offlinePrimaryVertices.clone()
process.PVProducer1.TrackLabel = cms.InputTag("TrackSplitterProducer","SplittedTracks1")
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
process.PVProducer2.TrackLabel = cms.InputTag("TrackSplitterProducer","SplittedTracks2")
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
process.ana.verbose = False
process.ana.realData = True
#process.ana.analyzeOnTheFly = True
process.ana.saventuple = True
process.ana.OutputFileName = 'OUTFILE1'
process.ana.histoFileName = 'OUTFILE2'
process.ana.nTrkMin = 2
process.ana.nTrkMax = 40 
process.ana.ntrkdiffcut = 0.1

#======================================
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False), ## default is false
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.allPath=cms.Path(process.hltTriggerTypeFilter*process.hltLevel1GTSeed*process.TrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.ana)

