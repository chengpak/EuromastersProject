// This code reads an input file and outputs the x,y and z position of events on a 3D
// cylinder with the dimensions of the Hyper-K experiment. 
// The plot also has additional information regarding charge of photoelectrons and
// the timing of events, effectively making it a 5-dimensional plot.
//
// The code is run as $ root -l -x 'OD3Devent.C("OD.root")' where OD.root refers to the
// location of the input file. 
// The input file right now is ("../OD.root")


#include <stdio.h>     
#include <stdlib.h>    


//****************  Creating macro and opening input file  **************************



void OD3Devent(char *filename=NULL) {

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




  //**************  Loading the tree  ************************************




  TTree  *wcsimT = (TTree*)f->Get("wcsimT");

  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent_OD",&wcsimrootsuperevent);


  TTree *wcsimGeoT = (TTree*)f->Get("wcsimGeoT");

  WCSimRootGeom *wcsimrootgeom = new WCSimRootGeom();
  wcsimGeoT->SetBranchAddress("wcsimrootgeom",&wcsimrootgeom);
  wcsimGeoT->GetEntry(0); //Geometry is the same for all events, so can just use GetEntry(0)


  
  const long unsigned int nEntries = wcsimT->GetEntries();
  cout << "Nb of entries " << wcsimT->GetEntries() << endl;
  



  //*******************  Histograms definition  ***************************************



  const int nBins = 20;
  const int xMin = -3600;
  const int xMax = 3600;  //Radius of HyperK is 3551
  const int zMin = -3000;
  const int zMax = 3000; //Length of HyperK is 5597
  

  TH3D *hit3D = new TH3D("hit3D","hit3D",nBins,xMin,xMax,nBins,xMin,xMax,nBins,zMin,zMax);


   hit3D->GetXaxis()->SetTitle("x-axis");
   hit3D->GetYaxis()->SetTitle("y-axis");
   hit3D->GetYaxis()->SetTitle("z-axis");

   
   //Declare the charge and time too?




   //*******************  Filling histograms  **********************************

   

   
   for(long unsigned int iEntry = 0; iEntry <= nEntries; iEntry++){
      wcsimT->GetEvent(iEntry); 
     // Point to event iEntry inside WCSimTree
  
       const long unsigned int nTriggers = wcsimrootsuperevent->GetNumberOfEvents();
       const long unsigned int nSubTriggers = wcsimrootsuperevent->GetNumberOfSubEvents();
      
       std::cout<< "Reading event " << iEntry <<std::endl;

       for(long unsigned int iTrig = 0; iTrig < nTriggers; iTrig++){
	 WCSimRootTrigger *wcsimroottrigger = wcsimrootsuperevent->GetTrigger(iTrig);
	 int ncherenkovdigihits = wcsimroottrigger->GetNcherenkovdigihits();
	 
	 for (int i=0; i<ncherenkovdigihits; i++){
	   WCSimRootCherenkovDigiHit *hit = (WCSimRootCherenkovDigiHit*)
	     (wcsimroottrigger->GetCherenkovDigiHits()->At(i));


	   int tubeID = hit->GetTubeId();
	   WCSimRootPMT pmt = wcsimrootgeom->GetPMT(tubeID);
	   double pmtX = pmt.GetPosition(0);
	   double pmtY = pmt.GetPosition(1);
	   double pmtZ = pmt.GetPosition(2);

	   double charge = hit->GetQ();

	  
	   hit3D->Fill(pmtX,pmtY,pmtZ);
	   
	  
	   
	   
	   
	 }

       }

   }





   //******************  Drawing histogram  *********************************



   TCanvas *c1;
   c1 = new TCanvas("hit3D","hit3D", 800,600);
  


   hit3D->Draw("BOX2 ");
   

   
   // makes the plot very small

   // TGeoTube *myCy1 = new TGeoTube(0,3551,2798.5);
   // myCy1->Draw("SAME ");


  


}
