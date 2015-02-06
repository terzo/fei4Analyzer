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
}

//====================================================================================
EventMaker::hitMapDef fei4TelEventMaker::makeEvents(std::string infilename, std::string outfilename, int lv1diff, int nevt)
{
  EventMaker::hitMapDef hitMap;
/*
  Tfile *file = new TFile(infilename, "READ");

  unsigned int planeCount = 0;
  while (true)
  {
    std::stringstream ss;
    ss << "Plane" << planeCount;

    // Try to get this plane's directory
    TDirectory* dir = 0;
    file->GetObject(ss.str().c_str(), dir);
    if (!dir) break;

    planeCount++;

    TTree* hits;
    TTree* eventInfo;
    //TTree* clusters;

    file->GetObject(ss.str().append("/Hits").c_str(), hits);
    //file->GetObject(ss.str().append("/Clusters").c_str(), clusters);
    numPlanes++;

    if(hits)
    {
      hits->SetBranchAddress("NHits", &numHits, &bNumHits);
      hits->SetBranchAddress("PixX", hitPixX, &bHitPixX);
      hits->SetBranchAddress("PixY", hitPixY, &bHitPixY);
      hits->SetBranchAddress("Value", hitValue, &bHitValue);
      hits->SetBranchAddress("Timing", hitTiming, &bHitTiming);
      hits->SetBranchAddress("HitInCluster", hitInCluster, &bHitInCluster);
      hits->SetBranchAddress("PosX", hitPosX, &bHitPosX);
      hits->SetBranchAddress("PosY", hitPosY, &bHitPosY);
      hits->SetBranchAddress("PosZ", hitPosZ, &bHitPosZ);
    }
  }

  file->GetObject("Event", _eventInfo);

  if (_eventInfo)
  {
    eventInfo->SetBranchAddress("TimeStamp", &timeStamp, &bTimeStamp);
    eventInfo->SetBranchAddress("FrameNumber", &frameNumber, &bFrameNumber);
    eventInfo->SetBranchAddress("TriggerOffset", &triggerOffset, &bTriggerOffset);
    eventInfo->SetBranchAddress("TriggerInfo", &triggerInfo, &bTriggerInfo);
    eventInfo->SetBranchAddress("Invalid", &invalid, &bInvalid);
  }
*/
  std::cout << "ERROR: " << "cannot read " << infilename << ", file type not supported yet\n";
  return hitMap;
}

