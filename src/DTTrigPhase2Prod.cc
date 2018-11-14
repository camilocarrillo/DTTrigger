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
#include <Geometry/Records/interface/MuonGeometryRecord.h>
#include <Geometry/DTGeometry/interface/DTGeometry.h>
#include "Geometry/DTGeometry/interface/DTLayer.h"
#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TMath.h"


#include "L1Trigger/DTTrigger/src/muonpath.h"
//#include "L1Trigger/DTTrigger/src/pathanalyzer.h"


using namespace edm;
using namespace std;

typedef vector<DTSectCollPhSegm>  SectCollPhiColl;
typedef SectCollPhiColl::const_iterator SectCollPhiColl_iterator;
typedef vector<DTSectCollThSegm>  SectCollThetaColl;
typedef SectCollThetaColl::const_iterator SectCollThetaColl_iterator;

/*
  Channels are labeled following next schema:
    ---------------------------------
    |   6   |   7   |   8   |   9   |
    ---------------------------------
        |   3   |   4   |   5   |
        -------------------------
            |   1   |   2   |
            -----------------
                |   0   |
                ---------
*/

/* Cell's combination, following previous labeling, to obtain every possible  muon's path. Others cells combinations imply non straight paths */
const int CHANNELS_PATH_ARRANGEMENTS[8][4] = {
    {0, 1, 3, 6}, {0, 1, 3, 7}, {0, 1, 4, 7}, {0, 1, 4, 8},
    {0, 2, 4, 7}, {0, 2, 4, 8}, {0, 2, 5, 8}, {0, 2, 5, 9}
};

/* For each of the previous cell's combinations, this array stores the associated cell's displacement, relative to lower layer cell, measured in semi-cell length units */

const int CELL_HORIZONTAL_LAYOUTS[8][4] = {
    {0, -1, -2, -3}, {0, -1, -2, -1}, {0, -1, 0, -1}, {0, -1, 0, 1},
    {0,  1,  0, -1}, {0,  1,  0,  1}, {0,  1, 2,  1}, {0,  1, 2, 3}
};


DTTrigPhase2Prod::DTTrigPhase2Prod(const ParameterSet& pset) : 
    my_trig(nullptr),
    //check these lines
    inMuonPath(inMuonPath),
    outValidMuonPath(outValidMuonPath)
    
{
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

    //delete inMuonPath;
    //delete outValidMuonPath;

    if(my_debug) std::cout<<"DTp2: writing histograms and files"<<std::endl;
    theFileOut->cd();

    allTDChisto->Write();
    wh0_se6_st1_sl1or3_TDChisto->Write();
    wh0_se6_st1_sl1_TDChisto->Write();

    allTDCPhase2histo->Write();
    wh0_se6_st1_sl1or3_TDCPhase2histo->Write();
    wh0_se6_st1_sl1_TDCPhase2histo->Write();
    
    allTIMEhisto->Write();
    wh0_se6_st1_sl1or3_TIMEhisto->Write();
    wh0_se6_st1_sl1_TIMEhisto->Write();


    allTIMEPhase2histo->Write();
    wh0_se6_st1_sl1or3_TIMEPhase2histo->Write();
    wh0_se6_st1_sl1_TIMEPhase2histo->Write();

    allT0histo->Write();
    allT0Phase2histo->Write();
    
    wh0_se6_st1_segment_x->Write();
    wh0_se6_st1_segment_tanPhi->Write();
    //wh0_se6_st1_T0histo->Write();

    //wh0_se6_st1_segment_BX->Write();

    wh0_se6_st1_segment_vs_jm_x->Write();
    wh0_se6_st1_segment_vs_jm_x_gauss->Write();
    
    wh0_se6_st1_segment_vs_jm_tanPhi->Write();
    wh0_se6_st1_segment_vs_jm_tanPhi_gauss->Write();

    wh0_se6_st1_segment_vs_jm_T0histo->Write();
    wh0_se6_st1_segment_vs_jm_T0histo_gauss->Write();
 
    wirevslayer->Write();
    //wirevslayerzTDC->Write();


    theFileOut->Write();
    theFileOut->Close();
    if (my_trig) delete my_trig;
}

void DTTrigPhase2Prod::beginRun(edm::Run const& iRun, const edm::EventSetup& iEventSetup) {
    if(my_debug)
	cout << "DTTrigPhase2Prod::beginRun  " << iRun.id().run() << endl;

    //iEventSetup.get<MuonGeometryRecord>().get(dtGeo);1103

    
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
    allTDCPhase2histo = new TH1F("allTDCPhase2histo","allTDCPhase2histo",3563*32,-0.5,3563*32+1);
    wh0_se6_st1_sl1or3_TDCPhase2histo = new TH1F("wh0_se6_st1_sl1or3_TDCPhase2histo","wh0_se6_st1_sl1or3_TDCPhase2histo",3563*32+1,-0.5,3563*32.5);
    wh0_se6_st1_sl1_TDCPhase2histo = new TH1F("wh0_se6_st1_sl1_TDCPhase2histo","wh0_se6_st1_sl1_TDCPhase2histo",3563*32+1,-0.5,3563*32.5+1);
    
    //TIME
    allTIMEhisto = new TH1F("allTIMEhisto","allTIMEhisto",1275,-0.5,1274.5);
    wh0_se6_st1_sl1or3_TIMEhisto = new TH1F("wh0_se6_st1_sl1or3_TIMEhisto","wh0_se6_st1_sl1or3_TIMEhisto",1275,-0.5,1274.5);
    wh0_se6_st1_sl1_TIMEhisto = new TH1F("wh0_se6_st1_sl1_TIMEhisto","wh0_se6_st1_sl1_TIMEhisto",1275,-0.5,1274.5);
    allTIMEPhase2histo = new TH1F("allTIMEPhase2histo","allTIMEPhase2histo",8907,-0.5,89075.5);
    wh0_se6_st1_sl1or3_TIMEPhase2histo = new TH1F("wh0_se6_st1_sl1or3_TIMEPhase2histo","wh0_se6_st1_sl1or3_TIMEPhase2histo",8907,-0.5,89075.5);
    wh0_se6_st1_sl1_TIMEPhase2histo = new TH1F("wh0_se6_st1_sl1_TIMEPhase2histo","wh0_se6_st1_sl1_TIMEPhase2histo",8907,-0.5,89075.5);
    
    //T0
    allT0histo = new TH1F("allT0histo","allT0histo",100,-10,10);
    allT0Phase2histo = new TH1F("allT0Phase2histo","allT0Phase2histo",100,-0.5,89075.5);
    //wh0_se6_st1_T0histo = new TH1F("wh0_se6_st1_T0histo","wh0_se6_st1_T0histo",100,-10,10);
    
    //2D
    wh0_se6_st1_segment_x = new TH1F("wh0_se6_st1_segment_x","wh0_se6_st1_segment_x",100,-102,102);
    wh0_se6_st1_segment_tanPhi = new TH1F("wh0_se6_st1_segment_tanPhi","wh0_se6_st1_segment_tanPhi",100,-1.,1.);
    
    wh0_se6_st1_segment_vs_jm_x = new TH2F("wh0_se6_st1_segment_vs_jm_x","wh0_se6_st1_segment_vs_jm_x",100,-102,102,100,-102,102);
    wh0_se6_st1_segment_vs_jm_x_gauss = new TH1F("wh0_se6_st1_segment_vs_jm_x_gauss","wh0_se6_st1_segment_vs_jm_x_gauss",1000,-8,8);

    wh0_se6_st1_segment_vs_jm_tanPhi = new TH2F("wh0_se6_st1_segment_vs_jm_tanPhi","wh0_se6_st1_segment_vs_jm_tanPhi",100,-1.,1.,100,-1.,1.);
    wh0_se6_st1_segment_vs_jm_tanPhi_gauss = new TH1F("wh0_se6_st1_segment_vs_jm_tanPhi_gauss","wh0_se6_st1_segment_vs_jm_tanPhi_gauss",100,-1.,1.);

    wh0_se6_st1_segment_vs_jm_T0histo= new TH2F("wh0_se6_st1_segment_vs_jm_T0histo","wh0_se6_st1_segment_vs_jm_T0histo",201,0,90000,201,0,90000);
    wh0_se6_st1_segment_vs_jm_T0histo_gauss= new TH1F("wh0_se6_st1_segment_vs_jm_T0histo_gauss","wh0_se6_st1_segment_vs_jm_T0histo_gauss",1000,-1000,1000);
    
    wirevslayer     = new TH2F("wirevslayer","wirevslayer",50,0.5,50.5,8,0.5,8.5);
    //wirevslayerzTDC = new TH2F("wirevslayerzTDC","wirevslayerzTDC",50*1600,0.5,50+0.5,8,0.5,8.5);
    
}


