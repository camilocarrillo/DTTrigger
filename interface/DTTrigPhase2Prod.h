#ifndef L1Trigger_DTTrigger_DTTrigPhase2Prod_cc
#define L1Trigger_DTTrigger_DTTrigPhase2Prod_cc
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/DTTrigger/interface/DTTrig.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

class TFile;

class DTTrigPhase2Prod: public edm::EDProducer{

public:

  //! Constructor
  DTTrigPhase2Prod(const edm::ParameterSet& pset);

  //! Destructor
  ~DTTrigPhase2Prod() override;

  //! Create Trigger Units before starting event processing
  //void beginJob(const edm::EventSetup & iEventSetup);
  void beginRun(edm::Run const& iRun, const edm::EventSetup& iEventSetup) override;
  
  //! Producer: process every event and generates trigger data
  void produce(edm::Event & iEvent, const edm::EventSetup& iEventSetup) override;

  //plots
  TFile * theFileOut;

  TH1F * allTDChisto;
  TH1F * wh0_se6_st1_sl1or3_TDChisto;
  TH1F * wh0_se6_st1_sl1_TDChisto;
  TH1F * wh0_se6_st1_sl3_TDChisto;
  TH1F * allTDCPhase2histo;
  TH1F * wh0_se6_st1_sl1or3_TDCPhase2histo;
  TH1F * wh0_se6_st1_sl1_TDCPhase2histo;
  TH1F * wh0_se6_st1_sl3_TDCPhase2histo;

  TH1F * allTIMEhisto;
  TH1F * wh0_se6_st1_sl1or3_TIMEhisto;
  TH1F * wh0_se6_st1_sl1_TIMEhisto;
  TH1F * wh0_se6_st1_sl3_TIMEhisto;
  TH1F * allTIMEPhase2histo;
  TH1F * wh0_se6_st1_sl1or3_TIMEPhase2histo;
  TH1F * wh0_se6_st1_sl1_TIMEPhase2histo;
  TH1F * wh0_se6_st1_sl3_TIMEPhase2histo;

  TH2F * wirevslayer;
  TH2F * wirevslayerzTDC;
  

private:

  // Trigger istance
  DTTrig* my_trig;

  // Trigger Configuration Manager CCB validity flag
  bool my_CCBValid;

  // Sector Format Flag true=[0-11] false=[1-12]
  bool my_DTTFnum;

  // BX offset used to correct DTTPG output
  int my_BXoffset;

  // Debug Flag
  bool my_debug;

  // Lut dump file parameters
  bool my_lut_dump_flag;
  short int my_lut_btic;

  // ParameterSet
  edm::ParameterSet my_params;
  edm::InputTag digiLabel_;

};
 
#endif

