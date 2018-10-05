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


#include "muonpath.h"


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
  dt4DSegments = consumes<DTRecSegment4DCollection>(pset.getParameter < edm::InputTag > ("dt4DSegments"));
  
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
    
    allTIMEhisto->Write();
    wh0_se6_st1_sl1or3_TIMEhisto->Write();
    wh0_se6_st1_sl1_TIMEhisto->Write();
    wh0_se6_st1_sl3_TIMEhisto->Write();

    allTIMEPhase2histo->Write();
    wh0_se6_st1_sl1or3_TIMEPhase2histo->Write();
    wh0_se6_st1_sl1_TIMEPhase2histo->Write();
    wh0_se6_st1_sl3_TIMEPhase2histo->Write();

    wh0_se6_st1_segment_x->Write();
    wh0_se6_st1_segment_tanPhi->Write();

    wirevslayer->Write();
    wirevslayerzTDC->Write();


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

  //TDC
  allTDChisto = new TH1F("allTDChisto","allTDChisto",1601,-0.5,1600.5);
  wh0_se6_st1_sl1or3_TDChisto = new TH1F("wh0_se6_st1_sl1or3_TDChisto","wh0_se6_st1_sl1or3_TDChisto",1601,-0.5,1600.5);
  wh0_se6_st1_sl1_TDChisto = new TH1F("wh0_se6_st1_sl1_TDChisto","wh0_se6_st1_sl1_TDChisto",1601,-0.5,1600.5);
  wh0_se6_st1_sl3_TDChisto = new TH1F("wh0_se6_st1_sl3_TDChisto","wh0_se6_st1_sl3_TDChisto",1601,-0.5,1600.5);
  allTDCPhase2histo = new TH1F("allTDCPhase2histo","allTDCPhase2histo",3563*32,-0.5,3563*32+1);
  wh0_se6_st1_sl1or3_TDCPhase2histo = new TH1F("wh0_se6_st1_sl1or3_TDCPhase2histo","wh0_se6_st1_sl1or3_TDCPhase2histo",3563*32+1,-0.5,3563*32.5);
  wh0_se6_st1_sl1_TDCPhase2histo = new TH1F("wh0_se6_st1_sl1_TDCPhase2histo","wh0_se6_st1_sl1_TDCPhase2histo",3563*32+1,-0.5,3563*32.5+1);
  wh0_se6_st1_sl3_TDCPhase2histo = new TH1F("wh0_se6_st1_sl3_TDCPhase2histo","wh0_se6_st1_sl3_TDCPhase2histo",3563*32+1,-0.5,3563*32.5+1);
  
  //TIME
  allTIMEhisto = new TH1F("allTIMEhisto","allTIMEhisto",1275,-0.5,1274.5);
  wh0_se6_st1_sl1or3_TIMEhisto = new TH1F("wh0_se6_st1_sl1or3_TIMEhisto","wh0_se6_st1_sl1or3_TIMEhisto",1275,-0.5,1274.5);
  wh0_se6_st1_sl1_TIMEhisto = new TH1F("wh0_se6_st1_sl1_TIMEhisto","wh0_se6_st1_sl1_TIMEhisto",1275,-0.5,1274.5);
  wh0_se6_st1_sl3_TIMEhisto = new TH1F("wh0_se6_st1_sl3_TIMEhisto","wh0_se6_st1_sl3_TIMEhisto",1275,-0.5,1274.5);
  allTIMEPhase2histo = new TH1F("allTIMEPhase2histo","allTIMEPhase2histo",89076,-0.5,89075.5);
  wh0_se6_st1_sl1or3_TIMEPhase2histo = new TH1F("wh0_se6_st1_sl1or3_TIMEPhase2histo","wh0_se6_st1_sl1or3_TIMEPhase2histo",89076,-0.5,89075.5);
  wh0_se6_st1_sl1_TIMEPhase2histo = new TH1F("wh0_se6_st1_sl1_TIMEPhase2histo","wh0_se6_st1_sl1_TIMEPhase2histo",89076,-0.5,89075.5);
  wh0_se6_st1_sl3_TIMEPhase2histo = new TH1F("wh0_se6_st1_sl3_TIMEPhase2histo","wh0_se6_st1_sl3_TIMEPhase2histo",89076,-0.5,89075.5);

  wh0_se6_st1_segment_x = new TH1F("wh0_se6_st1_segment_x","wh0_se6_st1_segment_x",50,-200,200);
  wh0_se6_st1_segment_tanPhi = new TH1F("wh0_se6_st1_segment_tanPhi","wh0_se6_st1_segment_tanPhi",50,-1.,1.);
  
  wirevslayer     = new TH2F("wirevslayer","wirevslayer",50,0.5,50.5,8,0.5,8.5);
  wirevslayerzTDC = new TH2F("wirevslayerzTDC","wirevslayerzTDC",50*1600,0.5,50+0.5,8,0.5,8.5);

}


