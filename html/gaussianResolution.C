#include "Riostream.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include "TLegend.h"

void gaussianResolution(){
  gROOT->Reset();
  gStyle->SetOptStat(0);
  //gStyle->SetOptStat(S);
  gStyle->SetOptFit(0);

  cout<<"getting the file"<<endl;

  TFile * theFile = new TFile("../dt_phase2.root");  
	
  cout<<"getting the histo"<<endl;

  string title = "wh0_se6_st1_segment_vs_jm_x_gauss";

  TH1F * residuals = (TH1F*) (theFile->Get(title.c_str()));
			      
  residuals->GetXaxis()->SetTitle((title+" (cm)").c_str());
  residuals->GetXaxis()->SetRangeUser(-0.2,0.2);
  residuals->SetLineWidth(3);
  residuals->Fit("gaus","Q" ,"C" ,-0.2,0.2);
  float sigma = residuals->GetFunction("gaus")->GetParameter(2);
  float mean = residuals->GetFunction("gaus")->GetParameter(1);

  cout<<mean<<endl;
  cout<<sigma<<endl;
  
  stringstream legend;
  stringstream legend2;
    
  TLegend *leg = new TLegend(0.65,0.9,0.9,0.65);
  legend.str("");
  legend<<"#sigma = "<<setprecision(3)<<sigma*10<<"mm";   
  legend2.str("");
  legend2<<"<x> = "<<setprecision(3)<<mean*10<<"mm";

  residuals->GetFunction("gaus")->SetLineWidth(4);
  leg->AddEntry(residuals->GetFunction("gaus"),legend.str().c_str(),"l");
  leg->AddEntry(residuals->GetFunction("gaus"),legend2.str().c_str(),"l");
  
  if(!residuals) cout<<"histo not found"<<endl;
  
  cout<<"creating canvas"<<endl;
  TCanvas * Ca0 = new TCanvas("Ca0","Residuals",1200,800);
  residuals->Draw();
  leg->Draw("same");
  cout<<"saving plot"<<endl;
  Ca0->SaveAs((title+".png").c_str());

  exit(0);
  
}
