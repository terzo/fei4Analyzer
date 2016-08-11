/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/
#include "tbtrackEventMaker.h"


tbtrackEventMaker::tbtrackEventMaker(bool quiet, bool readTimeStamp, bool design25)
{
  quiet_ = quiet;
  readTimeStamp_ = readTimeStamp;
  design25_ = design25;
  std::cout << "tbtrack event converter for FE-I4 (no totcode!)" << "\n";
  evn = 0;
}

//====================================================================================
EventMaker::hitMapDef tbtrackEventMaker::makeEvents(std::string infilename, std::string outfilename, int lv1diff, int nevt)
{
   TFile *tbtrackfile = new TFile(infilename.c_str(), "READ"); 
    
   TTree *zspix = (TTree*) tbtrackfile->Get("zspix");
    
   int sensorID = 0;
   EventMaker::hitDef aHit;
   aHit.col = 0;
   aHit.row = 0;
   aHit.tot = 0;
   aHit.chip= 0;
   aHit.l1id= 0;
   aHit.bcid= evn;//bcid;
   if(design25_) this->design25Encode(aHit);
   hitMap[evn][sensorID].push_back(aHit);
   
   tbtrackfile->Close();
   delete tbtrackfile;
  
   return hitMap;
}

//====================================================================================
bool  tbtrackEventMaker::writeEvents(hitMapDef &hitMap, std::string outfilename, int runNum)
{ 

  return true;
}
