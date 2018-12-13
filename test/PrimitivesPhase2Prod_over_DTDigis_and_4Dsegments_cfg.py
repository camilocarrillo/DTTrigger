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


## DT unpacker
process.load("EventFilter.DTRawToDigi.dtunpacker_cfi")
process.muonDTDigis.inputLabel = 'rawDataCollector'

process.load("RecoLocalMuon.Configuration.RecoLocalMuon_cff")

process.dtTriggerPhase2PrimitiveDigis.digiTag = "muonDTDigis"
process.dtTriggerPhase2PrimitiveDigis.dt4DSegments = "dt4DSegments"
process.dtTriggerPhase2PrimitiveDigis.debug = True


process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
        'file:/store/user/carrillo/digis_segments_Run2016BSingleMuonRAW-RECO_1000.root'
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






