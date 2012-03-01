void SAMPLE()
{
  gROOT ->Reset();
  gROOT ->SetBatch();
  
  //=========  settings ====================
  gROOT->SetStyle("Plain");
  //gStyle->SetPadGridX(kTRUE);
  //gStyle->SetPadGridY(kTRUE);
  gStyle->SetPadRightMargin(0.06);
  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetPadBottomMargin(0.13);

  
  std::string pvtxfileName = "/uscms/home/sergo/nobackup/crab_JET7TEV_Aug05/res/histo_merged_Run2011A_Express.root";
  //std::string pvtxfileName = "pvtxana_hist_MB2360GeV_STARTUPMC.root";
  TFile *pvtxfile = new TFile(pvtxfileName.c_str());
  
  TCanvas *canvas = new TCanvas("nTracks","nTracks",500,500); 
  TText* te; 
  
  createtrkcompPlot(canvas,"trkPt", 0, 10, pvtxfile, te, "UU", 0.55, 0.75, false, false, true, true);
  createtrkcompPlot(canvas,"trkEta", -2.5, 2.5, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"trkPhi", -3.2, 3.2, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"trkDxy", -0.5, 0.5, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"trkDxyCorr", -0.5, 0.5, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"trkDz", -10, 10, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"nTrk", -0.5, 149.5, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);

  createtrkcompPlot(canvas,"trkPtPV", 0, 10, pvtxfile,  te, "UU", 0.55, 0.75, false, false, true, true);
  createtrkcompPlot(canvas,"trkEtaPV", -2.5, 2.5, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"trkPhiPV", -3.2, 3.2, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"trkDxyPV", -0.5, 0.5, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"trkDxyCorrPV", -0.5, 0.5, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"trkDzPV", -10, 10, pvtxfile,  te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"nTrkPV", -0.5, 149.5, pvtxfile, te, "UU", 0.55, 0.75, false, false, false, true);  
  createtrkcompPlot(canvas,"nHWTrkPV", -0.5, 149.5, pvtxfile, te, "UU", 0.55, 0.75, false, false, false, true);  
  createtrkcompPlot(canvas,"ndofPV", 0, 50, pvtxfile, te, "UU", 0.55, 0.75, false, false, false, true);
  createtrkcompPlot(canvas,"nrecPV", 0, 10, pvtxfile, te, "UU",0.55, 0.75, false, false, false, true);

  //createresPlot(canvas,"resx", pvtxfile,  te, "UU", 0.15, 0.75);
  //createresPlot(canvas,"resy", pvtxfile,  te, "UU", 0.15, 0.75);
  //createresPlot(canvas,"resz", pvtxfile,  te, "UU", 0.15, 0.75);
  //createresPlot(canvas,"pullx", pvtxfile,  te, "UU", 0.15, 0.75);
  //createresPlot(canvas,"pully", pvtxfile,  te, "UU", 0.15, 0.75);
  //createresPlot(canvas,"pullz", pvtxfile,  te, "UU", 0.15, 0.75);

  create1Plot(canvas, "nrecPVDiff",  pvtxfile,  te, "UU", false, true, -5, 5);    
  create1Plot(canvas, "nTrkPVDiff",  pvtxfile, te, "UU", false, true, -10, 10);
  create1Plot(canvas, "nTrkPVRelDiff",  pvtxfile,  te, "UU", false, true, -1, 1);
  create1Plot(canvas, "twovtxzsign",  pvtxfile,  te, "UU", false, false, -10, 10); 
  create1Plot(canvas, "ndofPVDiff",  pvtxfile,  te, "UU", false, false, -10, 10); 

}

