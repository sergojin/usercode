void COMPPUPVTXSAMPLE()
{
  gROOT ->Reset();
  gROOT ->SetBatch();
  
  //=========  settings ====================
  gROOT->SetStyle("Plain");
  //gStyle->SetPadGridX(kTRUE);
  //gStyle->SetPadGridY(kTRUE);
  gStyle->SetPadRightMargin(0.07);
  gStyle->SetPadLeftMargin(0.13);
  gStyle->SetPadBottomMargin(0.13);

  std::string ntupledir = "ROOTDIR/";
  
  // TTBar
  std::string pvtxfile1Name = ntupledir+"pvtxana_hist_SINGLEPVTXSAMPLE.root";
  TFile *pvtxfile1 = new TFile(pvtxfile1Name.c_str());
  cout<<pvtxfile1Name.c_str()<<endl;
  // TTBar with PU
  std::string pvtxfile2Name =  ntupledir+"pvtxana_hist_PUSAMPLE.root";
  TFile *pvtxfile2 = new TFile(pvtxfile2Name.c_str());
   
  TFile *pvtxfile[2] = {pvtxfile1, pvtxfile2}; 
   
  TCanvas *canvas = new TCanvas;
  TText* te; 

  const int nsamples = 2;

  canvas = new TCanvas("nTracks","nTracks",700,500);
  createresPlot(canvas,"resx", pvtxfile, nsamples, false, te, "UU", 0.2, 0.75);
  createresPlot(canvas,"resx", pvtxfile,  nsamples, true, te, "UU", 0.2, 0.75);  
  createresPlot(canvas,"resy", pvtxfile,  nsamples, false, te, "UU", 0.2, 0.75);
  createresPlot(canvas,"resy", pvtxfile,  nsamples, true, te, "UU", 0.2, 0.75);
  createresPlot(canvas,"resz", pvtxfile,  nsamples, false, te, "UU", 0.2, 0.75);
  createresPlot(canvas,"resz", pvtxfile,  nsamples, true, te, "UU", 0.2, 0.75);

  createresPlot(canvas,"pullx", pvtxfile,  nsamples, false, te, "UU", 0.2, 0.75);
  createresPlot(canvas,"pullx", pvtxfile,  nsamples, true, te, "UU", 0.2, 0.75);  
  createresPlot(canvas,"pully", pvtxfile,  nsamples, false, te, "UU", 0.2, 0.75);
  createresPlot(canvas,"pully", pvtxfile,  nsamples, true, te, "UU", 0.2, 0.75);
  createresPlot(canvas,"pullz", pvtxfile,  nsamples, false, te, "UU", 0.2, 0.75);
  createresPlot(canvas,"pullz", pvtxfile,  nsamples, true, te, "UU", 0.2, 0.75);  

  create1DPlot(canvas,"nTrkPV", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, 0, 300);
  create1DPlot(canvas,"trkPtPV", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, 0, 100);
  create1DPlot(canvas,"trkEtaPV", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, -3, 3);
  create1DPlot(canvas,"trkPhiPV", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, -3.2, 3.2);
  create1DPlot(canvas,"trkDxyPV", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, -0.5, 0.5);
  create1DPlot(canvas,"trkDzPV", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, -50, 50);
  
  create1DPlot(canvas,"nrecPVDiff", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, -5, 5);
  create1DPlot(canvas,"nTrkPVRelDiff", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, -0.5, 0.5);
  create1DPlot(canvas,"nTrkPVDiff", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, -20.5, 20.5);
  
  create1DPlot(canvas,"nrecPV", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, 0, 10);
  create1DPlot(canvas,"nrecPV1_spl", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, 0, 10);
  create1DPlot(canvas,"nrecPV2_spl", pvtxfile,  nsamples, te, "UU",  false,false, 0.2, 0.75, 0, 10);



}

