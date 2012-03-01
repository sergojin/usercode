#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TF1.h>
#include <TAxis.h>
#include <TGraphErrors.h>
#include <TPaveStats.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <TLegend.h>
#include <TNamed.h>

using namespace std;

bool fit(TH1*&,double []);

bool verbose_ = false;

map<string, TFile*> files;
map<string, TH1*> h;
map<string, TH2*> hSum;
vector<string> pvNames;

double cm2um = 10000;
int nTrkMin_ = 10;
int nTrkMax_ = 100;
int nEntries = nTrkMax_ - nTrkMin_ + 1;

void FillHistos(string, vector<string>, vector<string>, double ***, double ***,
 		double ****, double ****);



void analyzePV()
{
  gROOT->SetStyle("CMS");
  gStyle->SetOptFit(1111);
  gStyle->SetFillColor(0);

  files["TTbar"]   = TFile::Open("ntuple/pvtxana_hist_ttbar.root");

  vector<string> sampleNames;
  for (map<string, TFile*>::const_iterator ifile = files.begin(); ifile!=files.end(); ifile++) {
    cout << ifile->first << endl;
    sampleNames.push_back(ifile->first);
  }

  pvNames.push_back("mct");
  pvNames.push_back("spl1_mct");
  pvNames.push_back("spl2_mct");
  pvNames.push_back("");

  TFile *rootFile_ = new TFile("Summary.root","RECREATE");
  rootFile_->cd();

  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(0111);
  gStyle->SetStatW(0.2);
  gStyle->SetStatFontSize(0.03);

  // Histo to fill:
  for (vector<string>::iterator iname = sampleNames.begin(); iname != sampleNames.end(); iname++) {
    if (verbose_) cout << "Booking histograms for " << *iname << endl;
    for (vector<string>::iterator jname = pvNames.begin(); jname != pvNames.end(); jname++) {
      string suffix_ = ( *jname == "") ? *jname : *jname+"_";
      string title_ = ( *jname == "") ? "diff_spl" : *jname;
      hSum["h_errResX_nTrk_" + suffix_ + *iname] = new TH2D(TString("errResX_nTrk_" + suffix_ + *iname),TString("X error (Primary Vertex) vs number of tracks - " + title_),nEntries,nTrkMin_,nTrkMax_,500,0.,500.);
      hSum["h_errResX_nTrk_" + suffix_ + *iname]->GetXaxis()->SetTitle("Num of tracks");
      hSum["h_errResX_nTrk_" + suffix_ + *iname]->GetYaxis()->SetTitle("#mum");
      hSum["h_errResY_nTrk_" + suffix_ + *iname] = new TH2D(TString("errResY_nTrk_" + suffix_ + *iname),TString("Y error (Primary Vertex) vs number of tracks - " + title_),nEntries,nTrkMin_,nTrkMax_,500,0.,500.);
      hSum["h_errResY_nTrk_" + suffix_ + *iname]->GetXaxis()->SetTitle("Num of tracks");
      hSum["h_errResY_nTrk_" + suffix_ + *iname]->GetYaxis()->SetTitle("#mum");
      hSum["h_errResZ_nTrk_" + suffix_ + *iname] = new TH2D(TString("errResZ_nTrk_" + suffix_ + *iname),TString("Z error (Primary Vertex) vs number of tracks - " + title_),nEntries,nTrkMin_,nTrkMax_,500,0.,500.);
      hSum["h_errResZ_nTrk_" + suffix_ + *iname]->GetXaxis()->SetTitle("Num of tracks");
      hSum["h_errResZ_nTrk_" + suffix_ + *iname]->GetYaxis()->SetTitle("#mum");
    }
    //     hSum["h_errResX_nTrk_" + *iname] = new TH2D(TString("errResX_nTrk_" + *iname),"X error (Primary Vertex) vs number of tracks",nEntries,nTrkMin_,nTrkMax_,500,0.,500.);
    //     hSum["h_errResX_nTrk_" + *iname]->GetXaxis()->SetTitle("Num of tracks");
    //     hSum["h_errResX_nTrk_" + *iname]->GetYaxis()->SetTitle("#mum");
  }

  // Variables and data holders:
  int nFiles = files.size();
  int nPVCol = pvNames.size();

  // Containers for data points
  double ***arrNtrks;
  arrNtrks = new double **[nFiles];
  for (int i=0; i < nFiles; ++i) {
    arrNtrks[i] = new double*[nPVCol];
    for (int j=0; j < nPVCol; ++j) {
      arrNtrks[i][j] = new double[nEntries];
    }   
  }

  double ***arrErrNtrks;
  arrErrNtrks = new double **[nFiles];
  for (int i=0; i < nFiles; ++i) {
    arrErrNtrks[i] = new double*[nPVCol];
    for (int j=0; j < nPVCol; ++j) {
      arrErrNtrks[i][j] = new double[nEntries];
    }   
  }
  
  double ****arrRes;
  arrRes = new double ***[nFiles];
  for (int i=0; i < 3; ++i) {
    arrRes[i] = new double**[nFiles];
    for (int j=0; j < nFiles; ++j) {
      arrRes[i][j] = new double*[nPVCol];
      for (int k=0; k < nPVCol; ++k) {
	arrRes[i][j][k] = new double[nEntries];
      }   
    }
  }
  
  double ****arrErrRes;
  arrErrRes = new double ***[nFiles];
  for (int i=0; i < 3; ++i) {
    arrErrRes[i] = new double**[nFiles];
    for (int j=0; j < nFiles; ++j) {
      arrErrRes[i][j] = new double*[nPVCol];
      for (int k=0; k < nPVCol; ++k) {
	arrErrRes[i][j][k] = new double[nEntries];
      }
    } 
  }
  
  double ****arrPull;
  arrPull = new double ***[nFiles];
  for (int i=0; i < 3; ++i) {
    arrPull[i] = new double**[nFiles];
    for (int j=0; j < nFiles; ++j) {
      arrPull[i][j] = new double*[nPVCol];
      for (int k=0; k < nPVCol; ++k) {
	arrPull[i][j][k] = new double[nEntries];
      }
    } 
  }
  
  double ****arrErrPull;
  arrErrPull = new double ***[nFiles];
  for (int i=0; i < 3; ++i) {
    arrErrPull[i] = new double**[nFiles];
    for (int j=0; j < nFiles; ++j) {
      arrErrPull[i][j] = new double*[nPVCol];
      for (int k=0; k < nPVCol; ++k) {
	arrErrPull[i][j][k] = new double[nEntries];
      }
    } 
  }
  
  // Loop through all files:
  int fid = 0;
  double fpar[2];
  for (map<string,TFile*>::const_iterator ifile = files.begin(); ifile != files.end(); ifile++) {
    if (verbose_) cout << "Processing file: " << ifile->first << endl;
    // Loop over all PV collections
    int pvid = 0;
    for (vector<string>::iterator jname = pvNames.begin(); jname != pvNames.end(); jname++) {
      if (verbose_) cout << "PV collection: " << ((pvid==0) ? "diff_spl" : *jname) << endl;
      // Loop all points of number of tracks:
      int id = 0;
      for (int ntrk = nTrkMin_; ntrk <= nTrkMax_; ++ntrk) {
	fpar[0] = -999.;
	fpar[1] = -999.;
	if (verbose_) cout << ">>>>>> Point of ntrk = " << ntrk << ": " << endl;
	map<int,double> data[nFiles][nPVCol];
	stringstream ss;
	ss << ntrk;
	string suffix = ss.str();
	string tag_ = ifile->first;
	string tag1_ = ( *jname == "") ? *jname : *jname+"_";

	arrNtrks[fid][pvid][id] = ntrk;
	arrErrNtrks[fid][pvid][id] = 0.;

	if (!(TH1*) ifile->second->Get(TString("Others/deltax_"+tag1_+suffix))) continue;
	if (!(TH1*) ifile->second->Get(TString("Others/deltay_"+tag1_+suffix))) continue;
	if (!(TH1*) ifile->second->Get(TString("Others/deltaz_"+tag1_+suffix))) continue;
	if (!(TH1*) ifile->second->Get(TString("Others/pullx_"+tag1_+suffix))) continue;
	if (!(TH1*) ifile->second->Get(TString("Others/pully_"+tag1_+suffix))) continue;
	if (!(TH1*) ifile->second->Get(TString("Others/pullz_"+tag1_+suffix))) continue;

	h["gresx_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/deltax_"+tag1_+suffix));
	h["gresy_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/deltay_"+tag1_+suffix));
	h["gresz_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/deltaz_"+tag1_+suffix));
	h["gpullx_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/pullx_"+tag1_+suffix));
	h["gpully_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/pully_"+tag1_+suffix));
	h["gpullz_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/pullz_"+tag1_+suffix));
	h["gerrResx_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/errPVx_"+tag1_+suffix));
	h["gerrResy_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/errPVy_"+tag1_+suffix));
	h["gerrResz_"+tag1_+tag_+suffix] = (TH1*) ifile->second->Get(TString("Others/errPVz_"+tag1_+suffix));

	// Fit with Gaussian
	for ( int i = 0; i < 6; ++i) {
	  switch (i) {
	  case 0:
	    if (fit(h["gresx_"+tag1_+tag_+suffix], fpar)) {
	      data[fid][pvid][i] = fpar[0]*cm2um;
	      arrRes[i][fid][pvid][id] = data[fid][pvid][i];
	      arrErrRes[i][fid][pvid][id] = fpar[1]*cm2um;
	      hSum["h_errResX_nTrk_"+tag1_+tag_]->Fill(ntrk,h["gerrResx_"+tag1_+tag_+suffix]->GetMean(1) * cm2um);
	    }
	    else {
	      data[fid][pvid][i] = fpar[0];
	      arrRes[i][fid][pvid][id] = data[fid][pvid][i];
	      arrErrRes[i][fid][pvid][id] = fpar[1];
	    }
	    break;
	  case 1:
	    if (fit(h["gresy_"+tag1_+tag_+suffix], fpar)) {
	      data[fid][pvid][i] = fpar[0]*cm2um;
	      arrRes[i][fid][pvid][id] = data[fid][pvid][i];
	      arrErrRes[i][fid][pvid][id] = fpar[1]*cm2um;
	      hSum["h_errResY_nTrk_"+tag1_+tag_]->Fill(ntrk,h["gerrResy_"+tag1_+tag_+suffix]->GetMean(1) * cm2um);
	    }
	    else {
	      data[fid][pvid][i] = fpar[0];
	      arrRes[i][fid][pvid][id] = data[fid][pvid][i];
	      arrErrRes[i][fid][pvid][id] = fpar[1];
	    }
	    break;
	  case 2:
	    if (fit(h["gresz_"+tag1_+tag_+suffix], fpar)) {
	      data[fid][pvid][i] = fpar[0]*cm2um;
	      arrRes[i][fid][pvid][id] = data[fid][pvid][i];
	      arrErrRes[i][fid][pvid][id] = fpar[1]*cm2um;
	      hSum["h_errResZ_nTrk_"+tag1_+tag_]->Fill(ntrk,h["gerrResz_"+tag1_+tag_+suffix]->GetMean(1) * cm2um);
	    }
	    else {
	      data[fid][pvid][i] = fpar[0];
	      arrRes[i][fid][pvid][id] = data[fid][pvid][i];
	      arrErrRes[i][fid][pvid][id] = fpar[1];
	    }
	    break;
	  case 3:
	    if (fit(h["gpullx_"+tag1_+tag_+suffix], fpar)) {
	      data[fid][pvid][i] = fpar[0];
	      arrPull[i-3][fid][pvid][id] = data[fid][pvid][i];
	      arrErrPull[i-3][fid][pvid][id] = fpar[1];
	    }
	    else {
	      data[fid][pvid][i] = fpar[0];
	      arrPull[i-3][fid][pvid][id] = data[fid][pvid][i];
	      arrErrPull[i-3][fid][pvid][id] = fpar[1];
	    }
	    break;
	  case 4:
	    if (fit(h["gpully_"+tag1_+tag_+suffix], fpar)) {
	      data[fid][pvid][i] = fpar[0];
	      arrPull[i-3][fid][pvid][id] = data[fid][pvid][i];
	      arrErrPull[i-3][fid][pvid][id] = fpar[1];
	    }
	    else {
	      data[fid][pvid][i] = fpar[0];
	      arrPull[i-3][fid][pvid][id] = data[fid][pvid][i];
	      arrErrPull[i-3][fid][pvid][id] = fpar[1];
	    }
	    break;
	  case 5:
	    if (fit(h["gpullz_"+tag1_+tag_+suffix], fpar)) {
	      data[fid][pvid][i] = fpar[0];
	      arrPull[i-3][fid][pvid][id] = data[fid][pvid][i];
	      arrErrPull[i-3][fid][pvid][id] = fpar[1];
	    }
	    else {
	      data[fid][pvid][i] = fpar[0];
	      arrPull[i-3][fid][pvid][id] = data[fid][pvid][i];
	      arrErrPull[i-3][fid][pvid][id] = fpar[1];
	    }
	    break;
	  }
	  if (verbose_ && data[fid][pvid][i] > 0) cout << "data[" << fid <<"]["<<pvid<<"]["<<i << "] = " << data[fid][pvid][i] << (i<3?" micro m":" ") << endl;
	}
	id++;
 	delete h["gresx_"+tag1_+tag_+suffix];
 	delete h["gresy_"+tag1_+tag_+suffix];
 	delete h["gresz_"+tag1_+tag_+suffix];
	delete h["gpullx_"+tag1_+tag_+suffix];
	delete h["gpully_"+tag1_+tag_+suffix];
	delete h["gpullz_"+tag1_+tag_+suffix];
	delete h["gerrResx_"+tag1_+tag_+suffix];
	delete h["gerrResy_"+tag1_+tag_+suffix];
	delete h["gerrResz_"+tag1_+tag_+suffix];
      }// End of tracks loop
      pvid++;
    }//End of pv collections loop
    fid++;
  }//End of samples loop


  // /////////////////////////////////
  // Fill and Save Summary Histograms:

  TDirectory *d1 = rootFile_->mkdir("Summary");
  d1->cd();


  FillHistos("Res", sampleNames, pvNames, arrNtrks, arrErrNtrks, arrRes, arrErrRes);
  FillHistos("Pull", sampleNames, pvNames, arrNtrks, arrErrNtrks, arrPull, arrErrPull);


  // Primary Vtx Mean Errors
  map<int,TCanvas*> cf_; // for error histos
  vector<string> coord;
  coord.push_back("X");
  coord.push_back("Y");
  coord.push_back("Z");

  int ei = 0;
  for(vector<string>::iterator iname = sampleNames.begin(); iname != sampleNames.end(); iname++, ei++) {
    cf_[ei] = new TCanvas(TString("VtxError_"+*iname),TString("Vtx Mean Error - "+*iname),1);
    cf_[ei]->cd();
    cf_[ei]->Divide(nPVCol,3);
    string suffix = *iname;
    for (int j = 0; j < 3; j++) {//XYZ
      string suffix1 = coord[j];
      int ej = 0;
      for (vector<string>::iterator jname = pvNames.begin(); jname != pvNames.end(); jname++, ej++) {
	string suffix_ = ( *jname == "") ? *jname : *jname+"_";
	cf_[ei]->cd(nPVCol*j+ej+1);
	//cout << "h_errRes"<<suffix1<<"_nTrk_"<<suffix_<<*iname << endl;
	hSum["h_errRes"+suffix1+"_nTrk_"+suffix_+*iname]->SetMarkerStyle(21);
	hSum["h_errRes"+suffix1+"_nTrk_"+suffix_+*iname]->SetMarkerSize(0.6);
	hSum["h_errRes"+suffix1+"_nTrk_"+suffix_+*iname]->SetMarkerColor(4);
	hSum["h_errRes"+suffix1+"_nTrk_"+suffix_+*iname]->DrawCopy();
      }
    }
    cf_[ei]->Write();
  }
  rootFile_->cd();
  rootFile_->Write();
  rootFile_->Close();

  // ///////////////////////////
  // Delete all dynamic pointers

  for (int i=0; i < nFiles; ++i) {
    for (int j=0; j<nPVCol; ++j) {
      delete [] arrNtrks[i][j];
    }
    delete [] arrNtrks[i];
  }
  delete [] arrNtrks;
  
  for (int i=0; i < nFiles; ++i) {
    for (int j=0; j<nPVCol; ++j) {
      delete [] arrErrNtrks[i][j];
    }
    delete [] arrErrNtrks[i];
  }
  delete [] arrErrNtrks;
  
  for (int i=0; i < 3; ++i) {
    for (int j=0; j < nFiles; ++j) {
      for (int k=0; k < nPVCol; ++k) {
	delete [] arrRes[i][j][k];
      }
      delete [] arrRes[i][j];
    }
    delete [] arrRes[i];
  }
  delete [] arrRes;
  
  for (int i=0; i < 3; ++i) {
    for (int j=0; j < nFiles; ++j) {
      for (int k=0; k < nPVCol; ++k) {
	delete [] arrErrRes[i][j][k];
      }
      delete [] arrErrRes[i][j];
    }
    delete [] arrErrRes[i];
  }
  delete [] arrErrRes;
  
  for (int i=0; i < 3; ++i) {
    for (int j=0; j < nFiles; ++j) {
      for (int k=0; k < nPVCol; ++k) {
	delete [] arrPull[i][j][k];
      }
      delete [] arrPull[i][j];
    }
    delete [] arrPull[i];
  }
  delete [] arrPull;
  
  for (int i=0; i < 3; ++i) {
    for (int j=0; j < nFiles; ++j) {
      for (int k=0; k < nPVCol; ++k) {
	delete [] arrErrPull[i][j][k];
      }
      delete [] arrErrPull[i][j];
    }
    delete [] arrErrPull[i];
  }
  delete [] arrErrPull;
  // End of deletion
}



