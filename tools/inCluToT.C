// g++ -o inCluToT inCluToT.C /home/iwsatlas1/terzo/utility/ClusterAnalysis/Analyzer/obj/Fitter.o -I/usr/include/ -L/usr/lib/ -lboost_regex -lMinuit  `root-config --cflags --libs --glibs` -lMinuit
#include <vector>
#include <fstream>
#include <sstream>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <string>
#include <iomanip>

#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TMultiGraph.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

#include <boost/regex.hpp>
#include <boost/cregex.hpp>

//#include "/home/iwsatlas1/terzo/utility/langaus_fitter.C"
#include "/home/iwsatlas1/terzo/utility/ClusterAnalysis/Analyzer/inc/Fitter.h"
#include "/home/iwsatlas1/terzo/utility/calc_eff.C"

bool quiet_ = true;
//======================================================================
template <typename N, typename S>
void string_to_number(const S theString, N &n)
{
  std::istringstream data_s;
  data_s.str(theString)	  ;
  data_s >> n		  ;
}
//======================================================================
template<typename H>
void addToCanvas(H* plot, TCanvas* canvas, std::string options)
{
   canvas->cd();
   plot->DrawClone(options.c_str());
}
//=================================================================================
void setRootStyle()
{
   gROOT->Reset();						   
   TStyle *terzo_stile  = new TStyle("terzo_stile","terzo stile"); 
   terzo_stile->SetCanvasBorderMode(0); 			   
   terzo_stile->SetCanvasBorderSize(2); 			   
   terzo_stile->SetCanvasColor(0);				   
   terzo_stile->SetDrawBorder(0);				   
   terzo_stile->SetFrameBorderMode(0);				   
   terzo_stile->SetLabelFont(132,"X");				   
   terzo_stile->SetLabelFont(132,"Y");				   
   terzo_stile->SetLabelFont(132,"Z");				   
   terzo_stile->SetLabelSize(0.06,"X"); 			   
   terzo_stile->SetLabelSize(0.06,"Y"); 			   
   terzo_stile->SetLabelSize(0.06,"Z"); 			   
   terzo_stile->SetLegendFillColor(0);				   
   terzo_stile->SetLegendFont(132);				   
   terzo_stile->SetNdivisions(510,"X"); 			   
   terzo_stile->SetNdivisions(510,"Y"); 			   
   terzo_stile->SetNdivisions(510,"Z"); 			   
   terzo_stile->SetOptStat(0);					   
   terzo_stile->SetOptTitle(0); 				   
   terzo_stile->SetPadBorderMode(0);				   
   terzo_stile->SetPadBorderSize(0);				   
   terzo_stile->SetPadBottomMargin(0.15);			   
   terzo_stile->SetPadColor(0); 				   
   terzo_stile->SetPadGridX(true);				   
   terzo_stile->SetPadGridY(true);				   
   terzo_stile->SetPadLeftMargin(0.15); 			   
   terzo_stile->SetPadRightMargin(0.05);			   
   terzo_stile->SetPadTickX(1); 				   
   terzo_stile->SetPadTickY(1); 				   
   terzo_stile->SetPadTopMargin(0.05);				   
   terzo_stile->SetStatFont(132);				   
   terzo_stile->SetTextFont(132);				   
   terzo_stile->SetTickLength(0.03,"X");			   
   terzo_stile->SetTickLength(0.03,"Y");			   
   terzo_stile->SetTickLength(0.03,"Z");			   
   terzo_stile->SetTitleFillColor(0);				   
   terzo_stile->SetTitleFont(132,"X");				   
   terzo_stile->SetTitleFont(132,"Y");				   
   terzo_stile->SetTitleFont(132,"Z");				   
   terzo_stile->SetTitleFontSize(132);				   
   terzo_stile->SetTitleOffset(0.07,"Z");			   
   terzo_stile->SetTitleSize(0.06,"X"); 			   
   terzo_stile->SetTitleSize(0.06,"Y"); 			   
   terzo_stile->SetTitleSize(0.06,"Z"); 			   
   terzo_stile->SetTitleXOffset(1);				   
   terzo_stile->SetTitleYOffset(0.9);				   
   gROOT->SetStyle("terzo_stile"); 				   
}
//===================================================================================