void createtrkcompPlot(TCanvas *canvas, TString name, double xMin, double xMax, TFile *pvtxfile, 
		       TText* te, char * option, double startingY, 
		       double startingX = .1, bool fit = false, 
		       bool logx=false, bool logy= false, bool normalizehistogram=false){
  
  TH1F *h1;
  TH1F *h2;
  TH1F *h;

  TString basename = "Summary/";
  
  TString name1 = basename;
  name1.Append(name);
  TString name2 = basename;
  name2.Append(name);
  
  TString genname = basename;
  genname.Append(name); 

  if(!name.Contains("errPV")) {
    name1.Append("1_spl");
    name2.Append("2_spl");
  }
  else {
    name1.Append("_spl1_mct"); 
    name2.Append("_spl2_mct");
    genname.Append("_mct");
  }
  
  h1 = (TH1F*) pvtxfile->Get(name1);
  h2 = (TH1F*) pvtxfile->Get(name2);
  h = (TH1F*) pvtxfile->Get(genname);
   

  if(normalizehistogram) 
    NormalizeHistogramsByEntries(h1, h2, h);
  
  TString x_title = "";
  if( name.Contains("trkPt",TString::kExact) )   x_title = "P_{t} (Track)" ;
  if( name.Contains("trkEta",TString::kExact) )   x_title = "#eta (Track)" ;
  if( name.Contains("trkPhi",TString::kExact) )   x_title = "#phi (Track)" ;
  if( name.Contains("trkDxy",TString::kExact) )   x_title = "Dxy (Track) (cm)" ;
  if( name.Contains("trkDxyCorr",TString::kExact) )   x_title = "BS corrected Dxy (Track) (cm)" ;
  if( name.Contains("trkDz",TString::kExact) )   x_title = "Dz (Track) (cm)" ;
  if( name.Contains("nTrk",TString::kExact) )   x_title = "#Tracks" ;
  if( name.Contains("nTrkPV",TString::kExact) )   x_title = "#Tracks in Pvtx" ;
  if( name.Contains("nHWTrkPV",TString::kExact) )   x_title = "#Tracks with weight > 0.5 in Pvtx" ;
  if( name.Contains("ndofPV",TString::kExact) )   x_title = "Pvtx ndof";

  TString y_title = "Entries";
  if(normalizehistogram) y_title.Prepend("Normalized ");
  
  h1->GetXaxis()->SetRangeUser(xMin,xMax);
  h2->GetXaxis()->SetRangeUser(xMin,xMax);
  h->GetXaxis()->SetRangeUser(xMin,xMax);

  h1->SetLineColor(2);
  h1->StatOverflows(kTRUE);
  h1->GetYaxis()->SetLabelSize(0.038);
  h1->GetYaxis()->SetLabelFont(42);
  h1->SetLineWidth(3);
  h1->SetTitle("");
  h1->GetYaxis()->SetTitleFont(42);
  h1->GetYaxis()->SetTitleOffset(1.1);
  h1->GetYaxis()->SetTitleSize(0.06);
  h1->GetXaxis()->SetTitleFont(42);
  h1->GetXaxis()->SetTitleOffset(0.9);
  h1->GetXaxis()->SetTitleSize(0.06);


  h1->GetYaxis()->SetTitle(y_title);
  h1->GetXaxis()->SetTitle(x_title);
  
  
  h2->SetLineColor(4);
  h2->StatOverflows(kTRUE);
  h2->GetYaxis()->SetLabelSize(0.038); 
  h2->GetYaxis()->SetLabelFont(42);
  h2->SetLineWidth(3);
  h2->SetTitle("");  
  h2->GetYaxis()->SetTitleFont(42);
  h2->GetYaxis()->SetTitleOffset(1.1);
  h2->GetYaxis()->SetTitleSize(0.06);
  h2->GetXaxis()->SetTitleFont(42);
  h2->GetXaxis()->SetTitleOffset(0.9);
  h2->GetXaxis()->SetTitleSize(0.06);
  
  
  h->SetLineColor(1);
  h->StatOverflows(kTRUE);
  h->GetYaxis()->SetLabelSize(0.038);
  h->GetYaxis()->SetLabelFont(42);
  h->SetLineWidth(3);
  h->SetTitle("");
  
  h->GetYaxis()->SetTitleFont(42);
  h->GetYaxis()->SetTitleOffset(1.1);
  h->GetYaxis()->SetTitleSize(0.06);
  h->GetXaxis()->SetTitleFont(42);
  h->GetXaxis()->SetTitleOffset(0.9);
  h->GetXaxis()->SetTitleSize(0.06);
  h->GetYaxis()->SetTitle(y_title);
  h->GetXaxis()->SetTitle(x_title);
  
  setStats(h1,h2,h, startingY, startingX, fit);
  //gPad->Update();
  if(logx) gPad->SetLogx();
  else gPad->SetLogx(0);
  if(logy) gPad->SetLogy();
  else gPad->SetLogy(0);
  
  h1->Draw();
  h2->Draw("sames");
  h->Draw("sames");  
  canvas->Update();
  
  //c1->Update();
 

 
  TString pngfilename = name; 
  TString pngdir = "PNGDIR/SAMPLE/";
  //TString pngdir="pngfiles/";
  pngfilename.Prepend(pngdir);
  pngfilename.Append(".png");
  canvas->Print(pngfilename);
  

  TString epsfilename = name; 
  TString epsdir = "EPSDIR/SAMPLE/";
  //TString epsdir="epsfiles/";
  epsfilename.Prepend(epsdir);
  epsfilename.Append(".eps");
  canvas->Print(epsfilename);
  
  delete h1;
  delete h2;
  delete h;


}