void DTTrigPhase2Prod::produce(Event & iEvent, const EventSetup& iEventSetup){
  Handle<DTDigiCollection> dtdigis;
  iEvent.getByLabel(digiLabel_, dtdigis);
  
  edm::Handle<DTRecSegment4DCollection> all4DSegments;
  iEvent.getByToken(dt4DSegments, all4DSegments);
  if(my_debug) std::cout<<"DTp2: I got the segments"<<std::endl;


//digiLoop
  DTDigiCollection::DigiRangeIterator dtLayerId_It;
  for (dtLayerId_It=dtdigis->begin(); dtLayerId_It!=dtdigis->end(); ++dtLayerId_It){
      for (DTDigiCollection::const_iterator digiIt = ((*dtLayerId_It).second).first;digiIt!=((*dtLayerId_It).second).second; ++digiIt){
	  const DTLayerId dtLId = (*dtLayerId_It).first;
		  
	  int wire = (*digiIt).wire();
		  
	  int digiTDC = (*digiIt).countsTDC();
	  int digiTDCPhase2 =  (*digiIt).countsTDC()+ iEvent.eventAuxiliary().bunchCrossing()*32;
		  
	  int digiTIME = (*digiIt).time();
	  int digiTIMEPhase2 =  (*digiIt).time()+ iEvent.eventAuxiliary().bunchCrossing()*25-325;
		  
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
	      //wirevslayerzTDC->Fill(wire-0.5+double(digiTDC)/1600.,(superlayer-1)*2+layer);
	  }
      }
  }
  

  //*******************************4D segments analysis*******************************//
  
  std::map<DTChamberId,int> DTSegmentCounter;
 
  DTChamberId ciemat_chamber_ID(0,1,6);  
  
  //counting all segments and making plots for all segments:--------------------------//
  DTRecSegment4DCollection::const_iterator segment1;
  for (segment1 = all4DSegments->begin();segment1!=all4DSegments->end(); ++segment1){
      DTSegmentCounter[segment1->chamberId()]++;
      //double segment_x=-1;
      //double segment_tanPhi=-1;
      double segment_t0=-1;
      double segment_t0Phase2=-1;
      
      if(segment1->hasPhi()){
	  segment_t0=segment1->phiSegment()->t0();
	  segment_t0Phase2=segment_t0+25*iEvent.eventAuxiliary().bunchCrossing();
	  if(my_debug) std::cout<<"DTp2: segment_t0"<<segment_t0<<" "<<segment_t0Phase2<<std::endl;
	  allT0histo->Fill(segment_t0);
	  allT0Phase2histo->Fill(segment_t0Phase2);

      }
  }

  //focus on ciemat chamber------------------------------------------------------------//

  if(DTSegmentCounter[ciemat_chamber_ID]==1){
      DTRecSegment4DCollection::const_iterator segment;
      for (segment = all4DSegments->begin();segment!=all4DSegments->end(); ++segment){
	  if(segment->hasPhi() && segment->chamberId()==ciemat_chamber_ID  && segment->dimension()==4 && (segment->phiSegment()->recHits()).size()==8 && segment->hasZed()){
	      //T0
	      double segment_t0=segment->phiSegment()->t0();
	      double segment_t0Phase2=segment_t0+25*iEvent.eventAuxiliary().bunchCrossing();
	      //wh0_se6_st1_T0histo->Fill(segment_t0);
	      //wh0_se6_st1_T0Phase2histo->Fill(segment_t0Phase2);

	      //tanPhi
	      LocalVector segmentDirection=segment->localDirection();
	      double dx=segmentDirection.x();
	      double dz=segmentDirection.z(); 
	      double segment_tanPhi=(-1)*(dx/dz);
	      wh0_se6_st1_segment_tanPhi->Fill(segment_tanPhi);
	      
	      //x
	      LocalPoint segmentPosition= segment->localPosition();
	      double segment_x=segmentPosition.x()-11.75*segment_tanPhi; 
	      wh0_se6_st1_segment_x->Fill(segment_x);
	      
	      if(my_debug) std::cout<<"DTp2: we found a perfect segment in ciemat's chamber"<<std::endl;
	      if(my_debug) std::cout<<"DTp2: segment_x:"<<segment_x
		       <<" segment_tanPhi:"<<segment_tanPhi
		       <<" segment_t0:"<<segment_t0
		       <<" segment_t0Phase2:"<<segment_t0Phase2
		       <<std::endl;
		  
	      int numPrimsPerLayer[4] = {0, 0, 0, 0};
	      int savedTime[4] = {0, 0, 0, 0};
	      int savedWire[4] = {0, 0, 0, 0};
      
	      //getting digis from ciemats chamber (later loop over range in the chamber
		  
	      DTDigiCollection::DigiRangeIterator dtLayerId_It;
	      for (dtLayerId_It=dtdigis->begin(); dtLayerId_It!=dtdigis->end(); ++dtLayerId_It){
		  for (DTDigiCollection::const_iterator digiIt = ((*dtLayerId_It).second).first;digiIt!=((*dtLayerId_It).second).second; ++digiIt){		  
		  
		      const DTLayerId dtLId = (*dtLayerId_It).first;

		      int wire = (*digiIt).wire();
		      
		      int digiTDC = (*digiIt).countsTDC();
		      int digiTDCPhase2 =  digiTDC + iEvent.eventAuxiliary().bunchCrossing()*32;
			  
		      int digiTIME = (*digiIt).time();
		      int digiTIMEPhase2 =  digiTIME + iEvent.eventAuxiliary().bunchCrossing()*25-325;
			  
		      int layer = dtLId.layer();
		      int superlayer = dtLId.superlayer();

		      if(dtLId.wheel()==0 && dtLId.sector()==6 && dtLId.station()==1 && (superlayer==1 || superlayer==3)){
			  if(superlayer==1){
			      wh0_se6_st1_sl1_TDChisto->Fill(digiTDC);wh0_se6_st1_sl1_TDCPhase2histo->Fill(digiTDCPhase2);
			      wh0_se6_st1_sl1_TIMEhisto->Fill(digiTIME);wh0_se6_st1_sl1_TIMEPhase2histo->Fill(digiTIMEPhase2);
			      numPrimsPerLayer[layer-1]++;
			      savedTime[layer-1]=digiTIMEPhase2;
			      savedWire[layer-1]=wire-1;
			  }
			  //if(my_debug) std::cout<<"DTp2: dtLId,wire,digiTDC:"<<dtLId<<" , "<<wire<<" , "<<digiTDC<<std::endl;
		      }
		  }
	      }

	      bool perfect_digi_set=true;
	      for(int i=1;i<4;i++)
		  if(numPrimsPerLayer[i]!=1)
		      perfect_digi_set=false;
  
	      DTPrimitive *ptrPrimitive[4];
	      
	      if(perfect_digi_set){
		  if(my_debug) std::cout<<"DTp2:\t\t we found one hit per layer"<<std::endl;
		  if(my_debug) std::cout<<"DTp2:\t\t we have (numPrims/DTDigis(cmssw)):"
			   <<" layer 1:"<<numPrimsPerLayer[0]
			   <<" layer 2:"<<numPrimsPerLayer[1]
			   <<" layer 3:"<<numPrimsPerLayer[2]
			   <<" layer 4:"<<numPrimsPerLayer[3]
			   <<std::endl;
		      
		  if(my_debug) std::cout<<"DTp2:\t\t (savedTime):"
			   <<" layer 1:"<<savedTime[0]
			   <<" layer 2:"<<savedTime[1]
			   <<" layer 3:"<<savedTime[2]
			   <<" layer 4:"<<savedTime[3]
			   <<std::endl;
		      
		  if(my_debug) std::cout<<"DTp2:\t\t (savedWire(cmssw)):"
			   <<" layer 1:"<<savedWire[0]
			   <<" layer 2:"<<savedWire[1]
			   <<" layer 3:"<<savedWire[2]
			   <<" layer 4:"<<savedWire[3]
			   <<std::endl;
		      
		  for (int i = 0; i <= 3; i++) {
		      ptrPrimitive[i] = new DTPrimitive();
		      ptrPrimitive[i]->setTDCTime(savedTime[i]); 
		      ptrPrimitive[i]->setChannelId(savedWire[i]);	    
		      ptrPrimitive[i]->setLayerId(i);	    
		      if(my_debug) std::cout<<"DTp2:\t\t\t Capa: "<<ptrPrimitive[i]->getLayerId()<<" Canal: "<<ptrPrimitive[i]->getChannelId()<<" TDCTime: "<<ptrPrimitive[i]->getTDCTime()<<std::endl;
		  }
		  
		  
		  //Jose Manuel's code starts from saved values from digis
		  MuonPath *ptrMuonPath = new MuonPath(ptrPrimitive);
		  
		  if (ptrMuonPath->isAnalyzable() && ptrMuonPath->completeMP()){
		      if(my_debug) std::cout<<"DTp2:\t\t\t input: MuonPath' analyzable, TDC Phase2 Times are: "
					    <<ptrMuonPath->getPrimitive(0)->getTDCTime()
					    <<" "<<ptrMuonPath->getPrimitive(1)->getTDCTime()
					    <<" "<<ptrMuonPath->getPrimitive(2)->getTDCTime()
					    <<" "<<ptrMuonPath->getPrimitive(3)->getTDCTime()<<std::endl;
			  
		      int pathId = compute_pathId(ptrMuonPath);
		      int horizLayout[4];
		      memcpy(horizLayout, CELL_HORIZONTAL_LAYOUTS[pathId], 4 * sizeof(int));     
		      ptrMuonPath->setCellHorizontalLayout(horizLayout);      
		      analyze(ptrMuonPath);	      
		      
		      double jm_x=(ptrMuonPath->getHorizPos()/10.)-101.3;
		      double jm_tanPhi=ptrMuonPath->getTanPhi();
		      double jm_t0=ptrMuonPath->getBxTimeValue();
		      
		      if(my_debug) std::cout<<"DTp2: jm_output_x="<<jm_x
					    <<"jm_out_tanPhi="<<jm_tanPhi
					    <<"jm_out_BxTimeValue="<<jm_t0
					    <<std::endl;     
		      
		      wh0_se6_st1_segment_vs_jm_x->Fill(segment_x,jm_x);
		      wh0_se6_st1_segment_vs_jm_x_gauss->Fill(segment_x-jm_x);
		      
		      wh0_se6_st1_segment_vs_jm_tanPhi->Fill(segment_tanPhi,jm_tanPhi);
		      wh0_se6_st1_segment_vs_jm_tanPhi_gauss->Fill(segment_tanPhi-jm_tanPhi);
		      
		      wh0_se6_st1_segment_vs_jm_T0histo->Fill(segment_t0Phase2,jm_t0);
		      wh0_se6_st1_segment_vs_jm_T0histo_gauss->Fill(segment_t0Phase2-jm_t0);
		      
		      if(my_debug) std::cout<<"DTp2: segment_t0Phase2="<<segment_t0Phase2
					    <<" jm_t0="<<jm_t0
					    <<std::endl;     
		  }
		  
		  delete ptrMuonPath;
	      }//perfeect digi set   
	      //delete ptrPrimitive;
	  }//perfect segment in ciemats chamber
      }//loop over 4D segments
  }//if one and only one segment in ciemats chamber


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


