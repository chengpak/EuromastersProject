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



void ODClusterNov22(char *filename=NULL) {

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
  
  
  TNtuple *ntuple = new TNtuple("ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge");  


  // variables for clustering algorithm

  TNtuple *maxchargentuple = new TNtuple("maxchargentuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge");
  TNtuple *n2ntuple = new TNtuple("nextntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge");
  TNtuple *maxcharge2ntuple = new TNtuple("maxcharge2ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge");
  Double_t maxCharge = 0.0;
  Double_t maxpmtX = 0.0;
  Double_t maxpmtY = 0.0;
  Double_t maxpmtZ = 0.0;
  Double_t maxnextCharge = 0.0;
  Double_t maxnextpmtX = 0.0;
  Double_t maxnextpmtY = 0.0;
  Double_t maxnextpmtZ = 0.0;





  


   //*******************  Filling ntuples  **********************************

   

   
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
	   Double_t pmtY = pmt.GetPosition(1);
	   Double_t pmtZ = pmt.GetPosition(2);

	   Double_t charge = hit->GetQ();
	

	   ntuple->Fill(pmtX,pmtY,pmtZ,charge);
	   


	   if (charge>maxCharge){
	     maxCharge = charge;
	     maxpmtX = pmtX;
	     maxpmtY = pmtY;
	     maxpmtZ = pmtZ;
	   }


	   
	 }

       }

   }




   

   // ******************  Clustering algorithm  *********************************


   
   cout << maxpmtX << " " <<maxpmtY << " " <<maxpmtZ << " " <<  maxCharge << endl;
   


   //using 10% diameter = 710.2 and 10% height =  599.7  -> divided by two to get centre of my event cube -> one day maybe extract this straight from root file and use "const"
  

   
   float  pX,pY,pZ, Q;
   ntuple->SetBranchAddress("pmtX",&pX);
   ntuple->SetBranchAddress("pmtY",&pY);
   ntuple->SetBranchAddress("pmtZ",&pZ);
   ntuple->SetBranchAddress("charge",&Q);


    
   for ( Long64_t ient = 0; ient < ntuple -> GetEntries(); ient++ ) {
    
     ntuple->GetEntry(ient);

 
     
     
     
     
     if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*maxCharge) )){

	 maxchargentuple->Fill(pX,pY,pZ,Q);

       }


       else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*maxCharge) ) ){                       
	   continue;
       
	 }
       

	 else { n2ntuple->Fill(pX,pY,pZ,Q); 
	   if (Q>maxnextCharge){                        // This is the highest charge outside of the first cluster, and its associated coordinates
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ; 
	     }
	 }
     

   }

 


   // **********************************  Repeating ntuple analysis on nextntuṕle to get next highest charge and associated cluster  ********************

   


   nextntuple->SetBranchAddress("pmtX",&pX);
   nextntuple->SetBranchAddress("pmtY",&pY);
   nextntuple->SetBranchAddress("pmtZ",&pZ);
   nextntuple->SetBranchAddress("charge",&Q);
   
   for ( Long64_t ient = 0; ient < nextntuple -> GetEntries(); ient++ ) {
    
     nextntuple->GetEntry(ient);
     
         
     if ((pX>maxnextpmtX-355.1) && (pX<maxnextpmtX+355.1) && (pY>maxnextpmtY-355.1) && (pY<maxnextpmtY+355.1) && (pZ>maxnextpmtZ-299.9) && (pZ<maxnextpmtZ+299.9 ) && (Q>(0.05*maxnextCharge) )){

	 maxcharge2ntuple->Fill(pX,pY,pZ,Q);

       }


     else {

       continue;
     }
   }





   /*  ****** check if the data entries are correct

   ntuple->Print();
   maxchargentuple->Print();  
   nextntuple->Print();
   maxchargenextntuple->Print();
   
   */
   
     






   //******************  Drawing histogram  *********************************
  




   TCanvas *c1;

  
   c1 = new TCanvas("ntuple","ntuple", 800,600);
   //  c1->Divide(2,1);
   // c1->cd(1);


   TGeoTube *myCy1 = new TGeoTube(0,3551,2798.5);        // Drawing a cylinder shape with the same dimensions as Hyper K
   myCy1->Draw();
   



   maxchargentuple->SetMarkerStyle(21);
   maxchargentuple->SetMarkerColor(2);

   maxchargentuple->Draw("pmtZ:pmtX:pmtY","","same");



   // This makes a COLZ plot of charge against x,y,z
   //ntuple->Draw("pmtZ:pmtX:pmtY:charge", "", "COLZ");

   

   //  c1->cd(2);



   maxcharge2ntuple->SetMarkerStyle(21);
   maxcharge2ntuple->SetMarkerColor(4);

   maxcharge2ntuple->Draw("pmtZ:pmtX:pmtY","", "same");



  
    ntuple->~TNtuple(); 
    maxchargentuple->~TNtuple();
    maxcharge2ntuple->~TNtuple();
    n2ntuple->~TNtuple(); //Destructor


   

}