void DTTrigPhase2Prod::produce(Event & iEvent, const EventSetup& iEventSetup){
  Handle<DTDigiCollection> dtdigis;
  iEvent.getByLabel(digiLabel_, dtdigis);
  
  edm::Handle<DTRecSegment4DCollection> all4DSegments;
  iEvent.getByToken(dt4DSegments, all4DSegments);
  if(my_debug) std::cout<<"I got the segments"<<std::endl;

  //*******************************4D segments analysis*******************************//
  
  std::map<DTChamberId,int> DTSegmentCounter;
 
  DTChamberId ciemat_chamber_ID(0,1,6);  
  
  float segment_x=-1;
  float segment_tanPhi=-1;
  //float segment_t0=-1;
  DTRecSegment4DCollection::const_iterator segment;
  for (segment = all4DSegments->begin();segment!=all4DSegments->end(); ++segment){
      DTSegmentCounter[segment->chamberId()]++;
      if(segment->chamberId()==ciemat_chamber_ID && segment->dimension()==4 && (segment->phiSegment()->recHits()).size()==8){
	  //segment_to=segment->t0();
	  LocalPoint segmentPosition= segment->localPosition();
	  LocalVector segmentDirection=segment->localDirection();
	  segment_x=segmentPosition.x(); wh0_se6_st1_segment_x->Fill(segment_x);
	  //if(segmentDirection.z()!=0)//a priori it would never happen in this scope
	  segment_tanPhi=segmentDirection.x()/segmentDirection.z(); wh0_se6_st1_segment_tanPhi->Fill(segment_tanPhi);
      }
  }
  if(DTSegmentCounter[ciemat_chamber_ID]==1 && segment_tanPhi!=-1){
      std::cout<<"we found a perfect segment in ciemat's chamber"<<std::endl;
      std::cout<<"segment.x:"<<segment_x<<" segment_tanPhi:"<<segment_tanPhi
	       <<std::endl;
    
      //**********************************************************************************/


      int numPrimsPerLayer[4] = {0, 0, 0, 0};
      int savedTime[4] = {0, 0, 0, 0};
      int savedWire[4] = {0, 0, 0, 0};
      
      /*
	DTPrimitive DTPrimitiveForLayer1;
	DTPrimitive DTPrimitiveForLayer2;
	DTPrimitive DTPrimitiveForLayer3;
	DTPrimitive DTPrimitiveForLayer4;

	//data es un vector de dimension variable en dond cada entrada es un hit de la superlayer dada!
	vector<DTPrimitive*> data[4];
      */
  
      DTDigiCollection::DigiRangeIterator dtLayerId_It;
      for (dtLayerId_It=dtdigis->begin(); dtLayerId_It!=dtdigis->end(); ++dtLayerId_It){
	  for (DTDigiCollection::const_iterator digiIt = ((*dtLayerId_It).second).first;digiIt!=((*dtLayerId_It).second).second; ++digiIt){
	      const DTLayerId dtLId = (*dtLayerId_It).first;
	  
	      int wire = (*digiIt).wire();
	  
	      int digiTDC = (*digiIt).countsTDC();
	      int digiTDCPhase2 =  (*digiIt).countsTDC()+ iEvent.eventAuxiliary().bunchCrossing()*32;
	  
	      int digiTIME = (*digiIt).time();
	      int digiTIMEPhase2 =  (*digiIt).time()+ iEvent.eventAuxiliary().bunchCrossing()*25;
	  
	      int layer = dtLId.layer();
	      int superlayer = dtLId.superlayer();
	  
	      allTDChisto->Fill(digiTDC);
	      allTDCPhase2histo->Fill(digiTDCPhase2);
	  
	      allTIMEhisto->Fill(digiTIME);
	      allTIMEPhase2histo->Fill(digiTIMEPhase2);
	  
	      //only same superlayer as CIEMAT study
	      if(dtLId.wheel()==0 && dtLId.sector()==6 && dtLId.station()==1 && (superlayer==1 || superlayer==3)){
	      
		  wh0_se6_st1_sl1or3_TDChisto->Fill(digiTDC); 
		  wh0_se6_st1_sl1or3_TDCPhase2histo->Fill(digiTDCPhase2);  
	      
		  wh0_se6_st1_sl1or3_TIMEhisto->Fill(digiTIME); 
		  wh0_se6_st1_sl1or3_TIMEPhase2histo->Fill(digiTIMEPhase2);  

		  wirevslayer->Fill(wire,(superlayer-1)*2+layer);
		  wirevslayerzTDC->Fill(wire-0.5+float(digiTDC)/1600.,(superlayer-1)*2+layer);
	  
		  if(superlayer==1){
		      wh0_se6_st1_sl1_TDChisto->Fill(digiTDC);wh0_se6_st1_sl1_TDCPhase2histo->Fill(digiTDCPhase2);
		      wh0_se6_st1_sl1_TIMEhisto->Fill(digiTIME);wh0_se6_st1_sl1_TIMEPhase2histo->Fill(digiTIMEPhase2);
		      numPrimsPerLayer[layer-1]++;
		      savedTime[layer-1]=digiTIMEPhase2;
		      savedWire[layer-1]=wire;
		  }
		  if(superlayer==3){
		      wh0_se6_st1_sl3_TDChisto->Fill(digiTDC);wh0_se6_st1_sl3_TDCPhase2histo->Fill(digiTDCPhase2);
		      wh0_se6_st1_sl3_TIMEhisto->Fill(digiTIME);wh0_se6_st1_sl3_TIMEPhase2histo->Fill(digiTIMEPhase2);
		  }
	      }
	      //std::cout<<"dtLId,wire,digiTDC:"<<dtLId<<" , "<<wire<<" , "<<digiTDC<<std::endl;
	  }
      }

      std::cout<<" for this event we have:"
	       <<" layer 1:"<<numPrimsPerLayer[0]
	       <<" layer 2:"<<numPrimsPerLayer[1]
	       <<" layer 3:"<<numPrimsPerLayer[2]
	       <<" layer 4:"<<numPrimsPerLayer[3]
	       <<std::endl;

      std::cout<<" for this event we have:"
	       <<" layer 1:"<<savedTime[0]
	       <<" layer 2:"<<savedTime[1]
	       <<" layer 3:"<<savedTime[2]
	       <<" layer 4:"<<savedTime[3]
	       <<std::endl;

      std::cout<<" for this event we have:"
	       <<" layer 1:"<<savedWire[0]
	       <<" layer 2:"<<savedWire[1]
	       <<" layer 3:"<<savedWire[2]
	       <<" layer 4:"<<savedWire[3]
	       <<std::endl;

      bool perfect_digi_set=true;
      for(int i=1;i<4;i++)
	  if(numPrimsPerLayer[i]!=1)
	      perfect_digi_set=false;
  
      DTPrimitive *ptrPrimitive[4];
  
      if(perfect_digi_set){
	  std::cout<<"we found a perfect event"<<std::endl;
  
	  for (int i = 0; i <= 3; i++) {
	      ptrPrimitive[i] = new DTPrimitive();
	      ptrPrimitive[i]->setTDCTime(savedTime[i]); 
	      ptrPrimitive[i]->setChannelId(savedWire[i]);	    
	      ptrPrimitive[i]->setLayerId(i);	    
	      std::cout<<"Capa: "<<ptrPrimitive[i]->getLayerId()<<" Canal: "<<ptrPrimitive[i]->getChannelId()<<" TDCTime: "<<ptrPrimitive[i]->getTDCTime()<<std::endl;
	  }

	  MuonPath *ptrMuonPath = new MuonPath(ptrPrimitive);
	  //ptrMuonPath->setCellHorizontalLayout(horizLayout);//Supongo que esto no lo necesitamos!?

	  if (ptrMuonPath->isAnalyzable() && ptrMuonPath->completeMP()){
	      std::cout<<"'MuonPath' analyzable. TDC Time's: "
		       <<ptrMuonPath->getPrimitive(0)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(1)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(2)->getTDCTime()<<" "
		       <<ptrMuonPath->getPrimitive(3)->getTDCTime()<<" "
		       <<"Horizontal Position:"<<ptrMuonPath->getHorizPos()<<" tan(phi):"<<ptrMuonPath->getTanPhi()<<std::endl;
	      
	      delete ptrMuonPath;
	  }
	  
      } //we have a perfeect digi set   
  } //we have a perfect segment in the interesting chamber
  
  /*
  for (int layer = 0; layer <= 3; layer++) {
      int numData = data[layer].size();
      for (int i = 0; i < numData; i++) {
	  delete (DTPrimitive*) ((data[layer])[i]);
      }
      data[layer].clear();
  }
  */
  
  
  vector<L1MuDTChambPhDigi> outPhi;
  vector<L1MuDTChambThDigi> outTheta;

  /*FILLING UP PRIMITIVES:
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

