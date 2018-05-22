// g++ -o pix_eff pix_eff.C -I/usr/include/ -L/usr/lib/ -lboost_regex  -lMinuit `root-config --cflags --libs`
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
double pixEff(TH2 *histo, unsigned int widthMin, unsigned int widthMax, double& sup, double& inf)
{  
   double cluN   = histo->Integral(widthMin,widthMax,1,20);
   double cHole  = histo->Integral(widthMin,widthMax,2,20);
   double cGood  = histo->Integral(widthMin,widthMax,1,1);
   double holeN  = 0;
   double pixN   = 0;
   double inPixN = 0;
      
   for(unsigned int x=widthMin; x< widthMax+1; ++x)
   {
     for(unsigned int y=1; y< 21; ++y)
     {
         double binC = histo->GetBinContent(x,y);
     	 holeN  += binC*(y-1);
     	 pixN   += binC*(x);
	 inPixN += binC*(x-2);
     }
   }
   
   
   
   double pixEff     = (pixN - holeN) / pixN  ;
   double pixIneff   = holeN          / pixN  ;
   double inPixEff   = calc_eff( inPixN , (inPixN-holeN), sup, inf);
   double inPixIneff = holeN          / inPixN;
   double cluEff     = cGood          / cluN  ;
   double cluIneff   = cHole          / cluN  ;
   
   if(!quiet_)
   {
      std::cout << "pixEff:\t\t"   << std::setprecision(4) << pixEff	 *100. << "%\n";
      std::cout << "pixIneff:\t"   << std::setprecision(4) << pixIneff   *100. << "%\n";
      std::cout << "inPixEff:\t"   << std::setprecision(4) << inPixEff   *100. << "%\n";
      std::cout << "inPixIneff:\t" << std::setprecision(4) << inPixIneff *100. << "%\n";
      std::cout << "cluEff:\t\t"   << std::setprecision(4) << cluEff	 *100. << "%\n";
      std::cout << "cluIneff:\t"   << std::setprecision(4) << cluIneff   *100. << "%\n";
   }
     
   return inPixEff;
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
int main(int argc, char **argv)
{
   setRootStyle();
   
   double sup, inf;
   
   TMultiGraph *mg_cdr_vs_eff      = new TMultiGraph();
   TMultiGraph *mg_cdr_vs_eff_peak = new TMultiGraph();
   TMultiGraph *mg_cwr_vs_eff      = new TMultiGraph();
    
   boost::regex expression("_r(\\d+)");
   boost::match_results<std::string::const_iterator> what;
   
   std::map<unsigned int, TGraphErrors*> gr_cdr_vs_eff;
   std::map<unsigned int, TGraphErrors*> gr_cwr_vs_eff;
   std::vector<TH1*> gr_cwr;
   
   TGraphAsymmErrors* gr_cdr_vs_eff_peak = new TGraphAsymmErrors();
   gr_cdr_vs_eff_peak->SetMarkerStyle(20);
   gr_cdr_vs_eff_peak->SetMarkerSize(2);
   gr_cdr_vs_eff_peak->SetMarkerColor(1);
   gr_cdr_vs_eff_peak->SetFillColor(1);
   gr_cdr_vs_eff_peak->SetFillStyle(3004);
   
   TGraphAsymmErrors* gr_cdr_vs_eff_prePeak = new TGraphAsymmErrors();
   gr_cdr_vs_eff_prePeak->SetMarkerStyle(20);
   gr_cdr_vs_eff_prePeak->SetMarkerSize(2);
   gr_cdr_vs_eff_prePeak->SetMarkerColor(4);
   gr_cdr_vs_eff_prePeak->SetFillColor(4);
   gr_cdr_vs_eff_prePeak->SetFillStyle(3004);
   
   TGraphAsymmErrors* gr_cdr_vs_eff_postPeak = new TGraphAsymmErrors();
   gr_cdr_vs_eff_postPeak->SetMarkerStyle(20);
   gr_cdr_vs_eff_postPeak->SetMarkerSize(2);
   gr_cdr_vs_eff_postPeak->SetMarkerColor(2);
   gr_cdr_vs_eff_postPeak->SetFillColor(2);
   gr_cdr_vs_eff_postPeak->SetFillStyle(3004);
   
   bool first = true;
   
   if (argc > 1)
   {
      for(unsigned int f=1; f<argc; f++)
      {
         TFile file( argv[f] );
	 if( !file.IsOpen() )
	 { 
	    std::cout << "file " << argv[f] << " not found" << std::endl;
	    continue;
	 }
         std::cout << "Opening " << argv[f]  << std::endl;
	 
	 TH2I *clusterHolesRow = (TH2I*)file.FindObjectAny("chip20_clusterHolesRow");
   	 if(clusterHolesRow == NULL) 
   	 {
   	     std::cout << "WARNING: no histogram with name " << "chip20_clusterHolesRow" << " found in " <<  argv[f] << "\n";
   	     continue;
   	 }
         
         double cluDistRow = 0;
	 if( regex_search(std::string(argv[f]), what, expression, boost::match_default) ) string_to_number(what[1], cluDistRow);
	 else std::cout << "WARNING: \"_r#\" expression not found in: " << what[0] << std::endl;
	 
	 if( gr_cwr_vs_eff.count(cluDistRow) == 0 ) 
	                gr_cwr_vs_eff[cluDistRow] = new TGraphErrors();
	 
	 gr_cdr_vs_eff_peak->SetPoint(f-1, cluDistRow , pixEff( clusterHolesRow, 6, 8, sup, inf)*100. );
         gr_cdr_vs_eff_peak->SetPointError(f-1, 0 , 0, inf, sup);
	 
	 gr_cdr_vs_eff_prePeak->SetPoint(f-1, cluDistRow , pixEff( clusterHolesRow, 8, 8, sup, inf)*100. );
	 gr_cdr_vs_eff_prePeak->SetPointError(f-1, 0 , 0, inf, sup  );
	 
	 gr_cdr_vs_eff_postPeak->SetPoint(f-1, cluDistRow , pixEff( clusterHolesRow, 6, 7, sup, inf)*100. );
	 gr_cdr_vs_eff_postPeak->SetPointError(f-1, 0 , 0,  inf, sup );
	 
	 for(unsigned int width=3; width < 100; width++)
	 {
	   double eff = pixEff( clusterHolesRow, width, width, sup, inf)*100.;
	   
	   if(width < 15)
	   {
	       if( gr_cdr_vs_eff.count(width) == 0 )
	                gr_cdr_vs_eff[width] = new TGraphErrors();

	       gr_cdr_vs_eff[width]->SetPoint(f-1, cluDistRow , eff );
               gr_cdr_vs_eff[width]->SetPointError(f-1, 0 , 0 );    
	   }
	   
	   gr_cwr_vs_eff[cluDistRow]->SetPoint(width-3, width, eff );
	   gr_cwr_vs_eff[cluDistRow]->SetPointError(f-1, 0 , 0 );
	 }
	 file.Close();
      }
   } 
      
   TCanvas *c_cdr_vs_eff = new TCanvas("c_cdr_vs_eff","c_cdr_vs_eff",200,10,900,700);
   c_cdr_vs_eff->Range(-39.06123,-3.426778,355.5918,19.16736);
   for(std::map<unsigned int, TGraphErrors*>::iterator g=gr_cdr_vs_eff.begin(); g!=gr_cdr_vs_eff.end(); ++g)
   {
     g->second->SetMarkerStyle(20);
     g->second->SetMarkerSize(2);
     g->second->SetMarkerColor(g->first+27);
     g->second->SetFillColor(g->first+27);
     g->second->SetFillStyle(3004);
     mg_cdr_vs_eff->Add(g->second);
   }   
   mg_cdr_vs_eff->Draw("AP");
   mg_cdr_vs_eff->GetYaxis()->SetTitle("inPix Efficiency [%]");
   mg_cdr_vs_eff->GetXaxis()->SetTitle("Cluster separation [rows]");
   
   TCanvas *c_cdr_vs_eff_peak = new TCanvas("c_cdr_vs_eff_peak","c_cdr_vs_eff_peak",200,10,900,700);
   c_cdr_vs_eff_peak->Range(-39.06123,-3.426778,355.5918,19.16736);
   mg_cdr_vs_eff_peak->Add(gr_cdr_vs_eff_peak);
   //mg_cdr_vs_eff_peak->Add(gr_cdr_vs_eff_prePeak);
   //mg_cdr_vs_eff_peak->Add(gr_cdr_vs_eff_postPeak);
   mg_cdr_vs_eff_peak->Draw("AP");
   mg_cdr_vs_eff_peak->GetYaxis()->SetTitle("inPix Efficiency [%]");
   mg_cdr_vs_eff_peak->GetXaxis()->SetTitle("Cluster separation [rows]");
   
   
   
   
   TCanvas *c_cwr_vs_eff = new TCanvas("c_cwr_vs_eff","c_cwr_vs_eff",200,10,900,700);
   c_cwr_vs_eff->Range(-39.06123,-3.426778,355.5918,19.16736);
   for(std::map<unsigned int, TGraphErrors*>::iterator g=gr_cwr_vs_eff.begin(); g!=gr_cwr_vs_eff.end(); ++g)
   {
     g->second->SetMarkerStyle(20);
     g->second->SetMarkerSize(2);
     g->second->SetMarkerColor(g->first);
     g->second->SetFillColor(g->first);
     g->second->SetFillStyle(3004);
     mg_cwr_vs_eff->Add(g->second);
   }  
   mg_cwr_vs_eff->Draw("AP");
   mg_cwr_vs_eff->GetYaxis()->SetTitle("inPix Efficiency [%]");
   mg_cwr_vs_eff->GetXaxis()->SetTitle("Cluster width [rows]");

   TFile outRootFile("pix_effOutput.root","recreate");
   c_cdr_vs_eff->Write();
   c_cdr_vs_eff_peak->Write();
   c_cwr_vs_eff->Write();
   outRootFile.Close();
}
