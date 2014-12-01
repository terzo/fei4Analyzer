#ifndef __COSMIC_EVENTMAKER_H__
#define __COSMIC_EVENTMAKER_H__


#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "FormattedRecord.hh"
#include "EventMaker.h"
#include "macros.h"
#include "ANSIColors.h" 

class CosmicEventMaker : public EventMaker
{
 public :
 
   CosmicEventMaker(bool quiet =  false, bool readTimeStamp = false)  ;
  ~CosmicEventMaker(void                    ) {;}
  
   hitMapDef makeEvents(std::string infilename, std::string outfilename = "", int lv1diff = 1, int nevt = -1);
   void setQuiet(bool quiet) {quiet_=quiet;}
   void setReadTimeStamp(bool readTimeStamp){readTimeStamp_=readTimeStamp;}
   
 private:
   
   unsigned getWord(char *p, int word);
   unsigned nextcurrent(char* p, int& indx);
   bool     triggerDecode(char* block, unsigned long long &counter, unsigned long long &trgtime, unsigned long long &deadtime, int &bitpos);
 
   std::stringstream  ss_;
   bool quiet_;
   
   bool readTimeStamp_   ;
 
} ;

#endif
