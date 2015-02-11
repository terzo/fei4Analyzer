/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#ifndef __PLOTTER_H__
#define __PLOTTER_H__

#include <TApplication.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TH2.h>
#include <TF1.h>
#include <TFile.h>
  
#include "Clusterizer.h"
#include "Calibrator.h"
#include "Fitter.h"

class Plotter
{
 public :
   Plotter(bool quiet, int module_type=0       ) ;
  ~Plotter(void                    )  ;
  
   void fillClusterPlots(Clusterizer::clusterMapDef &clusterMap, double noise = -1, bool calibname = "calib.root");
   void fillHitPlots(EventMaker::hitMapDef& hitMap);
   void fitPlots(double voltage = 0, unsigned int dofit = 1);
   void writePlots(std::string rootFileName, bool bunch);
   void setCuts(int colRowCuts[4], bool borders = false);
   void setCuts(int minCol = -1, int minRow = -1, int maxCol = -1, int maxRow= -1, bool borders = false);
   void setModuleType(int module_type);
   bool isEmpty(){return empty_;}
   void showGraph(std::vector<double> correction_factor,unsigned int fit_function = 1);
   
  
 private:
   
   void quadEncode(const int chip, int &col, int &row);
   void deletePlots();
   bool outOfLimits(int &col,int &row);
   
   template <class H>
   void addPlot(std::map<int, H*> &histo, std::string name, int chip, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
   template <class H>
   void addPlot(std::map<int, H*> &histo, std::string name, int chip, Int_t nbinsx, Double_t xlow, Double_t xup);
   template <class H>
   H* addPlot(H* histo, std::string name, Int_t nbinsx, Double_t xlow, Double_t xup);
   template <class H>
   H* addPlot(H* histo, std::string name, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
   
   std::stringstream  ss_;
   bool isQuad_;
   bool quiet_;
   bool empty_;
   double v_;
   int minColCut_;
   int minRowCut_;
   int maxColCut_;
   int maxRowCut_;
   bool borders_;
   
   //single plots
   std::map<int, TH1I*> clusterToT_, three_hitToT_, two_hitToT_, one_hitToT_, clusterSize_, clusterSizeRow_, clusterSizeCol_;
   std::map<int, TH1D*> clusterCharge_,clusterCharge_cs1_,clusterCharge_cs2_, clusterCharge_cs3_;
   std::map<int, TH1I*> totMax_, totMin_;
   std::map<int, TH2I*> hitMap_, clusterMap_cs1_, clusterMap_cs2_, clusterHolesRow_, clusterHolesCol_, clusterToT_CSn_;
   std::map<int, TH2D*> clusterMeanTotMap_cs1_, clusterMeanTotMap_cs2_, clusterTotMap_cs1_, clusterTotMap_cs2_;
   //     #chip,           CS,RowVSToT
   std::map<int, std::map<int, TH2I*> > inClusterRowToT_, inClusterColToT_;
   
   TH2I *clusterMap_cs1_all_;  
   TH2I *clusterMap_cs2_all_;      
   TH2D *clusterTotMap_cs1_all_;    
   TH2D *clusterTotMap_cs2_all_;   
   TH2D *clusterMeanTotMap_cs1_all_;
   TH2I *hitMap_all_            ;
   TH1I *clusterToT_all_    	;   
   TH1I *one_hitToT_all_	;   
   TH1I *two_hitToT_all_	;   
   TH1I *totMax_all_ 	   	;
   TH1I *totMin_all_ 	   	;
   TH1I *clusterSize_all_	;
   TH1I *clusterSizeRow_all_    ;
   TH1I *clusterSizeCol_all_    ;
   TH1D *clusterCharge_all_     ;
   TH1D *clusterCharge_cs1_all_ ; 
   TH1D *clusterCharge_cs2_all_ ;
   
   bool design25_;
   
   std::vector<TObject*> trashbin_;
   
   //batch vectors
   //std::vector<TGraphErrors*> graphs;
   std::map<int, std::map<double,Fitter::fitResultDef> > ToT_all_;
   std::map<int, std::map<double,Fitter::fitResultDef> > ToT_cs1_;
   std::map<int, std::map<double,Fitter::fitResultDef> > ToT_cs2_;
   std::map<double,Fitter::fitResultDef> ToT_mod_all_;
   std::map<double,Fitter::fitResultDef> ToT_mod_cs1_;
   std::map<double,Fitter::fitResultDef> ToT_mod_cs2_;
   
   
   Fitter *theFitter_;
} ;

#endif