void createresPlot(TCanvas *canvas, TString name, TFile *pvtxfile, 
		   TText* te, char * option, double startingY, 
		   double startingX = .1, bool fit = false){
  
  TH2D *h1;
  TH2D *h2;
  TH2D *h3;
  TH2D *h4;
  
  TString basename = "Results/";
  TString name1 = basename;
  name1.Append(name);
  name1.Append("_spl1_mct_nTrk");
  
  TString name2 = basename;
  name2.Append(name);
  name2.Append("_spl2_mct_nTrk");
  
  TString name3 = basename;
  name3.Append(name);
  name3.Append("_nTrk");
  
  TString name4 = basename;
  name4.Append(name);
  name4.Append("_mct_nTrk");
  
 
  
  h1 = (TH2D*) pvtxfile->Get(name1);
  h2 = (TH2D*) pvtxfile->Get(name2);
  h3 = (TH2D*) pvtxfile->Get(name3);
  h4 = (TH2D*) pvtxfile->Get(name4);
  
  if (name.Contains("pull"))    {
    h1->GetYaxis()->SetRangeUser(0,2);
    h2->GetYaxis()->SetRangeUser(0,2);
    h3->GetYaxis()->SetRangeUser(0,2);
    h4->GetYaxis()->SetRangeUser(0,2);
  }

  
  if (name.Contains("res"))    {
    h1->GetYaxis()->SetRangeUser(0,350);
    h2->GetYaxis()->SetRangeUser(0,350);
    h3->GetYaxis()->SetRangeUser(0,350);
    h4->GetYaxis()->SetRangeUser(0,350);
  }

 h1->GetXaxis()->SetRangeUser(2,30);
  h2->GetXaxis()->SetRangeUser(2,30);
  h3->GetXaxis()->SetRangeUser(2,30);
  h4->GetXaxis()->SetRangeUser(2,30);
  TString y_title = "";
  if( name.Contains("resx") )     y_title = "x-Resolution (#mu m)"; 
  if( name.Contains("resy") )      y_title = "y-Resolution (#mu m)"; 
  if( name.Contains("resz") )   y_title = "y-Resolution (#mu m)"; 
  if( name.Contains("pullx") )   y_title = "Gausian #sigma of x-Pull";
  if( name.Contains("pully") )   y_title = "Gausian #sigma of y-Pull";
  if( name.Contains("pullz") )   y_title = "Gausian #sigma of z-Pull";
    
  
  TString x_title = "Number of Tracks in PVT";
  
  h1->SetLineColor(2);
  h1->SetMarkerColor(2);
  h1->StatOverflows(kTRUE);
  h1->GetYaxis()->SetLabelSize(0.038);
  h1->SetLineWidth(3);
  h1->SetTitle("");
  h1->GetYaxis()->SetTitle(y_title);
  h1->GetXaxis()->SetTitle(x_title);
  
  
  h2->SetLineColor(4);
  h2->SetMarkerColor(4);
  h2->StatOverflows(kTRUE);
  h2->GetYaxis()->SetLabelSize(0.038);
  h2->SetLineWidth(3);
  h2->SetTitle("");
  h2->GetYaxis()->SetTitle(y_title);
  h2->GetXaxis()->SetTitle(x_title);
  
  h3->SetLineColor(3); 
  h3->SetMarkerColor(3);
  h3->StatOverflows(kTRUE);
  h3->GetYaxis()->SetLabelSize(0.038);
  h3->SetLineWidth(3);
  h3->SetTitle("");
  h3->GetYaxis()->SetTitle(y_title);
  h3->GetXaxis()->SetTitle(x_title);

  h4->SetLineColor(1); 
  h4->SetMarkerColor(1);
  h4->StatOverflows(kTRUE);
  h4->GetYaxis()->SetLabelSize(0.038);
  h4->SetLineWidth(3);
  h4->SetTitle("");
  h4->GetYaxis()->SetTitle(y_title);
  h4->GetXaxis()->SetTitle(x_title);

  
  setStats(h1,h2,h3,h4,startingY, startingX);
  
  h1->Draw();
  h2->Draw("sames");
  h3->Draw("sames");  
  h4->Draw("sames");  

  canvas->Update();
  gPad->Update();
  
  c1->Update();
  gPad->Update();

  TString pngfilename = name; 
  TString pngdir = "PNGDIR/SAMPLE/";
  //TString pngdir="pngfiles/";
  pngfilename.Prepend(pngdir);
  pngfilename.Append(".png");
  canvas->Print(pngfilename);
  

  TString epsfilename = name; 
  TString epsdir = "EPSDIR/SAMPLE/";
  //TString epsdir="epsfiles/";
  epsfilename.Prepend(epsdir);
  epsfilename.Append(".eps");
  canvas->Print(epsfilename);
  
  delete h1;
  delete h2;
  delete h3;
  delete h4;
}

