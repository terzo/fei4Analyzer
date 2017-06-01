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
#include <TTree.h>

#include "Clusterizer.h"
#include "Calibrator.h"
#include "Fitter.h"

class Plotter
{
public :
    Plotter(bool quiet);
    ~Plotter(void);

    void fillClusterPlots(Clusterizer::clusterMapDef &clusterMap, double noise = -1);
    void fillHitPlots(EventMaker::hitMapDef& hitMap);
    void fitPlots(double voltage = 0, unsigned int dofit = 1);
    void writePlots(std::string rootFileName);
    void setFrameCuts(std::map<unsigned int,std::vector<int> >, bool borders = false);
    void setFrameCuts(int dutID, int minCol = -1, int minRow = -1, int maxCol = -1, int maxRow= -1, bool borders = false);
    void setClusterCuts(int colRowCluCuts[4]);
    void setClusterCuts(int minWidthCol = 0, int minWidthRow = 0, int maxWidthCol = -1, int maxWidthRow= -1);
    void setRefDutHitLimit(int dutID, int minCluNum, int maxCluNum);
    void setRefDutHitLimit(std::map<unsigned int, std::pair<unsigned int,unsigned int> > refDutHitLimit) {refDutHitLimit_ = refDutHitLimit;};
    void useChargeCalibration(bool use_charge_calibration){use_charge_calibration_ = use_charge_calibration;};
    void saveClusterData(bool save_cluster_data) {save_cluster_data_ = save_cluster_data;};
    void setModuleType(int module_type=0);
    bool isEmpty(){return empty_;};
    void showGraph(std::vector<double> correction_factor,unsigned int fit_function = 1);


private:
    void quadEncode(const int chip, int &col, int &row);
    void deletePlots();
    bool outOfLimits(int dutID, int &col,int &row);

    template <class H>
    void addPlot(std::map<int, H*> &histo, std::string name, int chip, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
    template <class H>
    void addPlot(std::map<int, H*> &histo, std::string name, int chip, Int_t nbinsx, Double_t xlow, Double_t xup);
    template <class H>
    H* addPlot(H* histo, std::string name, Int_t nbinsx, Double_t xlow, Double_t xup);
    template <class H>
    H* addPlot(H* histo, std::string name, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);

    std::stringstream  ss_;
    bool quiet_;
    bool empty_;
    bool use_charge_calibration_;
    bool save_cluster_data_;
    bool isQuad_;
    bool isDesign25_;
    double v_;
    int minWidthCol_, minWidthRow_, maxWidthCol_, maxWidthRow_;
    std::map<unsigned int,std::vector<int> > colRowCuts_;
    bool borders_;
    std::map<unsigned int, std::pair<unsigned int,unsigned int> > refDutHitLimit_;

    // variables for plot filling
    int cCol,cRow,cToT;

    //single plots
    std::map<int, TH1I*> clusterToT_, clusterToT_cs1_, clusterToT_cs2_, clusterToT_cs3_, clusterSize_, clusterSizeRow_, clusterSizeCol_, clusterNumber_;
    std::map<int, TH1D*> clusterCharge_,clusterCharge_cs1_,clusterCharge_cs2_, clusterCharge_cs3_;
    std::map<int, TH1I*> totMax_, totMin_;
    std::map<int, TH2I*> clusterToT_csn_, hitMap_, clusterMap_cs1_, clusterMap_cs2_, clusterHolesRow_, clusterHolesCol_;
    std::map<int, TH2D*> clusterMeanTotMap_cs1_, clusterMeanTotMap_cs2_, clusterTotMap_cs1_, clusterTotMap_cs2_;
    //     #chip,           CS,RowVSToT
    std::map<int, std::map<int, TH2I*> > inClusterRowToT_, inClusterColToT_, lvl1_;

    // plots for quads
    TH1I *clusterToT_all_;
    TH1I *clusterToT_cs1_all_;
    TH1I *clusterToT_cs2_all_;
    TH1I *clusterToT_cs3_all_;
    TH2I *clusterToT_csn_all_;

    TH1I *totMax_all_;
    TH1I *totMin_all_;

    TH2I *hitMap_all_;

    TH1I *clusterSize_all_;
    TH1I *clusterSizeRow_all_;
    TH1I *clusterSizeCol_all_;

    TH2I *clusterMap_cs1_all_;
    TH2I *clusterMap_cs2_all_;
    TH2D *clusterTotMap_cs1_all_;
    TH2D *clusterTotMap_cs2_all_;
    TH2D *clusterMeanTotMap_cs1_all_;
    
    TH1D *clusterCharge_all_;
    TH1D *clusterCharge_cs1_all_;
    TH1D *clusterCharge_cs2_all_;

    // tree for cluster data
    TTree *tree_;

    // trashbin
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
};

#endif
