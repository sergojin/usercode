import FWCore.ParameterSet.Config as cms

maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0008/E07F4B02-CD7C-DE11-AF7D-001D09F2AF1E.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0008/B891B5F4-9A7C-DE11-86D8-001D09F2AF1E.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0008/96CD4EE7-C77C-DE11-8077-001D09F2AD4D.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0008/7EEE6B8B-DA7C-DE11-95AC-001D09F24EE3.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0008/6A7CCA50-837C-DE11-BDBB-000423D98DB4.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0008/26AC5805-917C-DE11-B975-0019B9F70607.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/FE760D2E-4D7C-DE11-A433-000423D98950.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/F810B0CA-4C7C-DE11-AC49-000423D95220.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/F0190D04-4C7C-DE11-B6EC-000423D9517C.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/E63A7E20-4D7C-DE11-8FFD-000423D98930.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/D8D11C0F-7D7C-DE11-BF8A-001D09F24691.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/CCEC008A-507C-DE11-9D13-000423D33970.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/CCAA43D4-4B7C-DE11-B40B-000423D98930.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/A8A49E62-4B7C-DE11-B97A-000423D6A6F4.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/9A99DDA0-4B7C-DE11-807C-000423D98868.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/94FD3D6D-4D7C-DE11-956D-001D09F29533.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/8EAE940C-4D7C-DE11-A242-001D09F2A465.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/7EAC725E-4D7C-DE11-AB5C-000423D9A212.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/7C915EC2-4C7C-DE11-9225-000423D996C8.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/786A9241-4C7C-DE11-A52F-000423D99B3E.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/72D218B7-4D7C-DE11-A3BA-001D09F251D1.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/6C9A4A87-4D7C-DE11-8CFC-000423D94494.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/2E0E2497-4C7C-DE11-8E37-000423D95220.root',
       '/store/relval/CMSSW_3_1_2/RelValZmumuJets_Pt_20_300_GEN/GEN-SIM-RECO/MC_31X_V3_LowLumiPileUp-v1/0007/2667AC9D-767C-DE11-8E70-001D09F25460.root' ] );


secFiles.extend( [
               ] )

