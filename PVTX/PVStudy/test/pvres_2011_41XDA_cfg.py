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
process.GlobalTag.globaltag ='GR_E_V14::All'

#======================================
# Input
#======================================
process.source = cms.Source("PoolSource",
                            skipEvents = cms.untracked.uint32(0),
                            fileNames = cms.untracked.vstring(
    'file:/smurf/skim/Run2010A-Express-SingleMuon/ExpressPhysicsRun2011A-Express-v1FEVT_SinglMuonSkim.root',
    ),
                            )
process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(1000))

#====================================
# Refit the vertex using the DA algorithm
#====================================
process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVerticesDA_cfi")
process.PVProducer = process.offlinePrimaryVerticesDA.clone()
process.PVProducer.TkClusParameters.coolingFactor = cms.double(0.6)
process.PVProducer.TkClusParameters.Tmin = cms.double(4.0)
process.PVProducer.TkClusParameters.vertexSize = cms.double(0.01)
process.PVProducer.TkClusParameters.d0CutOff = cms.double(3.0)
process.PVProducer.TkClusParameters.dzCutOff = cms.double(4.0)

#======================================
# Quality Filter on the primary vertex
#======================================
process.primaryVertexFilter = cms.EDFilter("VertexSelector",
                                           src = cms.InputTag("PVProducer"),
                                           cut = cms.string("!isFake && ndof > 0 && abs(z) <= 24 && position.Rho <= 2"),
                                           filter = cms.bool(True)
                                           )

#====================================
# VtxTrackSplitterProducer
#====================================
process.load("UserCode.PVStudy.VtxTrackSplitterProducer_cff")
process.VtxTrackSplitterProducer.vertexCollectionTag = "PVProducer"

#====================================
# PVProducer based on SplittedTracks1
#====================================
process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVerticesDA_cfi")
process.PVProducer1 = process.offlinePrimaryVerticesDA.clone()
process.PVProducer1.TrackLabel = cms.InputTag("VtxTrackSplitterProducer","SplittedTracks1")
process.PVProducer1.TkClusParameters.algorithm = 'DA'
process.PVProducer1.TkClusParameters.coolingFactor = cms.double(0.6)
process.PVProducer1.TkClusParameters.Tmin = cms.double(4.0)
process.PVProducer1.TkClusParameters.vertexSize = cms.double(0.01)
process.PVProducer1.TkClusParameters.d0CutOff = cms.double(3.0)
process.PVProducer1.TkClusParameters.dzCutOff = cms.double(4.0)

#====================================
# PVProducer based on SplittedTracks2
#====================================
process.PVProducer2 = process.offlinePrimaryVerticesDA.clone()
process.PVProducer2.TrackLabel = cms.InputTag("VtxTrackSplitterProducer","SplittedTracks2")
process.PVProducer2.beamSpotLabel = cms.InputTag("offlineBeamSpot")
process.PVProducer2.TkClusParameters.algorithm = 'DA'
process.PVProducer2.TkClusParameters.coolingFactor = cms.double(0.6)
process.PVProducer2.TkClusParameters.Tmin = cms.double(4.0)
process.PVProducer2.TkClusParameters.vertexSize = cms.double(0.01)
process.PVProducer2.TkClusParameters.d0CutOff = cms.double(3.0)
process.PVProducer2.TkClusParameters.dzCutOff = cms.double(4.0)

#====================================
# PVT Analyzer 
#==================================== 
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pvstudy_cfi")
process.ana.realData = True
process.ana.histoFileName = 'histo_Run2011A_Express_DA.root'
process.ana.nTrkMin = 5
process.ana.nTrkMax = 100
process.ana.ntrkdiffcut = cms.untracked.double(0.1)
process.ana.avgTrkPtInPVMin = cms.untracked.double(0)
process.ana.avgTrkPtInPVMax = cms.untracked.double(10000)

#======================================
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False),
                                      SkipEvent = cms.untracked.vstring('ProductNotFound')
                                      )
process.allPath=cms.Path(process.PVProducer*process.primaryVertexFilter*process.VtxTrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.ana)
