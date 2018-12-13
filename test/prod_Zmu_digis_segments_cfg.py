import FWCore.ParameterSet.Config as cms

process = cms.Process("DigisSegments")

process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cff")
process.load("Geometry.DTGeometry.dtGeometry_cfi")
process.DTGeometryESModule.applyAlignment = False

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.GlobalTag.globaltag = "90X_dataRun2_Express_v2"

process.load("RecoLocalMuon.Configuration.RecoLocalMuon_cff")

## DT unpacker
process.load("EventFilter.DTRawToDigi.dtunpacker_cfi")
process.muonDTDigis.inputLabel = 'rawDataCollector'

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/90000/A8CD55CA-1F94-E611-8017-0CC47A7C35A8.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/90000/5E3D3C6E-6594-E611-AB43-0CC47A4D7616.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/FEB68681-5A87-E611-9374-FA163EBB015F.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/F8A610D8-6287-E611-845E-FA163E57640C.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/F80380BB-7687-E611-84D6-02163E01653D.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/F4A6C6FD-B089-E611-9227-002590DE6E64.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/F40149E2-7387-E611-B057-0025904CF766.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/F21AA882-6C87-E611-8F39-FA163EA18210.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/F0A05CAE-5387-E611-BACB-FA163E9FDE85.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/F095A793-B389-E611-8A84-00259021A39E.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/EE733CB6-B189-E611-A2A6-B499BAAC054A.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/EA70D05E-4589-E611-BFE3-FA163E3F2846.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/E46AF903-6C87-E611-8658-FA163EDB91EF.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/E4332BCB-D687-E611-A9EA-0025905A6126.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/E09E85A0-EB86-E611-B17D-20CF3019DEEF.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/DEEA9DDD-E187-E611-B13B-FA163E73AE79.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/DA12BA92-B087-E611-B7A3-0242AC130002.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/D615D999-B189-E611-B46C-FA163E8E175A.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/D6055073-6E87-E611-8E91-FA163E8D8332.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/D4FD5F54-658B-E611-BED9-0CC47A4D7646.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/D029A3C8-4B89-E611-9D1F-FA163E631428.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/CE3E1EE9-9789-E611-998C-FA163ED21222.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/CCDC87DF-1A88-E611-9B2A-1CC1DE19274E.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/C8F8C3B2-5387-E611-B9FC-FA163E5669B0.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/C4B49819-F286-E611-B127-549F358EB76F.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/C2B91F86-5A87-E611-B7E7-02163E014C10.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/C09B5970-4B88-E611-9C48-901B0E5427A6.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/BE837F4E-9E87-E611-8DC8-3417EBE7047A.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/BAC10123-7787-E611-A0DE-02163E015FDF.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/B856E93F-E586-E611-BA74-FA163E1909D1.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/B64FD56E-6E87-E611-BD9C-02163E016438.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/B4BC5C81-6987-E611-B97A-F04DA275C2FB.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/B43870D6-1A88-E611-A7C0-0026B9F8CC18.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/AE723C49-B287-E611-ACE5-0CC47A78A42E.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/AC213957-658B-E611-A7AF-0025905B8612.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/AA51B3CF-4B89-E611-A41A-02163E013C40.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/A8FF1F89-E586-E611-BC37-FA163E08C002.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/A28F9FFD-B489-E611-B864-008CFAFBF132.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/A21B22EA-8888-E611-B0C4-0CC47A4DEEBA.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/A0C3ADD4-0E87-E611-892F-02163E014D8C.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/9EE91F3C-1B87-E611-878C-FA163E775232.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/9CAF60BB-4489-E611-A29C-FA163EEF018D.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/9AAB70FE-D587-E611-834C-FA163ECD5C62.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/9A522CDD-6287-E611-BA23-FA163E3DAA96.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/9A345235-E586-E611-9CE6-FA163EFA00C3.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/98CD93AB-3A88-E611-A4C8-B083FED04276.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/961767D1-B189-E611-A1A3-20CF305B05AE.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/90AAF1A6-5387-E611-B9B8-0025905C3DF6.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/90522755-9587-E611-A29C-C45444922BB0.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/901352B9-B189-E611-89EC-0CC47A6C183A.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/8E949801-8288-E611-B9D6-047D7BD6DF22.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/88ADAECF-5789-E611-81B2-FA163EDB91EF.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/8823F019-8587-E611-A162-00259073E3EA.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/80CD4397-2A88-E611-9639-20474791CCC4.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/8095FC8B-B389-E611-ADD9-7CD30AB7F868.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/767A38E6-6287-E611-B225-02163E015D84.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/744FC7C0-5387-E611-BA6F-FA163E06DFEA.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/6CFB58E7-1587-E611-BD35-FA163EC97E57.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/68DE47B4-7E88-E611-A6AE-001E67792422.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/68A0DCD5-BB87-E611-8BF3-008CFA0F5040.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/68171F81-6187-E611-A9DF-001E67504F1D.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/66421E3C-5489-E611-B0BE-001E67505A2D.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/64FB46E3-8887-E611-AAAA-FA163EFA220C.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/64CC5933-4088-E611-B8DD-0025904C641E.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/6448920B-7D87-E611-A5EA-02163E017614.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/6297FABA-5789-E611-A918-0CC47AC08BF8.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/5C7F15A9-9A88-E611-A80B-FA163EC5FCBC.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/5A63963D-B887-E611-88DC-001E6739C801.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/5404AF86-6187-E611-8DB3-44A84225CDA4.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/509458EF-B189-E611-9F85-FA163E17EB18.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/4C3B6518-B189-E611-93B3-0025905A612A.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/48E6AB0F-F286-E611-9792-FA163EDB91EF.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/40E2E2DE-8887-E611-9531-FA163E2AAF83.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/38FF87C2-B189-E611-B665-0CC47A1DF7FA.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/38CE04DC-5788-E611-9240-848F69FD2853.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/32775AB1-6C87-E611-A388-02163E0165D4.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/30A4019E-FE86-E611-B70E-02163E0165B6.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/2C6B53B6-5387-E611-9582-FA163E75F411.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/26D33DC4-3889-E611-B1AF-FA163E743F0B.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/26181B1F-6387-E611-AC9E-02163E01304E.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/24A815DC-0E87-E611-8D96-B083FED13C9E.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/22256107-6887-E611-847F-002590DE6E86.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/20263DED-9B88-E611-9630-001E67F336E0.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/1EF43C44-DE87-E611-BB70-6C3BE5B5B340.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/128426B7-8988-E611-BB9C-008CFA0A5A10.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/1041B010-6F87-E611-BA26-02163E015FDB.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/0E90E9AF-5387-E611-9FFA-FA163EC80D44.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/0C894F1B-5289-E611-8381-0025904C5DE0.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/0C6DFDD5-4D87-E611-9CF3-FA163E0B7F2E.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/0C153439-B187-E611-96D9-002590E1E9B8.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/083DFA38-B189-E611-BD7C-A0369F7FC770.root",
"/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-23Sep2016-v1/70000/02A6971D-F286-E611-8364-002590DE6E32.root"
        )
                            )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.out = cms.OutputModule("PoolOutputModule",
                               outputCommands = cms.untracked.vstring('drop *','keep *_muonDTDigis_*_*','keep *_dt4DSegments_*_*'),
                               fileName = cms.untracked.string('digis_segments_Run2016BSingleMuonRAW-RECO.root')
)



process.p = cms.Path(process.muonDTDigis*process.dtlocalreco)
process.this_is_the_end = cms.EndPath(process.out)






