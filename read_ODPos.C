// This code sort of works. It only produces 39 events so I know that it is not looping 
// over something. Other than that, it works. I want to make a new, cleaner code that
// gives me information abour not only x,y,z position but also charge and time. I will 
// write that code in the file called OD3Devent



#include <stdio.h>     
#include <stdlib.h>    

void read_ODPos(char *filename=NULL) {
  /* A simple script to plot aspects of phototube hits 
   * 
   * I like to run this macro as 
   * $ root -l -x 'read_OD.C("OD.root")'
   */

  gROOT->Reset();
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.rootmap");
  }else{
    std::cout << "Can't load WCSim ROOT dictionaries" << std::endl;
  }
  gStyle->SetOptStat(1);

  TFile *f;
  char fTest[128];
  if (filename==NULL){
    std::cout << "Please provide filename in option" << std::endl;
    std::cout << "Will load auto wcsim.root in WCSIMDIR ..." << std::endl;
    char* name = "wcsim.root";
    strncpy(fTest, wcsimdirenv, sizeof(fTest));
    strncat(fTest, "/", (sizeof(fTest) - strlen(fTest)) );
    strncat(fTest, name, (sizeof(fTest) - strlen(fTest)) );
    f = new TFile(fTest);
  }else{
    f = new TFile(filename);
  }
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }else{
    if (filename==NULL) cout << "File open bro: " << fTest << endl;
    else cout << "File open bro: " << filename << endl;
  }

  TTree  *wcsimT = (TTree*)f->Get("wcsimT");

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent_OD",&wcsimrootsuperevent);

 
  // I set all "event" to "geom" for this part - 24/10/2018
 
 TTree *wcsimGeoT = (TTree*)f->Get("wcsimGeoT");

  WCSimRootGeom *wcsimrootgeom = new WCSimRootGeom();
  wcsimGeoT->SetBranchAddress("wcsimrootgeom",&wcsimrootgeom);
  wcsimGeoT->GetEntry(0);
 


  
  //Loading the triggers - 24/10/2018
  int ev = 0; // *************************Maybe find a way to loop over all events
    

  wcsimT->GetEntry(ev);
  WCSimRootTrigger *wcsimroottrigger = wcsimrootsuperevent->GetTrigger(0);
  
  int ncherenkovdigihits = wcsimroottrigger->GetNcherenkovdigihits();

  // After creating wcsimroottrigger, use it to get Ncherenov hits


  //Loop through elements in TClonesArray
  for (int i=0; i<ncherenkovdigihits; i++){
    WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)
      (wcsimroottrigger->GetCherenkovDigiHits()->At(i));

    double charge = hit->GetQ();
     int tubeID = hit->GetTubeId();
  WCSimRootPMT pmt = wcsimrootgeom->GetPMT(tubeID);
  double pmtX = pmt.GetPosition(0);
  double pmtY = pmt.GetPosition(1);
  double pmtZ = pmt.GetPosition(2);

  // std::cout << charge << std::endl;
  //  std::cout << tubeID << std::endl;
  //std::cout << pmtX << pmtY << pmtZ << std::endl;

  }
  




 


  //////////////////////////////////////////
  // HISTOGRAMS DEFINITION /////////////////
  //////////////////////////////////////////



  const int nBins = 1000;
  const int nEvents = 1000;
  const int nBinZAxis = 100; //Number of bins
  const int nMaxBinZAxis = 3500;

  
  TH1D *hitAgainstZAxis = new TH1D("hitAgainstZAxis","Number of Digitized Hits against Z-axis position",
				  nBinZAxis,-5000,5000);
 hitAgainstZAxis->GetXaxis()->SetTitle("z-axis position");
 hitAgainstZAxis->SetLineColor(kBlue-4);
 hitAgainstZAxis->SetMarkerColor(kBlue-4); 


 TH1D *hitAgainstXAxis = new TH1D("hitAgainstXAxis","Number of Digitized Hits against X-axis position",
				  nBinZAxis,-5000,5000 );
 hitAgainstXAxis ->GetXaxis()->SetTitle("x-axis position");
 hitAgainstXAxis->SetLineColor(kRed-4);
 hitAgainstXAxis->SetMarkerColor(kRed-4); 


 TH1D *hitAgainstYAxis = new TH1D("hitAgainstYAxis","Number of Digitized Hits against Y-axis position",
				   nBinZAxis,-5000,5000);
  hitAgainstYAxis->GetXaxis()->SetTitle("y-axis position");
  hitAgainstYAxis->SetLineColor(kGreen-4);
  hitAgainstYAxis->SetMarkerColor(kGreen-4); 

  TH3D *hit3D = new TH3D("hit3D","hit3D",nBinZAxis,-5000,5000,nBinZAxis,-5000,5000,nBinZAxis,-5000,5000);
  hit3D->GetXaxis()->SetTitle("x");
   hit3D->GetYaxis()->SetTitle("y");
  hit3D->GetYaxis()->SetTitle("z");

  // End histogram definition. Maybe make r2 and theta plot too

  
  int nbEntries= wcsimrootsuperevent->GetNumberOfEvents();
  
  for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
    // Point to event iEntry inside WCSimTree
    wcsimT->GetEvent(iEntry); 
    
    // Nb of Trigger


    // inside the event
    const unsigned int nbTriggers = wcsimrootsuperevent->GetNumberOfEvents();
    const unsigned int nbSubTriggers = wcsimrootsuperevent->GetNumberOfSubEvents();

    // cout << "iEntry : " << iEntry << endl;
    // cout << "nbTrig : " << nbTriggers << endl;
    // cout << "nbSubTrig : " << nbSubTriggers << endl;
    
    for(long unsigned int iTrig = 0; iTrig < nbTriggers; iTrig++){
      WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(iTrig);
     
      
      for (int i=0; i<ncherenkovdigihits; i++){
    WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)
      (wcsimroottrigger->GetCherenkovDigiHits()->At(i));

    double charge = hit->GetQ();
     int tubeID = hit->GetTubeId();
  WCSimRootPMT pmt = wcsimrootgeom->GetPMT(tubeID);
  double pmtX = pmt.GetPosition(0);
  double pmtY = pmt.GetPosition(1);
  double pmtZ = pmt.GetPosition(2);


hitAgainstZAxis->Fill(pmtZ);
hitAgainstXAxis->Fill(pmtX);
hitAgainstYAxis->Fill(pmtY);
 hit3D->Fill(pmtX,pmtY,pmtZ);


//Not sure if this is the right way I want to visualise my data. Is this how to plot a histogram?




//Filling histogram. Can do similarly for charge (Fill(pmtZ) etc..)   



     
      }

    } // END FOR iTRIG
    
  } // END FOR iENTRY





  //////////////////////////////////////////
  // DRAWING ///////////////////////////////
  //////////////////////////////////////////

  TCanvas *c1;

  c1 = new TCanvas("cPE","cPE",800,600);
  c1->Divide(3,1);
  c1->cd(1);
  hitAgainstZAxis->Draw("HIST");
  c1->cd(2);
  hitAgainstXAxis->Draw("HIST");
  c1->cd(3);
  hitAgainstYAxis->Draw("HIST");

 c2 = new TCanvas("cPE","cPE",800,600);
  c2->Divide(1,1);
  c2->cd(1);
  hit3D->Draw("p");


  // Edit this section if need parameters output //

  /*
  cout << "Mean nb of tubes hit by events : " << hNbTubesHit->GetMean()
       << " +- " << hNbTubesHit->GetRMS() << endl;
  cout << "Mean raw PE by events by PMT : " << hPEByEvtsByPMT->GetMean()
       << " +- " << hPEByEvtsByPMT->GetRMS() << endl;  
  cout << "Mean PE collected by events by PMT : " << hPECollectedByEvtsByPMT->GetMean()
       << " +- " << hPECollectedByEvtsByPMT->GetRMS() << endl;
  cout << "Mean raw PE by events : " << hPEByEvts->GetMean()
       << " +- " << hPEByEvts->GetRMS() << endl;  
  cout << "Mean PE collected by events : " << hPECollectedByEvts->GetMean()
       << " +- " << hPECollectedByEvts->GetRMS() << endl;
  cout << "FIT : " << fit->GetParameter(1)
       << " +- " << fit->GetParError(1) << endl;  

  */





}


 



















