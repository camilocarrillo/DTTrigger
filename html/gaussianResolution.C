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
			      
  residuals->SetTitle(("#Delta x,"+title+" (cm)").c_str());
  residuals->GetXaxis()->SetTitle(("#Delta x,"+title+" (cm)").c_str());
  residuals->GetXaxis()->SetRangeUser(-0.02,0.02);
  residuals->SetLineWidth(3);
  residuals->Fit("gaus","Q" ,"C" ,-0.02,0.02);
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
  // Ca0->SetLeftMargin(0.25);
  Ca0->SaveAs((title+".png").c_str());

  
  //T0
  {

      cout<<"getting the histo"<<endl;
  
      string title = "wh0_se6_st1_segment_vs_jm_T0histo_gauss";

      TH1F * T0gauss = (TH1F*) (theFile->Get(title.c_str()));
			      
      T0gauss->GetXaxis()->SetTitle(("#Delta t0,"+title+" (ns)").c_str());
      T0gauss->SetTitle(("#Delta t0,"+title+" (ns)").c_str());
      T0gauss->GetXaxis()->SetRangeUser(-10,40);
      T0gauss->SetLineWidth(3);
      T0gauss->Fit("gaus","Q" ,"C" ,-10,40);
      float sigma = T0gauss->GetFunction("gaus")->GetParameter(2);
      float mean = T0gauss->GetFunction("gaus")->GetParameter(1);

      cout<<mean<<endl;
      cout<<sigma<<endl;
  
      stringstream legend;
      stringstream legend2;
    
      TLegend *leg = new TLegend(0.65,0.9,0.9,0.65);
      legend.str("");
      legend<<"#sigma = "<<setprecision(3)<<sigma<<"ns";   
      legend2.str("");
      legend2<<"<x> = "<<setprecision(3)<<mean<<"ns";

      T0gauss->GetFunction("gaus")->SetLineWidth(4);
      leg->AddEntry(T0gauss->GetFunction("gaus"),legend.str().c_str(),"l");
      leg->AddEntry(T0gauss->GetFunction("gaus"),legend2.str().c_str(),"l");
  
      if(!T0gauss) cout<<"histo not found"<<endl;
  
      cout<<"creating canvas"<<endl;
      TCanvas * Ca0 = new TCanvas("Ca0","T0gauss",1200,800);
      T0gauss->Draw();
      leg->Draw("same");
      cout<<"saving plot"<<endl;
      Ca0->SaveAs((title+".png").c_str());
  }


  //tanPhi
  {

      cout<<"getting the histo"<<endl;
  
      string title = "wh0_se6_st1_segment_vs_jm_tanPhi_gauss";

      TH1F * tanPhigauss = (TH1F*) (theFile->Get(title.c_str()));
			      
      tanPhigauss->GetXaxis()->SetTitle(("#Delta tan #phi,"+title+"()").c_str());
      tanPhigauss->SetTitle(("#Delta tan #phi,"+title+"()").c_str());
      tanPhigauss->GetXaxis()->SetRangeUser(-0.05,0.05);
      tanPhigauss->SetLineWidth(3);
      tanPhigauss->Fit("gaus","Q" ,"C" ,-0.05,0.05);
      float sigma = tanPhigauss->GetFunction("gaus")->GetParameter(2);
      float mean = tanPhigauss->GetFunction("gaus")->GetParameter(1);

      cout<<mean<<endl;
      cout<<sigma<<endl;
  
      stringstream legend;
      stringstream legend2;
    
      TLegend *leg = new TLegend(0.65,0.9,0.9,0.65);
      legend.str("");
      legend<<"#sigma = "<<setprecision(3)<<sigma<<"";   
      legend2.str("");
      legend2<<"<x> = "<<setprecision(3)<<mean<<"";

      tanPhigauss->GetFunction("gaus")->SetLineWidth(4);
      leg->AddEntry(tanPhigauss->GetFunction("gaus"),legend.str().c_str(),"l");
      leg->AddEntry(tanPhigauss->GetFunction("gaus"),legend2.str().c_str(),"l");
  
      if(!tanPhigauss) cout<<"histo not found"<<endl;
  
      cout<<"creating canvas"<<endl;
      TCanvas * Ca0 = new TCanvas("Ca0","tanPhigauss",1200,800);
      tanPhigauss->Draw();
      leg->Draw("same");
      cout<<"saving plot"<<endl;
      Ca0->SaveAs((title+".png").c_str());
  }



  exit(0);
  
}
