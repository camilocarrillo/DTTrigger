#include "L1Trigger/DTTrigger/interface/DTTrigPhase2Prod.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Run.h"
#include "L1TriggerConfig/DTTPGConfig/interface/DTConfigManager.h"
#include "L1TriggerConfig/DTTPGConfig/interface/DTConfigManagerRcd.h"
#include "L1Trigger/DTSectorCollector/interface/DTSectCollPhSegm.h"
#include "L1Trigger/DTSectorCollector/interface/DTSectCollThSegm.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhContainer.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhDigi.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThContainer.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThDigi.h"
#include "Geometry/DTGeometry/interface/DTLayer.h"
#include <iostream>
#include "TFile.h"
#include "TH1F.h"

using namespace edm;
using namespace std;

typedef vector<DTSectCollPhSegm>  SectCollPhiColl;
typedef SectCollPhiColl::const_iterator SectCollPhiColl_iterator;
typedef vector<DTSectCollThSegm>  SectCollThetaColl;
typedef SectCollThetaColl::const_iterator SectCollThetaColl_iterator;

DTTrigPhase2Prod::DTTrigPhase2Prod(const ParameterSet& pset) : my_trig(nullptr) {
  produces<L1MuDTChambPhContainer>();
  produces<L1MuDTChambThContainer>();

  my_debug = pset.getUntrackedParameter<bool>("debug");
  my_DTTFnum = pset.getParameter<bool>("DTTFSectorNumbering");
  my_params = pset;

  my_lut_dump_flag = pset.getUntrackedParameter<bool>("lutDumpFlag");
  my_lut_btic = pset.getUntrackedParameter<int>("lutBtic");
  if(!(my_trig)) my_trig = new DTTrig(my_params,consumesCollector());
  digiLabel_ = pset.getParameter<edm::InputTag>("digiTag");
}

DTTrigPhase2Prod::~DTTrigPhase2Prod(){
    std::cout<<"writing histograms and files"<<std::endl;
    theFileOut->cd();

    allTDChisto->Write();
    wh0_se6_st1_sl1or3_TDChisto->Write();
    wh0_se6_st1_sl1_TDChisto->Write();
    wh0_se6_st1_sl3_TDChisto->Write();

    allTDCPhase2histo->Write();
    wh0_se6_st1_sl1or3_TDCPhase2histo->Write();
    wh0_se6_st1_sl1_TDCPhase2histo->Write();
    wh0_se6_st1_sl3_TDCPhase2histo->Write();

    wirevslayer->Write();


    theFileOut->Write();
    theFileOut->Close();
    if (my_trig) delete my_trig;
}

void DTTrigPhase2Prod::beginRun(edm::Run const& iRun, const edm::EventSetup& iEventSetup) {

  if(my_debug)
    cout << "DTTrigPhase2Prod::beginRun  " << iRun.id().run() << endl;
  
  ESHandle< DTConfigManager > dtConfig ;
  iEventSetup.get< DTConfigManagerRcd >().get( dtConfig ) ;

  my_trig->createTUs(iEventSetup);
  if (my_debug)
      cout << "[DTTrigPhase2Prod] TU's Created" << endl;
    
  if(my_lut_dump_flag) {
      cout << "Dumping luts...." << endl;
      my_trig->dumpLuts(my_lut_btic, dtConfig.product());
  }	

  

  theFileOut = new TFile("dt_phase2.root", "RECREATE");

  allTDChisto = new TH1F("allTDChisto","allTDChisto",1600,0.5,1600.5);
  wh0_se6_st1_sl1or3_TDChisto = new TH1F("wh0_se6_st1_sl1or3_TDChisto","wh0_se6_st1_sl1or3_TDChisto",1600,0.5,1600.5);
  wh0_se6_st1_sl1_TDChisto = new TH1F("wh0_se6_st1_sl1_TDChisto","wh0_se6_st1_sl1_TDChisto",1600,0.5,1600.5);
  wh0_se6_st1_sl3_TDChisto = new TH1F("wh0_se6_st1_sl3_TDChisto","wh0_se6_st1_sl3_TDChisto",1600,0.5,1600.5);
  
  allTDCPhase2histo = new TH1F("allTDCPhase2histo","allTDCPhase2histo",89075,-0.5,89075.5+1);
  wh0_se6_st1_sl1or3_TDCPhase2histo = new TH1F("wh0_se6_st1_sl1or3_TDCPhase2histo","wh0_se6_st1_sl1or3_TDCPhase2histo",89075,-0.5,89075.5+1);
  wh0_se6_st1_sl1_TDCPhase2histo = new TH1F("wh0_se6_st1_sl1_TDCPhase2histo","wh0_se6_st1_sl1_TDCPhase2histo",89075,-0.5,89075.5+1);
  wh0_se6_st1_sl3_TDCPhase2histo = new TH1F("wh0_se6_st1_sl3_TDCPhase2histo","wh0_se6_st1_sl3_TDCPhase2histo",89075,-0.5,89075.5+1);
  
  wirevslayer = new TH2F("wirevslayer","wirevslayer",50,0.5,50.5,8,0.5,8.5);

}


