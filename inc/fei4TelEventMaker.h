/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#ifndef __FEI4TEL_EVENTMAKER_H__
#define __FEI4TEL_EVENTMAKER_H__

#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "TSystem.h"
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include "FormattedRecord.hh"
#include "EventMaker.h"
#include "macros.h"
#include "ANSIColors.h" 

class fei4TelEventMaker : public EventMaker
{
 public :
 
   fei4TelEventMaker(bool quiet =  false, bool readTimeStamp = false, bool design25=false)  ;
  ~fei4TelEventMaker(void		     ) {;}
  
   hitMapDef makeEvents(std::string infilename, std::string outfilename = "", int lv1diff = 1, int nevt = -1);
   void setQuiet(bool quiet) {quiet_=quiet;}
   void setReadTimeStamp(bool readTimeStamp){readTimeStamp_=readTimeStamp;}
   void setDesign25(void){design25_=true;}
   
 private:
    
   std::stringstream  ss_;  
 
   int    numHits;
   int    hitPixX[1000];
   int    hitPixY[1000];
   double hitPosX[1000];
   double hitPosY[1000];
   double hitPosZ[1000];
   int    hitValue[1000];
   int    hitTiming[1000];
   int    hitInCluster[1000];
} ;

#endif
