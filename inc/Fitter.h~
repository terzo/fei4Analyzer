#ifndef __FITTER_H__
#define __FITTER_H__

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TFile.h>
#include <TMatrixDSym.h>
#include <TMath.h>
#include <TVirtualFitter.h>
#include <TMinuit.h>

class Fitter
{
 public :
   Fitter(bool quiet);
  ~Fitter(void );
  
  typedef struct fitResult 
  {		     
     double chi2  ;	     
     int    ndf   ;	     
     double MP    ;
     double MPerr ;	     
     double MPV   ;
     double FWHM  ;
  } fitResultDef;   
  
  fitResultDef fit(TH1* histo);
  void getStat(void);
  
 private:
      
   std::stringstream  ss_;
   bool quiet_;
   TMatrixDSym cov;
   Double_t sv[4], pllo[4], plhi[4], fp[4], fpe[4], npar[4];
   
   std::vector<std::string> failed;
   
   
   TF1* langaufit(TH1 *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF, TMatrixDSym *cov);
   
   Int_t langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM);

} ;

#endif