void create1Plot(TCanvas *canvas, TString name, TFile *pvtxfile, 
		 TText* te, char * option, 
		 bool logx=false, bool logy= false, double xMin, double xMax) {
  
  TH1F *h1;
  
  TString basename = "Summary/";
  TString name1 = basename;
  name1.Append(name);
  
  h1 = (TH1F*) pvtxfile->Get(name1);

  TString x_title = "";
  if( name.Contains("nTrkPVDiff",TString::kExact) )   x_title = "nTrk(vtx1) - nTrk(vtx2)" ;
  if( name.Contains("nTrkPVRelDiff",TString::kExact) )   x_title = "(nTrk1-nTrk2)/(nTrk1+nTrk2)";
  if( name.Contains("nrecPVDiff",TString::kExact) )   x_title = "nrecPV1 - nrecPV2";
  if( name.Contains("twovtxzsign",TString::kExact) )   x_title = "(z1-z2)/max(#sigmaz1, #sigmaz2)";
  if( name.Contains("ndofPVDiff",TString::kExact) )   x_title = "ndof(vtx1)-ndof(vtx2)";

  TString y_title = "Entries";

  h1->SetLineColor(1);
  h1->StatOverflows(kTRUE);
  h1->GetYaxis()->SetLabelSize(0.038);
  h1->GetYaxis()->SetLabelFont(42);
  h1->SetLineWidth(3);
  h1->SetTitle("");
  h1->GetYaxis()->SetTitle(y_title);
  h1->GetXaxis()->SetTitle(x_title);
  h1->GetXaxis()->SetRangeUser(xMin, xMax);
  
  h1->GetYaxis()->SetTitleFont(42);
  h1->GetYaxis()->SetTitleOffset(1.1);
  h1->GetYaxis()->SetTitleSize(0.06);
  h1->GetXaxis()->SetTitleFont(42);
  h1->GetXaxis()->SetTitleOffset(0.9);
  h1->GetXaxis()->SetTitleSize(0.06);

  gPad->Update();
  if(logx) gPad->SetLogx();
  else gPad->SetLogx(0);
  if(logy) gPad->SetLogy();
  else gPad->SetLogy(0);
  
  h1->Draw();
  canvas->Update();
  //c1->Update();
  
  TString pngfilename = name; 
  TString pngdir = "PNGDIR/SAMPLE/";
  //TString pngdir="pngfiles/";
  pngfilename.Prepend(pngdir);
  pngfilename.Append(".png");
  canvas->Print(pngfilename);
  

  TString epsfilename = name; 
  TString epsdir = "EPSDIR/SAMPLE/";
  //TString epsdir="epsfiles/";
  epsfilename.Prepend(epsdir);
  epsfilename.Append(".eps");
  canvas->Print(epsfilename);

  delete h1;
 

}

