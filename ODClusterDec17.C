// This code reads an input file and extracts the position (x,y,z) of entries and its 
// associated charge and stores it in an ntuple.
// This code then plots on a single canvas the x,y,z position of entries,as well as the 
// distribution of charge along the x-axis, y-axis and z-axis.
//
// Further fucntionality: a COLZ plot of x:y:z:charge can be made by uncommenting the 
// relevant block of text. Can also superimpose a cylinder on the plot by uncommenting, but // this makes the plot smaller.
//
// The code is run as $ root -l -x 'ODClusterJan17.C("OD.root")' where OD.root refers to the
// location of the input file. 
// The input file right now is ("../OD.root")


#include <stdio.h>     
#include <stdlib.h>    


//****************  Creating macro and opening input file  **************************



void ODClusterDec17(char *filename=NULL) {

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
  
  
  TNtuple *ntuple = new TNtuple("ntuple","x y z charge time ntuple","pmtX:pmtY:pmtZ:charge:time");  


  // variables for clustering algorithm

  TNtuple *n2ntuple = new TNtuple("n2ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *n3ntuple = new TNtuple("n3ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *n4ntuple = new TNtuple("n4ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *n5ntuple = new TNtuple("n5ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *n6ntuple = new TNtuple("n6ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *n7ntuple = new TNtuple("n7ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *n8ntuple = new TNtuple("n8ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *n9ntuple = new TNtuple("n9ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *n10ntuple = new TNtuple("n10ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxchargentuple = new TNtuple("maxchargentuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge2ntuple = new TNtuple("maxcharge2ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge3ntuple = new TNtuple("maxcharge3ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge4ntuple = new TNtuple("maxcharge4ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge5ntuple = new TNtuple("maxcharge5ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge6ntuple = new TNtuple("maxcharge6ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge7ntuple = new TNtuple("maxcharge7ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge8ntuple = new TNtuple("maxcharge8ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge9ntuple = new TNtuple("maxcharge9ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
  TNtuple *maxcharge10ntuple = new TNtuple("maxcharge10ntuple","x y z charge ntuple","pmtX:pmtY:pmtZ:charge:time");
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





  


   //*******************  Filling ntuples  **********************************

   

   
   for(long unsigned int iEntry = 0; iEntry <=0; iEntry++){
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

   }




   

   // ******************  Clustering algorithm  *********************************


   
   cout << "Max charge event in cluster 1: " << " X: " << maxpmtX << " Y: " <<maxpmtY << " Z: " <<maxpmtZ << " Q: " <<  maxCharge << " time: " <<  maxtime << endl;
  
   Double_t max1Charge = maxCharge; // max1Charge is the value of the highest charge across all events


   //using 10% diameter = 710.2 and 10% height =  599.7  -> divided by two to get centre of my event cube -> one day maybe extract this straight from root file and use "const"
  

   
   float  pX,pY,pZ, Q, T;
   ntuple->SetBranchAddress("pmtX",&pX);
   ntuple->SetBranchAddress("pmtY",&pY);
   ntuple->SetBranchAddress("pmtZ",&pZ);
   ntuple->SetBranchAddress("charge",&Q);
   ntuple->SetBranchAddress("time",&T);


    
   for ( Long64_t ient = 0; ient < ntuple -> GetEntries(); ient++ ) {
    
     ntuple->GetEntry(ient);

 
     
      if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) ) ){

	maxchargentuple->Fill(pX,pY,pZ,Q,T);
	totalqincluster = totalqincluster+Q;

       }


     else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge) ) ){                       
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

   cout << "Total charge in cluster 1 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 1 is: " << maxchargentuple->GetEntries()<< endl;




   // **********************************  Repeating ntuple analysis on n2ntuṕle to get next highest charge and associated cluster  ********************

   


   n2ntuple->SetBranchAddress("pmtX",&pX);
   n2ntuple->SetBranchAddress("pmtY",&pY);
   n2ntuple->SetBranchAddress("pmtZ",&pZ);
   n2ntuple->SetBranchAddress("charge",&Q);
   n2ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 2: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n2ntuple -> GetEntries(); ient++ ) {
    
     n2ntuple->GetEntry(ient);

 
     
      if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) ) ){

	maxcharge2ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;

       }


     else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge) ) ){                   
    
	   continue;
       
	 }
       

     else { n3ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                        
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ; 
	     maxnexttime = T;
	     }
	 }
     
   }

   cout << "Total charge in cluster 2 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 2 is: " << maxcharge2ntuple->GetEntries()<< endl;





   // ************************  n3ntuple (third cluster)  *********************************



   n3ntuple->SetBranchAddress("pmtX",&pX);
   n3ntuple->SetBranchAddress("pmtY",&pY);
   n3ntuple->SetBranchAddress("pmtZ",&pZ);
   n3ntuple->SetBranchAddress("charge",&Q);
   n3ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 3: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n3ntuple -> GetEntries(); ient++ ) {
    
     n3ntuple->GetEntry(ient);

 
     
      if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) ) ){

	maxcharge3ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;

       }


      else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge) )){                   
    
	   continue;
       
	 }
       

      else { n4ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                        
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ; 
	     maxnexttime = T;
	     }
	 }
     
   }

   cout << "Total charge in cluster 3 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 3 is: " << maxcharge3ntuple->GetEntries()<< endl;


   // We define clusters arbitarily as having at least 3 pmt hits.This next line of code terminates the clustering algorithm after the first cluster that has
   // PMT hits less than three.This cluster still gets printed for information. 

   if ( maxcharge3ntuple->GetEntries() < 3 ){ 
     std::cout<< "Clustering algorithm terminated. Event has 2 clusters" << std::endl;
     goto endClusteringAlgorithm;
   }

 

   // **********  n4ntuple (4th cluster)  *********************



   n4ntuple->SetBranchAddress("pmtX",&pX);
   n4ntuple->SetBranchAddress("pmtY",&pY);
   n4ntuple->SetBranchAddress("pmtZ",&pZ);
   n4ntuple->SetBranchAddress("charge",&Q);
   n4ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 4: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n4ntuple -> GetEntries(); ient++ ) {
    
     n4ntuple->GetEntry(ient);

 
     
     if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) ) ){

       maxcharge4ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;

       }


     else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge) )){                   
    
	   continue;
       
	 }
       

     else { n5ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                        
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ; 
	     maxnexttime = T;
	     }
	 }
     
   }

   cout << "Total charge in cluster 4 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 4 is: " << maxcharge4ntuple->GetEntries()<< endl;

   
   if ( maxcharge4ntuple->GetEntries() < 3 ){std::cout<< "Clustering algorithm terminated. Event has 3 clusters" << std::endl; goto endClusteringAlgorithm; }


   // ********* n5ntuple (5th cluster)  *********************************



   n5ntuple->SetBranchAddress("pmtX",&pX);
   n5ntuple->SetBranchAddress("pmtY",&pY);
   n5ntuple->SetBranchAddress("pmtZ",&pZ);
   n5ntuple->SetBranchAddress("charge",&Q);
   n5ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 5: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n5ntuple -> GetEntries(); ient++ ) {
    
     n5ntuple->GetEntry(ient);

 
     
      if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) )  ){

	maxcharge5ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;
	 
       }


      else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge)) ){                   
    
	   continue;
       
	 }
       

      else { n6ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                        
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ; 
	     maxnexttime = T;
	     }
	 }
     
   }

   cout << "Total charge in cluster 5 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 5 is: " << maxcharge5ntuple->GetEntries()<< endl;

   if ( maxcharge5ntuple->GetEntries() < 3 ){ std::cout<< "Clustering algorithm terminated. Event has 4 clusters" << std::endl; goto endClusteringAlgorithm; }

   // ******* n6ntuple (6th cluster)  *****************



   n6ntuple->SetBranchAddress("pmtX",&pX);
   n6ntuple->SetBranchAddress("pmtY",&pY);
   n6ntuple->SetBranchAddress("pmtZ",&pZ);
   n6ntuple->SetBranchAddress("charge",&Q);
   n6ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 6: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n6ntuple -> GetEntries(); ient++ ) {
    
     n6ntuple->GetEntry(ient);

 
     
      if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) ) ){

	maxcharge6ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;

       }


     else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge) )){                   
    
	   continue;
       
	 }
       

     else { n7ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                       
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ; 
	     maxnexttime = T;
	     }
	 }
     
   }

   cout << "Total charge in cluster 6 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 6 is: " << maxcharge6ntuple->GetEntries()<< endl;

   if ( maxcharge6ntuple->GetEntries() < 3 ){ std::cout<< "Clustering algorithm terminated. Event has 5 clusters" << std::endl; goto endClusteringAlgorithm; }


   // ************  n7ntuple (7th cluster)  **********************

   n7ntuple->SetBranchAddress("pmtX",&pX);
   n7ntuple->SetBranchAddress("pmtY",&pY);
   n7ntuple->SetBranchAddress("pmtZ",&pZ);
   n7ntuple->SetBranchAddress("charge",&Q);
   n7ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 7: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n7ntuple -> GetEntries(); ient++ ) {
    
     n7ntuple->GetEntry(ient);

 
     
     if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) )){

       maxcharge7ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;

       }


     else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge) ) ){                   
    
	   continue;
       
	 }
       

     else { n8ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                      
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ; 
	     maxnexttime = T;
	     }
	 }
     
   }

   cout << "Total charge in cluster 7 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 7 is: " << maxcharge7ntuple->GetEntries()<< endl;

   if ( maxcharge7ntuple->GetEntries() < 3 ){ std::cout<< "Clustering algorithm terminated. Event has 6 clusters" << std::endl; goto endClusteringAlgorithm; }



   // ***********  n8ntuple (8th cluster)  ************************

   n8ntuple->SetBranchAddress("pmtX",&pX);
   n8ntuple->SetBranchAddress("pmtY",&pY);
   n8ntuple->SetBranchAddress("pmtZ",&pZ);
   n8ntuple->SetBranchAddress("charge",&Q);
   n8ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 8: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n8ntuple -> GetEntries(); ient++ ) {
    
     n8ntuple->GetEntry(ient);

 
     
      if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) )  ){

	maxcharge8ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;

       }


     else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge) )){                   
    
	   continue;
       
	 }
       

     else { n9ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                        
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ;
	     maxnexttime = T; 
	     }
	 }
     
   }

   cout << "Total charge in cluster 8 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 8 is: " << maxcharge8ntuple->GetEntries()<< endl;

   if ( maxcharge8ntuple->GetEntries() < 3 ){ std::cout<< "Clustering algorithm terminated. Event has 7 clusters" << std::endl; goto endClusteringAlgorithm; }



   // *************  n9ntuple (9th cluster)  ****************


   n9ntuple->SetBranchAddress("pmtX",&pX);
   n9ntuple->SetBranchAddress("pmtY",&pY);
   n9ntuple->SetBranchAddress("pmtZ",&pZ);
   n9ntuple->SetBranchAddress("charge",&Q);
   n9ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 9: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n9ntuple -> GetEntries(); ient++ ) {
    
     n9ntuple->GetEntry(ient);

 
     
      if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) ) ){

	maxcharge9ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;

       }


     else if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q<(0.05*max1Charge) ) ){                   
    
	   continue;
       
	 }
       

     else { n10ntuple->Fill(pX,pY,pZ,Q,T); 
	   if (Q>maxnextCharge){                        
	     maxnextCharge = Q;
	     maxnextpmtX = pX;
	     maxnextpmtY = pY;
	     maxnextpmtZ = pZ; 
	     maxnexttime = T;
	     }
	 }
     
   }

   cout << "Total charge in cluster 9 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 9 is: " << maxcharge9ntuple->GetEntries()<< endl;


   if ( maxcharge9ntuple->GetEntries() < 3 ){ std::cout<< "Clustering algorithm terminated. Event has 8 clusters" << std::endl; goto endClusteringAlgorithm; }


   // ********* n10ntuple (10th cluster)  *****************

   n10ntuple->SetBranchAddress("pmtX",&pX);
   n10ntuple->SetBranchAddress("pmtY",&pY);
   n10ntuple->SetBranchAddress("pmtZ",&pZ);
   n10ntuple->SetBranchAddress("charge",&Q);
   n10ntuple->SetBranchAddress("time",&T);
   

   cout << "Max charge event in cluster 10: " << " X: " << maxnextpmtX << " Y: " <<maxnextpmtY << " Z: " <<maxnextpmtZ << " Q: " <<  maxnextCharge << " time: " <<  maxnexttime << endl;

   // Resetting these values for next analysis
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



   for ( Long64_t ient = 0; ient < n10ntuple -> GetEntries(); ient++ ) {
    
     n10ntuple->GetEntry(ient);

 
     
      if ((pX>maxpmtX-355.1) && (pX<maxpmtX+355.1) && (pY>maxpmtY-355.1) && (pY<maxpmtY+355.1) && (pZ>maxpmtZ-299.9) && (pZ<maxpmtZ+299.9 ) && (Q>(0.05*max1Charge) ) ){

	maxcharge10ntuple->Fill(pX,pY,pZ,Q,T);
	 totalqincluster = totalqincluster+Q;

       }


     else {                   
    
	   continue;
       
	 }
     
   }

   cout << "Total charge in cluster 10 is " << totalqincluster << endl;
   cout << "Number of PMT hits in cluster 10 is: " << maxcharge10ntuple->GetEntries()<< endl;

   
   if ( maxcharge10ntuple->GetEntries() < 3 ){ std::cout<< "Clustering algorithm terminated. Event has 9 clusters" << std::endl; goto endClusteringAlgorithm; }
   else { std::cout<< "Clustering algorithm terminated. Event has more than 10 clusters. More loops needed" << std::endl; }



 endClusteringAlgorithm:    //*******  if clusters less than 3 pmt hits, clustering algorithm terminates and gets sent here.   *********





   /*  ****** check if the data entries are correct */
   
   /* 
   ntuple->Print();
   n10ntuple ->Print();
   maxcharge2ntuple->Print();
   maxcharge3ntuple->Print();
   maxcharge4ntuple->Print();
   maxcharge5ntuple->Print();
   maxcharge6ntuple->Print();
   maxcharge7ntuple->Print();
   maxcharge8ntuple->Print();
   maxcharge9ntuple->Print();
     
   */





   //******************  Drawing histogram  *********************************
  
   



   TCanvas *c1;

   c1 = new TCanvas("ntuple","ntuple", 800,600);



   // Drawing a cylinder shape with the same dimensions as Hyper K
  
   TGeoTube *myCy1 = new TGeoTube(0,3551,2798.5);        
   myCy1->Draw();
   
   
   
   
   maxchargentuple->SetMarkerStyle(21);
   maxchargentuple->SetMarkerColor(2);

   maxchargentuple->Draw("pmtZ:pmtX:pmtY","","same");


   maxcharge2ntuple->SetMarkerStyle(21);
   maxcharge2ntuple->SetMarkerColor(4);

   maxcharge2ntuple->Draw("pmtZ:pmtX:pmtY","", "same");


   
   maxcharge3ntuple->SetMarkerStyle(21);
   maxcharge3ntuple->SetMarkerColor(6);

   maxcharge3ntuple->Draw("pmtZ:pmtX:pmtY","", "same");
   

   maxcharge4ntuple->SetMarkerStyle(21);
   maxcharge4ntuple->SetMarkerColor(6);

   maxcharge4ntuple->Draw("pmtZ:pmtX:pmtY","", "same");
   

   

   // Destructors 

    ntuple->~TNtuple(); 
    n2ntuple->~TNtuple();
    n3ntuple->~TNtuple();
    n4ntuple->~TNtuple();
    n5ntuple->~TNtuple();
    n6ntuple->~TNtuple();
    n7ntuple->~TNtuple();
    n8ntuple->~TNtuple();
    n9ntuple->~TNtuple();
    n10ntuple->~TNtuple();
    maxchargentuple->~TNtuple();
    maxcharge2ntuple->~TNtuple();
    maxcharge3ntuple->~TNtuple();
    maxcharge4ntuple->~TNtuple();
    maxcharge5ntuple->~TNtuple();
    maxcharge6ntuple->~TNtuple();
    maxcharge7ntuple->~TNtuple();
    maxcharge8ntuple->~TNtuple();
    maxcharge9ntuple->~TNtuple();
    maxcharge10ntuple->~TNtuple();


    


   

}
