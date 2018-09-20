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

  system("mkdir tdc");
  //system("mkdir hlt");

  cout<<"creating canvas"<<endl;
  TCanvas * Ca0 = new TCanvas("Ca0","Ca0",9600,1600);
  Ca0->cd();
 
  TH1F * allTDChisto;
  allTDChisto  = (TH1F*) (theFile->Get("allTDChisto"));
  allTDChisto->SetXTitle("DTDigi TDC readout");
  allTDChisto->SetYTitle("counts");
  allTDChisto->SetTitle("All TDC counts");
  allTDChisto->Draw();
  allTDChisto->SetFillColor(kBlack);
  Ca0->SaveAs("allTDChisto.png");
  Ca0->Clear();Ca0->Clear();

  TH1F * wh0_se6_st1_sl1or3_TDChisto;
  wh0_se6_st1_sl1or3_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1or3_TDChisto"));
  wh0_se6_st1_sl1or3_TDChisto->SetXTitle("DTDigi TDC readout");
  wh0_se6_st1_sl1or3_TDChisto->SetYTitle("counts");
  wh0_se6_st1_sl1or3_TDChisto->SetTitle("TDC counts from wh0 se6 st1 SL1 and SL3");
  wh0_se6_st1_sl1or3_TDChisto->Draw();
  wh0_se6_st1_sl1or3_TDChisto->SetFillColor(kGreen);
  Ca0->SaveAs("wh0_se6_st1_sl1or3_TDChisto.png");
  Ca0->Clear();Ca0->Clear();

  TH1F * wh0_se6_st1_sl1_TDChisto;
  wh0_se6_st1_sl1_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1_TDChisto"));
  wh0_se6_st1_sl1_TDChisto->SetXTitle("DTDigi TDC readout");
  wh0_se6_st1_sl1_TDChisto->SetYTitle("counts");
  wh0_se6_st1_sl1_TDChisto->SetTitle("TDC counts from wh0 se6 st1 SL1");
  wh0_se6_st1_sl1_TDChisto->Draw();
  wh0_se6_st1_sl1_TDChisto->SetFillColor(kRed);
  Ca0->SaveAs("wh0_se6_st1_sl1_TDChisto.png");
  Ca0->Clear();Ca0->Clear();


  TH1F * wh0_se6_st1_sl3_TDChisto;
  wh0_se6_st1_sl3_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl3_TDChisto"));
  wh0_se6_st1_sl3_TDChisto->SetXTitle("DTDigi TDC readout");
  wh0_se6_st1_sl3_TDChisto->SetYTitle("counts");
  wh0_se6_st1_sl3_TDChisto->SetTitle("TDC counts from wh0 se6 st1 SL3");
  wh0_se6_st1_sl3_TDChisto->Draw();
  wh0_se6_st1_sl3_TDChisto->SetFillColor(kYellow);
  Ca0->SaveAs("wh0_se6_st1_sl3_TDChisto.png");
  Ca0->Clear();Ca0->Clear();

  //Phase-2

  TCanvas * Ca1 = new TCanvas("Ca1","Ca1",9600,1600);
  Ca1->cd();


  TH1F * allTDCPhase2histo;
  allTDCPhase2histo  = (TH1F*) (theFile->Get("allTDCPhase2histo"));
  allTDCPhase2histo->SetXTitle("DTDigi TDCPhase2 [ns]");
  allTDCPhase2histo->SetYTitle("counts");
  allTDCPhase2histo->SetTitle("All TDCPhase2 counts");
  allTDCPhase2histo->Draw();
  //allTDCPhase2histo->SetFillColor(kBlack);
  Ca1->SaveAs("allTDCPhase2histo.png");
  Ca1->Clear(); Ca1->Clear();

  TH1F * wh0_se6_st1_sl1or3_TDCPhase2histo;
  wh0_se6_st1_sl1or3_TDCPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1or3_TDCPhase2histo"));
  wh0_se6_st1_sl1or3_TDCPhase2histo->SetXTitle("DTDigi TDCPhase2 [ns]");
  wh0_se6_st1_sl1or3_TDCPhase2histo->SetYTitle("counts");
  wh0_se6_st1_sl1or3_TDCPhase2histo->SetTitle("TDCPhase2 counts from wh0 se6 st1 SL1 and SL3");
  wh0_se6_st1_sl1or3_TDCPhase2histo->Draw();
  //wh0_se6_st1_sl1or3_TDCPhase2histo->SetFillColor(kGreen);
  Ca1->SaveAs("wh0_se6_st1_sl1or3_TDCPhase2histo.png");
  Ca1->Clear(); Ca1->Clear();

  TH1F * wh0_se6_st1_sl1_TDCPhase2histo;
  wh0_se6_st1_sl1_TDCPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1_TDCPhase2histo"));
  wh0_se6_st1_sl1_TDCPhase2histo->SetXTitle("DTDigi TDCPhase2 [ns]");
  wh0_se6_st1_sl1_TDCPhase2histo->SetYTitle("counts");
  wh0_se6_st1_sl1_TDCPhase2histo->SetTitle("TDCPhase2 counts from wh0 se6 st1 SL1");
  wh0_se6_st1_sl1_TDCPhase2histo->Draw();
  //wh0_se6_st1_sl1_TDCPhase2histo->SetFillColor(kRed);
  Ca1->SaveAs("wh0_se6_st1_sl1_TDCPhase2histo.png");
  Ca1->Clear(); Ca1->Clear();


  TH1F * wh0_se6_st1_sl3_TDCPhase2histo;
  wh0_se6_st1_sl3_TDCPhase2histo  = (TH1F*) (theFile->Get("wh0_se6_st1_sl3_TDCPhase2histo"));
  wh0_se6_st1_sl3_TDCPhase2histo->SetXTitle("DTDigi TDCPhase2 [ns]");
  wh0_se6_st1_sl3_TDCPhase2histo->SetYTitle("counts");
  wh0_se6_st1_sl3_TDCPhase2histo->SetTitle("TDCPhase2 counts from wh0 se6 st1 SL3");
  wh0_se6_st1_sl3_TDCPhase2histo->Draw();
  //wh0_se6_st1_sl3_TDCPhase2histo->SetFillColor(kYellow);
  Ca1->SaveAs("wh0_se6_st1_sl3_TDCPhase2histo.png");
  Ca1->Clear(); Ca1->Clear();

  //2D
  TH2F * wirevslayer;
  wirevslayer  = (TH2F*) (theFile->Get("wirevslayer"));
  wirevslayer->SetXTitle("wire");
  wirevslayer->SetYTitle("(SL-1)*2 + L");
  wirevslayer->SetTitle("occupancy phi-layers vs wire wh0 se6 st1");
  wirevslayer->Draw("colz");
  Ca1->SaveAs("wirevslayer.png");
  Ca1->Clear(); Ca1->Clear();

  exit(0);

}
     
