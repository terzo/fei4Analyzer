/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#ifndef __TBTRACK_EVENTMAKER_H__
#define __TBTRACK_EVENTMAKER_H__

#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

#include <TFile.h>
#include <TH2F.h>
#include <TTree.h>
#include <TROOT.h>
#include <TParameter.h>

#include <TString.h>

#include "FormattedRecord.hh"
#include "EventMaker.h"
#include "macros.h"
#include "ANSIColors.h" 

class tbtrackEventMaker : public EventMaker
{
 public :
 
   tbtrackEventMaker(bool quiet =  false, bool readTimeStamp = false, bool design25=false)  ;
  ~tbtrackEventMaker(void                    ) {;}
  
   hitMapDef makeEvents(std::string infilename, std::string outfilename = "", int lv1diff = 1, int nevt = -1);
   bool writeEvents(hitMapDef &hitMap, std::string outfilename, int runNum);
   void setQuiet(bool quiet) {quiet_=quiet;}
   void setReadTimeStamp(bool readTimeStamp){readTimeStamp_=readTimeStamp;}
   void setDesign25(void){design25_=true;}
   
 private:
    
   std::stringstream  ss_;
   //EventMaker::hitMapDef hitMap;
   //int evn;
 
} ;

#endif
