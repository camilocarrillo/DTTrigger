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
  residuals->GetXaxis()->SetRangeUser(-4.3,-4);
  residuals->SetLineWidth(3);
  residuals->Fit("gaus","Q" ,"C" ,-4.3,-4);
  float sigma = residuals->GetFunction("gaus")->GetParameter(2);
  float mean = residuals->GetFunction("gaus")->GetParameter(1);

  cout<<mean<<endl;
  cout<<sigma<<endl;
  
  stringstream legend;
    
  TLegend *leg = new TLegend(0.5,0.9,0.9,0.5);
  legend.str("");
  legend<<"sigma = "<<int(sigma*100)<<"mm ";   
  legend<<"mean = "<<int(mean*100)<<"mm";
  residuals->GetFunction("gaus")->SetLineWidth(4);
  leg->AddEntry(residuals->GetFunction("gaus"),legend.str().c_str(),"l");
  
  if(!residuals) cout<<"histo not found"<<endl;
  
  cout<<"creating canvas"<<endl;
  TCanvas * Ca0 = new TCanvas("Ca0","Residuals",1200,800);
  residuals->Draw();
  leg->Draw("same");
  cout<<"saving plot"<<endl;
  Ca0->SaveAs((title+".png").c_str());

  exit(0);
  
}