void createresPlot(TCanvas *canvas, TString name, TFile **pvtxfile, 
		   const int n, bool withmc, TText* te, char * option, double startingY, 
		   double startingX = .1, bool fit = false){
  TH2D *h1[n];
  TH2D *h2[n];
  
  TString basename = "ana/Summary/";
  TString histname = basename;
  histname.Append(name);
  histname.Append("_nTrk");
  
  TString mchistname = basename;
  mchistname.Append(name);
  mchistname.Append("_mct_nTrk");
  //  cout<<mchistname<<endl;
  
  TString x_title = "Number of Tracks in PVTX";

  TString y_title = "";
  if( name.Contains("resx") )     y_title = "x-Resolution (#mu m)"; 
  if( name.Contains("resy") )      y_title = "y-Resolution (#mu m)"; 
  if( name.Contains("resz") )   y_title = "y-Resolution (#mu m)"; 
  if( name.Contains("pullx") )   y_title = "Gausian #sigma of x-Pull";
  if( name.Contains("pully") )   y_title = "Gausian #sigma of y-Pull";
  if( name.Contains("pullz") )   y_title = "Gausian #sigma of z-Pull";

  if( name.Contains("trkPt") )  { 
    histname = basename + name;  
    mchistname = basename + name;  
    cout<<"histname = "<<histname<<endl;
    x_title = "Track pT (GeV)";
  }
  
  for( int i =0; i<n; i++) {

    h1[i] = (TH2D*) pvtxfile[i]->Get(histname);
    h2[i] = (TH2D*) pvtxfile[i]->Get(mchistname);

    if (name.Contains("pull"))    {
      h1[i]->GetYaxis()->SetRangeUser(0,4);
      h2[i]->GetYaxis()->SetRangeUser(0,4);
    }
    if (name.Contains("res"))    {
      h1[i]->GetYaxis()->SetRangeUser(0,100);
      h2[i]->GetYaxis()->SetRangeUser(0,100);
    }
    
    h1[i]->SetLineColor(i+2);
    h1[i]->SetMarkerColor(i+2);
    h1[i]->SetMarkerStyle(21);
    h1[i]->SetMarkerSize(0.7);
    h1[i]->GetXaxis()->SetRangeUser(5,100);
    h1[i]->StatOverflows(kTRUE);
    h1[i]->GetYaxis()->SetLabelSize(0.038);
    h1[i]->SetLineWidth(3);
    h1[i]->SetTitle("");
    h1[i]->GetYaxis()->SetTitle(y_title);
    h1[i]->GetXaxis()->SetTitle(x_title);
    h1[i]->SetStats(0);
    h1[i]->GetXaxis()->SetTitleSize(0.08);
    h1[i]->GetXaxis()->SetTitleOffset(0.6);
    h1[i]->GetYaxis()->SetTitleSize(0.08);
    h1[i]->GetYaxis()->SetTitleOffset(0.6);
  
    h2[i]->SetLineColor(i+2);
    h2[i]->SetMarkerColor(i+2); 
    h2[i]->SetMarkerStyle(25);  
    h2[i]->GetXaxis()->SetRangeUser(5,100);
    h2[i]->StatOverflows(kTRUE);
    h2[i]->GetYaxis()->SetLabelSize(0.038);
    h2[i]->SetLineWidth(3);
    h2[i]->SetTitle("");
    h2[i]->GetYaxis()->SetTitle(y_title);
    h2[i]->GetXaxis()->SetTitle(x_title);
    h2[i]->SetStats(0);
    h2[i]->GetXaxis()->SetTitleSize(0.08);
    h2[i]->GetXaxis()->SetTitleOffset(0.6);
    h2[i]->GetYaxis()->SetTitleSize(0.08);
    h2[i]->GetYaxis()->SetTitleOffset(0.6);

    if(i==0)  h1[i]->Draw();
    else h1[i]->Draw("sames");
    if(withmc) h2[i]->Draw("sames"); 
  }  
  
  canvas->Update();
  TString pngfilename = name;
  TString pngdir = "PNGDIR/";
  pngfilename.Prepend(pngdir);
  if(withmc) pngfilename.Append("_mc");
  pngfilename.Append(".png");
  canvas->Print(pngfilename);
  
  TString epsfilename = name;
  TString epsdir =  "EPSDIR/";
  epsfilename.Prepend(epsdir);
  if(withmc) epsfilename.Append("_mc");                                     
  epsfilename.Append(".eps");
  canvas->Print(epsfilename);
  
  delete h1;
  delete h2;
}

void create1DPlot(TCanvas *canvas, TString name, TFile **pvtxfile, 
		  const int n, TText* te, char * option,  bool logx=false, bool logy= false,
		  double startingY, double startingX = .1,  double xMin, double xMax) {
  TH1F *h1[n];

  TString basename = "ana/Summary/";
  TString histname = basename+name;
  TString x_title = "";
  TString y_title = "Entries";
  
  if( name.Contains("trkPt"))   x_title = "P_{t} (Track)" ;
  if( name.Contains("trkEta") )   x_title = "#eta (Track)" ;
  if( name.Contains("trkPhi") )   x_title = "#phi (Track)" ;
  if( name.Contains("trkDxy") )   x_title = "dxy (Track)" ;
  if( name.Contains("trkDz"))   x_title = "dz (Track)" ;
  if( name.Contains("nTrkPV") )  x_title = "# Tracks in PV";


  for( int i =0; i<n; i++) {

    h1[i] = (TH1F*) pvtxfile[i]->Get(histname);
    h1[i]->SetLineColor(i+2);
    h1[i]->SetMarkerColor(i+2);
    h1[i]->SetMarkerStyle(21);
    h1[i]->SetMarkerSize(0.7);
    h1[i]->StatOverflows(kTRUE); 
    h1[i]->GetXaxis()->SetRangeUser(xMin, xMax);
    h1[i]->GetYaxis()->SetLabelSize(0.038);
    h1[i]->SetLineWidth(3);
    h1[i]->SetTitle("");
    h1[i]->GetYaxis()->SetTitle(y_title);
    h1[i]->GetXaxis()->SetTitle(x_title);
    h1[i]->SetStats(0);
    h1[i]->GetXaxis()->SetTitleSize(0.08);
    h1[i]->GetXaxis()->SetTitleOffset(0.6);
    h1[i]->GetYaxis()->SetTitleSize(0.08);
    h1[i]->GetYaxis()->SetTitleOffset(0.6);
  }
  
  //Normalize the trkPt
  if( name.Contains("trkPt") )  { 
    //NormalizeHistograms(h1[0], h1[1]); 
    //fixRangeY(h1[0], h1[1]); 
  }
  
  if(n==2) fixRangeY(h1[0], h1[1]); 
  for( int i =0; i<n; i++) {
    if(i==0)  h1[i]->Draw();
    else h1[i]->Draw("sames"); 
  }

  if(logx) gPad->SetLogx();
  else gPad->SetLogx(0);
  if(logy) gPad->SetLogy();
  else gPad->SetLogy(0);

  
  canvas->Update();
  if(n==2) setStats(h1[0],h1[1],startingY,startingX);
  
  TString pngfilename = name;  
  TString pngdir = "PNGDIR/";
  pngfilename.Prepend(pngdir);
  pngfilename.Append(".png");
  canvas->Print(pngfilename);
  
  TString epsfilename = name;  
  TString epsdir =  "EPSDIR/";
  epsfilename.Prepend(epsdir);
  epsfilename.Append(".eps");
  canvas->Print(epsfilename);
  

  
  delete h1;
}

