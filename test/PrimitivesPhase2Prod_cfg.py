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


process.dtTriggerPhase2PrimitiveDigis.digiTag = "hltMuonDTDigis"
process.dtTriggerPhase2PrimitiveDigis.debug = True


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/tmp/carrillo/08FB56BD-80B4-E811-8B09-FA163EC6F3D0.root')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('DTTriggerPhase2Primitives.root')
)

process.p = cms.Path(process.dtTriggerPhase2PrimitiveDigis)
process.this_is_the_end = cms.EndPath(process.out)

