import FWCore.ParameterSet.Config as cms

maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles,
                     lumisToProcess = cms.untracked.VLuminosityBlockRange(
    '124120:1-124120:max',
    '124275:1-124275:30' 
    )
                     )
readFiles.extend( [
    '/store/data/BeamCommissioning09/MinimumBias/RAW-RECO/BSCNOBEAMHALO-Dec19thSkim_336p3_v1/0008/1630A134-E0F0-DE11-8A34-001D0967DC0B.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW-RECO/BSCNOBEAMHALO-Dec19thSkim_336p3_v1/0006/EAC5876C-E6EE-DE11-9FA5-0024E87687CB.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW-RECO/BSCNOBEAMHALO-Dec19thSkim_336p3_v1/0006/10F36BBF-D9EE-DE11-90B1-0024E87683B7.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW-RECO/BSCNOBEAMHALO-Dec19thSkim_336p3_v1/0008/86029E8B-E0F0-DE11-9C7D-00151796D814.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW-RECO/BSCNOBEAMHALO-Dec19thSkim_336p3_v1/0007/C822227F-F9EE-DE11-94C3-0024E8767D52.root'
    ] );


secFiles.extend( [
               ] )

