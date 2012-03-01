import FWCore.ParameterSet.Config as cms

LhcTrackAnalyzer = cms.EDAnalyzer("LhcTrackAnalyzer",
                                  TrackCollectionTag = cms.InputTag("generalTracks"),
                                  OutputFileName = cms.string("LhcTrackAnalyzer.root"),
                                  Debug = cms.bool(False)
                                  )

