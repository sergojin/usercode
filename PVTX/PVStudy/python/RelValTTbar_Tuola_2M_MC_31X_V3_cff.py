import FWCore.ParameterSet.Config as cms

source = cms.Source("PoolSource",
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

#======================================
# https://cmsweb.cern.ch/dbs_discovery/getLFNsForSite?dbsInst=cms_dbs_prod_global&site=all&datasetPath=/RelValTTbar_Tauola_2M/CMSSW_3_1_2-MC_31X_V3-v1/GEN-SIM-RECO&what=cff&userMode=user&run=*
