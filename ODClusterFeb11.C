// This code reads an input file and extracts the position (x,y,z) of entries and its 
// associated charge and stores it in an ntuple.
// This code then plots on a single canvas the x,y,z position of entries,as well as the 
// distribution of charge along the x-axis, y-axis and z-axis.
//
// Further fucntionality: a COLZ plot of x:y:z:charge can be made by uncommenting the 
// relevant block of text. Can also superimpose a cylinder on the plot by uncommenting, but // this makes the plot smaller.
//
// The code is run as $ root -l -x 'ODClusterJan30.C("OD.root")' where OD.root refers to the
// location of the input file. 
// The input file right now is ("../OD.root")


#include <stdio.h>     
#include <stdlib.h>   
#include<sstream>
#include <vector>


//****************  Creating macro and opening input file  **************************



void ODClusterFeb11(char *filename=NULL) {

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
 
  TCanvas *c1;

  
   c1 = new TCanvas("ntuple","ntuple", 2400,1200);
   c1->Divide(5,2);

  


   //*******************  Filling ntuples  **********************************

   

   
   for(long unsigned int iEntry = 0; iEntry <=nEntries; iEntry++){
      wcsimT->GetEvent(iEntry); 
      // Point to event iEntry inside WCSimTree

      // variables for clustering algorithm
      TNtuple *ntuple = new TNtuple("ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time"); 
      TNtuple *n2ntuple = new TNtuple("n2ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
      TNtuple *maxchargentuple = new TNtuple("maxchargentuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
      Double_t maxCharge = 0.0;
      Double_t maxpmtX = 0.0;
      Double_t maxpmtY = 0.0;
      Double_t maxpmtZ = 0.0;
      Double_t maxtime = 0.0;
      Double_t maxnextCharge = 0.0;
      Double_t maxnextpmtX = 0.0;
      Double_t maxnextpmtY = 0.0;
      Double_t maxnextpmtZ = 0.0;
      Double_t maxnexttime = 0.0;
      Double_t totalqincluster = 0.0; 
      Double_t nClusters = 0;
      std::vector < TNtuple*> ClustersArray;
      const long unsigned int nTriggers = wcsimrootsuperevent->GetNumberOfEvents();
      const long unsigned int nSubTriggers = wcsimrootsuperevent->GetNumberOfSubEvents();
      cout << "Number triggers" << nTriggers << endl;
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
	   Double_t time = hit->GetT();
	

	   ntuple->Fill(pmtX,pmtY,pmtZ,charge,time);
	   


	   if (charge>maxCharge){
	     maxCharge = charge;
	     maxpmtX = pmtX;
	     maxpmtY = pmtY;
	     maxpmtZ = pmtZ;
	     maxtime = time;
	   }


	   
	 }

       }
  

  
   c1->cd(iEntry+1);      
   TGeoTube *myCy1 = new TGeoTube(0,3551,2798.5);        
   myCy1->Draw();
   
  // ******************  Clustering algorithm  *********************************

      
       do {     
   
   cout << "Max charge event in cluster " << nClusters << " is: " << " X: " << maxpmtX << " Y: " <<maxpmtY << " Z: " <<maxpmtZ << " Q: " <<  maxCharge << " time: " <<  maxtime << endl;


   //using 10% diameter = 710.2 and 10% height =  559.7  -> divided by two to get centre of my event cube -> one day maybe extract this straight from root file and use "const"
  

   
   float  pX,pY,pZ, Q, T;
   ntuple->SetBranchAddress("pmtX",&pX);
   ntuple->SetBranchAddress("pmtY",&pY);
   ntuple->SetBranchAddress("pmtZ",&pZ);
   ntuple->SetBranchAddress("charge",&Q);
   ntuple->SetBranchAddress("time",&T);
   n2ntuple->SetBranchAddress("pmtX",&pX);
   n2ntuple->SetBranchAddress("pmtY",&pY);
   n2ntuple->SetBranchAddress("pmtZ",&pZ);
   n2ntuple->SetBranchAddress("charge",&Q);
   n2ntuple->SetBranchAddress("time",&T);
   maxchargentuple->SetBranchAddress("pmtX",&pX);
   maxchargentuple->SetBranchAddress("pmtY",&pY);
   maxchargentuple->SetBranchAddress("pmtZ",&pZ);
   maxchargentuple->SetBranchAddress("charge",&Q);
   maxchargentuple->SetBranchAddress("time",&T);
   TNtuple *maxchargentuple = new TNtuple("maxchargentuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
   

    
   for ( Long64_t ient = 0; ient < ntuple -> GetEntries(); ient++ ) {
    
     ntuple->GetEntry(ient);
 
    
     
      if ((pX>maxpmtX-710.2) && (pX<maxpmtX+710.2) && (pY>maxpmtY-710.2) && (pY<maxpmtY+710.2) && (pZ>maxpmtZ-559.7) && (pZ<maxpmtZ+599.7 ) && (Q>(0.05*maxCharge) ) ){

	maxchargentuple->Fill(pX,pY,pZ,Q,T);
	totalqincluster = totalqincluster+Q;

       }


      else if ((pX>maxpmtX-710.2) && (pX<maxpmtX+710.2) && (pY>maxpmtY-710.2) && (pY<maxpmtY+710.2) && (pZ>maxpmtZ-559.7)&& (pZ<maxpmtZ+599.7 ) && (Q<(0.05*maxCharge) ) ){                       
	   continue;
       
	 }
       

     else { n2ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                        // This is the highest charge outside of the first cluster, and its associated coordinates
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ;
	     maxnexttime = T;
	     }
	 }
     
   }
   ClustersArray.push_back(maxchargentuple);
   //  maxchargentuple->Print();
   nClusters++;
   Double_t nHitsInEachCluster = maxchargentuple->GetEntries();
   cout << "Total charge in cluster " << nClusters << " is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster " << nClusters << "  is: " << nHitsInEachCluster << endl;
   

      // ******* Resetting values for next loop ********** //
   TNtuple *ntuple = new TNtuple("ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time"); 
   ntuple = n2ntuple;
   TNtuple *n2ntuple = new TNtuple("n2ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
   maxCharge = maxnextCharge;
   maxpmtX = maxnextpmtX;
   maxpmtY = maxnextpmtY;
   maxpmtZ = maxnextpmtZ;
   maxnextCharge = 0.0;
   maxnextpmtX = 0.0;
   maxnextpmtY = 0.0;
   maxnextpmtZ = 0.0;
   maxnexttime = 0.0;
   totalqincluster = 0.0;


       }
       while (nHitsInEachCluster > 3 &&  iEntry <= nEntries );

       
   cout << "Event " << iEntry << " has " << nClusters << " clusters." << endl;
   
   
   for (Long64_t eachCluster = 1 ; eachCluster < nClusters; eachCluster++){

   ClustersArray[eachCluster]->SetMarkerStyle(21);
   ClustersArray[eachCluster]->SetMarkerColor(iEntry+1);  // red
   ClustersArray[eachCluster]->Draw("pmtZ:pmtY:pmtX","", "same");
   }
   
   }






  // Destructors 

    ntuple->~TNtuple(); 
    n2ntuple->~TNtuple();
    maxchargentuple->~TNtuple();

}