const int DTTrigPhase2Prod::LAYER_ARRANGEMENTS[MAX_VERT_ARRANG][3] = {
  {0, 1, 2}, {1, 2, 3},                       // Grupos consecutivos
  {0, 1, 3}, {0, 2, 3}                        // Grupos salteados
};

/*CONSTRUCTOR NOT PORTED... ONLY THE FUNCTIONS
PathAnalyzer::PathAnalyzer(MuonPath &inMuonPath, MuonPath &outValidMuonPath) //: inMuonPath(inMuonPath), outValidMuonPath(outValidMuonPath)
{
    if(my_debug) std::cout<<"DTp2: Creando un 'PathAnalyzer'"<<std::endl;
    bxTolerance = 0;
    minQuality  = LOWQGHOST;
    reset();
    
    chiSquareThreshold = 50;
}
*/


//------------------------------------------------------------------
//--- MEtodos pUblicos
//------------------------------------------------------------------

void DTTrigPhase2Prod::analyze(MuonPath *mPath) {
    if(my_debug) std::cout<<"DTp2:\t\t\t\t In analyze! checking if mPath->isAnalyzable() "<<mPath->isAnalyzable()<<std::endl;
    if (mPath->isAnalyzable()) {
	setCellLayout( mPath->getCellHorizontalLayout() );
	evaluatePathQuality(mPath);
	stats.mpAnalized++;
    }else{
	stats.mpNotAnalized++;
    }
    
    // Clonamos el objeto analizado.
    MuonPath *mpAux = new MuonPath(mPath);
    
    if ( mPath->getQuality() >= minQuality ) {
	if(my_debug) std::cout<<"DTp2:\t\t\t\t Calidad: "<<mPath->getQuality()<<std::endl;
	for (int i = 0; i <= 3; i++){
	    if(my_debug) std::cout<<"DTp2:\t\t\t\t  Capa: "<<mPath->getPrimitive(i)->getLayerId()<<" Canal: "<<mPath->getPrimitive(i)->getChannelId()<<" TDCTime: "<<mPath->getPrimitive(i)->getTDCTime()<<std::endl;
	}
 	for (int i = 0; i < totalNumValLateralities; i++) {
	    //* Puede ocurrir que un segmento contenga, p. ej., una lat-comb HIGHQ y varias adicionales LOWQ, en cuyo caso sólo hay que enviar aquellas lat-comb cuya calidad sea análoga a la asignada como global. Es decir: si se asigna global HIGHQ o HIGHQGHOST, se envían aquella de calidad parcial HIGHQ; si LOWQ o LOWQGHOST, se envían  las de calidad parcial LOWQ.
	    if (latQuality[i].valid &&  (((mPath->getQuality() == HIGHQ ||  mPath->getQuality() == HIGHQGHOST) &&  latQuality[i].quality == HIGHQ) 
					 || ((mPath->getQuality() == LOWQ  || mPath->getQuality() == LOWQGHOST)  && latQuality[i].quality == LOWQ))){
		mpAux->setBxTimeValue(latQuality[i].bxValue);
		mpAux->setLateralComb(lateralities[i]);
		/* 
		 * Si hay que invalidar algún 'hit en alguna combinación de
		 * lateralidad concreta, lo eliminamos del 'segmento'.
		 * 
		 * De forma chapucera lo hacemos eliminando la primitiva, y 
		 * reemplazándola por una nueva vacía (se crean inválidas).
		 */
		int idxHitNotValid = latQuality[i].invalidateHitIdx;
		if (idxHitNotValid >= 0) {
		    delete mpAux->getPrimitive(idxHitNotValid);
		    mpAux->setPrimitive(new DTPrimitive(), idxHitNotValid);
		}
		
		if(my_debug) std::cout<<"DTp2:\t\t\t\t  calculating parameters "<<std::endl;
		calculatePathParameters(mpAux);
		/* 
		 * Si, tras calcular los parámetros, y si se trata de un segmento
		 * con 4 hits, el chi2 resultante es superior al umbral programado,
		 * lo eliminamos y no se envía al exterior.
		 * Y pasamos al siguiente elemento.
		 */
		if ((mpAux->getQuality() == HIGHQ || mpAux->getQuality() == HIGHQGHOST) 
		    && mpAux->getChiSq() > chiSquareThreshold) {
		    delete mpAux;
		}
		else {
		    if(my_debug) std::cout<<"DTp2:\t\t\t\t  BX Time = "<<mpAux->getBxTimeValue()<<std::endl;
		    if(my_debug) std::cout<<"DTp2:\t\t\t\t  BX Id   = "<<mpAux->getBxNumId()<<std::endl;
		    if(my_debug) std::cout<<"DTp2:\t\t\t\t  XCoor   = "<<mpAux->getHorizPos()<<std::endl;
		    if(my_debug) std::cout<<"DTp2:\t\t\t\t  tan(Phi)= "<<mpAux->getTanPhi()<<std::endl;
		    
		    mPath->setTanPhi(mpAux->getTanPhi());
		    //mPath->setBxNumId(mpAux->getBxNumId());
		    mPath->setHorizPos(mpAux->getHorizPos());
		    mPath->setBxTimeValue(mpAux->getBxTimeValue());

		    stats.mpWithValidPath++;
		}
	    }
	}
    }
    else {
	stats.mpNoValidPath++;
    }
}

