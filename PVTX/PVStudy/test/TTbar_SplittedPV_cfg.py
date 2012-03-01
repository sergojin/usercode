import FWCore.ParameterSet.Config as cms

process = cms.Process("pvtxana")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.Geometry_cff")

process.MessageLogger.cerr.FwkReport.reportEvery = 1000
#process.MessageLogger.categories = ['Analysis']

#======================================
# Global Tag
#======================================
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'MC_31X_V3::All'
process.prefer("GlobalTag")

#======================================
# Input
#======================================
process.load("UserCode.PVStudy.RelValTTbar_Tuola_2M_MC_31X_V3_cff")                                                 
process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(-1) )

#====================================
# TrackSplitterProducer
#====================================

process.load("UserCode.PVStudy.TrackSplitterProducer_cff")
#process.TrackSplitterProducer.Debug = cms.bool(True)

#====================================
# PVT for SplittedTracks1
#====================================
process.load("RecoVertex.Configuration.RecoVertex_cff")
process.PVProducer1 = process.offlinePrimaryVertices.clone()
process.PVProducer1.TrackLabel = cms.InputTag("TrackSplitterProducer","SplittedTracks1")

#====================================
# PVT for SplittedTracks2
#====================================
process.PVProducer2 = process.offlinePrimaryVertices.clone()
process.PVProducer2.TrackLabel = cms.InputTag("TrackSplitterProducer","SplittedTracks2")

#====================================
# PVT Analyzer for SplittedTracks1
#==================================== 
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pvstudy_cfi")
process.ana.verbose = False
process.ana.realData = False
process.ana.analyzeOnTheFly = False
process.ana.saventuple = True
process.ana.OutputFileName = "ntuple/pvtxana_ntuple_ttbar.root"
process.ana.histoFileName = "ntuple/pvtxana_hist_ttbar.root"
process.ana.nTrkMin = 2
process.ana.nTrkMax = 100    
process.ana.ntrkdiffcut = 0.1

#======================================
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) ) ## default is false
process.allPath=cms.Path(process.TrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.ana)
