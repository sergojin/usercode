{

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  //  TFile f1("/uscms_data/d3/sergo/crab_MB7TEV_Aug05_v3/res/histo_merged_Run2011A_Express.root");
    TFile f1("/uscms_data/d3/sergo/crab_JET7TEV_Aug05/res/histo_merged_Run2011A_Express.root");
    TFile f3("/uscms_data/d3/sergo/crab_QCDMC_Pythia6_TuneZ2_test/res/histo_Pythia_merged.root");

   TFile f2("/uscms_data/d3/sergo/crab_QCDMC_Pythia6_TuneZ2/res/histo_merged_QCDMC_Pythia6_TuneZ2.root");
 
  TString hname ="Summary/trkPhiPV";


  TH1F* t1=f1->Get(hname);
  TH1F* t2=f2->Get(hname);
  TH1F* t3=f3->Get(hname);

  //  t1->Rebin(5);
  //  t2->Rebin(5);

  t1->Scale(1/(t1->GetMaximum()));
  t2->Scale(1/(t2->GetMaximum()));
  t3->Scale(1/(t3->GetMaximum()));

  t1->Draw();
  t2->Draw("same");
  //  t3->Draw("same");


  c1=new TCanvas("c1","c1",600,500);

  t1->SetLineColor(kRed);
  t1->SetLineWidth(3);
  t1->Draw();

  t2->SetLineColor(kBlue);
  t2->SetLineWidth(3);
  t2->Draw("same");

  t3->SetLineColor(kCyan);
  t3->SetLineWidth(3);
  //  t3->Draw("same");


  
  leg=new TLegend(0.1, 0.75,0.4,0.9);
  leg->AddEntry(t1,"Jet data","l");
  leg->AddEntry(t2,"QCD Pythia6","l");
  //  leg->AddEntry(t3,"Pythia6 dijet","l");
  leg->Draw();


}