void DTTrigPhase2Prod::reset(void) {
  stats.mpAnalized      = 0;
  stats.mpNull          = 0;
  stats.mpNotAnalized   = 0;
  stats.mpWithValidPath = 0;
  stats.mpNoValidPath   = 0;
  stats.mpHighQ         = 0;
  stats.mpHighQGhost    = 0;
  stats.mpLowQ          = 0;
  stats.mpLowQGhost     = 0;
}

//------------------------------------------------------------------
//--- Métodos get / set
//------------------------------------------------------------------
void DTTrigPhase2Prod::setBXTolerance(int t) { bxTolerance = t; }
int  DTTrigPhase2Prod::getBXTolerance(void)  { return bxTolerance; }

void DTTrigPhase2Prod::setChiSquareThreshold(float ch2Thr) {
  chiSquareThreshold = ch2Thr;
}

void DTTrigPhase2Prod::setMinimumQuality(MP_QUALITY q) {
  if (minQuality >= LOWQGHOST) minQuality = q;
}
MP_QUALITY DTTrigPhase2Prod::getMinimumQuality(void) { return minQuality; }

const DTTrigPhase2Prod::STATISTICS* DTTrigPhase2Prod::getStatistics(void) {
  return &stats;
}

//------------------------------------------------------------------
//--- Métodos privados
//------------------------------------------------------------------
void DTTrigPhase2Prod::setCellLayout(const int layout[4]) {
    memcpy(cellLayout, layout, 4 * sizeof(int));
    //celllayout[0]=layout[0];
    //celllayout[1]=layout[1];
    //celllayout[2]=layout[2];
    //celllayout[3]=layout[3];
    
  buildLateralities();
}

/**
 * Para una combinación de 4 celdas dada (las que se incluyen en el analizador,
 * una por capa), construye de forma automática todas las posibles
 * combinaciones de lateralidad (LLLL, LLRL,...) que son compatibles con una
 * trayectoria recta. Es decir, la partícula no hace un zig-zag entre los hilos
 * de diferentes celdas, al pasar de una a otra.
 */
void DTTrigPhase2Prod::buildLateralities(void) {

  LATERAL_CASES (*validCase)[4], sideComb[4];

  totalNumValLateralities = 0;
  /* Generamos todas las posibles combinaciones de lateralidad para el grupo
     de celdas que forman parte del analizador */
  for(int lowLay = LEFT; lowLay <= RIGHT; lowLay++)
    for(int midLowLay = LEFT; midLowLay <= RIGHT; midLowLay++)
      for(int midHigLay = LEFT; midHigLay <= RIGHT; midHigLay++)
        for(int higLay = LEFT; higLay <= RIGHT; higLay++) {

          sideComb[0] = static_cast<LATERAL_CASES>(lowLay);
          sideComb[1] = static_cast<LATERAL_CASES>(midLowLay);
          sideComb[2] = static_cast<LATERAL_CASES>(midHigLay);
          sideComb[3] = static_cast<LATERAL_CASES>(higLay);

          /* Si una combinación de lateralidades es válida, la almacenamos */
          if (isStraightPath(sideComb)) {
            validCase = lateralities + totalNumValLateralities;
            memcpy(validCase, sideComb, 4 * sizeof(LATERAL_CASES));

            latQuality[totalNumValLateralities].valid            = false;
            latQuality[totalNumValLateralities].bxValue          = 0;
            latQuality[totalNumValLateralities].quality          = NOPATH;
            latQuality[totalNumValLateralities].invalidateHitIdx = -1;

            totalNumValLateralities++;
          }
        }
}

/**
 * Para automatizar la generación de trayectorias compatibles con las posibles
 * combinaciones de lateralidad, este método decide si una cierta combinación
 * de lateralidad, involucrando 4 celdas de las que conforman el DTTrigPhase2Prod,
 * forma una traza recta o no. En caso negativo, la combinación de lateralidad
 * es descartada y no se analiza.
 * En el caso de implementación en FPGA, puesto que el diseño intentará
 * paralelizar al máximo la lógica combinacional, el equivalente a este método
 * debería ser un "generate" que expanda las posibles combinaciones de
 * lateralidad de celdas compatibles con el análisis.
 *
 * El métoda da por válida una trayectoria (es recta) si algo parecido al
 * cambio en la pendiente de la trayectoria, al cambiar de par de celdas
 * consecutivas, no es mayor de 1 en unidades arbitrarias de semi-longitudes
 * de celda para la dimensión horizontal, y alturas de celda para la vertical.
 */
bool DTTrigPhase2Prod::isStraightPath(LATERAL_CASES sideComb[4]) {

  int i, ajustedLayout[4], pairDiff[3], desfase[3];

  /* Sumamos el valor de lateralidad (LEFT = 0, RIGHT = 1) al desfase
     horizontal (respecto de la celda base) para cada celda en cuestion */
  for(i = 0; i <= 3; i++) ajustedLayout[i] = cellLayout[i] + sideComb[i];
  /* Variación del desfase por pares de celdas consecutivas */
  for(i = 0; i <= 2; i++) pairDiff[i] = ajustedLayout[i+1] - ajustedLayout[i];
  /* Variación de los desfases entre todas las combinaciones de pares */
  for(i = 0; i <= 1; i++) desfase[i] = abs(pairDiff[i+1] - pairDiff[i]);
  desfase[2] = abs(pairDiff[2] - pairDiff[0]);
  /* Si algún desfase es mayor de 2 entonces la trayectoria no es recta */
  bool resultado = (desfase[0] > 1 || desfase[1] > 1 || desfase[2] > 1);

  return ( !resultado );
}

/**
 * Recorre las calidades calculadas para todas las combinaciones de lateralidad
 * válidas, para determinar la calidad final asignada al "MuonPath" con el que
 * se está trabajando.
 */
void DTTrigPhase2Prod::evaluatePathQuality(MuonPath *mPath) {

  int totalHighQ = 0, totalLowQ = 0;
  
  if(my_debug) std::cout<<"DTp2:\t\t\t\t\t En evaluatePathQuality Evaluando PathQ. Celda base: "<<mPath->getBaseChannelId()<<std::endl;
  if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Total lateralidades: "<<totalNumValLateralities<<std::endl;

  // Por defecto.
  mPath->setQuality(NOPATH);

  /* Ensayamos los diferentes grupos de lateralidad válidos que constituyen
     las posibles trayectorias del muón por el grupo de 4 celdas.
     Posiblemente esto se tenga que optimizar de manera que, si en cuanto se
     encuentre una traza 'HIGHQ' ya no se continue evaluando mas combinaciones
     de lateralidad, pero hay que tener en cuenta los fantasmas (rectas
     paralelas) en de alta calidad que se pueden dar en los extremos del BTI.
     Posiblemente en la FPGA, si esto se paraleliza, no sea necesaria tal
     optimización */
  for (int latIdx = 0; latIdx < totalNumValLateralities; latIdx++) {
      if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Analizando combinacion de lateralidad: "
	       <<lateralities[latIdx][0]<<" "
	       <<lateralities[latIdx][1]<<" "	 
	       <<lateralities[latIdx][2]<<" "
	       <<lateralities[latIdx][3]<<std::endl;
	  
	  evaluateLateralQuality(latIdx, mPath, &(latQuality[latIdx]));

      if (latQuality[latIdx].quality == HIGHQ) {
	  totalHighQ++;
	  if(my_debug) std::cout<<"DTp2:\t\t\t\t\t\t Lateralidad HIGHQ"<<std::endl;
      }
      if (latQuality[latIdx].quality == LOWQ) {
	  totalLowQ++;
	  if(my_debug) std::cout<<"DTp2:\t\t\t\t\t\t Lateralidad LOWQ"<<std::endl;
      }
  }
  /*
   * Establecimiento de la calidad.
   */
  if (totalHighQ == 1) {
    mPath->setQuality(HIGHQ);
    stats.mpHighQ++;
  }
  else if (totalHighQ > 1) {
    mPath->setQuality(HIGHQGHOST);
    stats.mpHighQGhost++;
  }
  else if (totalLowQ == 1) {
    mPath->setQuality(LOWQ);
    stats.mpLowQ++;
  }
  else if (totalLowQ > 1) {
    mPath->setQuality(LOWQGHOST);
    stats.mpLowQGhost++;
  }
}

