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
#include "fei4TelEventMaker.h"
#include "USBpixEventMaker.h"
#include "CosmicEventMaker.h"
#include "tbtrackEventMaker.h"
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
    data_s.str(theString);
    data_s >> n;
}

int main(int argc, char **argv)
{
    int skipcount=0;
    std::vector<std::string> infilename, rootfilename;
    //std::string use_charge_calibration = "calib.root";
    bool use_charge_calibration = false;
    std::string outfilename;
    bool merge = false;
    int maxMerge = 1;
    std::string option;
    int module_type = 0;
    bool readTimeStamp = false;
    std::pair<int,int> rowCol_size = std::make_pair(336,80);
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
    bool save_cluster_data=false;
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
                    std::cout << "-i  filename.[raw,dat,slcio]\t:" << "input file[s]: .raw   for USBpix files" << "\n" 
                    << "\t\t\t\t "                       << "               .dat   for CosmicGui binary files" << "\n"
                    << "\t\t\t\t "                       << "               .slcio for EUTelescope lcio files" << "\n";
                    std::cout << "-v\t \t \t \t:" << "verbose (debug mode)" << "\n";
                    std::cout << "-r  filename[.root]\t\t:" << "define the ROOT output file name[s]: if not set or #input<#output the output_filename[s] = input_filename[s].root" << "\n";
                    std::cout << "-o  filename[.txt]\t\t:" << "writes out a txt log file compatible with the Timepix reconstruction software (CosmicGUI binary data only)" << "\n";
                    std::cout << "-n  [0..1]\t\t\t:" << "specify the hit frequency for noise suppression (no noise suppression by default)" << "\n";
                    std::cout << "-g  [0..inf]\t\t\t:" << "correct the charge in the temporary output plots by the specified factor" << "\n";
                    std::cout << "-m  [4,25,50]\t\t\t:" << "define module type:  4-> quad module, \n"
                                              << "\t\t\t\t "         << "                    25-> 500x25um FE-I4 pitch arrangement\n"
                                              << "\t\t\t\t "         << "                    50-> 50x50um RD53A pitch arrangement (same as -S 192 400)"
                                              << "\n";
                    std::cout << "-S [rows cols]\t\t\t:" << "specify the number of rows and columns of the sensor (does overrides option -m 50)\n";
                    std::cout << "-t\t \t \t \t:" << "read timestaps (for ComsicGUI test beam applications only)" << "\n";
                    std::cout << "-f  [0..2]\t\t\t:"  << "0-> doesn't perform langaus fits (faster), " << "\n"
                    << "\t\t\t\t "          << "1-> perform langaus fits for ToT plots only (default), " << "\n"
                    << "\t\t\t\t "          << "2-> perform langaus fits for all plots " << "\n";
                    std::cout << "-0\t \t \t \t:" << "doesn't perform langaus fits (same as -f 0)" << "\n";
                    std::cout << "-ff [1,2]\t\t\t:" << "select the returned value for ToT plots: 1-> landau MP (default), \n"
                    << "\t\t\t\t "        << "                                         2-> langaus MPV;" << "\n";
                    std::cout << "-dr [1..inf]\t\t\t:" << "max row distance in a cluster (default is 1)" << "\n";
                    std::cout << "-dc [1..inf]\t\t\t:" << "max column distance in a cluster (default is 1)" << "\n";
                    std::cout << "-wr min max\t\t\t:" << "min and max cluster width row (default is all)" << "\n";
                    std::cout << "-wc min max\t\t\t:" << "min and max cluster width columns (default is all)" << "\n";
                    std::cout << "-l  [0..16]\t\t\t:" << "maximum lvl1 difference for clustering (default is 3)" << "\n";
                    std::cout << "-lm [0..inf]\t\t\t:" << "merge consecutive triggers" << "\n";
                    std::cout << "-c  id minCol minRow maxCol maxRow\t:" << "define square cuts in pixel coordinates for the analysis of detctor id" << "\n";
                    std::cout << "-cb id minCol minRow maxCol maxRow\t:" << "invert the logic of the square cuts to select the borders only" << "\n";
                    std::cout << "-e  [0..inf]\t\t\t:" << "maximum number of event to process (default process all events)" << "\n";
                    std::cout << "-k\t \t \t \t:" << "enable calibration (needs A.root, B.root, C.root)" << "\n";
                    std::cout << "-s  [0..inf]\t\t\t:" << "skip the first n events (NOT IMPLEMENTED)" << "\n";
                    std::cout << "-b  filename[.root]\t\t:" << "merge the input files and perform a single analysis for the full bunch, result are saved in 'filename.root'" << "\n";
                    std::cout << "-dh id min max\t\t\t:" << "min and max number of clusters on the detector 'id' to accept an event" << "\n";
                    std::cout << "-scd\t \t \t \t:" << "save cluster data in a ROOT TTree" << "\n";
                    std::cout <<  std::endl;
                    std::cout << "STControl quad module analysis example:" << std::endl;
                    std::cout << "\tfei4Analyzer -m 4 -r root_outputfile.root -i stcontrol_rawfile.raw -l 2" << std::endl;      
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
                    while( i<(argc-1) && option[0]!='-')
                    {
                        rootfilename.push_back( std::string(argv[++i]) );
                        if(i<(argc-1)) option = argv[i+1];
                    }
                    break;
                } 
                case 'o': 
                {
                    outfilename = argv[++i];
                    break;
                }
                case 'm': 
                {
                    string_to_number(argv[++i], module_type);
                    if (module_type == 25) design25 = true;
                    else if(module_type == 50) std::cout << "RD53A support is experimental!\n";
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
                    while( i<(argc-1) && option[0]!='-')
                    {
                        double correction_factor = 1;
                        string_to_number(argv[++i],correction_factor);
                        correction_factors.push_back(correction_factor);
                        if(i<(argc-1)) option = argv[i+1];
                    }
                    break;
                }
                case 's': 
                {
                    if(option[2] == 'c')
                    {
                        if(option[3] == 'd')
                        {
                            save_cluster_data=true;
                            std::cout << "Saving cluster data." << std::endl;
                            break;
                        }
                    }
                    string_to_number(argv[++i], skipcount);
                    break;
                }
                case 'S':
                {
                    string_to_number(argv[++i], rowCol_size.first);
                    string_to_number(argv[++i], rowCol_size.second);
                    break;
                }
                case 'd': 
                {
                    if(option[2] == 'c') string_to_number(argv[++i], cdCol);
                    if(option[2] == 'r') string_to_number(argv[++i], cdRow);
                    if(option[2] == 'h')
                    {
                        option = argv[i+1];
                        unsigned int j = 0;
                        std::vector<int> vec;
                        while( j<3 && i<(argc-1) && option[0]!='-')
                        {
                            int val = 0;
                            string_to_number(argv[++i],val);
                            vec.push_back(val);
                            if(i<(argc-1)) option = argv[i+1];
                            j++;
                        }
                        if(vec.size() == 3) hitsRef[vec[0]] = std::make_pair(vec[1],vec[2]);
                        break;
                    }
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
                    if(option[2] == 'f') string_to_number(argv[++i],fitFunction);
                    else                 string_to_number(argv[++i],dofit);
                    break;
                }
                case '0': 
                {
                    dofit = 0;
                    break;
                }
                case 'l':
                { 
                    if(option[2] == 'm')
                    {
                        merge=true;
                        string_to_number(argv[++i],maxMerge);

                    }
                    else 
                        string_to_number(argv[++i],lv1diff);
                    break;
                }   
                case 'c':
                {
                    if(option[2] == 'b') borders=true;
                    option = argv[i+1];
                    std::vector<int> vec;       
                    unsigned int j = 0;
                    while( j<5 && i<(argc-1) && option[0]!='-')
                    {
                        int limit = 0;
                        string_to_number(argv[++i],limit);
                        vec.push_back(limit);
                        if(i<(argc-1)) option = argv[i+1];
                        j++;
                    }
                    if(vec.size() == 5) colRowFrameCuts[vec[0]] = std::vector<int>(vec.begin()+1, vec.end());
                    break;
                }
                case 'w':
                {
                    if(option[2] == 'c')
                    {
                        option = argv[i+1];
                        unsigned int j = 0;
                        while( j<3 && i<(argc-1) && option[0]!='-')
                        {
                            int limit = 0;
                            string_to_number(argv[++i],limit);
                            colRowCluCuts[j] = limit;
                            if(i<(argc-1)) option = argv[i+1];
                            j = j+2;
                        }
                        break;
                    }
                    if(option[2] == 'r')
                    {
                        option = argv[i+1];
                        unsigned int j = 1;
                        while( j<4 && i<(argc-1) && option[0]!='-')
                        {
                            int limit = 0;
                            string_to_number(argv[++i],limit);
                            colRowCluCuts[j] = limit;
                            if(i<(argc-1)) option = argv[i+1];
                            j = j+2;
                        }
                        break;
                    }
                    break;
                }
                case 'k':
                {
                    /*
                    option = argv[i+1];
                    while( option[0]!='-' && i<(argc-1))
                    {
                    use_charge_calibration.push_back( std::string(argv[++i]) );
                    if(i<(argc-1)) option = argv[i+1];
                    }
                    break;
                    */
                    //use_charge_calibration = std::string(argv[++i]);
                    use_charge_calibration = true;
                    break;
                }
                default: 
                {
                    break;
                }
            }
        }
    }

    Plotter *thePlotter = new Plotter(quiet);
    thePlotter->useChargeCalibration(use_charge_calibration);
    thePlotter->saveClusterData(save_cluster_data);
    thePlotter->setModuleType(module_type);
    thePlotter->setNRowCols(rowCol_size.first,rowCol_size.second);

    #pragma omp parallel for if(!bunch)
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

        EventMaker::hitMapDef hitMap = theEventMaker->makeEvents(infilename[i], outfilename, lv1diff, maxevents);

        delete theEventMaker;

        Clusterizer *theClusterizer = new Clusterizer();
        Clusterizer::clusterMapDef clusterMap = theClusterizer->makeCluster(hitMap, cdCol, cdRow, lv1diff);
        if(merge)theClusterizer->mergeLv1(clusterMap, maxMerge);

        delete theClusterizer;

        #pragma omp critical(plotter)
        {
            thePlotter->setFrameCuts(colRowFrameCuts,borders);
            thePlotter->setClusterCuts(colRowCluCuts);
            thePlotter->setRefDutHitLimit(hitsRef);

            thePlotter->fillHitPlots(hitMap);
            thePlotter->fillClusterPlots(clusterMap, noise);

            //#pragma omp critical
            if(dofit!=0) thePlotter->fitPlots(voltage, dofit);

            if(!bunch)
            {
                if(rootfilename.size() < infilename.size() )
                {
                    rootfilename.push_back( infilename[i].substr(infilename[i].find_last_of("/\\")+1, infilename[i].substr( infilename[i].find_last_of("/\\")+1 ).find_last_of(".") ) + std::string("_analysis.root") );
                }
                std::cout << "Write out file: " << rootfilename.back() << "\n";
                thePlotter->writePlots(rootfilename.back());
            }
        }
    } 

    if(bunch)
    {
        std::cout << "Write out file: " << rootfilename.back() << "\n";
        thePlotter->writePlots(rootfilename.back());
    }
    if(dofit) thePlotter->showGraph(correction_factors, fitFunction);
    delete thePlotter;
}
