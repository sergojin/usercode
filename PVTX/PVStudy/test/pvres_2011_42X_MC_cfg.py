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
#process.GlobalTag.globaltag ='MC_42_V13::All'
process.GlobalTag.globaltag ='DESIGN44_V10B::All'

#======================================
# Input
#======================================

readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)

#process.source = cms.Source("PoolSource",
#                            skipEvents = cms.untracked.uint32(0),
#                            fileNames = cms.untracked.vstring(

#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/FE151BA9-6FBC-E011-9632-003048678B8E.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/FCFFCE4D-98BC-E011-B98E-002354EF3BCE.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/FCCEAADE-98BC-E011-8961-002618943916.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/FC3F255E-65BC-E011-B1F4-002354EF3BDA.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/FACBD524-FABC-E011-BF71-003048D3FC94.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/FA87F3A9-69BC-E011-9BE5-002354EF3BDA.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/FA4F710D-A7BC-E011-9A41-002618943916.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/FA227E82-7FBC-E011-80DA-002618943896.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/F8E26E64-80BC-E011-A0F3-002618943901.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/F897F0B7-78BC-E011-8291-00261894392B.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0009/F6FB7DA9-7EBC-E011-B1C4-002354EF3BDE.root'

#        ),
#    )
#process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(30000))
 
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
process.ana.realData = False
process.ana.saventuple = True 
process.ana.histoFileName = 'histo_MBMC_Pythia.root'
process.ana.OutputFileName = 'ntuple_MBMC_Pythia.root'
process.ana.Menu = 'HLT'
process.ana.filterHLT = False
process.ana.filterExpressions = cms.vstring("HLT_ZeroBias_v4",
                                            "HLT_Physics_v2",
                                            "HLT_Random_v1")
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
