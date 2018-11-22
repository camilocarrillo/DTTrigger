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
process.dtTriggerPhase2PrimitiveDigis.debug = False

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
        'file:/tmp/carrillo/008F5CE5-F045-E711-99C2-02163E01A1F0.root',
        'file:/tmp/carrillo/F8B12CAE-EC45-E711-A114-02163E014522.root',
        'file:/tmp/carrillo/F8BC99EB-F045-E711-BD92-02163E01A2E3.root',
        'file:/tmp/carrillo/FCBE1392-9845-E711-8F7A-02163E01A72A.root',
        'file:/tmp/carrillo/FCDFCBC3-9245-E711-93D0-02163E01A2CF.root'
        )
                            )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
#    input = cms.untracked.int32(100000)
)

process.out = cms.OutputModule("PoolOutputModule",
   outputCommands = cms.untracked.vstring('drop *','keep L1MuDTChambPhContainer_*_*_*','keep L1MuDTChambThContainer_*_*_*'),
   fileName = cms.untracked.string('DTTriggerPhase2Primitives.root')
)

process.p = cms.Path(process.dtTriggerPhase2PrimitiveDigis)
#process.this_is_the_end = cms.EndPath(process.out)






