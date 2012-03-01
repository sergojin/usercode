import FWCore.ParameterSet.Config as cms

source = cms.Source("PoolSource",
                            # replace 'myfile.root' with the source file you want to use
                            fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_3_1_2/RelValQCD_Pt_80_120/GEN-SIM-RECO/MC_31X_V3-v1/0007/A4B8D236-9178-DE11-982D-001D09F24600.root',
    '/store/relval/CMSSW_3_1_2/RelValQCD_Pt_80_120/GEN-SIM-RECO/MC_31X_V3-v1/0006/C28BCD25-5278-DE11-9EC6-001D09F24EAC.root',
    '/store/relval/CMSSW_3_1_2/RelValQCD_Pt_80_120/GEN-SIM-RECO/MC_31X_V3-v1/0006/B2441B1B-5278-DE11-93F6-000423D99E46.root',
    '/store/relval/CMSSW_3_1_2/RelValQCD_Pt_80_120/GEN-SIM-RECO/MC_31X_V3-v1/0006/AA2E6BD1-5078-DE11-A537-0019B9F705A3.root',
    '/store/relval/CMSSW_3_1_2/RelValQCD_Pt_80_120/GEN-SIM-RECO/MC_31X_V3-v1/0006/9EB9610F-5278-DE11-9F3A-001D09F26509.root',
    '/store/relval/CMSSW_3_1_2/RelValQCD_Pt_80_120/GEN-SIM-RECO/MC_31X_V3-v1/0006/34733721-5278-DE11-BA19-001D09F2A49C.root'
    )
                            )

#======================================
# https://cmsweb.cern.ch/dbs_discovery/getLFNsForSite?dbsInst=cms_dbs_prod_global&site=all&datasetPath=/RelValQCD_Pt_80_120/CMSSW_3_1_2-MC_31X_V3-v1/GEN-SIM-RECO&what=cff&userMode=user&run=*