void NormalizeHistogramsByEntries(TH1F* h1, TH1F* h2, TH1F *h3)
{
  if (h1==0 || h2==0 || h3 ==0) return;
  float scale1 = -9999.9;
  float scale2 = -9999.9;
  float scale3 = -9999.9;

  if ( h1->Integral() != 0 && h2->Integral() != 0 && h3->Integral()!=0){
      scale1 = 1.0/(float)h1->Integral();
      scale2 = 1.0/(float)h2->Integral();
      scale3 = 1.0/(float)h3->Integral();
    
      //h1->Sumw2();
      //h2->Sumw2();
      //h3->Sumw2();
      h1->Scale(scale1);
      h2->Scale(scale2);
      h3->Scale(scale3);
    }
}


void setStats(TH1* h1, TH1* h2, TH1* h3, double startingY, double startingX = .1,bool fit){
  if (startingY<0){
    h1->SetStats(0);
    h2->SetStats(0);
    h3->SetStats(0);
  } else {
    h1->SetStats(1);
    h2->SetStats(1);
    h3->SetStats(1);
    
    if (fit){
      h1->Fit("gaus", "Q", "0", -2, 2);
      TF1* f1 = (TF1*) h1->GetListOfFunctions()->FindObject("gaus");
      f1->SetLineColor(2);
      f1->SetLineWidth(1);
    }
    h1->Draw();
    gPad->Update(); 
    TPaveStats* st1 = (TPaveStats*) h1->GetListOfFunctions()->FindObject("stats");
    if (fit) {st1->SetOptFit(0010);    st1->SetOptStat(1001);} 
    else     st1->SetOptStat(1111);
    st1->SetX1NDC(startingX);
    st1->SetX2NDC(startingX+0.20);
    st1->SetY1NDC(startingY+0.30);
    st1->SetY2NDC(startingY+0.45);
    st1->SetTextColor(2);
        

    if (fit) {
      h2->Fit("gaus","Q","0", -2,2);
      TF1* f2 = (TF1*) h2->GetListOfFunctions()->FindObject("gaus");
      f2->SetLineColor(4);
      f2->SetLineWidth(1);    
    }
    h2->Draw();
    gPad->Update(); 
    TPaveStats* st2 = (TPaveStats*) h2->GetListOfFunctions()->FindObject("stats");
    if (fit) {st2->SetOptFit(0010);    st2->SetOptStat(1001);}
    else     st2->SetOptStat(1111);
    st2->SetX1NDC(startingX);
    st2->SetX2NDC(startingX+0.20);
    st2->SetY1NDC(startingY+0.15);
    st2->SetY2NDC(startingY+0.30);
    st2->SetTextColor(4);


   if (fit) {
     h3->Fit("gaus","Q","0",-2,2);
      TF1* f2 = (TF1*) h3->GetListOfFunctions()->FindObject("gaus");
      f2->SetLineColor(1);
      f2->SetLineWidth(1);    
    }
    h3->Draw();
    gPad->Update(); 
    TPaveStats* st3 = (TPaveStats*) h3->GetListOfFunctions()->FindObject("stats");
    if (fit) {st3->SetOptFit(0010);    st3->SetOptStat(1001);} 
    else     st3->SetOptStat(1111);
    st3->SetX1NDC(startingX);
    st3->SetX2NDC(startingX+0.20);
    st3->SetY1NDC(startingY);
    st3->SetY2NDC(startingY+0.15);
    st3->SetTextColor(1);

  }
}

