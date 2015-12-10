/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#include "Plotter.h"

Plotter::Plotter(bool quiet, int module_type)
{
    this->setModuleType(module_type);
    theFitter_ = new Fitter(quiet);
    quiet_ = quiet;
    empty_ = true;
    borders_ = false;
    v_=0;
    refDutHitLimit_.clear();
    colRowCuts_.clear();
    
    //define root style
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
//==========================================
Plotter::~Plotter()
{
	theFitter_->getStat();
	delete theFitter_;
	
	if(!empty_) this->deletePlots();
}
//================================================================================================
void Plotter::setFrameCuts(int dutID, int minCol, int minRow, int maxCol, int maxRow, bool borders)
{  
   std::vector<int> vec;
   vec.push_back(minCol);
   vec.push_back(minRow);
   vec.push_back(maxCol);
   vec.push_back(maxRow);
   colRowCuts_[dutID] = vec;
   
   borders_   = borders;	 
}
//================================================================================================
void Plotter::setFrameCuts(std::map<unsigned int,std::vector<int> > colRowCuts, bool borders)
{
   colRowCuts_ = colRowCuts;
   
   borders_   = borders;
}
//================================================================================================
void Plotter::setClusterCuts(int minWidthCol, int minWidthRow, int maxWidthCol, int maxWidthRow)
{
   minWidthCol_ = minWidthCol;
   minWidthRow_ = minWidthRow;
   maxWidthCol_ = maxWidthCol;
   maxWidthRow_ = maxWidthRow;	 
}
//================================================================================================
void Plotter::setClusterCuts(int colRowCluCuts[4])
{
   minWidthCol_ = colRowCluCuts[0];
   minWidthRow_ = colRowCluCuts[1];
   maxWidthCol_ = colRowCluCuts[2];
   maxWidthRow_ = colRowCluCuts[3];
}
//================================================================================================
void Plotter::setRefDutHitLimit(int dutID, int minCluNum, int maxCluNum)
{
  refDutHitLimit_[dutID]= std::make_pair(minCluNum,maxCluNum);
}
//================================================================================================
template <class H>
void Plotter::addPlot(std::map<int, H*> &histo, std::string name, int chip, Int_t nbinsx, Double_t xlow, Double_t xup)
{
   std::stringstream ss;
   ss.str(""); ss << "chip"<< chip << "_" << name;
   histo[chip] = new H(ss.str().c_str(),name.c_str(),nbinsx,xlow,xup);
   trashbin_.push_back(histo[chip]);
}
//================================================================================================
template <class H>
void Plotter::addPlot(std::map<int, H*> &histo, std::string name, int chip, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
{
   std::stringstream ss;
   ss.str(""); ss << "chip"<< chip << "_" << name;
   histo[chip] = new H(ss.str().c_str(),name.c_str(),nbinsx,xlow,xup,nbinsy,ylow,yup);
   trashbin_.push_back(histo[chip]);
}
//================================================================================================
template <class H>
H* Plotter::addPlot(H* histo, std::string name, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
{
   std::stringstream ss;
   ss.str(""); ss << name << " - " << "all chips";
   histo = new H(name.c_str(),name.c_str(),nbinsx,xlow,xup,nbinsy,ylow,yup);
   trashbin_.push_back(histo);
   return histo;
}
//================================================================================================
template <class H>
H* Plotter::addPlot(H* histo, std::string name, Int_t nbinsx, Double_t xlow, Double_t xup)
{
   std::stringstream ss;
   ss.str(""); ss << name << " - " << "all chips";
   histo = new H(name.c_str(),name.c_str(),nbinsx,xlow,xup);
   trashbin_.push_back(histo);
   return histo;
}
//==========================================CLUSTER TOT ANALYSIS=====================================
void Plotter::fillClusterPlots(Clusterizer::clusterMapDef &clusterMap, double noise, bool calibname)
{
  //if(!empty_) this->deletePlots();
  
  int cols = 80;
  int rows = 336;
  if(design25_)
  {
    cols /=2;
    rows *=2;
  }

  TH1::AddDirectory(kFALSE);
  TH2::AddDirectory(kFALSE);
  Calibrator *theCalibrator = new Calibrator(calibname);
  
  if(isQuad_)
  {
     clusterMap_cs1_all_    	= addPlot(clusterMap_cs1_all_	    ,"clusterMap_cs1"	    ,80*2,0,80*2,336*2,0,336*2);	   
     clusterMap_cs2_all_    	= addPlot(clusterMap_cs2_all_	    ,"clusterMap_cs2"	    ,80*2,0,80*2,336*2,0,336*2);	   
     clusterTotMap_cs1_all_     = addPlot(clusterTotMap_cs1_all_    ,"clusterTotMap_cs1"    ,80*2,0,80*2,336*2,0,336*2);   
     clusterTotMap_cs2_all_     = addPlot(clusterTotMap_cs2_all_    ,"clusterTotMap_cs2"    ,80*2,0,80*2,336*2,0,336*2);   
     clusterMeanTotMap_cs1_all_ = addPlot(clusterMeanTotMap_cs1_all_,"clusterMeanTotMap_cs1",80*2,0,80*2,336*2,0,336*2);
     
     clusterToT_all_ 	    = addPlot(clusterToT_all_	      ,"ToTdist"      ,150,-0.5,149.5) ;
     one_hitToT_all_ 	    = addPlot(one_hitToT_all_	      ,"ToTdistCS1"   ,150,-0.5,149.5) ;
     two_hitToT_all_ 	    = addPlot(two_hitToT_all_	      ,"ToTdistCS2"   ,150,-0.5,149.5) ;
     totMax_all_     	    = addPlot(totMax_all_	      ,"ToTdistCS2_LR",150,-0.5,149.5) ;    
     totMin_all_     	    = addPlot(totMin_all_	      ,"ToTdistCS2_UD",150,-0.5,149.5) ;    
     clusterSize_all_	    = addPlot(clusterSize_all_        ,"CSdist"       , 50, 0.5, 50.5) ;
     clusterSizeRow_all_    = addPlot(clusterSizeRow_all_     ,"CSdistRow"    , 50, 0.5, 50.5) ;
     clusterSizeCol_all_    = addPlot(clusterSizeCol_all_     ,"CSdistCol"    , 50, 0.5, 50.5) ;
     clusterCharge_all_     = addPlot(clusterCharge_all_      ,"Qdist"        ,100,   0,  100) ;
     clusterCharge_cs1_all_ = addPlot(clusterCharge_cs1_all_  ,"QdistCS1"     ,100,   0,  100) ;
     clusterCharge_cs2_all_ = addPlot(clusterCharge_cs2_all_  ,"QdistCS2"     ,100,   0,  100) ;
  }

  for(Clusterizer::clusterMapDef::iterator ev = clusterMap.begin(); ev!=clusterMap.end(); ++ev)
  {
    bool skipEvent=false;
    
    for(std::map<unsigned int, std::pair<unsigned int,unsigned int> >::iterator lim = refDutHitLimit_.begin(); lim != refDutHitLimit_.end(); ++lim)  
    {															  
       if( ev->second.count(lim->first)!=0) 								  
       {
    	 if(ev->second[lim->first].size() < lim->second.first || 						  
    	    ev->second[lim->first].size() > lim->second.second   ) 
    	 {
    	    skipEvent=true;
    	    break;
    	 }
       }
    }
    													  
    if(skipEvent) continue;												  
    
    for(std::map<int, Clusterizer::clustersDef>::iterator chip=(*ev).second.begin(); chip!=(*ev).second.end(); ++chip)
    {
      
      if(clusterToT_.count((*chip).first)==0)
      {
        std::cout << "Making new histograms for chip: " << (*chip).first << std::endl;
        
	addPlot(clusterToT_      ,"ToTdist"  	  ,(*chip).first, 150, -0.5, 149.5);
 	addPlot(one_hitToT_      ,"ToTdistCS1"	  ,(*chip).first, 150, -0.5, 149.5);
 	addPlot(two_hitToT_      ,"ToTdistCS2"	  ,(*chip).first, 150, -0.5, 149.5);
	addPlot(three_hitToT_    ,"ToTdistCS3"	  ,(*chip).first, 150, -0.5, 149.5);
	
	addPlot(totMax_          ,"ToTdistCS2_max",(*chip).first, 150, -0.5, 149.5);
	addPlot(totMin_          ,"ToTdistCS2_min",(*chip).first, 150, -0.5, 149.5);
	
	addPlot(clusterToT_CSn_  ,"ToTdistCSn"    ,(*chip).first, 150, -0.5, 149.5, 50, 0.5, 50.5);
	
	addPlot(clusterSize_   ,"CSdist"        ,(*chip).first,  50,  0.5,  50.5);
 	addPlot(clusterSizeRow_,"CSdistRow"     ,(*chip).first,  50,  0.5,  50.5);
        addPlot(clusterSizeCol_,"CSdistCol"     ,(*chip).first,  50,  0.5,  50.5);
	
	addPlot(clusterNumber_ ,"ClusterNumber" ,(*chip).first,  50, -0.5,  49.5);
	
	addPlot(clusterMap_cs1_       ,"clusterMap_cs1"       ,(*chip).first,cols,0,cols,rows,0,rows);
        addPlot(clusterMap_cs2_       ,"clusterMap_cs2"       ,(*chip).first,cols,0,cols,rows,0,rows);
	addPlot(clusterTotMap_cs1_    ,"clusterTotMap_cs1"    ,(*chip).first,cols,0,cols,rows,0,rows);
	addPlot(clusterTotMap_cs2_    ,"clusterTotMap_cs2"    ,(*chip).first,cols,0,cols,rows,0,rows);
        addPlot(clusterMeanTotMap_cs1_,"clusterMeanTotMap_cs1",(*chip).first,cols,0,cols,rows,0,rows);
	addPlot(clusterMeanTotMap_cs2_,"clusterMeanTotMap_cs2",(*chip).first,cols,0,cols,rows,0,rows);
	
	addPlot(clusterHolesCol_      ,"clusterHolesCol"      ,(*chip).first,cols,0.5,cols+0.5,cols,0,cols);
	addPlot(clusterHolesRow_      ,"clusterHolesRow"      ,(*chip).first,rows,0.5,rows+0.5,rows,0,rows);
	
	addPlot(clusterCharge_    ,"Qdist"   ,(*chip).first,100,0,100);
        addPlot(clusterCharge_cs1_,"QdistCS1",(*chip).first,100,0,100);	
 	addPlot(clusterCharge_cs2_,"QdistCS2",(*chip).first,100,0,100);
	addPlot(clusterCharge_cs3_,"QdistCS3",(*chip).first,100,0,100);
      }
      
      clusterNumber_[(*chip).first]->Fill( (*chip).second.size() );
      
      for( Clusterizer::clustersDef::iterator clus=(*chip).second.begin(); clus!=(*chip).second.end(); ++clus )
      {
    	 int cToT=0;
         double chargeSum = 0;
	 int maxRow, maxCol, minRow, minCol, maxRowTot, minRowTot , maxColTot, minColTot, maxTot, minTot, maxTotRow, minTotRow, maxTotCol, minTotCol;
	 maxRow=minRow=maxTotRow=minTotRow= (*clus).second[0].row;
	 maxCol=minCol=maxTotCol=minTotCol= (*clus).second[0].col;
	 maxTot=minTot=minRowTot=maxRowTot=maxColTot=minColTot=(*clus).second[0].tot;
	 unsigned int cSize = (*clus).second.size();
	 bool bad_cluster = false;
	 std::map<int,int> rowNum, colNum, rowLvl1;
	 
	 for(unsigned int hit=0; hit<cSize; hit++)
    	 {
	   if( outOfLimits((*chip).first,clus->second[hit].col,clus->second[hit].row) ) 
	   { 
	      bad_cluster = true;
	      break;
	   }
 
	   if(noise > 0 && 1.*hitMap_[(*chip).first]->GetBinContent((*clus).second[hit].col+1,(*clus).second[hit].row+1)/clusterMap.size() > noise) 
	   { 
	      bad_cluster = true;
	      //std::cout << "noise suppresssed" << "\n";
	      continue;
	   }
	   
	   int tot=(*clus).second[hit].tot;
           double charge = theCalibrator->calib( (*clus).second[hit] );
           chargeSum += charge;
    	   cToT+=tot;
	   
	   rowNum[(*clus).second[hit].row]+=tot;
	   rowLvl1[(*clus).second[hit].row]+=clus->second[hit].l1id;
           colNum[(*clus).second[hit].col]+=tot;
	   
           if( (*clus).second[hit].row > maxRow ) 
	   {
	     maxRow = (*clus).second[hit].row;
	     maxRowTot = (*clus).second[hit].tot;
	   }
	   if( (*clus).second[hit].row < minRow ) 
	   {
	     minRow = (*clus).second[hit].row;
	     minRowTot = (*clus).second[hit].tot;
	   }
	   if( (*clus).second[hit].col > maxCol ) 
	   {
	     maxCol = (*clus).second[hit].col;
	     maxColTot = (*clus).second[hit].tot;
	   }
	   if( (*clus).second[hit].col < minCol ) 
	   {
	     minCol = (*clus).second[hit].col;
	     minColTot = (*clus).second[hit].tot;
	   }
	   if(tot>maxTot) 
	   {
	     maxTotCol = (*clus).second[hit].col;
	     maxTotRow = (*clus).second[hit].row;
	     maxTot=tot;
	   }
	   if(tot<minTot) 
	   {
	     minTotCol = (*clus).second[hit].col;
             minTotRow = (*clus).second[hit].row;
	     minTot=tot;
	   } 
    	 }
	 
	 int rowSize = maxRow-minRow+1;
         int colSize = maxCol-minCol+1;
	 
	 if( maxWidthCol_>0 && (colSize > maxWidthCol_ || colSize < minWidthCol_) ) bad_cluster = true;
	 if( maxWidthRow_>0 && (rowSize > maxWidthRow_ || rowSize < minWidthRow_) ) bad_cluster = true;
	 
	 if(bad_cluster) continue;
	 
	 if(cSize!=0)  
	 { 
	    //  	 #row,tot
	    for( std::map<int,int>::iterator r = rowNum.begin(); r!= rowNum.end(); ++r)
	    {  
	    	if( inClusterRowToT_[(*chip).first].count(rowSize) == 0 )
	    	{ 
            	       std::stringstream ss;
	    	       ss.str(""); ss << "chip" << (*chip).first << "_inClusterRowVsToT_" << "CW"<< rowSize;
            	       inClusterRowToT_[(*chip).first][rowSize] = addPlot(inClusterRowToT_[(*chip).first][rowSize],ss.str(),rowSize,0,rowSize,500,-0.5,499.5);
	    	       
		       ss.str(""); ss << "chip" << (*chip).first << "_lvl1_" << "CW"<< rowSize;
            	       lvl1_[(*chip).first][rowSize] = addPlot(lvl1_[(*chip).first][rowSize],ss.str(),rowSize,0,rowSize,16,0,16);
	    	}
	    	inClusterRowToT_[(*chip).first][rowSize]->Fill( r->first - minRow, r->second );
		lvl1_[(*chip).first][rowSize]->Fill( r->first - minRow, rowLvl1[r->first]);
	    }
	    //  	 #col,tot
	    for( std::map<int,int>::iterator c = colNum.begin(); c!= colNum.end(); ++c)
 	    {  
	    	if( inClusterColToT_[(*chip).first].count(colSize) == 0 )
	    	{ 
            	       std::stringstream ss;
	    	       ss.str(""); ss << "chip" << (*chip).first << "_inClusterColVsToT_" << "CW"<< colSize;
            	       inClusterColToT_[(*chip).first][colSize] = addPlot(inClusterColToT_[(*chip).first][colSize],ss.str(),colSize,0,colSize,500,-0.5,499.5);
	    	}
 	    	inClusterColToT_[(*chip).first][colSize]->Fill( c->first - minCol, c->second );
		
	    }
	    clusterSize_[(*chip).first]->Fill(cSize);
	    clusterSizeRow_[(*chip).first]->Fill( rowSize );
	    clusterSizeCol_[(*chip).first]->Fill( colSize );
	    clusterHolesRow_[(*chip).first]->Fill( rowSize , rowSize - rowNum.size() );
	    clusterHolesCol_[(*chip).first]->Fill( colSize , colSize - colNum.size() );
	    clusterToT_CSn_[(*chip).first]->Fill(cToT, cSize);
	    clusterToT_[(*chip).first]->Fill(cToT);
	    if(chargeSum !=0 ) clusterCharge_[(*chip).first]->Fill(chargeSum);
	 
	    if( cSize==1 ) 
	    {
    	      one_hitToT_[(*chip).first]->Fill(cToT);
              if(chargeSum !=0 )clusterCharge_cs1_[(*chip).first]->Fill(chargeSum);
	      
	      clusterMap_cs1_[(*chip).first]->Fill(maxTotCol, maxTotRow);
              clusterTotMap_cs1_[(*chip).first]->Fill(maxTotCol, maxTotRow,cToT);
            }
            if( cSize==2 ) 
            {
    	       two_hitToT_[(*chip).first]->Fill(cToT);
               if(chargeSum !=0 ) clusterCharge_cs2_[(*chip).first]->Fill(chargeSum);
               
               totMax_[(*chip).first]->Fill(maxTot);
	       totMin_[(*chip).first]->Fill(minTot);
	       
	       clusterMap_cs2_[(*chip).first]->Fill(maxTotCol,maxTotRow);
               clusterTotMap_cs2_[(*chip).first]->Fill(maxTotCol,maxTotRow,cToT);
	    }
	    if( cSize==3 ) 
            {
	       three_hitToT_[(*chip).first]->Fill(cToT);
               if(chargeSum !=0 ) clusterCharge_cs3_[(*chip).first]->Fill(chargeSum); 
            }
	    if(isQuad_)
	    {
	       int col = maxTotCol;
               int row = maxTotRow;
	       this->quadEncode((*chip).first, col, row);
	       if( cSize==1 )
               {
	          clusterMap_cs1_all_->Fill(col, row);	   
                  clusterTotMap_cs1_all_->Fill(col, row,cToT);
	       }
	       if( cSize==2 )
               {
	          clusterMap_cs2_all_->Fill(col, row);
                  clusterTotMap_cs2_all_->Fill(col, row,cToT);
	       }
	    }
	 }
         else std::cout << "WARNING: cluster size 0" << std::endl;
      }
    }
  }

  for(std::map<int, TH1I*>::iterator chip=clusterToT_.begin(); chip!=clusterToT_.end(); ++chip)
  {   
    
    clusterMeanTotMap_cs1_[(*chip).first]->Divide(clusterTotMap_cs1_[(*chip).first],clusterMap_cs1_[(*chip).first],1.0,1.0,"B");
    clusterMeanTotMap_cs2_[(*chip).first]->Divide(clusterTotMap_cs2_[(*chip).first],clusterMap_cs2_[(*chip).first],1.0,1.0,"B");
    
    if(isQuad_) 						    
    {	
    							    
       clusterToT_all_	     ->Add( (*chip).second		     );
       one_hitToT_all_	     ->Add( one_hitToT_[(*chip).first]       );
       two_hitToT_all_	     ->Add( two_hitToT_[(*chip).first]       );
       totMax_all_	     ->Add( totMax_[(*chip).first]	     );  
       totMin_all_	     ->Add( totMin_[(*chip).first]	     );  
       clusterSize_all_	     ->Add( clusterSize_[(*chip).first]	     );  
       clusterSizeRow_all_   ->Add( clusterSizeRow_[(*chip).first]   );  
       clusterSizeCol_all_   ->Add( clusterSizeCol_[(*chip).first]   );
       clusterCharge_all_    ->Add( clusterCharge_[(*chip).first]    );  
       clusterCharge_cs1_all_->Add( clusterCharge_cs1_[(*chip).first]);
       clusterCharge_cs2_all_->Add( clusterCharge_cs2_[(*chip).first]);
    }
  }
  if(isQuad_) clusterMeanTotMap_cs1_all_->Divide(clusterTotMap_cs1_all_,clusterMap_cs1_all_,1.0,1.0,"B");						    
  
  empty_=false;
  delete theCalibrator;
}
  
//============================================
void Plotter::fillHitPlots(EventMaker::hitMapDef& hitMap)
{ 
  if(isQuad_) hitMap_all_= this->addPlot(hitMap_all_, "HitMap_all",80*2,0,80*2,336*2,0,336*2);

  for(EventMaker::hitMapDef::iterator ev = hitMap.begin(); ev!=hitMap.end(); ++ev)
  {
    for(std::map<int, EventMaker::hitsDef>::iterator chip=ev->second.begin(); chip!=(*ev).second.end(); ++chip)
    {
    
      if(hitMap_.count((*chip).first)==0)
      {
        std::cout << "Making new histograms for chip: " << (*chip).first << std::endl;
 	ss_.str("");
	ss_ << "chip"<< (*chip).first << "_HitMap";
	int rows = 336;
	int cols = 80;
	if(design25_)
	{
	   rows *=2;
	   cols /=2;
	}
 	addPlot(hitMap_, ss_.str().c_str(), (*chip).first,cols,0,cols,rows,0,rows);
      }
 
      for(unsigned int h=0; h<(*chip).second.size(); h++)
      {
        if(outOfLimits((*chip).first, chip->second[h].col,chip->second[h].row) ) continue;
      
        hitMap_[(*chip).first]->Fill((*chip).second[h].col, (*chip).second[h].row);
      
      	if(isQuad_)
      	{
           int col = (*chip).second[h].col;	        		       
           int row = (*chip).second[h].row;	        			       
      	   this->quadEncode((*chip).first, col, row);
           hitMap_all_->Fill(col, row); 	       
      	}
      } 
    }
  }
  
}
//========================================================================
bool Plotter::outOfLimits(int dutID, int &col, int &row)   
{
	bool out=false;
	if(colRowCuts_.count(dutID) > 0)
	{ 
	  if(colRowCuts_[dutID].size() == 4)
	  {
	    if(!borders_)
	    {
	    	//internal frame
	    	if( colRowCuts_[dutID][0] > 0 && col < colRowCuts_[dutID][0] ) out = true;
	    	if( colRowCuts_[dutID][1] > 0 && row < colRowCuts_[dutID][1] ) out = true;
	    	if( colRowCuts_[dutID][2] > 0 && col > colRowCuts_[dutID][2] ) out = true;
	    	if( colRowCuts_[dutID][3] > 0 && row > colRowCuts_[dutID][3] ) out = true;
	    	
	    }
	    else
	    {
	    	//external frame
	    	if( colRowCuts_[dutID][0] > 0 && colRowCuts_[dutID][2] > 0 && colRowCuts_[dutID][1] > 0 && colRowCuts_[dutID][3] > 0)
	    	{
	    	    if( col > colRowCuts_[dutID][0] && col < colRowCuts_[dutID][2] && row > colRowCuts_[dutID][1] && row < colRowCuts_[dutID][3]) out = true;
	    	}
	    }
	  }
	}
	return out;
}
//=========================================================================
void Plotter::quadEncode(const int chip, int &col, int &row)
{
  if(chip == 0)
  { 
  	col = col;
	row = 335-row;
  }
  if(chip == 1)
  {
  	col += 80;
	row = 335-row;
  }
  if(chip == 2)
  {
  	col = 159 - col;
	row += 336;
  }
  if(chip == 3)
  {
  	col = 79 - col;
	row += 336;
  }
}
//==================================================================
void Plotter::setModuleType(int module_type)
{
	if(module_type == 4) 
	{ 
	   std::cout << "Quad module analysis" << std::endl;
	   isQuad_=true ;
	}
	else isQuad_=false;
	
	if(module_type == 25)
	{
	   std::cout << "FE-I4 with 500x25um arrangment analysis" << std::endl;
	   design25_ = true;
	}
	else design25_ = false;
}
//=========================FIT HISTOS=========================================
void Plotter::fitPlots(double voltage, unsigned int dofit)
{
  if(voltage == 0) v_++;
  else             v_=voltage;
  
  if(dofit>0)
  {
      for(std::map<int, TH1I*>::iterator chip=clusterToT_.begin(); chip!=clusterToT_.end(); ++chip)
      {   
        ToT_all_[chip->first][v_] = theFitter_->fit( (*chip).second		  );
        ToT_cs1_[chip->first][v_] = theFitter_->fit( two_hitToT_[(*chip).first]    );
        ToT_cs2_[chip->first][v_] = theFitter_->fit( one_hitToT_[(*chip).first]    );
	if(dofit>1)
	{
          theFitter_->fit( clusterCharge_[(*chip).first]     );
          theFitter_->fit( clusterCharge_cs1_[(*chip).first] );
          theFitter_->fit( clusterCharge_cs2_[(*chip).first] );
	}
      }
      if(isQuad_)
      {
        ToT_mod_all_[v_] = theFitter_->fit( clusterToT_all_ );
        ToT_mod_cs1_[v_] = theFitter_->fit( one_hitToT_all_ );
        ToT_mod_cs2_[v_] = theFitter_->fit( two_hitToT_all_ );
	if(dofit>1)
	{
          theFitter_->fit( clusterCharge_all_	    );
          theFitter_->fit( clusterCharge_cs1_all_    );
          theFitter_->fit( clusterCharge_cs2_all_    );
	}
      }
  }
}
//=========================WRITEOUT HISTOS=========================================
void Plotter::writePlots(std::string rootFileName)
{ 
  
  TFile outRootFile(rootFileName.c_str(),"recreate");
  
  for(std::map<int, TH1I*>::iterator chip=clusterToT_.begin(); chip!=clusterToT_.end(); ++chip)
  {
    ss_.str("");
    ss_ << "chip"<< (*chip).first;
    TDirectory *dir = outRootFile.mkdir(ss_.str().c_str());
    dir->cd();
    
    (*chip).second->Write();
    two_hitToT_[(*chip).first]->Write();
    one_hitToT_[(*chip).first]->Write();
    three_hitToT_[(*chip).first]->Write();
    clusterToT_CSn_[(*chip).first]->Write();
    clusterNumber_[(*chip).first]->Write();
    clusterSize_[(*chip).first]->Write();
    clusterSizeRow_[(*chip).first]->Write();
    clusterSizeCol_[(*chip).first]->Write();
    clusterHolesRow_[(*chip).first]->Write();
    clusterHolesCol_[(*chip).first]->Write();
    totMax_[(*chip).first]->Write();
    totMin_[(*chip).first]->Write();
    hitMap_[(*chip).first]->Write();
    clusterMap_cs1_[(*chip).first]->Write();
    clusterMap_cs2_[(*chip).first]->Write();
    clusterTotMap_cs1_[(*chip).first]->Write();
    clusterTotMap_cs2_[(*chip).first]->Write();
    clusterMeanTotMap_cs1_[(*chip).first]->Write();
    clusterMeanTotMap_cs2_[(*chip).first]->Write();
    clusterCharge_[(*chip).first]->Write();
    clusterCharge_cs1_[(*chip).first]->Write();
    clusterCharge_cs2_[(*chip).first]->Write();
    clusterCharge_cs3_[(*chip).first]->Write();
    
    ss_.str("");
    ss_ << "inPixelToT";
    dir->mkdir(ss_.str().c_str())->cd();
    for(std::map<int, TH2I*>::iterator cs=inClusterRowToT_[(*chip).first].begin(); cs!=inClusterRowToT_[(*chip).first].end(); ++cs) cs->second->Write();
    for(std::map<int, TH2I*>::iterator cs=inClusterColToT_[(*chip).first].begin(); cs!=inClusterColToT_[(*chip).first].end(); ++cs) cs->second->Write();
    ss_.str("");
    ss_ << "lvl1";
    dir->mkdir(ss_.str().c_str())->cd();
    for(std::map<int, TH2I*>::iterator cs=lvl1_[(*chip).first].begin(); cs!=lvl1_[(*chip).first].end(); ++cs) cs->second->Write();
  }
  
  if(isQuad_)
  {
     outRootFile.cd();
     
     clusterToT_all_->Write();
     one_hitToT_all_->Write();
     two_hitToT_all_->Write();
     totMax_all_->Write();
     totMin_all_->Write();
     clusterSize_all_->Write();
     clusterSizeRow_all_->Write();
     clusterSizeCol_all_->Write();
     clusterCharge_all_->Write();
     clusterCharge_cs1_all_->Write();
     clusterCharge_cs2_all_->Write();
     clusterMap_cs1_all_->Write();	 
     clusterMap_cs2_all_->Write();	 
     clusterTotMap_cs1_all_->Write();    
     clusterTotMap_cs2_all_->Write();   
     clusterMeanTotMap_cs1_all_->Write();
     hitMap_all_->Write();
  }
  
  outRootFile.Close();
  this->deletePlots();
}
//========================================================================================================
void Plotter::showGraph(std::vector<double> correction_factors,unsigned int fit_function)
{ 
  TCanvas *canvas_mod;
  std::vector<TCanvas*> canvas;

  TFile outRootFile("tempOutput.root","recreate");

  for(std::map<int, std::map<double,Fitter::fitResultDef> >::iterator chip=ToT_all_.begin(); chip!=ToT_all_.end(); ++chip)
  { 
    ss_.str("");
    ss_ << "canvas_chip_" << chip->first;
    canvas.push_back( new TCanvas(ss_.str().c_str(),ss_.str().c_str(),200,10,900,700) );
    canvas.back()->cd();
    double hix=0,hiy=0,lox=0,loy=0;
    std::vector<TGraphErrors*> graphs;
    
    graphs.push_back( new TGraphErrors( ToT_all_.size() ) );
    graphs.push_back( new TGraphErrors( ToT_cs1_.size() ) );
    graphs.push_back( new TGraphErrors( ToT_cs2_.size() ) );

    std::map<double, Fitter::fitResultDef> vec;
    double correction_factor;
    //std::cout << __LINE__ << "] " << "correction_factors.size(): "  << correction_factors.size() << " > " << "chip->first: " << chip->first << "\n";
    if( correction_factors.size() > (unsigned int)chip->first ) 
            correction_factor = correction_factors[chip->first]; //2.46 2.42 2.75 2.49 2.59
    else    correction_factor = 1;
    for(unsigned int i=0; i<graphs.size(); ++i)
    {
      if(i==0) vec = ToT_all_[chip->first];
      if(i==1) vec = ToT_cs1_[chip->first];
      if(i==2) vec = ToT_cs2_[chip->first];
      unsigned int p=0;
      for(std::map<double, Fitter::fitResultDef>::iterator vbias=vec.begin(); vbias!=vec.end(); ++vbias)
      {
    	 double x = vbias->first; 
    	 double y;
	 if(fit_function == 2) 
	    y = vbias->second.MPV*correction_factor;
	 else    
	    y = vbias->second.MP*correction_factor;
	                
    	 graphs[i]->SetPoint(p,x,y);
    	 graphs[i]->SetPointError(p,0,0.1*y);
    	 if(x>hix) hix=x;
    	 if(y>hiy) hiy=y;
    	 if(x<lox) lox=x;
    	 if(y<loy) loy=y;
    	 p++;
      }
    }

    TH1F *hr = canvas.back()->cd()->DrawFrame(lox,loy,hix+abs(hix-lox)*0.1,hiy*1.1);
    hr->SetTitle("");
    hr->GetXaxis()->SetTitle("Bias voltage [V]");
    hr->GetYaxis()->SetTitle("Collected charge [ke]");

    for(unsigned int i=0; i<graphs.size(); ++i)
    {
       graphs[i]->SetMarkerStyle(20);
       graphs[i]->SetMarkerSize(2);
       graphs[i]->SetMarkerColor(2+i);
       graphs[i]->SetFillColor(2+i);
       graphs[i]->SetFillStyle(3004+i);
       graphs[i]->Draw("PE3");
    }
    canvas.back()->Write();
  }
  
  if(isQuad_)
  {  
     double hix=0,hiy=0,lox=0,loy=0;
     canvas_mod = new TCanvas("canvas_module","title",200,10,900,700);
     canvas_mod->cd();
     
     std::vector<TGraphErrors*>          graphs_mod;
     graphs_mod.push_back( new TGraphErrors( ToT_mod_all_.size() ) );
     graphs_mod.push_back( new TGraphErrors( ToT_mod_cs1_.size() ) );
     graphs_mod.push_back( new TGraphErrors( ToT_mod_cs2_.size() ) );
     
     std::map<double, Fitter::fitResultDef> vec;
     double correction_factor;
     if(correction_factors.size() != 0) 
                correction_factor = correction_factors.back();
     else       correction_factor = 1;
     for(unsigned int i=0; i<graphs_mod.size(); ++i)							  
     {  												  
        												  
       if(i==0) vec = ToT_mod_all_;									  
       if(i==1) vec = ToT_mod_cs1_;									  
       if(i==2) vec = ToT_mod_cs2_;									  
       unsigned int p=0;										  
       for(std::map<double, Fitter::fitResultDef>::iterator vbias=vec.begin(); vbias!=vec.end(); ++vbias) 
       {  												  
     	  double x = vbias->first;									  
     	  double y = vbias->second.MP*correction_factor;						  
     	  graphs_mod[i]->SetPoint(p,x,y);									  
     	  graphs_mod[i]->SetPointError(p,0,0.1*y);								  
     	  if(x>hix) hix=x;										  
     	  if(y>hiy) hiy=y;										  
     	  if(x<lox) lox=x;										  
     	  if(y<loy) loy=y;										  
     	  p++;												  
       }												  
     }													  
  
     TH1F *hr = canvas_mod->cd()->DrawFrame(lox,loy,hix+abs(hix-lox)*0.1,hiy*1.1);
     hr->SetTitle("");
     hr->GetXaxis()->SetTitle("Bias voltage [V]");
     hr->GetYaxis()->SetTitle("Collected charge [ke]");
  
     for(unsigned int i=0; i<graphs_mod.size(); ++i)
     { 
     	graphs_mod[i]->SetMarkerStyle(20);
     	graphs_mod[i]->SetMarkerSize(2);
     	graphs_mod[i]->SetMarkerColor(2+i);
     	graphs_mod[i]->SetFillColor(2+i);
     	graphs_mod[i]->SetFillStyle(3004+i);
     	graphs_mod[i]->Draw("PE3");
     }
     
     canvas_mod->Write();
  }
  
  
  //canvas->Write();
  outRootFile.Close();
}
//========================================================================================================
void Plotter::deletePlots(void)
{
  //std::cout << "thrash size: " << trashbin_.size() << "\n";
  for(unsigned int i=0; i<trashbin_.size(); i++)
  {
    if(trashbin_[i]) 
    {
       delete trashbin_[i];
    }
    else std::cout << "BOOOH" << std::endl;
  } 
  
  clusterToT_.clear();
  one_hitToT_.clear();
  two_hitToT_.clear();
  three_hitToT_.clear();
  clusterSize_.clear();
  clusterNumber_.clear();
  clusterSizeRow_.clear();
  clusterSizeCol_.clear();
  clusterHolesRow_.clear();
  clusterHolesCol_.clear();
  clusterToT_CSn_.clear();
  inClusterRowToT_.clear();
  inClusterColToT_.clear();
  lvl1_.clear();
  totMax_.clear();
  totMin_.clear();
  hitMap_.clear();
  clusterMap_cs1_.clear();
  clusterMap_cs2_.clear();
  clusterTotMap_cs1_.clear();
  clusterTotMap_cs2_.clear();
  clusterMeanTotMap_cs1_.clear();
  clusterMeanTotMap_cs2_.clear();
  clusterCharge_.clear();
  clusterCharge_cs1_.clear();
  clusterCharge_cs2_.clear();
  clusterCharge_cs3_.clear();
  trashbin_.clear();
  
  empty_ = true;
}
