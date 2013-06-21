import FWCore.ParameterSet.Config as cms

process = cms.Process("pvtxana")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("SimGeneral.MixingModule.mixNoPU_cfi")

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#======================================
# Global Tag
#======================================
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag ='MC_42_V13::All'
process.GlobalTag.globaltag ='START42_V13::All'

# Associators
# For the associators
process.load("SimTracker.TrackAssociation.TrackAssociatorByHits_cfi")
process.load("SimGeneral.TrackingAnalysis.trackingParticles_cfi")
process.load("Validation.RecoTrack.TrackValidation_cff")

process.TrackAssociatorByHits.SimToRecoDenominator = cms.string('reco')
process.TrackAssociatorByHits.Purity_SimToReco = cms.double(0.75)
process.TrackAssociatorByHits.Cut_RecoToSim = cms.double(0.75)


#======================================
# Input
#======================================
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)


#process.source = cms.Source("PoolSource",
#                            skipEvents = cms.untracked.uint32(0),
#                            fileNames = cms.untracked.vstring(
#    '/store/mc/Summer11/MinBias_TuneD6T_7TeV-pythia6/AODSIM/NoPileup_START42_V11-v2/0000/205605F8-393D-E111-8B73-002618943939.root',
#    '/store/mc/Summer11/MinBias_TuneD6T_7TeV-pythia6/AODSIM/NoPileup_START42_V11-v2/0000/A2DB5CA2-3A3D-E111-95FF-002618FDA210.root',
#    '/store/mc/Summer11/MinBias_TuneD6T_7TeV-pythia6/AODSIM/NoPileup_START42_V11-v2/0000/4E3CAF07-3A3D-E111-9FBC-003048679228.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/GEN-SIM-RECO/IDEAL_PU_S4_START42_V11-v1/0000/D212782D-16BE-E011-B63F-003048D3FC94.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0000/2C0436C5-D4B8-E011-ADB7-001A9281171E.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0000/267A8C71-D5B8-E011-A1F6-001BFCDBD19E.root',
#    '/store/mc/Summer11/MinBias_TuneZ2_7TeV-pythia6/AODSIM/IDEAL_PU_S4_START42_V11-v1/0000/620B6847-47BE-E011-9F81-001A92971B3C.root',
#       ),
#    )

# require scraping filter
process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                    applyfilter = cms.untracked.bool(True),
                                    debugOn = cms.untracked.bool(False),
                                    numtrack = cms.untracked.uint32(10),
                                    thresh = cms.untracked.double(0.25)
                                    )

process.VertexCountFilter = cms.EDFilter("VertexCountFilter",
                                        src = cms.InputTag('offlinePrimaryVertices'),
                                        minNumber = cms.uint32(1),
                                        maxNumber = cms.uint32(5)
                                        )

process.primaryVertexFilter = cms.EDFilter("VertexSelector",
                                           src = cms.InputTag("offlinePrimaryVertices"),
                                           cut = cms.string("!isFake && ndof > 0 && abs(z) <= 24 && position.Rho <= 2"),
#                                           cut = cms.string(""),
                                           filter = cms.bool(True),
                                           # otherwise it won't filter the events, just produce an empty vertex collection.
                                           )

process.GOODCOLL = cms.Sequence(process.VertexCountFilter*(process.primaryVertexFilter+process.scrapingVeto))

#====================================
# VtxTrackSplitterProducer
#====================================
process.load("UserCode.PVStudy.VtxTrackSplitterProducer_cff")
process.VtxTrackSplitterProducer.beamSpot = cms.InputTag("offlineBeamSpot")
process.VtxTrackSplitterProducer.nTrkPerBlock = cms.int32(3) # split 1:2
process.VtxTrackSplitterProducer.trackWeightCut = cms.double(1.0e-09)
process.VtxTrackSplitterProducer.useHWTrack = cms.bool(False)
process.VtxTrackSplitterProducer.trackPtCut = cms.double(0.3)

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
# PVT Analyzer for SplittedTracks1
#==================================== 
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pveffstudy_cfi")
process.pveffana.verbose = False
process.pveffana.realData = False
process.pveffana.useAssociator = False
process.pveffana.analyze = True
process.pveffana.reqCluster = True
process.pveffana.ptcut = 0.3
process.pveffana.zsigncut = 5.0
process.pveffana.histoFileName = cms.untracked.string('histo.root')
process.pveffana.nTrkMin = cms.untracked.int32(2)
process.pveffana.nTrkMax = cms.untracked.int32(50)

#======================================
#process.maxEvents = cms.untracked.PSet(
#    input = cms.untracked.int32(1000)
#     )


#======================================
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False),
                                      SkipEvent = cms.untracked.vstring('ProductNotFound')
                                      )

process.allPath=cms.Path(process.GOODCOLL*process.offlineBeamSpot*process.offlinePrimaryVertices*process.VtxTrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.pveffana)
#process.allPath=cms.Path(process.primaryVertexFilter*process.VtxTrackSplitterProducer*process.pveffana)

