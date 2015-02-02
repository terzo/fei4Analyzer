/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#include "Fitter.h"

Double_t langaufun(Double_t *x, Double_t *par) {

   //Fit parameters:
   //par[0]=Width (scale) parameter of Landau density
   //par[1]=Most Probable (MP, location) parameter of Landau density
   //par[2]=Total area (integral -inf to inf, normalization constant)
   //par[3]=Width (sigma) of convoluted Gaussian function
   //
   //In the Landau distribution (represented by the CERNLIB approximation), 
   //the maximum is located at x=-0.22278298 with the location parameter=0.
   //This shift is corrected within this function, so that the actual
   //maximum is identical to the MP parameter.

      // Numeric constants
      Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
      Double_t mpshift  = -0.22278298;       // Landau maximum location

      // Control constants
      Double_t np = 100.0;      // number of convolution steps
      Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

      // Variables
      Double_t xx;
      Double_t mpc;
      Double_t fland;
      Double_t sum = 0.0;
      Double_t xlow,xupp;
      Double_t step;
      Double_t i;


      // MP shift correction
      mpc = par[1] - mpshift * par[0]; 

      // Range of convolution integral
      xlow = x[0] - sc * par[3];
      xupp = x[0] + sc * par[3];

      step = (xupp-xlow) / np;

      // Convolution integral of Landau and Gaussian by sum
      for(i=1.0; i<=np/2; i++) {
         xx = xlow + (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);

         xx = xupp - (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);
      }

      return (par[2] * step * sum * invsq2pi / par[3]);
}
//======================================================================================================
Fitter::Fitter(bool quiet)
{
   quiet_ = quiet;
}
//======================================================================================================
Fitter::~Fitter()
{
}
//======================================================================================================
TF1* Fitter::langaufit(TH1 *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF, TMatrixDSym *cov)
{
   // Once again, here are the Landau * Gaussian parameters:
   //   par[0]=Width (scale) parameter of Landau density
   //   par[1]=Most Probable (MP, location) parameter of Landau density
   //   par[2]=Total area (integral -inf to inf, normalization constant)
   //   par[3]=Width (sigma) of convoluted Gaussian function
   //
   // Variables for langaufit call:
   //   his             histogram to fit
   //   fitrange[2]     lo and hi boundaries of fit range
   //   startvalues[4]  reasonable start values for the fit
   //   parlimitslo[4]  lower parameter limits
   //   parlimitshi[4]  upper parameter limits
   //   fitparams[4]    returns the final fit parameters
   //   fiterrors[4]    returns the final fit errors
   //   ChiSqr          returns the chi square
   //   NDF             returns ndf

   Int_t i;
   Char_t FunName[100];

   sprintf(FunName,"Fitfcn_%s",his->GetName());

   TF1 *ffitold = (TF1*)gROOT->GetListOfFunctions()->FindObject(FunName);
   if (ffitold) delete ffitold;

   TF1 *ffit = new TF1(FunName,langaufun,fitrange[0],fitrange[1],4);
   ffit->SetParameters(startvalues);
   ffit->SetParNames("Width","MP","Area","GSigma");
   
   for (i=0; i<4; i++) {
      ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
   }

   std::string option = "ERBN0";
   if(quiet_) option+= "Q";
   
   his->Fit(FunName, option.c_str());   // precise limit error calculation, fit within specified range, use ParLimits, do not plot, quiet
   
   TVirtualFitter *fitter = TVirtualFitter::GetFitter();
   cov->Use(fitter->GetNumberTotalParameters(),fitter->GetCovarianceMatrix());

   ffit->GetParameters(fitparams);    // obtain fit parameters
   for (i=0; i<4; i++) {
      fiterrors[i] = ffit->GetParError(i);     // obtain fit parameter errors
   }
   ChiSqr[0] = ffit->GetChisquare();  // obtain chi^2
   NDF[0] = ffit->GetNDF();           // obtain ndf

   return (ffit);              // return fit function
}
//===============================================================================================
Int_t Fitter::langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM) {

   // Seaches for the location (x value) at the maximum of the 
   // Landau-Gaussian convolute and its full width at half-maximum.
   //
   // The search is probably not very efficient, but it's a first try.

   Double_t p,x,fy,fxr,fxl;
   Double_t step;
   Double_t l,lold;
   Int_t i = 0;
   Int_t MAXCALLS = 10000;


   // Search for maximum

   p = params[1] - 0.1 * params[0];
   step = 0.05 * params[0];
   lold = -2.0;
   l    = -1.0;


   while ( (l != lold) && (i < MAXCALLS) ) {
      i++;

      lold = l;
      x = p + step;
      l = langaufun(&x,params);
 
      if (l < lold)
         step = -step/10;
 
      p += step;
   }

   if (i == MAXCALLS)
      return (-1);

   maxx = x;

   fy = l/2;


   // Search for right x location of fy

   p = maxx + params[0];
   step = params[0];
   lold = -2.0;
   l    = -1e300;
   i    = 0;


   while ( (l != lold) && (i < MAXCALLS) ) {
      i++;

      lold = l;
      x = p + step;
      l = TMath::Abs(langaufun(&x,params) - fy);
 
      if (l > lold)
         step = -step/10;
 
      p += step;
   }

   if (i == MAXCALLS)
      return (-2);

   fxr = x;


   // Search for left x location of fy

   p = maxx - 0.5 * params[0];
   step = -params[0];
   lold = -2.0;
   l    = -1e300;
   i    = 0;

   while ( (l != lold) && (i < MAXCALLS) ) {
      i++;

      lold = l;
      x = p + step;
      l = TMath::Abs(langaufun(&x,params) - fy);
 
      if (l > lold)
         step = -step/10;
 
      p += step;
   }

   if (i == MAXCALLS)
      return (-3);


   fxl = x;

   FWHM = fxr - fxl;
   return (0);
}
//=====================================================================
void Fitter::getStat()
{
  std::cout << "Number of fit at the limit: " << failed.size() << std::endl;
  for(unsigned int i=0; i < failed.size(); ++i)
  {
    std::cout << failed[i] << "\n";
  }
}
//=====================================================================
Fitter::fitResultDef Fitter::fit(TH1* histo)
{
   Fitter::fitResultDef fitResult;
   
   //TH1* histo_copy = (TH1*)histo->Clone("histo_copy");
   // Setting fit range and start values
   Double_t fr[2];

   pllo[0]=0.2; pllo[1]=2.0; pllo[2]=1.0; pllo[3]=0.4;
   plhi[0]=5.0; plhi[1]=50.0; plhi[2]=1000000.0; plhi[3]=5.0;
   sv[0]=histo->GetRMS(); 
   sv[1]=histo->GetBinLowEdge( histo->GetMaximumBin() ); 
   sv[2]=50000.0; 
   sv[3]=histo->GetRMS()/3.;
   
   Int_t    ndf = 100;
   Double_t chisqr  = 100000000;
   //Double_t chisqr_best = chisqr;

   TF1 *fitsnr;// = langaufit(histo,fr,sv,pllo,plhi,fp,fpe,&chisqr,&ndf, &cov);
   
   bool left =true;
   bool right=true;
   /*
   int nleft =1;
   int nright=1;   
   bool wasOK=false;
   */
      fr[0]=histo->GetBinCenter( histo->FindFirstBinAbove(1) );
      while(fr[0]<1) fr[0]++;
      fr[1]=13;
      std::cout<< "===============================================" << "\n";
      std::cout<< "START FITTING PROCEDURE FOR " << histo->GetName() << "\n";
      std::cout<< "===============================================" << "\n";
      while( (1.*chisqr/ndf) > 1 || ndf > 4)
      {
         if(!quiet_)
	 {
    	   std::cout<< "chi/ndf = " << 1.*chisqr/ndf << "\n";
    	   std::cout<< "ndf = " << ndf << "\n";
    	   std::cout<< "range = " << fr[0] << " - " << fr[1] << "\n";
	 }
	 //if (fitsnr) delete fitsnr;
	 fitsnr = langaufit(histo,fr,sv,pllo,plhi,fp,fpe,&chisqr,&ndf, &cov);

	 if(fr[0] > sv[1] - 1.5)		          
	 {					          
	    std::cout<< "LEFT RANGE TOO SMALL" << "\n";   
	    left  = false;			          
	 }					          
	 if(fr[1] < sv[1] + 0.2) 		          
	 {					          
	   std::cout<< "RIGHT RANGE TOO SMALL" << "\n";   
	   right = false;			          
	 }				          
         
	 if(!left && !right) 
	 { 
	   std::cout<< "BEST RANGE FOUND" << "\n";
	   std::cout<< "chi/ndf = " << 1.*chisqr/ndf << "\n";
           std::cout<< "ndf = " << ndf << "\n";
           std::cout<< "range = " << fr[0] << " - " << fr[1] << "\n";
	   break;
	 }
	 
         if( gMinuit->fLimset!=0 || fitsnr->GetMaximumX() < (sv[1]-1) || fitsnr->GetMaximumX() > (sv[1]+2) )
    	 {  
	    std::cout<< "MOVEMENT BECAUSE OF PARAMETER AT THE LIMIT" << "\n";	
	    if(left)		   
	    {								  
	       fr[0]++; 						  
	       right = true;						  
	       left = false;						  
	    }								  
	    else if(right)						  
	    {								  
	       fr[1]--; 						  
	       left = true;						  
	       right = false;						  
	    }												    		    
	 }
	 else break;

	 /*
	 else 
	 {
	    if( gMinuit->fLimset!=0 || fitsnr->GetMaximumX() < (sv[1]-1) || fitsnr->GetMaximumX() > (sv[1]+2) )
    	    {									      
	       std::cout<< "MOVEMENT BECAUSE OF PARAMETER AT THE LIMIT" << "\n";      
	       if(left) 							      
	       {								      
	    	  fr[0]--;
		  left  =false;							      						      
	       }								      
	       else if(right)							      
	       {								      
	    	  fr[1]++;
		  right = false;						      						      
	       }												        	
	    }									      
	    else								      
	    {									      							      
	       if( (1.*chisqr/ndf) < (1.*chisqr_best/ndf) )			      
	       {	
	    	  std::cout<< "MOVEMENT BECAUSE OF BETTER CHI2 FOUND" << "\n";        
	    	  chisqr_best = chisqr; 					      
	    									      
	    	  if(left)							      
	    	  {								      
	    	     fr[0]++;							      
	    	     right = true;						      
	    	     //left  =false;						      
	    	  }								      
	    	  else if(right)						      
	    	  {								      
	    	     fr[1]--;							      
	    	     left = true;						      
	    	     //right=false;						      
	    	  }								      
	       }
	       else
	       {
	           if(left) 
	           {	    
	              fr[0]--;
		      left  =false;
	           }	    
	           else if(right)
	           {	    
	              fr[1]++;
		      right = false;
	           }	    
	       }								      
	    }
	 }
	 */									     
      }
   
   //delete histo_copy;
   if(!quiet_)
   {
     std::cout<< "chi/ndf = " << 1.*chisqr/ndf << "\n";
     std::cout<< "ndf = " << ndf << "\n";
     std::cout<< "range = " << fr[0] << " - " << fr[1] << "\n";
   }
   fitsnr = langaufit(histo,fr,sv,pllo,plhi,fp,fpe,&chisqr,&ndf, &cov);
   if (gMinuit->fLimset==1)							      
   {												      
     failed.push_back( std::string(histo->GetName()) );
   }
   //chi2->Fill(1.*chisqr/ndf);
   //TF1 *fitenv = new TF1("envelope_up",envelope,fr[0],fr[1],20);
   //TF1 *fitenv2 = new TF1("envelope_up2",envelope2,fr[0],fr[1],1);
   
   Double_t SNRPeak, SNRFWHM; 
   //Double_t SNRPeak_min=0, SNRFWHM_min=0, SNRPeak_max=0, SNRFWHM_max=0;
   langaupro(fp,SNRPeak,SNRFWHM);
   
   fitResult.chi2  = chisqr;
   fitResult.ndf   = ndf;
   fitResult.MP    = fitsnr->GetParameter(1);
   fitResult.MPerr = fitsnr->GetParError(1);
   fitResult.MPV   = fitsnr->GetMaximumX();
   fitResult.FWHM  = SNRFWHM;
   
   histo->GetListOfFunctions()->Add(fitsnr);
   
   return fitResult;
   
   
   
/*   
   TMatrixDSym scov = cov;
   cov.Print();
   for(int i=0; i<4; i++)
   {
   	scov[i][i] = sqrt(scov[i][i]);
   }

   double mean = fitsnr->Mean(fr[0],fr[1]);
   fitsnr->DrawCopy("lsame");
   for(int i=0; i<4; ++i)
   {
   	fitenv->SetParameter(i,fp[i]);
        for(int j=0; j<4; ++j)
        {
   	    fitenv->SetParameter(4*(i+1)+j,-scov[j][i]);
        }
   }
   double mean_min = fitenv->Mean(fr[0],fr[1]);
   double mpv_min  = fitenv->GetMaximumX();
   fitenv->SetLineColor(4);
   //fitenv->DrawCopy("lsame");
   //fitenv2->SetParameter(1,-1);
   //fitenv2->SetLineColor(4);
   //fitenv2->DrawCopy("lsame");
   scov.Print();
   for(int i=0; i<4; ++i)
   {
   	fitenv->SetParameter(i,fp[i]);
        for(int j=0; j<4; ++j)
        {
   	    fitenv->SetParameter(4*(i+1)+j,scov[i][j]);
        }
   }
   double mean_max = fitenv->Mean(fr[0],fr[1]);
   double mpv_max  = fitenv->GetMaximumX();
   fitenv->SetLineColor(3);
   //fitenv->DrawCopy("lsame");
   //fitenv2->SetParameter(1,1);
   //fitenv2->SetLineColor(3);
   //fitenv2->DrawCopy("lsame");
   
   Double_t  peak=0;
   Double_t max_pars[4]; 
   Double_t min_pars[4];
   
   fitsnr->SetLineColor(4);
   for(int i=0; i<4; ++i)
   {
   	//fitsnr->SetParameters(fp[0]+scov[0][i],fp[1]+scov[1][i],fp[2]+scov[2][i],fp[3]+scov[3][i]);
	
	if(fitsnr->GetMaximum()>peak)
	{
		langaupro(fitsnr->GetParameters(),SNRPeak_min,SNRFWHM_min);
		//min_pars = fitsnr->GetParameters();
	}
	//fitsnr->DrawCopy("lsame");
   }
   //fitsnr->SetParameters(min_pars);
   //double mean_min = fitsnr->Mean(fr[0],fr[1]);
   //fitsnr->DrawCopy("lsame");
   peak=0;
   fitsnr->SetLineColor(3);
   for(int i=0; i<4; ++i)
   {
  
	//fitsnr->SetParameters(fp[0]+scov[0][i],fp[1]+scov[1][i],fp[2]+scov[2][i],fp[3]+scov[3][i]);
	
	if(fitsnr->GetMaximum()>peak)
	{
		langaupro(fitsnr->GetParameters(),SNRPeak_max,SNRFWHM_max);
		//max_pars = fitsnr->GetParameters();
	}
	//fitsnr->DrawCopy("lsame");
   }
   //fitsnr->SetParameters(max_pars);
   //double mean_max = fitsnr->Mean(fr[0],fr[1]);
   //fitsnr->DrawCopy("lsame");

   std::stringstream string;
   //string.precision(3);
   TPaveText *tpt = new TPaveText(0.622126,0.711864,0.981322,0.930085,"brNDC");
   string << "Mean     = " << std::setprecision(3) << mean << "^{+" << std::setprecision(1) << mean-mean_min << "}_{-" << -1*(mean-mean_max) << "} ke";
   tpt->AddText(string.str().c_str());
   string.str("");
   //string << "MPV     = " << std::setprecision(3) << SNRPeak << "_{-" << std::setprecision(1) << SNRPeak-SNRPeak_min << "}^{+" << SNRPeak_max-SNRPeak << "} ke";
   string << "MPV     = " << std::setprecision(3) << SNRPeak << "^{+" << std::setprecision(1) << SNRPeak-mpv_min << "}_{-" << -1*(SNRPeak-mpv_max) << "} ke";
   tpt->AddText(string.str().c_str());
   string.str("");
   string <<"FWHM = " << SNRFWHM << " ke";
   //tpt->AddText(string.str().c_str());
   tpt->SetBorderSize(1);
   tpt->SetFillColor(0);
   tpt->SetLineColor(1);
   tpt->SetTextFont(132);
   tpt->SetTextColor(kBlack);
   tpt->Draw();

   
   TLine *lineThr = new TLine(3,0,3,2.2*1000);
   lineThr->SetLineColor(2);
   lineThr->SetLineStyle(2);
   lineThr->SetLineWidth(3);
   lineThr->Draw();
   
   TLine *lineMean = new TLine(mean,0,mean,fitsnr->Eval(mean));
   lineMean->SetLineColor(kSpring+4);
   lineMean->SetLineStyle(2);
   lineMean->SetLineWidth(3);
   lineMean->Draw();
   
   TLine *lineMPV = new TLine(SNRPeak,0,SNRPeak,fitsnr->GetMaximum());
   lineMPV->SetLineColor(4);
   lineMPV->SetLineStyle(2);
   lineMPV->SetLineWidth(3);
   lineMPV->Draw();
   
   TLegend *leg = new TLegend(0.63936,0.625,0.978448,0.930085);
   //leg->SetNColumns(4);
   leg->AddEntry(lineThr,"Threshold","l");
   leg->AddEntry(lineMPV,"MPV","l");
   leg->AddEntry(lineMean,"Mean","l");
   leg->SetTextFont(132);
   leg->SetBorderSize(1);
   leg->SetFillColor(kWhite);
   leg->Draw();
*/
}