bool fit(TH1 *& hin, double fitPars[]){
  bool fitted = false;
  TH1 * hdist = (TH1*) hin->Clone();
  TAxis *axis0 = hdist->GetXaxis();
  int nbin = axis0->GetLast();
  double nOF = hdist->GetBinContent(nbin+1);
  double nUF = hdist->GetBinContent(0);
  double fitRange = axis0->GetXmax();
  double sigMax[2] = {0.,0.};
  
  if ( verbose_ ){
    cout << "Last bin = " << nbin;
    cout << "; hdist: Overflow Entries = " << nOF;
    cout << "; Underflow Entries = " << nUF;
    cout << "; hdist->GetEntries() = " << hdist->GetEntries();
    cout << "; fitting range = " << -fitRange << " to " << fitRange << endl;
  }

  if (hdist->GetEntries() - nOF - nUF >= 10) { // FIXME: for reasonable Gaussian fit
    for (int bi = 0; bi < nbin; bi++) {
      if ( (axis0->GetBinLowEdge(bi) < 0) && (hdist->GetBinContent(bi) > 0) ) {
	sigMax[0] = axis0->GetBinLowEdge(bi);
	if ( fabs(sigMax[0]) >= fabs(sigMax[1]) ) sigMax[1] = fabs(sigMax[0]);
      }
      if ( (axis0->GetBinLowEdge(bi) >= 0) && (hdist->GetBinContent(bi) > 0) ) {
	sigMax[0] = axis0->GetBinUpEdge(bi);
	if ( fabs(sigMax[0]) >= fabs(sigMax[1]) ) sigMax[1] = fabs(sigMax[0]);
      }
    }
    if (verbose_) cout << "Fit sigMax = " << sqrt(2.)*sigMax[1] << endl;

    // Uncomment this region to use gaussian fit:
    TF1 *fgaus = new TF1("fgaus","gaus",-fitRange, fitRange);
    fgaus->SetParameter(1, 0.);
    fgaus->SetParLimits(1, -fitRange/40., fitRange/40.);
    fgaus->SetParLimits(2, 0., sqrt(2.)*sigMax[1]);
    fgaus->SetLineColor(4);
    hdist->Fit(fgaus,"QLRM");
    //     gPad->Update();
    fitPars[0] = ((fgaus->GetParameter(2) != 0)?fgaus->GetParameter(2):-999);
    if (fgaus->GetParameter(2) != 0 ) {
      fitPars[1] = fgaus->GetParameter(2)/(1.*sqrt(hdist->GetEntries() - nOF -nUF));
    }
    else {
      cout << "Error = 0. " <<endl;
      fitPars[1] = 0.;
    }

    if (verbose_)
      cout << "Gaussian fit sigma = " << fitPars[0] << " +/- " << fitPars[1] << endl;
    fitted = (fgaus->GetParameter(2) != 0) ? true : false;
    delete fgaus;
// End of gaussian fit

//     fitted = true;
//     fitPars[0] = hdist->GetRMS();
//     fitPars[1] = hdist->GetRMSError();
// //     fitPars[1] = hdist->GetRMS()/(1.*sqrt(hdist->GetEntries() - nOF -nUF));
  }
  else {
    if (verbose_) cout << "Not enough entries!" << endl;
    fitPars[0] = -999.;
    fitPars[1] = 0.;
  }
  delete hdist;
  return fitted;
}


