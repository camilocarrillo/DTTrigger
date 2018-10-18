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
        'file:/tmp/carrillo/00CD2DAE-A045-E711-9FE6-02163E013611.root',
        'file:/tmp/carrillo/0268ACB3-9845-E711-855E-02163E019CA9.root',
        'file:/tmp/carrillo/02C331DE-9245-E711-8126-02163E01A2BB.root',
        'file:/tmp/carrillo/0805908A-8B45-E711-A5F8-02163E019CC0.root',
        'file:/tmp/carrillo/08ACE77B-8745-E711-BC01-02163E011A70.root',
        'file:/tmp/carrillo/0CB45EF2-8845-E711-B06A-02163E011B22.root',
        'file:/tmp/carrillo/149B2E0B-9845-E711-96F6-02163E01A5C0.root',
        'file:/tmp/carrillo/1613329C-F245-E711-AFC2-02163E013522.root',
        'file:/tmp/carrillo/16BA879D-8B45-E711-8C90-02163E01A4FD.root',
        'file:/tmp/carrillo/1C2A7911-F145-E711-ADD2-02163E01A777.root',
        'file:/tmp/carrillo/1C6C75C8-9245-E711-A7F8-02163E0138F9.root',
        'file:/tmp/carrillo/247F04F7-F045-E711-BECB-02163E01A593.root',
        'file:/tmp/carrillo/264870FB-9245-E711-AE3C-02163E014735.root',
        'file:/tmp/carrillo/264D8070-9045-E711-BB01-02163E01A621.root',
        'file:/tmp/carrillo/2A9B2909-9345-E711-9375-02163E01A777.root',
        'file:/tmp/carrillo/2C201C9D-9945-E711-AEF4-02163E01A6D4.root',
        'file:/tmp/carrillo/30654C9D-F245-E711-A876-02163E014337.root',
        'file:/tmp/carrillo/32C68A5E-9045-E711-B0D9-02163E0143FC.root',
        'file:/tmp/carrillo/36A838EC-F045-E711-84C3-02163E01287D.root',
        'file:/tmp/carrillo/3A42CDB8-9245-E711-96FA-02163E011C45.root'
        )
                            )

process.maxEvents = cms.untracked.PSet(
#    input = cms.untracked.int32(100)
    input = cms.untracked.int32(10000)
)

process.out = cms.OutputModule("PoolOutputModule",
   outputCommands = cms.untracked.vstring('drop *','keep L1MuDTChambPhContainer_*_*_*','keep L1MuDTChambThContainer_*_*_*'),
   fileName = cms.untracked.string('DTTriggerPhase2Primitives.root')
)

process.p = cms.Path(process.dtTriggerPhase2PrimitiveDigis)
#process.this_is_the_end = cms.EndPath(process.out)