void setStats(TH2* h1, TH2* h2, TH2* h3, TH2 *h4, double startingY, double startingX = .1){
  if (startingY<0){
    h1->SetStats(0);
    h2->SetStats(0);
    h3->SetStats(0);
    h4->SetStats(0);
  } else {
    h1->SetStats(10);
    h2->SetStats(10);
    h3->SetStats(10);
    h4->SetStats(10);
    
    h1->Draw();
    gPad->Update(); 
    TPaveStats* st1 = (TPaveStats*) h1->GetListOfFunctions()->FindObject("stats");
    st1->SetX1NDC(startingX);
    st1->SetX2NDC(startingX+0.20);
    st1->SetY1NDC(startingY+0.60);
    st1->SetY2NDC(startingY+0.80);
    st1->SetTextColor(2);
    st1->SetOptStat(1111);

    h2->Draw();
    gPad->Update(); 
    TPaveStats* st2 = (TPaveStats*) h2->GetListOfFunctions()->FindObject("stats");
    st2->SetX1NDC(startingX);
    st2->SetX2NDC(startingX+0.20);
    st2->SetY1NDC(startingY+0.40);
    st2->SetY2NDC(startingY+0.60);
    st2->SetTextColor(4);
    st2->SetOptStat(1111); 
   
   
    h3->Draw();
    gPad->Update(); 
    TPaveStats* st3 = (TPaveStats*) h3->GetListOfFunctions()->FindObject("stats");
    st3->SetX1NDC(startingX);
    st3->SetX2NDC(startingX+0.20);
    st3->SetY1NDC(startingY+0.20);
    st3->SetY2NDC(startingY+0.40);
    st3->SetTextColor(3);
    st3->SetOptStat(1111); 

    h4->Draw();
    gPad->Update(); 
    TPaveStats* st4 = (TPaveStats*) h4->GetListOfFunctions()->FindObject("stats");
    st4->SetX1NDC(startingX);
    st4->SetX2NDC(startingX+0.20);
    st4->SetY1NDC(startingY);
    st4->SetY2NDC(startingY+0.20);
    st4->SetTextColor(1);
    st4->SetOptStat(1111);
  }
}

// Doesn't work!!
void fixRangeY(TH1* h1,TH1* h2, TH1* h3){

  double ymin1 = (h1->GetBinContent(h1->GetMinimumBin()) < h2->GetBinContent(h2->GetMinimumBin())) ? 
    h1->GetBinContent(h1->GetMinimumBin()) : h2->GetBinContent(h2->GetMinimumBin());

  double ymin2 = (h2->GetBinContent(h2->GetMinimumBin()) < h3->GetBinContent(h3->GetMinimumBin())) ? 
    h2->GetBinContent(h2->GetMinimumBin()) : h3->GetBinContent(h3->GetMinimumBin());

  double ymin = (ymin1<ymin2)?ymin1:ymin2;


  double ymax1 = (h1->GetBinContent(h1->GetMaximumBin()) > h2->GetBinContent(h2->GetMaximumBin())) ?
    h1->GetBinContent(h1->GetMaximumBin()) : h2->GetBinContent(h2->GetMaximumBin());

  double ymax2 = (h2->GetBinContent(h2->GetMaximumBin()) > h3->GetBinContent(h3->GetMaximumBin())) ?
    h2->GetBinContent(h2->GetMaximumBin()) : h3->GetBinContent(h3->GetMaximumBin());
  
  double ymax= (ymax1>ymax2)?ymax1:ymax2;

  h1->GetYaxis()->SetRangeUser(ymin*0.9,ymax*1.1);
  h2->GetYaxis()->SetRangeUser(ymin*0.9,ymax*1.1);
  h3->GetYaxis()->SetRangeUser(ymin*0.9,ymax*1.1);

}

