#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <omp.h>
#include <boost/regex.hpp>
#include <boost/cregex.hpp>

#include "EventMaker.h"
#include "fei4TelEventMaker.h"
#include "USBpixEventMaker.h"
#include "CosmicEventMaker.h"
#include "Clusterizer.h"
#include "Plotter.h"
#include "TSystem.h"

#ifdef USE_LCIO
 #include "LCIOEventMaker.h"
#endif

template <typename N, typename S>
void string_to_number(const S theString, N &n)
{
  std::istringstream data_s;
  data_s.str(theString)	  ;
  data_s >> n		  ;
}

int main(int argc, char **argv)
{
  int skipcount=0;
  std::vector<std::string> infilename, rootfilename;
  //std::string calibname = "calib.root";
  bool calibname = false;
  std::string outfilename;
  bool merge = false;
  int maxMerge = 1;
  std::string option;
  int module_type = 0;
  bool readTimeStamp = false;
  bool quiet = true;
  bool bunch = false;
  bool borders = false;
  unsigned int  lv1diff = 3;
  unsigned int  cdRow = 1;
  unsigned int  cdCol = 1;
  std::vector<double> correction_factors;
  std::map<unsigned int,std::vector<int> > colRowFrameCuts;
  int colRowCluCuts[4] = {0,0,0,0};
  double noise = -1;
  unsigned int fitFunction = 1;
  int maxevents = -1;
  unsigned int dofit = 1;
  bool design25=false;
  std::map<unsigned int,std::pair<unsigned int,unsigned int> > hitsRef;
  
  gSystem->Load("libTree");
  
  if (argc > 1) 
  {
     for (int i=1; i<argc; i++) 
     {
       option = argv[i];
       if (option[0] != '-') break;
       switch (option[1]) 
       {
         case 'h':
	 {
	   return 0;
	 }
  	 case 'i': 
  	 {
	   option = argv[i+1];
	   while( option[0]!='-' && i<(argc-1))
	   {
  	      infilename.push_back( std::string(argv[++i]) );
	      if(i<(argc-1)) option = argv[i+1];
	   }
  	   break;
  	 }
  	 default: 
  	 {
  	   break;
  	 }
       }
     }
     
     for(unsigned int i=0; i<infilename.size(); ++i)
     {
     	     EventMaker *theEventMaker;
     	     	  
     	     std::string extension = infilename[i].substr(infilename[i].find_last_of(".") + 1); 
 
     	     if ( extension == "dat")
     	     		theEventMaker = new CosmicEventMaker(quiet, readTimeStamp, design25);
     	     else if(extension == "slcio")
     	     {
     	     		#ifdef USE_LCIO
     	     		theEventMaker = new LCIOEventMaker(quiet, readTimeStamp, design25);
	     		#else
	     		std::cout << "ERROR: variable USE_LCIO not defined" << std::endl;
	     		exit(0);
	     		#endif
     	     }
     	     else if(extension == "root")
     	     { 
     	     		if(infilename.size() > 1)
	     		{
	     			std::cout << "Multi file parallel processing not allowd with TFiles\n";
	     			continue;
	     		}
     	     		theEventMaker = new fei4TelEventMaker(quiet, readTimeStamp, design25);
     	     }
     	     else      
     	     		theEventMaker = new USBpixEventMaker(quiet, readTimeStamp, design25);
     	     	
     	     //if(design25) theEventMaker->setDesign25();
     	     std::cout << "Reading file: " << infilename[i] << std::endl;
     
     	     EventMaker::hitMapDef hitMap = theEventMaker->makeEvents(infilename[i], "", lv1diff, maxevents);
     
     	     delete theEventMaker;
     
     	     EventMaker *theConverter;
	     
	     extension = infilename[i].substr(infilename[i].find_last_of(".") + 1);
     	     
	     if ( extension == "dat")
	   		theConverter = new CosmicEventMaker(quiet, readTimeStamp, design25);
	     else if(extension == "slcio")
	     {
	     		#ifdef USE_LCIO
	     		theConverter = new LCIOEventMaker(quiet, readTimeStamp, design25);
	     		#else
	     		std::cout << "ERROR: variable USE_LCIO not defined" << std::endl;
	     		exit(0);
	     		#endif
	     }
	     else if(extension == "root")
	     { 
	     		if(infilename.size() > 1)
	     		{
	     			std::cout << "Multi file parallel processing not allowd with TFiles\n";
	     			continue;
	     		}
	     		theConverter = new fei4TelEventMaker(quiet, readTimeStamp, design25);
	     }
	     else      
	     		theConverter = new USBpixEventMaker(quiet, readTimeStamp, design25);
	     	
	     std::cout << "Writing file: " << outfilename << std::endl;

	     theConverter->writeEvents(hitMap, outfilename);

	     delete theConverter;
     }
  }
}