void FillHistos(string type, vector<string> sampleNames, vector<string> pvNames,
		double ***arrNtrks, double ***arrErrNtrks,
		double ****arrType, double ****arrErrType) { // type = "Res" or "Pull"

  int nEntries = nTrkMax_ - nTrkMin_ + 1;
  int nFiles = files.size();
  int nPVCol = pvNames.size();

  vector<string> coord;
  coord.push_back("X");
  coord.push_back("Y");
  coord.push_back("Z");

  // Resolution:
  map<int,TCanvas*> canvas_; // X,Y,Z
  map<int,TLegend*> legend_;
  for (int i = 0; i < 3; i++) {
    canvas_[i]=new TCanvas(TString(type+coord[i]),TString(type+coord[i]),1);
    legend_[i]=new TLegend(0.6,0.6,0.85,0.85);
  }

  int fid = 0;
  for (vector<string>::iterator iname = sampleNames.begin(); iname != sampleNames.end(); iname++, fid++) {
    int pvid = 0;
    for (vector<string>::iterator jname = pvNames.begin(); jname != pvNames.end(); jname++, pvid++) {
      string suffix0 = *iname;
      string suffix1 = (*jname == "") ? "_" : ("_"+*jname+"_");
      string legtag_ = (*jname == "") ? "_diff_spl" : ("_"+*jname);
      for (int j = 0; j < 3; j++) {
	canvas_[j]->cd();
	map<string,TGraphErrors*> gr;
	string suffix2 = coord[j];
	
	// Find how many valid entires
	int vnum = 0;
	for (int i = 0; i < nEntries; i++) {
	  if (arrType[j][fid][pvid][i] >= 0.) vnum++;
	}
	if (vnum == 0) continue;
	// Reassign valid data points to new containers
	double gNtrks[nFiles][nPVCol][vnum];
	double gErrNtrks[nFiles][nPVCol][vnum];
	double gType[3][nFiles][nPVCol][vnum];
	double gErrType[3][nFiles][nPVCol][vnum];
	
	int ipt = 0;
	for (int i = 0; i < nEntries; i++) {
	  if (arrType[j][fid][pvid][i] > 0.) {
	    gNtrks[fid][pvid][ipt] = arrNtrks[fid][pvid][i];
	    gErrNtrks[fid][pvid][ipt] = arrErrNtrks[fid][pvid][i];
	    gType[j][fid][pvid][ipt] = arrType[j][fid][pvid][i];
	    gErrType[j][fid][pvid][ipt] = arrErrType[j][fid][pvid][i];
	    ipt++;
	  }
	}
	//cout << "res_"<<suffix2<<suffix1<<suffix0 << endl;
	
	// ///////////////////////////////////////////////
	// Comp different pv collections of the same sample

	gr["res_"+suffix2+suffix1+suffix0] = new TGraphErrors(vnum,gNtrks[fid][pvid],gType[j][fid][pvid],gErrNtrks[fid][pvid],gErrType[j][fid][pvid]);
	if (type == "Res") gr["res_"+suffix2+suffix1+suffix0]->SetTitle(TString("Primary Vertex: "+suffix2+"-resolution vs. number of tracks"));
	else if (type == "Pull") gr["res_"+suffix2+suffix1+suffix0]->SetTitle(TString("Pull: "+suffix2+"-pull vs. number of tracks"));
	else break;
	gr["res_"+suffix2+suffix1+suffix0]->SetMarkerStyle(20+fid*nPVCol+pvid);
	gr["res_"+suffix2+suffix1+suffix0]->SetMarkerSize(0.8);
	gr["res_"+suffix2+suffix1+suffix0]->SetMarkerColor(1+fid*nPVCol+pvid);

	if (fid==0) {
	  if (pvid==0) {
	    gr["res_"+suffix2+suffix1+suffix0]->GetXaxis()->SetTitle("Num of tracks");
	    gr["res_"+suffix2+suffix1+suffix0]->GetYaxis()->SetTitle("#mum");
	    if (type == "Res") gr["res_"+suffix2+suffix1+suffix0]->SetMinimum(0.);
	    else {
	      gr["res_"+suffix2+suffix1+suffix0]->SetMaximum(2.);
	      gr["res_"+suffix2+suffix1+suffix0]->SetMinimum(0.);
	    }
	    gr["res_"+suffix2+suffix1+suffix0]->Draw("ap");
	    gr["res_"+suffix2+suffix1+suffix0]->SetName("grr");
	    legend_[j]->AddEntry(gr["res_"+suffix2+suffix1+suffix0],TString(suffix0+legtag_),"pl");
	  }
	  else {
	    gr["res_"+suffix2+suffix1+suffix0]->Draw("psame");
	    gr["res_"+suffix2+suffix1+suffix0]->SetName("grr");
	    legend_[j]->AddEntry(gr["res_"+suffix2+suffix1+suffix0],TString(suffix0+legtag_),"pl");
	  }
	}
	else {
	  gr["res_"+suffix2+suffix1+suffix0]->Draw("psame");
	  gr["res_"+suffix2+suffix1+suffix0]->SetName("grr");
	  legend_[j]->AddEntry(gr["res_"+suffix2+suffix1+suffix0],TString(suffix0+legtag_),"pl");
	}
      }//End looping XYZ
    }//End looping pv collections
  }//End loopint samples
  // Sav everything
  for (int i = 0; i < 3; i++) {
    canvas_[i]->cd();
    legend_[i]->Draw();
    canvas_[i]->Write();
  }

}

