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
    fileNames = cms.untracked.vstring('file:/tmp/carrillo/08FB56BD-80B4-E811-8B09-FA163EC6F3D0.root',
                                      'file:/tmp/carrillo/08FB56BD-80B4-E811-8B09-FA163EC6F3D0.root',
                                      'file:/tmp/carrillo/30B6F74D-94B4-E811-91DC-FA163E4DF038.root',
                                      'file:/tmp/carrillo/3E29E4E4-80B4-E811-9100-FA163E31B17C.root',
                                      'file:/tmp/carrillo/5A63E272-8AB4-E811-A260-FA163E3B4D11.root',
                                      'file:/tmp/carrillo/5E876866-8EB4-E811-B3E5-FA163E937ABD.root',
                                      'file:/tmp/carrillo/64C3EE58-88B4-E811-8B8D-FA163EC2B778.root',
                                      'file:/tmp/carrillo/6A2008CF-86B4-E811-8F07-02163E010D5D.root',
                                      'file:/tmp/carrillo/867B3BC2-7AB4-E811-A03B-FA163E7FC1F6.root',
                                      'file:/tmp/carrillo/9AF45D6E-79B4-E811-90F9-FA163E2BF51E.root',
                                      'file:/tmp/carrillo/BAC985EE-81B4-E811-AAA2-FA163E22858D.root',
                                      'file:/tmp/carrillo/BAD2D092-93B4-E811-81AA-FA163E868797.root',
                                      'file:/tmp/carrillo/CC1AE416-98B4-E811-B1E9-FA163E89857D.root'
                                      )
                            )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.out = cms.OutputModule("PoolOutputModule",
   outputCommands = cms.untracked.vstring('drop *','keep L1MuDTChambPhContainer_*_*_*','keep L1MuDTChambThContainer_*_*_*'),
   fileName = cms.untracked.string('DTTriggerPhase2Primitives.root')
)

process.p = cms.Path(process.dtTriggerPhase2PrimitiveDigis)
process.this_is_the_end = cms.EndPath(process.out)

