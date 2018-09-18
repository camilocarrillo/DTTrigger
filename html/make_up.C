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
  TCanvas * Ca0 = new TCanvas("Ca0","bit0",1200,800);
  Ca0->cd();
 
  TH1F * allTDChisto;
  allTDChisto  = (TH1F*) (theFile->Get("allTDChisto"));
  allTDChisto->SetXTitle("DTDigi TDC readout");
  allTDChisto->SetYTitle("counts");
  allTDChisto->Draw();
  allTDChisto->SetFillColor(kBlack);
  Ca0->SaveAs("allTDChisto.png");
  Ca0->Clear();

  TH1F * wh0_se6_st1_sl1or3_TDChisto;
  wh0_se6_st1_sl1or3_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1or3_TDChisto"));
  wh0_se6_st1_sl1or3_TDChisto->SetXTitle("DTDigi TDC readout");
  wh0_se6_st1_sl1or3_TDChisto->SetYTitle("counts");
  wh0_se6_st1_sl1or3_TDChisto->Draw();
  wh0_se6_st1_sl1or3_TDChisto->SetFillColor(kGreen);
  Ca0->SaveAs("wh0_se6_st1_sl1or3_TDChisto.png");
  Ca0->Clear();

  TH1F * wh0_se6_st1_sl1_TDChisto;
  wh0_se6_st1_sl1_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl1_TDChisto"));
  wh0_se6_st1_sl1_TDChisto->SetXTitle("DTDigi TDC readout");
  wh0_se6_st1_sl1_TDChisto->SetYTitle("counts");
  wh0_se6_st1_sl1_TDChisto->Draw();
  wh0_se6_st1_sl1_TDChisto->SetFillColor(kRed);
  Ca0->SaveAs("wh0_se6_st1_sl1_TDChisto.png");
  Ca0->Clear();


  TH1F * wh0_se6_st1_sl3_TDChisto;
  wh0_se6_st1_sl3_TDChisto  = (TH1F*) (theFile->Get("wh0_se6_st1_sl3_TDChisto"));
  wh0_se6_st1_sl3_TDChisto->SetXTitle("DTDigi TDC readout");
  wh0_se6_st1_sl3_TDChisto->SetYTitle("counts");
  wh0_se6_st1_sl3_TDChisto->Draw();
  wh0_se6_st1_sl3_TDChisto->SetFillColor(kYellow);
  Ca0->SaveAs("wh0_se6_st1_sl3_TDChisto.png");
  Ca0->Clear();

  exit;

}
     