void DTTrigPhase2Prod::produce(Event & iEvent, const EventSetup& iEventSetup){

  Handle<DTDigiCollection> dtdigis;
  iEvent.getByLabel(digiLabel_, dtdigis);

  DTDigiCollection::DigiRangeIterator dtLayerId_It;
  for (dtLayerId_It=dtdigis->begin(); dtLayerId_It!=dtdigis->end(); ++dtLayerId_It){
      for (DTDigiCollection::const_iterator digiIt = ((*dtLayerId_It).second).first;digiIt!=((*dtLayerId_It).second).second; ++digiIt){
	  const DTLayerId dtLId = (*dtLayerId_It).first;
	  int tdcTime = (*digiIt).countsTDC();
	  int wire = (*digiIt).wire();
	  int tdcPhase2 =  (*digiIt).countsTDC() *.78125 + iEvent.eventAuxiliary().bunchCrossing()*25;
	  
	  allTDChisto->Fill(tdcTime);
	  allTDCPhase2histo->Fill(tdcPhase2);

	  //only same superlayer as CIEMAT study
	  if(dtLId.wheel()==0 && dtLId.sector()==6 && dtLId.station()==1 && (dtLId.superlayer()==1 || dtLId.superlayer()==3)){
	      wh0_se6_st1_sl1or3_TDChisto->Fill(tdcTime); 
	      wh0_se6_st1_sl1or3_TDCPhase2histo->Fill(tdcPhase2);  
	      wirevslayer->Fill(wire,(dtLId.superlayer()-1)*2+dtLId.layer());
	  }
	  if(dtLId.wheel()==0 && dtLId.sector()==6 && dtLId.station()==1 && dtLId.superlayer()==1){
	      wh0_se6_st1_sl1_TDChisto->Fill(tdcTime);wh0_se6_st1_sl1_TDCPhase2histo->Fill(tdcPhase2);
	  }
	  if(dtLId.wheel()==0 && dtLId.sector()==6 && dtLId.station()==1 && dtLId.superlayer()==3){
	      wh0_se6_st1_sl3_TDChisto->Fill(tdcTime);wh0_se6_st1_sl3_TDCPhase2histo->Fill(tdcPhase2);
	  }
	  //std::cout<<"dtLId,wire,tdcTime:"<<dtLId<<" , "<<wire<<" , "<<tdcTime<<std::endl;
      }
  }

  vector<L1MuDTChambPhDigi> outPhi;
  vector<L1MuDTChambThDigi> outTheta;

  /*
  //for the moment we fill up the primitives edm container with empty objects
  my_trig->triggerReco(iEvent,iEventSetup);
  my_BXoffset = my_trig->getBXOffset();
  
  if (my_debug)
      cout << "[DTTrigPhase2Prod] Trigger algorithm run for " <<iEvent.id() << endl;
  
  // Convert Phi Segments
  SectCollPhiColl myPhiSegments;
  myPhiSegments = my_trig->SCPhTrigs();
  
  SectCollPhiColl_iterator SCPCend = myPhiSegments.end();
  for (SectCollPhiColl_iterator it=myPhiSegments.begin();it!=SCPCend;++it){
      int step = (*it).step() - my_BXoffset; // Shift correct BX to 0 (needed for DTTF data processing)
      int sc_sector = (*it).SCId().sector();
      if (my_DTTFnum == true) sc_sector--; // Modified for DTTF numbering [0-11]
      outPhi.push_back(L1MuDTChambPhDigi(step,
					 (*it).ChamberId().wheel(),
					 sc_sector,
					 (*it).ChamberId().station(),
					 (*it).phi(),
					 (*it).phiB(),
					 (*it).code(),
					 !(*it).isFirst(),
					 0
					 ));
  }
  
  // Convert Theta Segments
  SectCollThetaColl myThetaSegments;
  myThetaSegments = my_trig->SCThTrigs();
  
  SectCollThetaColl_iterator SCTCend = myThetaSegments.end();
  for (SectCollThetaColl_iterator it=myThetaSegments.begin();it!=SCTCend;++it){
      int pos[7], qual[7];
      for (int i=0; i<7; i++){
	  pos[i] =(*it).position(i);
	  qual[i]=(*it).quality(i);
      }
      int step =(*it).step() - my_BXoffset; // Shift correct BX to 0 (needed for DTTF data processing)
      int sc_sector =  (*it).SCId().sector();
      if (my_DTTFnum == true) sc_sector--; // Modified for DTTF numbering [0-11]
      outTheta.push_back(L1MuDTChambThDigi( step,
					    (*it).ChamberId().wheel(),
					    sc_sector,
					    (*it).ChamberId().station(),
					    pos,
					    qual
					    ));
  }
  
  */
  //Writing products:
  // Write everything into the event (CB write empty collection as default actions if emulator does not run)
  //std::unique_ptr<L1MuDTChambPhContainer> resultPhi (new L1MuDTChambPhContainer);
  //resultPhi->setContainer(outPhi);
  //iEvent.put(std::move(resultPhi));
  //std::unique_ptr<L1MuDTChambThContainer> resultTheta (new L1MuDTChambThContainer);
  //resultTheta->setContainer(outTheta);
  // iEvent.put(std::move(resultTheta));

}