/* ORIGINAL READ_OD CODE! 

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  //wcsimT->GetBranch("wcsimrootevent_OD")->SetAutoDelete(kTRUE);


  // const long unsigned int nbEntries = wcsimT->GetEntries();
  const long unsigned int nbEntries = wcsimT->GetEntries();
  cout << "Nb of entries " << wcsimT->GetEntries() << endl;

  //////////////////////////////////////////
  // HISTOGRAMS DEFINITION /////////////////
  //////////////////////////////////////////

  const int nbBins = 50;
  const int nbPEMax = 1000;
  const int nbBinsByPMT = 25;
  const int nbPEMaxByPMT = 200;
  
  TH1D *hPEByEvtsByPMT = new TH1D("hPEByEvtsByPMT","RAW PE by Evts by PMT",
				  nbBinsByPMT,0,nbPEMaxByPMT);
  hPEByEvtsByPMT->GetXaxis()->SetTitle("raw PE");
  hPEByEvtsByPMT->SetLineColor(kBlue-4);
  hPEByEvtsByPMT->SetMarkerColor(kBlue-4);  
  TH1D *hPECollectedByEvtsByPMT = new TH1D("hPECollectedByEvtsByPMT","collected PE by Evts by PMT",
					   nbBinsByPMT,0,nbPEMaxByPMT);
  hPECollectedByEvtsByPMT->GetXaxis()->SetTitle("digi PE");
  hPECollectedByEvtsByPMT->SetLineColor(kRed-4);
  hPECollectedByEvtsByPMT->SetMarkerColor(kRed-4);
  
  TH1D *hPEByEvts = new TH1D("hPEByEvts","Total RAW PE by Evts",nbBins,0,nbPEMax);
  hPEByEvts->GetXaxis()->SetTitle("raw PE");
  hPEByEvts->SetLineColor(kBlue+1);
  hPEByEvts->SetMarkerColor(kBlue+1);
  hPEByEvts->SetFillColor(kBlue+1);  
  TH1D *hPECollectedByEvts = new TH1D("hPECollectedByEvts","Total collected PE by Evts",nbBins,0,nbPEMax);
  hPECollectedByEvts->GetXaxis()->SetTitle("digi PE");
  hPECollectedByEvts->SetLineColor(kRed+1);
  hPECollectedByEvts->SetMarkerColor(kRed+1);
  hPECollectedByEvts->SetFillColor(kRed+1);

  TH1D *hNbTubesHit = new TH1D("hNbTubesHit","Nb of Tubes Hit",50,0,1000);
  
  // END HISTOGRAMS DEFINITION /////////////
  //////////////////////////////////////////

  for(long unsigned int iEntry = 0; iEntry < nbEntries; iEntry++){
    // Point to event iEntry inside WCSimTree
    wcsimT->GetEvent(iEntry); 
    
    // Nb of Trigger
 inside the event
    const unsigned int nbTriggers = wcsimrootsuperevent->GetNumberOfEvents();
    const unsigned int nbSubTriggers = wcsimrootsuperevent->GetNumberOfSubEvents();

    // cout << "iEntry : " << iEntry << endl;
    // cout << "nbTrig : " << nbTriggers << endl;
    // cout << "nbSubTrig : " << nbSubTriggers << endl;
    
    for(long unsigned int iTrig = 0; iTrig < nbTriggers; iTrig++){
      WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(iTrig);
      
      // RAW HITS
      int rawMax = wcsimrootevent->GetNcherenkovhits();
      int totRawPE = 0;
      for (int i = 0; i < rawMax; i++){
	WCSimRootCherenkovHit *chit = (WCSimRootCherenkovHit*)wcsimrootevent->GetCherenkovHits()->At(i);
	
	if(chit->GetTotalPe(1) != 0) hPEByEvtsByPMT->Fill(chit->GetTotalPe(1));
	totRawPE+=chit->GetTotalPe(1);
      } // END FOR RAW HITS

      hNbTubesHit->Fill(rawMax);
      hPEByEvts->Fill(totRawPE);

      // DIGI HITS
      int digiMax = wcsimrootevent->GetNcherenkovdigihits();
      int totDigiPE = 0;
      for (int i = 0; i < digiMax; i++){
	WCSimRootCherenkovDigiHit *cDigiHit =
	  (WCSimRootCherenkovDigiHit*)wcsimrootevent->GetCherenkovDigiHits()->At(i);
	//WCSimRootChernkovDigiHit has methods GetTubeId(), GetT(), GetQ()
	WCSimRootCherenkovHitTime *cHitTime =
	  (WCSimRootCherenkovHitTime*)wcsimrootevent->GetCherenkovHitTimes()->At(i);
	//WCSimRootCherenkovHitTime has methods GetTubeId(), GetTruetime()

	if(cDigiHit->GetQ() != 0) hPECollectedByEvtsByPMT->Fill(cDigiHit->GetQ());
	totDigiPE+=cDigiHit->GetQ();
      } // END FOR DIGI HITS

      hPECollectedByEvts->Fill(totDigiPE);
      
    } // END FOR iTRIG
    
  } // END FOR iENTRY

  //////////////////////////////////////////
  // DRAWING ///////////////////////////////
  //////////////////////////////////////////

  TCanvas *c1;

  c1 = new TCanvas("cPE","cPE",800,600);
  c1->Divide(2,2);
  c1->cd(1);
  gPad->SetLogy();
  hPEByEvtsByPMT->Draw("HIST");
  c1->cd(2);
  hPEByEvts->Draw(""); hPEByEvts->Fit("gaus");
  c1->cd(3);
  gPad->SetLogy();
  hPECollectedByEvtsByPMT->Draw("HIST");
  c1->cd(4);  
  hPECollectedByEvts->Draw("HIST");

  TF1 *fit = hPEByEvts->GetFunction("gaus");
  
  c1 = new TCanvas("cNbTubesHit","cNbTubesHit",800,600);
  hNbTubesHit->Draw("HIST");

  cout << "Mean nb of tubes hit by events : " << hNbTubesHit->GetMean()
       << " +- " << hNbTubesHit->GetRMS() << endl;
  cout << "Mean raw PE by events by PMT : " << hPEByEvtsByPMT->GetMean()
       << " +- " << hPEByEvtsByPMT->GetRMS() << endl;  
  cout << "Mean PE collected by events by PMT : " << hPECollectedByEvtsByPMT->GetMean()
       << " +- " << hPECollectedByEvtsByPMT->GetRMS() << endl;
  cout << "Mean raw PE by events : " << hPEByEvts->GetMean()
       << " +- " << hPEByEvts->GetRMS() << endl;  
  cout << "Mean PE collected by events : " << hPECollectedByEvts->GetMean()
       << " +- " << hPECollectedByEvts->GetRMS() << endl;
  cout << "FIT : " << fit->GetParameter(1)
       << " +- " << fit->GetParError(1) << endl;  

} // END MACRO


  */


