import FWCore.ParameterSet.Config as cms

process = cms.Process("pvtxana")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

process.MessageLogger.cerr.FwkReport.reportEvery = 1000
# For the associators
process.load("Validation.RecoTrack.TrackValidation_cff")
#process.load("SimTracker.TrackAssociation.TrackAssociatorByHits_cfi")

#======================================
# Global Tag
#======================================
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
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
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/1085BA33-C0C0-E011-BDE6-003048678F0C.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0E88C4D8-05C1-E011-8899-00248C0BE018.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0E58DA89-3BC2-E011-868B-002618943832.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0C0036A0-BBC0-E011-806C-003048D3FC94.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0A7AE049-B8C0-E011-BE89-0018F3D09658.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/08908455-D6C0-E011-B2EA-002618943854.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/0846251D-BDC0-E011-A738-00304867900C.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/064C8E70-98C0-E011-9D82-003048678EE2.root',
#    '/store/data/Run2011A/MinimumBias/AOD/05Aug2011-v1/0000/06457FA0-B5C0-E011-AA70-001A92971B8E.root'

#        ),
#    )


#======================================
# Trigger Filter
#======================================
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
from HLTrigger.HLTfilters.hltLevel1GTSeed_cfi import hltLevel1GTSeed
process.L1MinBias = hltLevel1GTSeed.clone(
    L1TechTriggerSeeding = cms.bool(True),
    L1SeedsLogicalExpression = cms.string('0 AND (40 OR 41) AND NOT (36 OR 37 OR 38 OR 39) AND NOT ((42 AND NOT 43) OR (43 AND NOT 42))')
    )



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
                                           filter = cms.bool(True),
                                           # otherwise it won't filter the events, just produce an empty vertex collection.
                                           )

#process.GOODCOLL = cms.Sequence(process.L1MinBias*process.VertexCountFilter*(process.primaryVertexFilter+process.scrapingVeto))
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
#process.PVProducer1.trackWeightCut = cms.double(0.0)

#====================================
# PVProducer based on SplittedTracks2
#====================================
process.PVProducer2 = process.offlinePrimaryVertices.clone()
process.PVProducer2.TrackLabel = cms.InputTag("VtxTrackSplitterProducer","SplittedTracks2")
#process.PVProducer2.trackWeightCut = cms.double(0.0)

#====================================
# PVT Analyzer for SplittedTracks1
#==================================== 
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pveffstudy_cfi")
process.pveffana.verbose = False
process.pveffana.realData = True
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
#    input = cms.untracked.int32(10000)
#     )

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False),
                                      SkipEvent = cms.untracked.vstring('ProductNotFound')
                                      )

process.allPath=cms.Path(process.GOODCOLL*process.offlineBeamSpot*process.offlinePrimaryVertices*process.VtxTrackSplitterProducer*(process.PVProducer1+process.PVProducer2)*process.pveffana)
