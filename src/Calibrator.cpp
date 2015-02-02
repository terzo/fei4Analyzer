/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#include "Calibrator.h"

Calibrator::Calibrator(bool calibname)
{
  TCanvas *dummy = new TCanvas();
  
  calibname_ = calibname;
  
  if(calibname_)
  {
     TFile fileA( "A.root" );
     if( !fileA.IsOpen() )
     {
       std::cout << "calibration file: " <<  "A.root" << " not found" << "\n";
       calibname_ = true;
     }
     else
     {
       TCanvas *pixcan = (TCanvas*)fileA.Get("pixcan");
       ParA_ = (TH1F*)pixcan->FindObject("ParA_0_MA")->Clone("parA");
       fileA.Close();
       delete pixcan;
     
       TFile fileB( "B.root" );
       if( !fileB.IsOpen() )
       {
         std::cout << "calibration file: " <<  "B.root" << " not found" << "\n";
         calibname_ = false;
       }
       else
       {
         pixcan = (TCanvas*)fileB.Get("pixcan");
         ParB_ = (TH1F*)pixcan->FindObject("ParB_0_MA")->Clone("parB");
         fileB.Close();
         delete pixcan;
     
         TFile fileC( "C.root" );
	 if( !fileC.IsOpen() )
	 {
	   std::cout << "calibration file: " <<  "C.root" << " not found" << "\n";
	   calibname_ = false;
	 }
	 else
         {
           pixcan = (TCanvas*)fileC.Get("pixcan");
           ParC_ = (TH1F*)pixcan->FindObject("ParC_0_MA")->Clone("parC");
           fileC.Close();
           delete pixcan;
	 }
       } 
     }
  }
  
  delete dummy;
}
//=====================================================================
Calibrator::~Calibrator(void)
{
        //if(ParA_ != NULL) delete ParA_;
	//if(ParB_ != NULL) delete ParB_;
	//if(ParC_ != NULL) delete ParC_;
}
//=====================================================================
double Calibrator::calib(EventMaker::hitDef hit)
{
/*
   double charge = 0;
   int col = hit.col;
   int row = hit.row;
   if (totmap_[col][row]>0) 
   {
     charge = hit.tot * 6. / totmap_ [col] [row];    
   }
*/
   if(calibname_)
   {
     //In Par histos is entry 1,1 for pixel 0,0
     double pA = ParA_->GetBinContent(hit.col+1, hit.row+1);
     double pB = ParB_->GetBinContent(hit.col+1, hit.row+1);
     double pC = ParC_->GetBinContent(hit.col+1, hit.row+1);

     //cout << ParA -> GetBinContent(col+1, row+1) << endl;;
     double Q = pA + pB * hit.tot + pC * hit.tot * hit.tot;
     
     return Q/1000.;
   }
   else
   {
     return 0;//14.*hit.tot/7.;
   }
}
