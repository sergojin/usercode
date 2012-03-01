import FWCore.ParameterSet.Config as cms

process = cms.Process("PVStudy")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("UserCode.PVStudy.pvstudy_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
       '/store/relval/CMSSW_3_1_2/RelValTTbar_Tauola_2M/GEN-SIM-RECO/MC_31X_V3-v1/0007/0AB2E447-B478-DE11-8BD8-000423D992DC.root',
       '/store/relval/CMSSW_3_1_2/RelValTTbar_Tauola_2M/GEN-SIM-RECO/MC_31X_V3-v1/0007/402647C3-B378-DE11-A125-001D09F2447F.root',
       '/store/relval/CMSSW_3_1_2/RelValTTbar_Tauola_2M/GEN-SIM-RECO/MC_31X_V3-v1/0007/4ECE7508-B278-DE11-99C2-001D09F291D2.root',
       '/store/relval/CMSSW_3_1_2/RelValTTbar_Tauola_2M/GEN-SIM-RECO/MC_31X_V3-v1/0007/50F2D5A9-B278-DE11-97AF-0019B9F7312C.root',
       '/store/relval/CMSSW_3_1_2/RelValTTbar_Tauola_2M/GEN-SIM-RECO/MC_31X_V3-v1/0007/9C9FD484-CB78-DE11-B4B8-000423D99896.root',
       '/store/relval/CMSSW_3_1_2/RelValTTbar_Tauola_2M/GEN-SIM-RECO/MC_31X_V3-v1/0007/A0079BA6-B378-DE11-A7A4-001D09F291D2.root',
       '/store/relval/CMSSW_3_1_2/RelValTTbar_Tauola_2M/GEN-SIM-RECO/MC_31X_V3-v1/0007/AE71556E-B478-DE11-B8C2-001D09F24637.root'
    )
)
process.ana.verbose = False
process.ana.realData = False
process.ana.analyzeOnTheFly = True

process.TFileService = cms.Service("TFileService",
    fileName = cms.string(
        'test.root'
    )
)

#process.Tracer = cms.Service("Tracer",
#    indention = cms.untracked.string('>>>')
#)

process.p = cms.Path(process.ana)
