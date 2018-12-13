import FWCore.ParameterSet.Config as cms

process = cms.Process("L1DTTrigPhase2Prod")

#process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cff")
process.load("Geometry.DTGeometry.dtGeometry_cfi")
process.DTGeometryESModule.applyAlignment = False

process.load("L1Trigger.DTTrigger.dtTriggerPhase2PrimitiveDigis_cfi")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.GlobalTag.globaltag = "90X_dataRun2_Express_v2"


process.load("RecoLocalMuon.Configuration.RecoLocalMuon_cff")
process.load("EventFilter.DTRawToDigi.dturosunpacker_cfi")
# have stareco use hlt digis
# (change all input tags in offline reco config to use hlt tags)
#process.load("MuonTools.Configuration.HltMuonDigis_cff")
# have stareco use hlt segments (1)
# (change all input tags in offline reco config to use hlt tags)
#process.load("MuonTools.Configuration.HltMuonSegments_cff")
# keep stareco from using rpcs
#process.load("MuonTools.Configuration.StandAloneNoRpc_cff")
# do we need this?
# process.load("RecoTracker.IterativeTracking.MuonSeededStep_cff")
# process.load("RecoTracker.CkfPattern.GroupedCkfTrajectoryBuilder_cfi")
# import RecoTracker.CkfPattern.GroupedCkfTrajectoryBuilder_cfi         
#process.load("RecoTracker.CkfPattern.GroupedCkfTrajectoryBuilder_cff")
#timing producer
#process.load("STASkim.ProducerTest.standAloneTiming_cff")

process.dtTriggerPhase2PrimitiveDigis.digiTag = "muonDTDigis"
process.dtTriggerPhase2PrimitiveDigis.dt4DSegments = "dt4DSegments"
process.dtTriggerPhase2PrimitiveDigis.debug = True

## DT unpacker
process.load("EventFilter.DTRawToDigi.dtunpacker_cfi")
process.muonDTDigis.inputLabel = 'rawDataCollector'

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
        'file:digis_segments_Run2016BSingleMuonRAW-RECO.root'
        )
                            )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
#    input = cms.untracked.int32(100000)
)

process.out = cms.OutputModule("PoolOutputModule",
                               outputCommands = cms.untracked.vstring('keep *'),
                               fileName = cms.untracked.string('DTTriggerPhase2Primitives.root')
)



process.p = cms.Path(process.dtTriggerPhase2PrimitiveDigis)
process.this_is_the_end = cms.EndPath(process.out)