void DTTrigPhase2Prod::evaluateLateralQuality(int latIdx, MuonPath *mPath,
                                          LATQ_TYPE *latQuality)
{
  int layerGroup[3];
  LATERAL_CASES sideComb[3];
  PARTIAL_LATQ_TYPE latQResult[4] = {
    {false, 0}, {false, 0}, {false, 0}, {false, 0}
  };

  // Default values.
  latQuality->valid            = false;
  latQuality->bxValue          = 0;
  latQuality->quality          = NOPATH;
  latQuality->invalidateHitIdx = -1;

  /* En el caso que, para una combinación de lateralidad dada, las 2
     combinaciones consecutivas de 3 capas ({0, 1, 2}, {1, 2, 3}) fueran
     traza válida, habríamos encontrado una traza correcta de alta calidad,
     por lo que sería innecesario comprobar las otras 2 combinaciones
     restantes.
     Ahora bien, para reproducir el comportamiento paralelo de la FPGA en el
     que el análisis se va a evaluar simultáneamente en todas ellas,
     construimos un código que analiza las 4 combinaciones, junto con una
     lógica adicional para discriminar la calidad final de la traza */
  for (int i = 0; i <= 3 ; i++) {
      memcpy(layerGroup, LAYER_ARRANGEMENTS[i], 3 * sizeof(int));

    // Seleccionamos la combinación de lateralidad para cada celda.
    for (int j = 0; j < 3; j++)
      sideComb[j] = lateralities[latIdx][ layerGroup[j] ];

    validate(sideComb, layerGroup, mPath, &(latQResult[i]));
  }
  /*
    Imponemos la condición, para una combinación de lateralidad completa, que
    todas las lateralidades parciales válidas arrojen el mismo valor de BX
    (dentro de un margen) para así dar una traza consistente.
    En caso contrario esa combinación se descarta.
  */
  if ( !sameBXValue(latQResult) ) {
    // Se guardan en los default values inciales.
    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Lateralidad DESCARTADA. Tolerancia de BX excedida"<<std::endl;
    return;
  }

  // Dos trazas complementarias válidas => Traza de muón completa.
  if ((latQResult[0].latQValid && latQResult[1].latQValid) ||
      (latQResult[0].latQValid && latQResult[2].latQValid) ||
      (latQResult[0].latQValid && latQResult[3].latQValid) ||
      (latQResult[1].latQValid && latQResult[2].latQValid) ||
      (latQResult[1].latQValid && latQResult[3].latQValid) ||
      (latQResult[2].latQValid && latQResult[3].latQValid))
  {
    latQuality->valid   = true;
//     latQuality->bxValue = latQResult[0].bxValue;
    /*
     * Se hace necesario el contador de casos "numValid", en vez de promediar
     * los 4 valores dividiendo entre 4, puesto que los casos de combinaciones
     * de 4 hits buenos que se ajusten a una combinación como por ejemplo:
     * L/R/L/L, dan lugar a que en los subsegmentos 0, y 1 (consecutivos) se
     * pueda aplicar mean-timer, mientras que en el segmento 3 (en el ejemplo
     * capas: 0,2,3, y combinación L/L/L) no se podría aplicar, dando un
     * valor parcial de BX = 0.
     */
    int sumBX = 0, numValid = 0;
    for (int i = 0; i <= 3; i++) {
      if (latQResult[i].latQValid) {
        sumBX += latQResult[i].bxValue;
        numValid++;
      }
    }

    latQuality->bxValue = sumBX / numValid;
    latQuality->quality = HIGHQ;

    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Lateralidad ACEPTADA. HIGHQ."<<std::endl;
  }
  // Sólo una traza disjunta válida => Traza de muón incompleta pero válida.
  else { 
    if (latQResult[0].latQValid || latQResult[1].latQValid ||
        latQResult[2].latQValid || latQResult[3].latQValid)
    {
      latQuality->valid   = true;
      latQuality->quality = LOWQ;
      for (int i = 0; i < 4; i++)
        if (latQResult[i].latQValid) {
          latQuality->bxValue = latQResult[i].bxValue;
          /*
           * En los casos que haya una combinación de 4 hits válidos pero
           * sólo 3 de ellos formen traza (calidad 2), esto permite detectar
           * la layer con el hit que no encaja en la recta, y así poder
           * invalidarlo, cambiando su valor por "-1" como si de una mezcla
           * de 3 hits pura se tratara.
           * Esto es útil para los filtros posteriores.
           */
          latQuality->invalidateHitIdx = getOmittedHit( i );
          break;
        }

      if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Lateralidad ACEPTADA. LOWQ."<<std::endl;
    }
    else {
      if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Lateralidad DESCARTADA. NOPATH."<<std::endl;
    }
  }
}

/**
 * Valida, para una combinación de capas (3), celdas y lateralidad, si los
 * valores temporales cumplen el criterio de mean-timer.
 * En vez de comparar con un 0 estricto, que es el resultado aritmético de las
 * ecuaciones usadas de base, se incluye en la clase un valor de tolerancia
 * que por defecto vale cero, pero que se puede ajustar a un valor más
 * adecuado
 *
 * En esta primera versión de la clase, el código de generación de ecuaciones
 * se incluye en esta función, lo que es ineficiente porque obliga a calcular
 * un montón de constantes, fijas para cada combinación de celdas, que
 * tendrían que evaluarse una sóla vez en el constructor de la clase.
 * Esta disposición en el constructor estaría más proxima a la realización que
 * se tiene que llevar a término en la FPGA (en tiempo de síntesis).
 * De momento se deja aquí porque así se entiende la lógica mejor, al estar
 * descrita de manera lineal en un sólo método.
 */