void setStats(TH2* h1, TH2* h2, double startingY, double startingX = .1){
  if (startingY<0){
    h1->SetStats(0);
    h2->SetStats(0);
  } else {
    h1->SetStats(10);
    h2->SetStats(10);
    gPad->Update(); 

    TPaveStats* st1 = (TPaveStats*) h1->GetListOfFunctions()->FindObject("stats");
    st1->SetX1NDC(startingX);
    st1->SetX2NDC(startingX+0.20);
    st1->SetY1NDC(startingY+0.60);
    st1->SetY2NDC(startingY+0.80);
    st1->SetTextColor(2);
    st1->SetOptStat(1111);

    TPaveStats* st2 = (TPaveStats*) h2->GetListOfFunctions()->FindObject("stats");
    st2->SetX1NDC(startingX);
    st2->SetX2NDC(startingX+0.20);
    st2->SetY1NDC(startingY+0.40);
    st2->SetY2NDC(startingY+0.60);
    st2->SetTextColor(3);
    st2->SetOptStat(1111);
  }
}

void setStats(TH1* h1, TH1* h2, double startingY, double startingX = .1){
  if (startingY<0){
    h1->SetStats(0);
    h2->SetStats(0);
  } else {
    h1->SetStats(10);
    h2->SetStats(10);

    h1->Draw();
    gPad->Update(); 
    TPaveStats* st1 = (TPaveStats*) h1->GetListOfFunctions()->FindObject("stats");
    st1->SetX1NDC(startingX);
    st1->SetX2NDC(startingX+0.20);
    st1->SetY1NDC(startingY+0.60);
    st1->SetY2NDC(startingY+0.80);
    st1->SetTextColor(2);
    st1->SetOptStat(1111);

    h2->Draw("sames");
    gPad->Update(); 
    TPaveStats* st2 = (TPaveStats*) h2->GetListOfFunctions()->FindObject("stats");
    st2->SetX1NDC(startingX);
    st2->SetX2NDC(startingX+0.20);
    st2->SetY1NDC(startingY+0.40);
    st2->SetY2NDC(startingY+0.60);
    st2->SetTextColor(3);
    st2->SetOptStat(1111);
  }
}
void fixRangeY(TH1* r,TH1* s){
  double ymin = (r->GetBinContent(r->GetMinimumBin()) < s->GetBinContent(s->GetMinimumBin())) ? 
    r->GetBinContent(r->GetMinimumBin()) : s->GetBinContent(s->GetMinimumBin());
  double ymax = (r->GetBinContent(r->GetMaximumBin()) > s->GetBinContent(s->GetMaximumBin())) ?
    r->GetBinContent(r->GetMaximumBin()) : s->GetBinContent(s->GetMaximumBin());
  r->GetYaxis()->SetRangeUser(ymin*0.9,ymax*1.1);
  s->GetYaxis()->SetRangeUser(ymin*0.9,ymax*1.1);
}
void NormalizeHistograms(TH1F* h1, TH1F* h2)
{
  if (h1==0 || h2==0) return;
  float scale1 = -9999.9;
  float scale2 = -9999.9;

  if ( h1->Integral() != 0 && h2->Integral() != 0 ){
      scale1 = 1.0/(float)h1->Integral();
      scale2 = 1.0/(float)h2->Integral();
    
      //h1->Sumw2();
      //h2->Sumw2();
      h1->Scale(scale1);
      h2->Scale(scale2);
    }
}


