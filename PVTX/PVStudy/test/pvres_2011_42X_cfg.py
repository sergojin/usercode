import FWCore.ParameterSet.Config as cms

process = cms.Process("pvtxana")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#======================================
# Global Tag
#======================================
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag ='FT_R_42_V13A::All'
process.GlobalTag.globaltag ='FT_R_42_V20A::All'

#======================================
# Input
#======================================

readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)

#process.source = cms.Source("PoolSource",
#                            skipEvents = cms.untracked.uint32(0),
#                            fileNames = cms.untracked.vstring(
#
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/126C2DAA-A7C0-E011-8AF6-0026189438BA.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/1085BA33-C0C0-E011-BDE6-003048678F0C.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0E88C4D8-05C1-E011-8899-00248C0BE018.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0E58DA89-3BC2-E011-868B-002618943832.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0C0036A0-BBC0-E011-806C-003048D3FC94.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0A7AE049-B8C0-E011-BE89-0018F3D09658.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/08908455-D6C0-E011-B2EA-002618943854.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0846251D-BDC0-E011-A738-00304867900C.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/064C8E70-98C0-E011-9D82-003048678EE2.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/06457FA0-B5C0-E011-AA70-001A92971B8E.root'
#
#        ),
#    )

#process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(5000))
 
#======================================
# Trigger Filter
#======================================
process.primaryVertexFilter = cms.EDFilter("VertexSelector",
                                           src = cms.InputTag("offlinePrimaryVertices"),
                                           cut = cms.string("!isFake && ndof > 0 && abs(z) <= 24 && position.Rho <= 2"),
                                           filter = cms.bool(True)
                                           )

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

#====================================
# PVProducer based on SplittedTracks2
#====================================
process.PVProducer2 = process.offlinePrimaryVertices.clone()
process.PVProducer2.TrackLabel = cms.InputTag("VtxTrackSplitterProducer","SplittedTracks2")

#====================================
# PVT Analyzer 
#==================================== 
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pvstudy_cfi")
process.ana.realData = True
process.ana.saventuple = True 
process.ana.histoFileName = 'histo_Run2011A_Express.root'
process.ana.OutputFileName = 'ntuple_Run2011A_Express.root'
process.ana.Menu = 'HLT'
process.ana.filterHLT = False
#process.ana.filterExpressions = cms.vstring("HLT_ZeroBias_v4",
#                                            "HLT_Physics_v2",
#                                            "HLT_Random_v1")
process.ana.filterExpressions = cms.vstring("HLT_Jet30_v6")

process.ana.nTrkMin = 5
process.ana.nTrkMax = 100
process.ana.ntrkdiffcut = cms.untracked.double(0.1)
process.ana.avgTrkPtInPVMin = cms.untracked.double(0)
process.ana.avgTrkPtInPVMax = cms.untracked.double(10000)


#======================================
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False),
                                      SkipEvent = cms.untracked.vstring('ProductNotFound')
                                      )
process.allPath=cms.Path(process.primaryVertexFilter*process.VtxTrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.ana)