void DTTrigPhase2Prod::validate(LATERAL_CASES sideComb[3], int layerIndex[3],MuonPath* mPath, PARTIAL_LATQ_TYPE *latq)
{
    // Valor por defecto.
    latq->bxValue   = 0;
    latq->latQValid = false;
  
    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t In validate Iniciando validacion de MuonPath para capas: "
	     <<layerIndex[0]<<"/"
	     <<layerIndex[1]<<"/"
	     <<layerIndex[2]<<std::endl;
  
    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Lateralidades parciales: "
	     <<sideComb[0]<<"/"
	     <<sideComb[1]<<"/"
	     <<sideComb[2]<<std::endl;
  
    /* Primero evaluamos si, para la combinación concreta de celdas en curso, el
       número de celdas con dato válido es 3. Si no es así, sobre esa
       combinación no se puede aplicar el mean-timer y devolvemos "false" */
    int validCells = 0;
    for (int j = 0; j < 3; j++)
	if (mPath->getPrimitive(layerIndex[j])->isValidTime()) validCells++;
  
    if (validCells != 3) {
	if(my_debug) std::cout<<"DTp2:\t\t\t\t\t No hay 3 celdas validas."<<std::endl;
	return;
    }

    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Valores de TDC: "
	     <<mPath->getPrimitive(layerIndex[0])->getTDCTime()<<"/"
	     <<mPath->getPrimitive(layerIndex[1])->getTDCTime()<<"/"
	     <<mPath->getPrimitive(layerIndex[2])->getTDCTime()<<"."
	     <<std::endl;

    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Valid TIMES: "
	     <<mPath->getPrimitive(layerIndex[0])->isValidTime()<<"/"
	     <<mPath->getPrimitive(layerIndex[1])->isValidTime()<<"/"
	     <<mPath->getPrimitive(layerIndex[2])->isValidTime()<<"."
	     <<std::endl;

  
    /* Distancias verticales entre capas inferior/media y media/superior */
    int dVertMI = layerIndex[1] - layerIndex[0];
    int dVertSM = layerIndex[2] - layerIndex[1];

    /* Distancias horizontales entre capas inferior/media y media/superior */
    int dHorzMI = cellLayout[layerIndex[1]] - cellLayout[layerIndex[0]];
    int dHorzSM = cellLayout[layerIndex[2]] - cellLayout[layerIndex[1]];

    /* Índices de pares de capas sobre las que se está actuando
       SM => Superior + Intermedia
       MI => Intermedia + Inferior
       Jugamos con los punteros para simplificar el código */
    int *layPairSM = &layerIndex[1];
    int *layPairMI = &layerIndex[0];

    /* Pares de combinaciones de celdas para composición de ecuación. Sigue la
       misma nomenclatura que el caso anterior */
    LATERAL_CASES smSides[2], miSides[2];

    /* Teniendo en cuenta que en el índice 0 de "sideComb" se almacena la
       lateralidad de la celda inferior, jugando con aritmética de punteros
       extraemos las combinaciones de lateralidad para los pares SM y MI */

    memcpy(smSides, &sideComb[1], 2 * sizeof(LATERAL_CASES));
  
    memcpy(miSides, &sideComb[0], 2 * sizeof(LATERAL_CASES));
  
    float bxValue = 0;
    int coefsAB[2] = {0, 0}, coefsCD[2] = {0, 0};
    /* It's neccesary to be careful with that pointer's indirection. We need to
       retrieve the lateral coeficientes (+-1) from the lower/middle and
       middle/upper cell's lateral combinations. They are needed to evaluate the
       existance of a possible BX value, following it's calculation equation */
    getLateralCoeficients(miSides, coefsAB);
    getLateralCoeficients(smSides, coefsCD);

    /* Cada para de sumas de los 'coefsCD' y 'coefsAB' dan siempre como resultado
       0, +-2.

       A su vez, y pese a que las ecuaciones se han construido de forma genérica
       para cualquier combinación de celdas de la cámara, los valores de 'dVertMI' y
       'dVertSM' toman valores 1 o 2 puesto que los pares de celdas con los que se
       opera en realidad, o bien están contiguos, o bien sólo están separadas por
       una fila de celdas intermedia. Esto es debido a cómo se han combinado los
       grupos de celdas, para aplicar el mean-timer, en 'LAYER_ARRANGEMENTS'.

       El resultado final es que 'denominator' es siempre un valor o nulo, o
       múltiplo de 2 */
    int denominator = dVertMI * (coefsCD[1] + coefsCD[0]) -
	dVertSM * (coefsAB[1] + coefsAB[0]);

    if(denominator == 0) {
	if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Imposible calcular BX. Denominador para BX = 0."<<std::endl;
	return;
    }

    /* Esta ecuación ha de ser optimizada, especialmente en su implementación
       en FPGA. El 'denominator' toma siempre valores múltiplo de 2 o nulo, por lo
       habría que evitar el cociente y reemplazarlo por desplazamientos de bits */
    bxValue = (
	       dVertMI*(dHorzSM*MAXDRIFT + eqMainBXTerm(smSides, layPairSM, mPath)) -
	       dVertSM*(dHorzMI*MAXDRIFT + eqMainBXTerm(miSides, layPairMI, mPath))
	       ) / denominator;

    if(bxValue < 0) {
	if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Combinacion no valida. BX Negativo."<<std::endl;
	return;
    }

    // Redondeo del valor del tiempo de BX al nanosegundo
    if ( (bxValue - int(bxValue)) >= 0.5 ) bxValue = float(int(bxValue + 1));
    else bxValue = float(int(bxValue));

    /* Ciertos valores del tiempo de BX, siendo positivos pero objetivamente no
       válidos, pueden dar lugar a que el discriminador de traza asociado de un
       valor aparentemente válido (menor que la tolerancia y típicamente 0). Eso es
       debido a que el valor de tiempo de BX es mayor que algunos de los tiempos
       de TDC almacenados en alguna de las respectivas 'DTPrimitives', lo que da
       lugar a que, cuando se establece el valore de BX para el 'MuonPath', se
       obtengan valores de tiempo de deriva (*NO* tiempo de TDC) en la 'DTPrimitive'
       nulos, o inconsistentes, a causa de la resta entre enteros.

       Así pues, se impone como criterio de validez adicional que el valor de tiempo
       de BX (bxValue) sea siempre superior a cualesquiera valores de tiempo de TDC
       almacenados en las 'DTPrimitives' que forman el 'MuonPath' que se está
       analizando.
       En caso contrario, se descarta como inválido */

    for (int i = 0; i < 3; i++)
	if (mPath->getPrimitive(layerIndex[i])->isValidTime()) {
	    int diffTime =
		mPath->getPrimitive(layerIndex[i])->getTDCTimeNoOffset() - bxValue;

	    if (diffTime < 0 || diffTime > MAXDRIFT) {
		if(my_debug) std::cout<<"DTp2:\t\t\t\t\t Valor de BX inválido. Al menos un tiempo de TDC sin sentido"<<std::endl;
		return;
	    }
	}

    /* Si se llega a este punto, el valor de BX y la lateralidad parcial se dan
     * por válidas.
     */
    latq->bxValue   = bxValue;
    latq->latQValid = true;
}//finish validate

/**
 * Evalúa la suma característica de cada par de celdas, según la lateralidad
 * de la trayectoria.
 * El orden de los índices de capa es crítico:
 *    layerIdx[0] -> Capa más baja,
 *    layerIdx[1] -> Capa más alta
 */
int DTTrigPhase2Prod::eqMainBXTerm(LATERAL_CASES sideComb[2], int layerIdx[2],MuonPath* mPath)
{
    int eqTerm = 0, coefs[2];
    
    getLateralCoeficients(sideComb, coefs);
    
    eqTerm = coefs[0] * mPath->getPrimitive(layerIdx[0])->getTDCTimeNoOffset() +
	coefs[1] * mPath->getPrimitive(layerIdx[1])->getTDCTimeNoOffset();
    
    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t In eqMainBXTerm EQTerm(BX): "<<eqTerm<<std::endl;
    
    return (eqTerm);
}

/**
 * Evalúa la suma característica de cada par de celdas, según la lateralidad
 * de la trayectoria. Semejante a la anterior, pero aplica las correcciones
 * debidas a los retardos de la electrónica, junto con la del Bunch Crossing
 *
 * El orden de los índices de capa es crítico:
 *    layerIdx[0] -> Capa más baja,
 *    layerIdx[1] -> Capa más alta
 */
int DTTrigPhase2Prod::eqMainTerm(LATERAL_CASES sideComb[2], int layerIdx[2],MuonPath* mPath, int bxValue)
{
    int eqTerm = 0, coefs[2];

    getLateralCoeficients(sideComb, coefs);
    
    eqTerm = coefs[0] * (mPath->getPrimitive(layerIdx[0])->getTDCTimeNoOffset() -
			 bxValue) +
	coefs[1] * (mPath->getPrimitive(layerIdx[1])->getTDCTimeNoOffset() -
		    bxValue);
    
    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t EQTerm(Main): "<<eqTerm<<std::endl;
    
    return (eqTerm);
}

/**
 * Devuelve los coeficientes (+1 ó -1) de lateralidad para un par dado.
 * De momento es útil para poder codificar la nueva funcionalidad en la que se
 * calcula el BX.
 */

