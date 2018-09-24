#include <iostream>
#include "Riostream.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <TEfficiency.h>
#include "TLegend.h"

//#define Maxselection 1
//#define Nhltpaths 442

void make_up(){
  gROOT->Reset();
  //  gStyle->SetOptStat(1111);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetPaintTextFormat("2.2f");
  
  //TFile * theFile = new TFile("/afs/cern.ch/user/c/carrillo/higgs/yy/hlt/CMSSW_5_3_2_patch4/src/genAnalyzer/GenAnalyzer/genAnalyzer.root");
  TFile * theFile = new TFile("../dt_phase2.root");
  //TFile * theFile = new TFile("HiggsGenHltRecoAnalyzer/test/genAnalyzer.root");

  //system("mkdir tdc");
  //system("mkdir hlt");

  //Phase-1

  cout<<"creating canvas"<<endl;
  TCanvas * Ca0 = new TCanvas("Ca0","Ca0",9600,3200);
  Ca0->cd();
 
  //TDC
  {
      TH1F * allTDChisto;
      allTDChisto  = (TH1F*) (theFile->Get("allTDChisto"));
      allTDChisto->SetXTitle("DTDigi tdc counts ");
      allTDChisto->SetYTitle("counts");
      allTDChisto->SetTitle("DTDigi tdc counts phase-1");
      allTDChisto->Draw();
      allTDChisto->SetFillColor(kBlack);
      Ca0->SaveAs("allTDChisto.png");
      Ca0->Clear();Ca0->Clear();

      TH1F * wh0_se6_st1_sl1or3_TDChisto;
      wh0_se6_st1_sl1or3_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1or3_TDChisto"));
      wh0_se6_st1_sl1or3_TDChisto->SetXTitle("DTDigi tdc counts ");
      wh0_se6_st1_sl1or3_TDChisto->SetYTitle("counts");
      wh0_se6_st1_sl1or3_TDChisto->SetTitle("DTDigi tdc counts phase-1 wh0 se6 st1 SL1 and SL3");
      wh0_se6_st1_sl1or3_TDChisto->Draw();
      wh0_se6_st1_sl1or3_TDChisto->SetFillColor(kGreen);
      Ca0->SaveAs("wh0_se6_st1_sl1or3_TDChisto.png");
      Ca0->Clear();Ca0->Clear();

      TH1F * wh0_se6_st1_sl1_TDChisto;
      wh0_se6_st1_sl1_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1_TDChisto"));
      wh0_se6_st1_sl1_TDChisto->SetXTitle("DTDigi tdc counts ");
      wh0_se6_st1_sl1_TDChisto->SetYTitle("counts");
      wh0_se6_st1_sl1_TDChisto->SetTitle("DTDigi tdc counts phase-1 wh0 se6 st1 SL1");
      wh0_se6_st1_sl1_TDChisto->Draw();
      wh0_se6_st1_sl1_TDChisto->SetFillColor(kRed);
      Ca0->SaveAs("wh0_se6_st1_sl1_TDChisto.png");
      Ca0->Clear();Ca0->Clear();


      TH1F * wh0_se6_st1_sl3_TDChisto;
      wh0_se6_st1_sl3_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl3_TDChisto"));
      wh0_se6_st1_sl3_TDChisto->SetXTitle("DTDigi tdc counts ");
      wh0_se6_st1_sl3_TDChisto->SetYTitle("counts");
      wh0_se6_st1_sl3_TDChisto->SetTitle("DTDigi tdc counts phase-1 wh0 se6 st1 SL3");
      wh0_se6_st1_sl3_TDChisto->Draw();
      wh0_se6_st1_sl3_TDChisto->SetFillColor(kYellow);
      Ca0->SaveAs("wh0_se6_st1_sl3_TDChisto.png");
      Ca0->Clear();Ca0->Clear();
  }
  //TIME
  {
      TH1F * allTIMEhisto;
      allTIMEhisto  = (TH1F*) (theFile->Get("allTIMEhisto"));
      allTIMEhisto->SetXTitle("DTDigi time (ns)");
      allTIMEhisto->SetYTitle("counts/ns");
      allTIMEhisto->SetTitle("DTDigi time phase-1");
      allTIMEhisto->Draw();
      allTIMEhisto->SetFillColor(kBlack);
      Ca0->SaveAs("allTIMEhisto.png");
      Ca0->Clear();Ca0->Clear();

      TH1F * wh0_se6_st1_sl1or3_TIMEhisto;
      wh0_se6_st1_sl1or3_TIMEhisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1or3_TIMEhisto"));
      wh0_se6_st1_sl1or3_TIMEhisto->SetXTitle("DTDigi time (ns)");
      wh0_se6_st1_sl1or3_TIMEhisto->SetYTitle("counts/ns");
      wh0_se6_st1_sl1or3_TIMEhisto->SetTitle("DTDigi time phase-1 wh0 se6 st1 SL1 and SL3");
      wh0_se6_st1_sl1or3_TIMEhisto->Draw();
      wh0_se6_st1_sl1or3_TIMEhisto->SetFillColor(kGreen);
      Ca0->SaveAs("wh0_se6_st1_sl1or3_TIMEhisto.png");
      Ca0->Clear();Ca0->Clear();

      TH1F * wh0_se6_st1_sl1_TIMEhisto;
      wh0_se6_st1_sl1_TIMEhisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1_TIMEhisto"));
      wh0_se6_st1_sl1_TIMEhisto->SetXTitle("DTDigi time (ns)");
      wh0_se6_st1_sl1_TIMEhisto->SetYTitle("counts/ns");
      wh0_se6_st1_sl1_TIMEhisto->SetTitle("DTDigi time phase-1 wh0 se6 st1 SL1");
      wh0_se6_st1_sl1_TIMEhisto->Draw();
      wh0_se6_st1_sl1_TIMEhisto->SetFillColor(kRed);
      Ca0->SaveAs("wh0_se6_st1_sl1_TIMEhisto.png");
      Ca0->Clear();Ca0->Clear();


      TH1F * wh0_se6_st1_sl3_TIMEhisto;
      wh0_se6_st1_sl3_TIMEhisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl3_TIMEhisto"));
      wh0_se6_st1_sl3_TIMEhisto->SetXTitle("DTDigi time (ns)");
      wh0_se6_st1_sl3_TIMEhisto->SetYTitle("counts/ns");
      wh0_se6_st1_sl3_TIMEhisto->SetTitle("DTDigi time phase-1 wh0 se6 st1 SL3");
      wh0_se6_st1_sl3_TIMEhisto->Draw();
      wh0_se6_st1_sl3_TIMEhisto->SetFillColor(kYellow);
      Ca0->SaveAs("wh0_se6_st1_sl3_TIMEhisto.png");
      Ca0->Clear();Ca0->Clear();
  }  

  //Phase-2

  TCanvas * Ca1 = new TCanvas("Ca1","Ca1",9600,3200);
  Ca1->cd();


  //TDC
  {
      TH1F * allTDCPhase2histo;
      allTDCPhase2histo  = (TH1F*) (theFile->Get("allTDCPhase2histo"));
      allTDCPhase2histo->SetXTitle("DTDigi tdc counts phase2 ");
      allTDCPhase2histo->SetYTitle("counts");
      allTDCPhase2histo->SetTitle("DTDigi tdc counts phase-2");
      allTDCPhase2histo->Draw();
      //allTDCPhase2histo->SetFillColor(kBlack);
      Ca1->SaveAs("allTDCPhase2histo.png");
      Ca1->Clear(); Ca1->Clear();

      TH1F * wh0_se6_st1_sl1or3_TDCPhase2histo;
      wh0_se6_st1_sl1or3_TDCPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1or3_TDCPhase2histo"));
      wh0_se6_st1_sl1or3_TDCPhase2histo->SetXTitle("DTDigi tdc counts phase2");
      wh0_se6_st1_sl1or3_TDCPhase2histo->SetYTitle("counts");
      wh0_se6_st1_sl1or3_TDCPhase2histo->SetTitle("DTDigi tdc counts phase-2 wh0 se6 st1 SL1 and SL3");
      wh0_se6_st1_sl1or3_TDCPhase2histo->Draw();
      //wh0_se6_st1_sl1or3_TDCPhase2histo->SetFillColor(kGreen);
      Ca1->SaveAs("wh0_se6_st1_sl1or3_TDCPhase2histo.png");
      Ca1->Clear(); Ca1->Clear();

      TH1F * wh0_se6_st1_sl1_TDCPhase2histo;
      wh0_se6_st1_sl1_TDCPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1_TDCPhase2histo"));
      wh0_se6_st1_sl1_TDCPhase2histo->SetXTitle("DTDigi tdc counts phase2");
      wh0_se6_st1_sl1_TDCPhase2histo->SetYTitle("counts");
      wh0_se6_st1_sl1_TDCPhase2histo->SetTitle("DTDigi tdc counts phase-2 wh0 se6 st1 SL1");
      wh0_se6_st1_sl1_TDCPhase2histo->Draw();
      //wh0_se6_st1_sl1_TDCPhase2histo->SetFillColor(kRed);
      Ca1->SaveAs("wh0_se6_st1_sl1_TDCPhase2histo.png");
      Ca1->Clear(); Ca1->Clear();


      TH1F * wh0_se6_st1_sl3_TDCPhase2histo;
      wh0_se6_st1_sl3_TDCPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl3_TDCPhase2histo"));
      wh0_se6_st1_sl3_TDCPhase2histo->SetXTitle("DTDigi tdc counts phase2");
      wh0_se6_st1_sl3_TDCPhase2histo->SetYTitle("counts");
      wh0_se6_st1_sl3_TDCPhase2histo->SetTitle("DTDigi tdc counts phase-2 wh0 se6 st1 SL3");
      wh0_se6_st1_sl3_TDCPhase2histo->Draw();
      //wh0_se6_st1_sl3_TDCPhase2histo->SetFillColor(kYellow);
      Ca1->SaveAs("wh0_se6_st1_sl3_TDCPhase2histo.png");
      Ca1->Clear(); Ca1->Clear();
  }
  
  //TIME
  { 
      TH1F * allTIMEPhase2histo;
      allTIMEPhase2histo  = (TH1F*) (theFile->Get("allTIMEPhase2histo"));
      allTIMEPhase2histo->SetXTitle("DTDigi time phase2 (ns)");
      allTIMEPhase2histo->SetYTitle("counts/ns");
      allTIMEPhase2histo->SetTitle("DTDigi time phase-2");
      allTIMEPhase2histo->Draw();
      //allTIMEPhase2histo->SetFillColor(kBlack);
      Ca1->SaveAs("allTIMEPhase2histo.png");
      Ca1->Clear(); Ca1->Clear();
      
      TH1F * wh0_se6_st1_sl1or3_TIMEPhase2histo;
      wh0_se6_st1_sl1or3_TIMEPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1or3_TIMEPhase2histo"));
      wh0_se6_st1_sl1or3_TIMEPhase2histo->SetXTitle("DTDigi time phase2 (ns)");
      wh0_se6_st1_sl1or3_TIMEPhase2histo->SetYTitle("counts/ns");
      wh0_se6_st1_sl1or3_TIMEPhase2histo->SetTitle("DTDigi time phase-2 wh0 se6 st1 SL1 and SL3");
      wh0_se6_st1_sl1or3_TIMEPhase2histo->Draw();
      //wh0_se6_st1_sl1or3_TIMEPhase2histo->SetFillColor(kGreen);
      Ca1->SaveAs("wh0_se6_st1_sl1or3_TIMEPhase2histo.png");
      Ca1->Clear(); Ca1->Clear();
      
      TH1F * wh0_se6_st1_sl1_TIMEPhase2histo;
      wh0_se6_st1_sl1_TIMEPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1_TIMEPhase2histo"));
      wh0_se6_st1_sl1_TIMEPhase2histo->SetXTitle("DTDigi time phase2 (ns)");
      wh0_se6_st1_sl1_TIMEPhase2histo->SetYTitle("counts/ns");
      wh0_se6_st1_sl1_TIMEPhase2histo->SetTitle("DTDigi time phase-2 wh0 se6 st1 SL1");
      wh0_se6_st1_sl1_TIMEPhase2histo->Draw();
      //wh0_se6_st1_sl1_TIMEPhase2histo->SetFillColor(kRed);
      Ca1->SaveAs("wh0_se6_st1_sl1_TIMEPhase2histo.png");
      Ca1->Clear(); Ca1->Clear();
      
      
      TH1F * wh0_se6_st1_sl3_TIMEPhase2histo;
      wh0_se6_st1_sl3_TIMEPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl3_TIMEPhase2histo"));
      wh0_se6_st1_sl3_TIMEPhase2histo->SetXTitle("DTDigi time phase2 (ns)");
      wh0_se6_st1_sl3_TIMEPhase2histo->SetYTitle("counts/ns");
      wh0_se6_st1_sl3_TIMEPhase2histo->SetTitle("DTDigi time phase-2 wh0 se6 st1 SL3");
      wh0_se6_st1_sl3_TIMEPhase2histo->Draw();
      //wh0_se6_st1_sl3_TIMEPhase2histo->SetFillColor(kYellow);
      Ca1->SaveAs("wh0_se6_st1_sl3_TIMEPhase2histo.png");
      Ca1->Clear(); Ca1->Clear();
  }

  //2D
  TH2F * wirevslayer;
  wirevslayer  = (TH2F*) (theFile->Get("wirevslayer"));
  wirevslayer->SetXTitle("wire");
  wirevslayer->SetYTitle("L + (SL-1)*2 or SL1:1-4 SL3:5-8");
  wirevslayer->SetTitle("occupancy phi-layers vs wire wh0 se6 st1");
  wirevslayer->Draw("colz");
  Ca1->SaveAs("wirevslayer.png");
  Ca1->Clear(); Ca1->Clear();

  TH2F * wirevslayerzTDC;
  wirevslayer  = (TH2F*) (theFile->Get("wirevslayerzTDC"));
  wirevslayer->SetXTitle("wire -0.5 + digiTDC/1600.");
  wirevslayer->SetYTitle("L + (SL-1)*2 or SL1:1-4 SL3:5-8");
  wirevslayer->SetTitle("occupancy phi-layers vs wire wh0 se6 st1");
  wirevslayer->Draw("colz");
  Ca1->SaveAs("wirevslayerzTDC.png");
  Ca1->Clear(); Ca1->Clear();

  exit(0);

}
     