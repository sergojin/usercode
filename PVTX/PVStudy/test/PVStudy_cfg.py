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
process.GlobalTag.globaltag ='GLOBALTAG::All'
process.prefer("GlobalTag")

#======================================
# Input
#======================================
process.load("UserCode.PVStudy.DATASET_cff")
process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(NEVENT) )

#====================================
# TrackSplitterProducer
#====================================

process.load("UserCode.PVStudy.TrackSplitterProducer_cff")
#process.TrackSplitterProducer.Debug = cms.bool(True)
process.TrackSplitterProducer.trkFilterZcut = cms.double(TRKFILTERZCUT)
process.TrackSplitterProducer.minPvtxZsep = cms.double(MINPVTXZSEP)

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
process.ana.analyzeOnTheFly = True
process.ana.saventuple = True
process.ana.OutputFileName = 'OUTPUTDIR/pvtxana_ntuple_SAMPLE.root'
process.ana.nTrkMin = cms.untracked.int32(NTRKMIN)
process.ana.nTrkMax = cms.untracked.int32(NTRKMAX)    
process.ana.ntrkdiffcut = cms.untracked.double(NTRKDIFFCUT)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('OUTPUTDIR/pvtxana_hist_SAMPLE.root')
                                   )

#======================================
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) ) ## default is false
process.allPath=cms.Path(process.TrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.ana)
