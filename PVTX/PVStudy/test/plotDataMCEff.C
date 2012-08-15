#include "tdrStyle.C"
#include "TStyle.h"
#include "TFile.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>


void plotDataMCEff(){

   //  gStyle->SetOptStat(0);
  //  gStyle->SetOptTitle(0);
  //  gStyle->SetCanvasBorderSize(1.2); 

  TFile f1("/uscms_data/d3/sergo/crab_MB7TEV_Aug05_GRL_pveff_7/res/histo_merged.root");
  //TFile f1("/uscms_data/d3/sergo/crab_MBMC_TuneZ2_pveff_10/res/histo_merged.root");
    TFile f2("/uscms_data/d3/sergo/crab_MBMC_TuneZ2_pveff_7/res/histo_merged.root");
  // TFile f2("histo.root");

 
    TString hname_num ="Summary/eff_numer_ntrack";
    TString hname_den ="Summary/eff_denom_ntrack";

    TString hname_num_m2 ="Summary/eff_numer_ntrack";
    TString hname_den_m2 ="Summary/eff_denom_ntrack";


    TH1F* h1_num=(TH1F*) f1.Get(hname_num);
    TH1F* h1_den=(TH1F*) f1.Get(hname_den);

    TH1F* h2_num=(TH1F*) f2.Get(hname_num_m2);
    TH1F* h2_den=(TH1F*) f2.Get(hname_den_m2);

    h1_num->Divide(h1_num, h1_den, 1, 1, "B");
    h2_num->Divide(h2_num, h2_den, 1, 1, "B");

    for (int i = 1; i<h1_num->GetNbinsX()+1; i++) {
      if(h1_num->GetBinContent(i) == 0 || h1_den->GetBinContent(i) == 0 ) continue;
      float error = sqrt(h1_num->GetBinContent(i)*(1-h1_num->GetBinContent(i))/h1_den->GetBinContent(i)); // Binominal-Error
      h1_num->SetBinError(i, error);
    }

    for (int i = 1; i<h2_num->GetNbinsX()+1; i++) {
      if(h2_num->GetBinContent(i) == 0 || h2_den->GetBinContent(i) == 0 ) continue;
      float error = sqrt(h2_num->GetBinContent(i)*(1-h2_num->GetBinContent(i))/h2_den->GetBinContent(i)); // Binominal-Error
      h2_num->SetBinError(i, error);
    }

    
    setTDRStyle();
    TCanvas* c1=new TCanvas("c1","c1",1);

    h2_num->SetMarkerColor(kRed);
    h2_num->SetLineColor(kRed);
    h2_num->SetMarkerStyle(21);
    h2_num->SetLineWidth(3);

    h2_num->SetMinimum(0.97);
    h2_num->SetMaximum(1.02);
    h2_num->Draw("E0");
    h2_num->GetXaxis()->SetRange(0,20);
    h2_num->GetXaxis()->SetTitle("Number of Tracks");
    //    h2_num->GetXaxis()->SetTitleOffset(1.2);
    h2_num->GetYaxis()->SetTitle("Primary Vertex Efficiency");
    //    h2_num->GetYaxis()->SetTitleOffset(1.35);
    c1->Update();
      

    h1_num->SetMarkerColor(kBlack);
    h1_num->SetMarkerStyle(21);
    h1_num->SetLineWidth(3);
    h1_num->Draw("E0 same");
    
    
    TLegend* leg=new TLegend(0.1, 0.75,0.4,0.9);
    leg->AddEntry(h1_num,"Data","pl");
    leg->AddEntry(h2_num,"Simulation","pl");
    leg->SetBorderSize(0);
    leg->Draw();

    c1->SaveAs("PVEff.root");
    c1->SaveAs("PVEff.eps");
}