void DTTrigPhase2Prod::getLateralCoeficients(LATERAL_CASES sideComb[2],int *coefs)
{
    if ((sideComb[0] == LEFT) && (sideComb[1] == LEFT)) {
	*(coefs)     = +1;
	*(coefs + 1) = -1;
    }
    else if ((sideComb[0] == LEFT) && (sideComb[1] == RIGHT)){
	*(coefs)     = +1;
	*(coefs + 1) = +1;
    }
    else if ((sideComb[0] == RIGHT) && (sideComb[1] == LEFT)){
	*(coefs)     = -1;
	*(coefs + 1) = -1;
    }
    else if ((sideComb[0] == RIGHT) && (sideComb[1] == RIGHT)){
	*(coefs)     = -1;
	*(coefs + 1) = +1;
    }
}

/**
 * Determines if all valid partial lateral combinations share the same value
 * of 'bxValue'.
 */
bool DTTrigPhase2Prod::sameBXValue(PARTIAL_LATQ_TYPE* latq) {

  bool result = true;
  /*
  Para evitar los errores de precision en el cálculo, en vez de forzar un
  "igual" estricto a la hora de comparar los diferentes valores de BX, se
  obliga a que la diferencia entre pares sea menor que un cierto valor umbral.
  Para hacerlo cómodo se crean 6 booleanos que evalúan cada posible diferencia
  */
  bool d01, d02, d03, d12, d13, d23;
  d01 = (abs(latq[0].bxValue - latq[1].bxValue) <= bxTolerance) ? true : false;
  d02 = (abs(latq[0].bxValue - latq[2].bxValue) <= bxTolerance) ? true : false;
  d03 = (abs(latq[0].bxValue - latq[3].bxValue) <= bxTolerance) ? true : false;
  d12 = (abs(latq[1].bxValue - latq[2].bxValue) <= bxTolerance) ? true : false;
  d13 = (abs(latq[1].bxValue - latq[3].bxValue) <= bxTolerance) ? true : false;
  d23 = (abs(latq[2].bxValue - latq[3].bxValue) <= bxTolerance) ? true : false;

  /* Casos con 4 grupos de combinaciones parciales de lateralidad válidas */
  if ((latq[0].latQValid && latq[1].latQValid && latq[2].latQValid &&
       latq[3].latQValid) && !(d01 && d12 && d23))
    result = false;
  else
    /* Los 4 casos posibles de 3 grupos de lateralidades parciales válidas */
    if ( ((latq[0].latQValid && latq[1].latQValid && latq[2].latQValid) &&
          !(d01 && d12)
          )
        ||
         ((latq[0].latQValid && latq[1].latQValid && latq[3].latQValid) &&
          !(d01 && d13)
          )
        ||
         ((latq[0].latQValid && latq[2].latQValid && latq[3].latQValid) &&
          !(d02 && d23)
          )
        ||
         ((latq[1].latQValid && latq[2].latQValid && latq[3].latQValid) &&
          !(d12 && d23)
          )
        )
      result = false;
    else
      /* Por último, los 6 casos posibles de pares de lateralidades parciales
      válidas */
      if ( ((latq[0].latQValid && latq[1].latQValid) && !d01) ||
           ((latq[0].latQValid && latq[2].latQValid) && !d02) ||
           ((latq[0].latQValid && latq[3].latQValid) && !d03) ||
           ((latq[1].latQValid && latq[2].latQValid) && !d12) ||
           ((latq[1].latQValid && latq[3].latQValid) && !d13) ||
           ((latq[2].latQValid && latq[3].latQValid) && !d23) )
	  result = false;
  
  return result;
}

/** Calcula los parámetros de la(s) trayectoria(s) detectadas.
 *
 * Asume que el origen de coordenadas está en al lado 'izquierdo' de la cámara
 * con el eje 'X' en la posición media vertical de todas las celdas.
 * El eje 'Y' se apoya sobre los hilos de las capas 1 y 3 y sobre los costados
 * de las capas 0 y 2.
 */
void DTTrigPhase2Prod::calculatePathParameters(MuonPath* mPath) {
    // El orden es importante. No cambiar sin revisar el codigo.
    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t\t  calculating calcCellDriftAndXcoor(mPath) "<<std::endl;
    calcCellDriftAndXcoor(mPath);
    //calcTanPhiXPosChamber(mPath);
    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t\t  checking mPath->getQuality() "<<mPath->getQuality()<<std::endl;
    if (mPath->getQuality() == HIGHQ || mPath->getQuality() == HIGHQGHOST){
	if(my_debug) std::cout<<"DTp2:\t\t\t\t\t\t\t  Quality test passed, now calcTanPhiXPosChamber4Hits(mPath) "<<std::endl;
	calcTanPhiXPosChamber4Hits(mPath);
    }else{
	if(my_debug) std::cout<<"DTp2:\t\t\t\t\t\t\t  Quality test NOT passed calcTanPhiXPosChamber3Hits(mPath) "<<std::endl;
	calcTanPhiXPosChamber3Hits(mPath);
    }
    if(my_debug) std::cout<<"DTp2:\t\t\t\t\t\t calcChiSquare(mPath) "<<std::endl;
    calcChiSquare(mPath);
}

void DTTrigPhase2Prod::calcTanPhiXPosChamber(MuonPath* mPath)
{
  /*
  La mayoría del código de este método tiene que ser optimizado puesto que
  se hacen llamadas y cálculos redundantes que ya se han evaluado en otros
  métodos previos.

  Hay que hacer una revisión de las ecuaciones para almacenar en el 'MuonPath'
  una serie de parámetro característicos (basados en sumas y productos, para
  que su implementación en FPGA sea sencilla) con los que, al final del
  proceso, se puedan calcular el ángulo y la coordenada horizontal.

  De momento se deja este código funcional extraído directamente de las
  ecuaciones de la recta.
  */
  int layerIdx[2];
  /*
  To calculate path's angle are only necessary two valid primitives.
  This method should be called only when a 'MuonPath' is determined as valid,
  so, at least, three of its primitives must have a valid time.
  With this two comparitions (which can be implemented easily as multiplexors
  in the FPGA) this method ensures to catch two of those valid primitives to
  evaluate the angle.

  The first one is below the middle line of the superlayer, while the other
  one is above this line
  */
  if (mPath->getPrimitive(0)->isValidTime()) layerIdx[0] = 0;
  else layerIdx[0] = 1;

  if (mPath->getPrimitive(3)->isValidTime()) layerIdx[1] = 3;
  else layerIdx[1] = 2;

  /* We identify along which cells' sides the muon travels */
  LATERAL_CASES sideComb[2];
  sideComb[0] = (mPath->getLateralComb())[ layerIdx[0] ];
  sideComb[1] = (mPath->getLateralComb())[ layerIdx[1] ];

  /* Horizontal gap between cells in cell's semi-length units */
  int dHoriz = (mPath->getCellHorizontalLayout())[ layerIdx[1] ] -
               (mPath->getCellHorizontalLayout())[ layerIdx[0] ];

  /* Vertical gap between cells in cell's height units */
  int dVert = layerIdx[1] -layerIdx[0];

  /*-----------------------------------------------------------------*/
  /*--------------------- Phi angle calculation ---------------------*/
  /*-----------------------------------------------------------------*/
  float num = CELL_SEMILENGTH * dHoriz +
              DRIFT_SPEED *
                 eqMainTerm(sideComb, layerIdx, mPath,
                            mPath->getBxTimeValue()
                           );

  float denom = CELL_HEIGHT * dVert;
  float tanPhi = num / denom;

  mPath->setTanPhi(tanPhi);

  /*-----------------------------------------------------------------*/
  /*----------------- Horizontal coord. calculation -----------------*/
  /*-----------------------------------------------------------------*/

  /*
  Using known coordinates, relative to superlayer axis reference, (left most
  superlayer side, and middle line between 2nd and 3rd layers), calculating
  horizontal coordinate implies using a basic line equation:
                      (y - y0) = (x - x0) * cotg(Phi)
  This horizontal coordinate can be obtained setting y = 0 on last equation,
  and also setting y0 and x0 with the values of a known muon's path cell
  position hit.
  It's enough to use the lower cell (layerIdx[0]) coordinates. So:
                      xC = x0 - y0 * tan(Phi)
  */
  float lowerXPHorizPos = mPath->getXCoorCell( layerIdx[0] );

  float lowerXPVertPos = 0; // This is only the absolute value distance.
  if (layerIdx[0] == 0) lowerXPVertPos = CELL_HEIGHT + CELL_SEMIHEIGHT;
  else                  lowerXPVertPos = CELL_SEMIHEIGHT;

  mPath->setHorizPos( lowerXPHorizPos + lowerXPVertPos * tanPhi );
}