//===================================================================================
int main(int argc, char **argv)
{
   setRootStyle();
   
   TMultiGraph *mg_pix_vs_mp = new TMultiGraph();
   TMultiGraph *mg_pixEff  = new TMultiGraph();
    
   //boost::regex expression("_r(\\d+)");
   //boost::match_results<std::string::const_iterator> what;
      
   std::vector<TGraphErrors*> gr;
   std::vector<TGraphAsymmErrors*> grEff;
   /*
   TGraphErrors *gr = new TGraphErrors();
   gr->SetMarkerStyle(20);
   gr->SetMarkerSize(2);
   gr->SetMarkerColor(2);
   gr->SetFillColor(2);
   gr->SetFillStyle(3004);
   */
   std::vector<TH1*> totPix;
   
   TFile outRootFile("inCluToTOutput.root","recreate");
   Fitter theFitter(false);
   
   if (argc > 1)
   {
      for(unsigned int f=1; f<argc; f++)
      {
         TFile file( argv[f] );
	 if( !file.IsOpen() )
	 { 
	    std::cout << "WARNING: file " << argv[f] << " not found" << std::endl;
	    if(f == argc-1) exit(-1);
	    else            continue;
	 }
         std::cout << "Opening " << argv[f]  << std::endl;
	
	 //int widthRow = 7;
	 
	 for(int widthRow=19; widthRow<22; widthRow++)
	 {
	    std::stringstream ss;
	    ss.str(""); ss << "chip0/inPixelToT/chip0_inClusterRowToT_CW" << widthRow;
	    TH2I *inClusterRowToT = (TH2I*)file.Get( ss.str().c_str() );
   	    if(inClusterRowToT == NULL) 
   	    {
	        ss.str(""); ss << "chip0/inPixelToT/chip0_inClusterRowVsToT_CW" << widthRow;
                inClusterRowToT = (TH2I*)file.Get( ss.str().c_str() );
   	    	if(inClusterRowToT == NULL)
		{
		  std::cout << "WARNING: no histogram " << ss.str() << " found in " <<  argv[f] << "\n";
   	    	  continue;
		}
   	    }
         
	    //if( regex_search(std::string(argv[f]), what, expression, boost::match_default) ) string_to_number(what[1], cluDistRow);
	    //else std::cout << "WARNING: \"_r#\" expression not found in: " << what[0] << std::endl;
	    gr.push_back( new TGraphErrors() );
   	    gr.back()->SetMarkerStyle(20);
   	    gr.back()->SetMarkerSize(2);
   	    gr.back()->SetMarkerColor(widthRow-17);
   	    gr.back()->SetFillColor(2);
   	    gr.back()->SetFillStyle(3004);
	    
	    grEff.push_back( new TGraphAsymmErrors() );
	    grEff.back()->SetMarkerStyle(20);
   	    grEff.back()->SetMarkerSize(2);
   	    grEff.back()->SetMarkerColor(widthRow-17);
   	    grEff.back()->SetFillColor(2);
   	    grEff.back()->SetFillStyle(3004);
	    
	    int totEntries = 0;
	    for(int pix=1; pix<widthRow+1; pix++)
	    {
	       ss.str(""); ss << "ToT_CW" << widthRow << "_pix" << pix; 
	       totPix.push_back( inClusterRowToT->ProjectionY(ss.str().c_str(), pix, pix) );
	       //TF1 *ff = fit( totPix.back(), 2.5, 8.5, 2 );
	       //gr->SetPoint( pix-1, pix-1, ff->GetParameter(1) );
               //gr->SetPointError( pix-1, 0, ff->GetParError(1) );
	       
	       
	       Fitter::fitResultDef fr = theFitter.fit( totPix.back() );
	       gr.back()->SetPoint( pix-1, pix-1, fr.MP );
               gr.back()->SetPointError( pix-1, 0, fr.MPerr ); 
	       outRootFile.WriteTObject(totPix.back());
	       
	       int entries = totPix.back()->GetEntries();
	       if(pix==1) totEntries = entries;
	       grEff.back()->SetPoint( pix-1, pix-1, (1.*entries)/(1.*totEntries)*100. );
	       double sup,inf;
	       calc_eff(totEntries,entries,sup, inf);
	       grEff.back()->SetPointError( pix-1, 0, 0, inf*100, sup*100 );
	    }
	 }
	 
	 file.Close();
      }
   } 
   
   TCanvas *c_pix_vs_mp = new TCanvas("c_pix_vs_mp","c_pix_vs_mp",200,10,900,700);
   c_pix_vs_mp->Range(-39.06123,-3.426778,355.5918,19.16736);
   for(int i=0; i<gr.size(); ++i)
   {
      mg_pix_vs_mp->Add(gr[i]);
   }
   mg_pix_vs_mp->Draw("AP");
   mg_pix_vs_mp->GetYaxis()->SetTitle("ToT [25 ns]");
   mg_pix_vs_mp->GetXaxis()->SetTitle("Pixel");
   
   TCanvas *c_pixEff = new TCanvas("c_pixEff","c_pixEff",200,10,900,700);
   c_pixEff->Range(-39.06123,-3.426778,355.5918,19.16736);
   for(int i=0; i<gr.size(); ++i)
   {
      mg_pixEff->Add(grEff[i]);
   }
   mg_pixEff->Draw("AP");
   mg_pixEff->GetYaxis()->SetTitle("Effieciny [%]");
   mg_pixEff->GetXaxis()->SetTitle("Pixel");

   
   //for(unsigned int i=0; i<totPix.size(); ++i)  totPix[i]->Write();
   outRootFile.WriteTObject(c_pix_vs_mp);
   outRootFile.WriteTObject(c_pixEff);
   outRootFile.Close();
}
