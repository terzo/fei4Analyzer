/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/
#include "fei4TelEventMaker.h"


fei4TelEventMaker::fei4TelEventMaker(bool quiet, bool readTimeStamp, bool design25)
{
  quiet_ = quiet;
  readTimeStamp_ = readTimeStamp;
  design25_ = design25;
  std::cout << "fei4Tel version!" << "\n";
  evn=0;
}

//====================================================================================
EventMaker::hitMapDef fei4TelEventMaker::makeEvents(std::string infilename, std::string outfilename, int lv1diff, int nevt)
{  
  gSystem->Load("libTree");
  TFile *infile = new TFile(infilename.c_str(), "READ");
  
  if(!infile->IsOpen())
  {
    std::cout << "ERROR: file " << infilename << " not found." << std::endl;
    exit(0);
  }
  
  unsigned int planeCount = 0;
  while(true)
  {
  
    std::stringstream ss;
    ss << "Plane" << planeCount;
    
    TDirectory* dir = 0;
    infile->GetObject(ss.str().c_str(), dir);
    if (!dir) 
    {
      std::cout << "Total number of planes in the file: " << planeCount << std::endl;
      break;
    }
    
    std::cout << "Found " << ss.str() << "\n";
    planeCount++;
    
    
    TTree* hits = (TTree*)infile->Get(ss.str().append("/Hits").c_str());      
    
    if (hits)
    {
      hits->SetBranchAddress("NHits"	, &numHits);
      hits->SetBranchAddress("PixX"   , hitPixX);
      hits->SetBranchAddress("PixY"   , hitPixY);
      hits->SetBranchAddress("Value"	, hitValue);
      hits->SetBranchAddress("Timing"	, hitTiming);
      //hits->SetBranchAddress("HitInCluster", &hitInCluster);
      //hits->SetBranchAddress("PosX"	, &hitPosX);
      //hits->SetBranchAddress("PosY"	, &hitPosY);
      //hits->SetBranchAddress("PosZ"	, &hitPosZ);
    }
    //std::cout << __LINE__ << "] " << numHits << "\n";
    for (unsigned int e = 0; e<hits->GetEntries(); e++) 
    {
      hits->GetEntry(e);
      
      for(unsigned int h = 0; h<numHits; h++)
      {
      	EventMaker::hitDef aHit;
      	aHit.tot = hitValue[h];
      	aHit.col = hitPixX[h];
      	aHit.row = hitPixY[h];
      	aHit.bcid= e;
      	aHit.l1id= hitTiming[h];
      
      	if(design25_) this->design25Encode(aHit);
      
      	ss_.str("");
      	ss_ << e;
      	hitMap[string_to_int(ss_.str())][planeCount].push_back(aHit);
      }
    }
    
  }
  
  infile->Close();
  delete infile;
  //std::cout << __LINE__ << "] I'm out here\n";
  return hitMap;
}

//===========================================================================================
