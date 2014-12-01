#ifndef __USBPIX_EVENTMAKER_H__
#define __USBPIX_EVENTMAKER_H__


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

class USBpixEventMaker : public EventMaker
{
 public :
 
   USBpixEventMaker(bool quiet =  false, bool readTimeStamp = false)  ;
  ~USBpixEventMaker(void                    ) {;}
  
   hitMapDef makeEvents(std::string infilename, std::string outfilename = "", int lv1diff = 1, int nevt = -1);
   void setQuiet(bool quiet) {quiet_=quiet;}
   void setReadTimeStamp(bool readTimeStamp){readTimeStamp_=readTimeStamp;}
   
 private:
    
   std::stringstream  ss_;
   bool quiet_;
   
   bool readTimeStamp_   ;
 
} ;

#endif