/**
 * Cálculos de coordenada y ángulo para un caso de 4 HITS de alta calidad.
 */
void DTTrigPhase2Prod::calcTanPhiXPosChamber4Hits(MuonPath* mPath) {
  float tanPhi = (3 * mPath->getXCoorCell(3) +
                      mPath->getXCoorCell(2) -
                      mPath->getXCoorCell(1) -
                  3 * mPath->getXCoorCell(0)) / (10 * CELL_HEIGHT);

  mPath->setTanPhi(tanPhi);

  float XPos = (mPath->getXCoorCell(0) +
                mPath->getXCoorCell(1) +
                mPath->getXCoorCell(2) +
                mPath->getXCoorCell(3)) / 4;

  mPath->setHorizPos( XPos );
}

/**
 * Cálculos de coordenada y ángulo para un caso de 3 HITS.
 */
void DTTrigPhase2Prod::calcTanPhiXPosChamber3Hits(MuonPath* mPath) {
  int layerIdx[2];

  if (mPath->getPrimitive(0)->isValidTime()) layerIdx[0] = 0;
  else layerIdx[0] = 1;

  if (mPath->getPrimitive(3)->isValidTime()) layerIdx[1] = 3;
  else layerIdx[1] = 2;

  /* We identify along which cells' sides the muon travels */
  LATERAL_CASES sideComb[2];
  sideComb[0] = (mPath->getLateralComb())[ layerIdx[0] ];
  sideComb[1] = (mPath->getLateralComb())[ layerIdx[1] ];

  /* Horizontal gap between cells in cell's semi-length units */
  int dHoriz = (mPath->getCellHorizontalLayout())[ layerIdx[1] ] -
               (mPath->getCellHorizontalLayout())[ layerIdx[0] ];

  /* Vertical gap between cells in cell's height units */
  int dVert = layerIdx[1] -layerIdx[0];

  /*-----------------------------------------------------------------*/
  /*--------------------- Phi angle calculation ---------------------*/
  /*-----------------------------------------------------------------*/
  float num = CELL_SEMILENGTH * dHoriz +
              DRIFT_SPEED *
                 eqMainTerm(sideComb, layerIdx, mPath,
                            mPath->getBxTimeValue()
                           );

  float denom = CELL_HEIGHT * dVert;
  float tanPhi = num / denom;

  mPath->setTanPhi(tanPhi);

  /*-----------------------------------------------------------------*/
  /*----------------- Horizontal coord. calculation -----------------*/
  /*-----------------------------------------------------------------*/
  float XPos = 0;
  if (mPath->getPrimitive(0)->isValidTime() &&
      mPath->getPrimitive(3)->isValidTime())
    XPos = (mPath->getXCoorCell(0) + mPath->getXCoorCell(3)) / 2;
  else
    XPos = (mPath->getXCoorCell(1) + mPath->getXCoorCell(2)) / 2;

  mPath->setHorizPos( XPos );
}

/**
 * Calcula las distancias de deriva respecto de cada "wire" y la posición
 * horizontal del punto de interacción en cada celda respecto del sistema
 * de referencia de la cámara.
 *
 * La posición horizontal de cada hilo es calculada en el "DTPrimitive".
 */
void DTTrigPhase2Prod::calcCellDriftAndXcoor(MuonPath *mPath) {
  //Distancia de deriva en la celda respecto del wire". NO INCLUYE SIGNO.
  float driftDistance;
  float wireHorizPos; // Posicion horizontal del wire.
  float hitHorizPos;  // Posicion del muon en la celda.

  for (int i = 0; i <= 3; i++)
    if (mPath->getPrimitive(i)->isValidTime()) {
      // Drift distance.
      driftDistance = DRIFT_SPEED *
              ( mPath->getPrimitive(i)->getTDCTimeNoOffset() -
                mPath->getBxTimeValue()
              );

      wireHorizPos = mPath->getPrimitive(i)->getWireHorizPos();

      if ( (mPath->getLateralComb())[ i ] == LEFT )
        hitHorizPos = wireHorizPos - driftDistance;
      else
        hitHorizPos = wireHorizPos + driftDistance;

      mPath->setXCoorCell(hitHorizPos, i);
      mPath->setDriftDistance(driftDistance, i);
    }
}

/**
 * Calcula el estimador de calidad de la trayectoria.
 */
void DTTrigPhase2Prod::calcChiSquare(MuonPath *mPath) {

  float xi, zi, factor;

  float chi = 0;
  float mu  = mPath->getTanPhi();
  float b   = mPath->getHorizPos();

  const float baseWireYPos = -1.5 * CELL_HEIGHT;

  for (int i = 0; i <= 3; i++)
    if ( mPath->getPrimitive(i)->isValidTime() ) {
      zi = baseWireYPos + CELL_HEIGHT * i;
      xi = mPath->getXCoorCell(i);

      factor = xi - mu*zi - b;
      chi += (factor * factor);
    }

  mPath->setChiSq(chi);
}


/**
 * Este método devuelve cual layer no se está utilizando en el
 * 'LAYER_ARRANGEMENT' cuyo índice se pasa como parámetro.
 * 
 * ¡¡¡ OJO !!! Este método es completamente dependiente de esa macro.
 * Si hay cambios en ella, HAY QUE CAMBIAR EL MÉTODO.
 * 
 *  LAYER_ARRANGEMENTS[MAX_VERT_ARRANG][3] = {
 *    {0, 1, 2}, {1, 2, 3},                       // Grupos consecutivos
 *    {0, 1, 3}, {0, 2, 3}                        // Grupos salteados
 *  };
 */
int DTTrigPhase2Prod::getOmittedHit(int idx) {
  
  int ans = -1;
  
  switch (idx) {
    case 0: ans = 3; break;
    case 1: ans = 0; break;
    case 2: ans = 2; break;
    case 3: ans = 1; break;
  }

  return ans;
}


int DTTrigPhase2Prod::compute_pathId(MuonPath *mPath) {
    if(my_debug) std::cout<<"DTp2:\t\t\t pathId: In function compute_pathId, computing_pathId for wires: ";
    for(int i=0;i<=3;i++)
	if(my_debug) std::cout<<mPath->getPrimitive(i)->getChannelId()<<" ";
    if(my_debug) std::cout<<std::endl;

    int base_wire = mPath->getPrimitive(0)->getChannelId();

    int high     = 2*(mPath->getPrimitive(3)->getChannelId()-base_wire)-1;

    int mid_high = 2*(mPath->getPrimitive(2)->getChannelId()-base_wire);

    int mid_low  = 2*(mPath->getPrimitive(1)->getChannelId()-base_wire)-1;
    

    int array[4]={0,mid_low,mid_high,high};
    
    if(my_debug) std::cout<<"DTp2:\t\t\t pathId: built array ";
    for(int i=0;i<=3;i++)
	if(my_debug) std::cout<<array[i]<<" ";
    if(my_debug) std::cout<<std::endl;

    int this_path=-1;
    
    for(this_path=0;this_path<8;this_path++){
	bool is_this_path=true;
	for(int layer=0;layer<4;layer++){
	    if(CELL_HORIZONTAL_LAYOUTS[this_path][layer]!=array[layer]){
		is_this_path=false;
	    }
	}
	if(is_this_path==true){
	    if(my_debug) std::cout<<"DTp2:\t\t\t  pathId: computing_pathId returning: "<<this_path<<std::endl;
	    return this_path;
	}
    }
    
    if(my_debug) std::cout<<"DTp2:\t\t\t pathId: pathId not found returning -1 (this should never happen)" <<this_path<<std::endl;
    return -1;
}
