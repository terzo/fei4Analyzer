/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

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
#include "USBpixEventMaker.h"
#include "CosmicEventMaker.h"
#include "Clusterizer.h"
#include "Plotter.h"

template <typename N, typename S>
void string_to_number(const S theString, N &n)
{
  std::istringstream data_s;
  data_s.str(theString)	  ;
  data_s >> n		  ;
}

int main(int argc, char **argv)
{
//  int skipcount=0;
  std::vector<std::string> infilename, rootfilename;
  std::string outfilename;
  bool merge = false;
  int maxMerge = 1;
  std::string option;
  int module_type = 0;
  bool readTimeStamp = false;
  bool quiet = true;
  bool bunch = false;
  int  lv1diff = 3;
  std::vector<double> correction_factors;
  int colRowCuts[4] = {-1,-1,-1,-1};
  double noise = -1;
  unsigned int fitFunction = 1;
  int maxevents = -1;
  bool dofit = true;
  bool design25=false;
  
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
	   std::cout << "-i filename.[raw,dat]\t\t:" << "input file[s]: .raw for USBpix files and .dat for CosmicGui binary files" << "\n";
	   std::cout << "-v\t \t \t \t:" << "verbosity: debug mode" << "\n";
	   std::cout << "-r filename[.root]\t\t:" << "define the ROOT output file name[s]: if not set or #input<#output the output_filename[s] = input_filename[s].root" << "\n";
	   std::cout << "-o filename[.txt]\t\t:" << "writes out a txt log file compatible with the Timepix reconstruction software (CosmicGUI binary data only)" << "\n";
	   std::cout << "-n [0..1]\t\t\t:" << "specify the hit frequency for noise suppression (no noise suppression by default)" << "\n";
	   std::cout << "-g [0..9999]\t\t\t:" << "correct the charge in the temporary output plots by the specified factor" << "\n";
	   std::cout << "-0\t \t \t \t:" << "doen't perform langaus fits (faster)" << "\n";
	   std::cout << "-x [4,25]\t\t\t:" << "define module type: 4 = quad module; 25 = 500x50um FE-I4 pitch arrangement" << "\n";
	   std::cout << "-t\t \t \t \t:" << "read timestaps (for ComsicGUI test beam applications only)" << "\n";
	   std::cout << "-f [1,2]\t\t\t:" << "select the fit function for ToT plots: 1->landau MP (default); 2->langaus MPV;" << "\n";
	   std::cout << "-m [0..inf]\t\t\t:" << "merge consecutive triggers" << "\n";
	   std::cout << "-l [0..16]\t\t\t:" << "maximum lvl1 difference for clustering" << "\n";
	   std::cout << "-c minCol minRow maxCol maxRow\t:" << "define square cuts in pixel coordinates for the analysis" << "\n";
	   std::cout << "-e [0..inf]\t\t\t:" << "maximum number of event to process (default process all events)" << "\n";
	   std::cout << "-s [0..inf]\t\t\t:" << "skip the first n events (NOT IMPLEMENTED)" << "\n";
	   std::cout << "-b [0..inf]\t\t\t:" << "I forgot what was that intended for... (NOT IMPLEMENTED)" << "\n";
	   std::cout <<  std::endl;
	   std::cout << "STControl quad module analysis example:" << std::endl;
	   std::cout << "\tfei4Analyzer -x 4 -r root_outputfile.root -i stcontrol_rawfile.raw -l 2" << std::endl;	   
	   std::cout << "CosmicGUI SCM analysis example:" << std::endl;
	   std::cout << "\tfei4Analyzer -t -r root_outputfile.root -i cosmic_rawfile.dat -l 2" << std::endl;
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
	 case 'b': 
  	 {
	   bunch = true;
           rootfilename.push_back( std::string(argv[++i]) );
	   break;
  	 }	 
  	 case 'v': 
  	 {
  	   quiet = false;
  	   break;
  	 }
  	 case 'r': 
  	 {
	   option = argv[i+1];
	   while( i<(argc-2) && option[0]!='-')
	   {
  	      rootfilename.push_back( std::string(argv[++i]) );
	      option = argv[i+1];
	   }
  	   break;
  	 } 
  	 case 'o': 
  	 {
  	   outfilename = argv[++i];
  	   break;
  	 }
  	 case 'x': 
  	 {
  	   string_to_number(argv[++i], module_type);
	   if (module_type == 25) design25 = true;
  	   break;
  	 } 
	 case 'n': 
  	 {
  	   string_to_number(argv[++i], noise);
  	   break;
  	 }
	 case 'g': 
  	 {
	   option = argv[i+1];
	   while( i<(argc-2) && option[0]!='-')
	   {
	       double correction_factor = 1;
  	       string_to_number(argv[++i],correction_factor);
	       correction_factors.push_back(correction_factor);
	       option = argv[i+1];
	   }
  	   break;
  	 }
  	 //case 's': 
  	 //{
  	 //  skipcount = atoi(option.substr(2).c_str());
  	 //  break;
  	 //}
	 case '0': 
  	 {
  	   dofit = false;
  	   break;
  	 } 
	 case 'e': 
  	 {
  	   string_to_number(argv[++i], maxevents);
  	   break;
  	 }
  	 case 't': 
  	 {
  	   readTimeStamp = true;
  	   break;
  	 }
	 case 'f': 
  	 {
  	   string_to_number(argv[++i],fitFunction);
  	   break;
  	 }
	 case 'm':
	 {
	   merge=true;
	   string_to_number(argv[++i],maxMerge);
	   break;
	 }
	 case 'l':
	 {
	   string_to_number(argv[++i],lv1diff);
	   break;
	 }	 
	 case 'c':
	 {
	   option = argv[i+1];
	   unsigned int j = 0;
	   while( j<4 && i<(argc-2) && option[0]!='-')
	   {
	       int limit = -1;
  	       string_to_number(argv[++i],limit);
	       colRowCuts[j] = limit;
	       option = argv[i+1];
	       j++;
	   }
  	   break;
	 }
  	 default: 
  	 {
  	   break;
  	 }
       }
     }
  }
  
  
  Plotter *thePlotter = new Plotter(quiet, module_type);
  #pragma omp parallel for
  for(unsigned int i=0; i<infilename.size(); ++i)
  {
     EventMaker *theEventMaker;
     
     double voltage = 0;
     
     boost::regex expression("(\\d+)V");
     boost::match_results<std::string::const_iterator> what;
     if(regex_search(infilename[i], what, expression, boost::match_default)) string_to_number(what[1], voltage);
          
     std::string extension = infilename[i].substr(infilename[i].find_last_of(".") + 1); 
     if ( extension == "dat")
     		theEventMaker = new CosmicEventMaker(quiet, readTimeStamp, design25); 
     else      
     		theEventMaker = new USBpixEventMaker(quiet, readTimeStamp, design25);
     	
     //if(design25) theEventMaker->setDesign25();
     EventMaker::hitMapDef hitMap = theEventMaker->makeEvents(infilename[i], outfilename, lv1diff, maxevents);
  
     delete theEventMaker;
  
     Clusterizer *theClusterizer = new Clusterizer();
     Clusterizer::clusterMapDef clusterMap = theClusterizer->makeCluster(hitMap, lv1diff);
     if(merge)theClusterizer->mergeLv1(clusterMap, maxMerge);
     
     delete theClusterizer;

     #pragma omp critical
     {
        thePlotter->setCuts(colRowCuts);
	thePlotter->fillHitPlots(hitMap);
     	thePlotter->fillClusterPlots(clusterMap, noise);
     	
     	//#pragma omp critical
     	if(dofit) thePlotter->fitPlots(voltage);
      
     	if( rootfilename.size() < infilename.size() ) 
     	{ 
     	    rootfilename.push_back( infilename[i].substr(0, infilename[i].substr( infilename[i].find_last_of("/\\")+1 ).find_last_of(".") ) + std::string(".root") );
     	    std::cout << "Write out file: " << rootfilename.back() << "\n";
     	}
     	thePlotter->writePlots(rootfilename.back(),bunch);
      }
   } 
//  }
  
  if(dofit) thePlotter->showGraph(correction_factors, fitFunction);
  delete thePlotter;
}
