// This code reads an input file and extracts the position (x,y,z) of entries and its 
// associated charge and stores it in an ntuple.
// This code then plots on a single canvas the x,y,z position of entries,as well as the 
// distribution of charge along the x-axis, y-axis and z-axis.
//
// Further fucntionality: a COLZ plot of x:y:z:charge can be made by uncommenting the 
// relevant block of text. Can also superimpose a cylinder on the plot by uncommenting, but // this makes the plot smaller.
//
// The code is run as $ root -l -x 'OD3Devent.C("OD.root")' where OD.root refers to the
// location of the input file. 
// The input file right now is ("../OD.root")


#include <stdio.h>     
#include <stdlib.h>    


//****************  Creating macro and opening input file  **************************



void ODTimeNov26(char *filename=NULL) {

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



  const int nBins = 25;
  const int xMin = -3600;
  const int xMax = 3600;  //Radius of HyperK is 3551
  const int zMin = -3000;
  const int zMax = 3000; //Length of HyperK is 5597
  

  //TH3D *hit3D = new TH3D("hit3D","hit3D",nBins,xMin,xMax,nBins,xMin,xMax,nBins,zMin,zMax);

  /*
   hit3D->GetXaxis()->SetTitle("x-axis");
   hit3D->GetYaxis()->SetTitle("y-axis");
   hit3D->GetYaxis()->SetTitle("z-axis");

  */
 TNtuple *ntuple = new TNtuple("ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge");  


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
	   Double_t pmtX = pmt.GetPosition(0);
	   Double_t  pmtY = pmt.GetPosition(1);
	   Double_t pmtZ = pmt.GetPosition(2);

	   Double_t charge = hit->GetQ();
	
	   // std::cout<<" "<<pmtX<< " "<<pmtY<<" " << pmtZ<<" " << tubeID << std::endl;
	 
	   
	   ntuple->Fill(pmtX,pmtY,pmtZ,charge);
	   
	   
	   
	   
	   
	   
	 }

       }

   }





   //******************  Drawing histogram  *********************************

   //  ntuple->Print();   to check if data entries correct




   TCanvas *c1;

  
   c1 = new TCanvas("ntuple","ntuple", 800,600);
   c1->Divide(2,2);


   c1->cd(1);
   ntuple->SetMarkerStyle(21);

   
   // This makes a COLZ plot of charge against x,y,z
   //ntuple->Draw("pmtZ:pmtX:pmtY:charge", "", "COLZ");



   ntuple->Draw("pmtZ:pmtX:pmtY");

   c1->cd(2);
   ntuple->Draw("charge:pmtZ");
   c1->cd(3);
   ntuple->Draw("charge:pmtX");
   c1->cd(4);
    ntuple->Draw("charge:pmtY");

  
   


 
    // This code superimposes a cylinder on the plot
    // makes the plot very small
    //TGeoTube *myCy1 = new TGeoTube(0,3551,2798.5);
    //myCy1->Draw("SAME ");


  
    ntuple->~TNtuple();  //Destructor

}
