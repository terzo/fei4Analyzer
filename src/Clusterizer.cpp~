#include "Clusterizer.h"

 //================CLUSTERIZER============================================
Clusterizer::clusterMapDef Clusterizer::makeCluster(EventMaker::hitMapDef hitMap, int lv1diff)
{
  Clusterizer::clusterMapDef clusterMap;
  int clusterID=0;
  
  std::cout << __LINE__ << "] CLUSTERING" << std::endl;
  for(EventMaker::hitMapDef::iterator ev = hitMap.begin(); ev!=hitMap.end(); ++ev)
  {
    for(std::map<int, EventMaker::hitsDef>::iterator chip = (*ev).second.begin(); chip!=(*ev).second.end(); ++chip)
    {
      while( (*chip).second.size() != 0 )
      {
   	EventMaker::hitsDef& clusterHits = clusterMap[(*ev).first][(*chip).first][clusterID];
   	clusterHits.push_back( (*chip).second[0] );
   	(*chip).second.erase( (*chip).second.begin());
   	for(unsigned int c=0; c < clusterHits.size(); c++ )
   	{
   	  int& cRow = clusterHits[c].row ;
   	  int& cCol = clusterHits[c].col ;
	  //int& cTot  = clusterHits[c].tot ;
	  int& cBcid= clusterHits[c].bcid;
	  int& cL1id= clusterHits[c].l1id;
   	  for(unsigned int h=0; h < (*chip).second.size(); h++ )
   	  {
   	    if ( ( abs(cCol - (*chip).second[h].col) <= 1  )  && ( abs(cRow - (*chip).second[h].row ) <= 1       ) && 
	         ( abs(cBcid- (*chip).second[h].bcid)<10000)  && ( abs(cL1id- (*chip).second[h].l1id) <= lv1diff ) )
   	    {
   	      clusterHits.push_back( (*chip).second[h] );
   	      (*chip).second.erase(  (*chip).second.begin()+h );
   	      h--;
   	    }
   	  }
   	}
   	++clusterID;
      }
    }
  }
  
  return clusterMap;
}
  
//===================================MERGE LV1=======================================================
void Clusterizer::mergeLv1(Clusterizer::clusterMapDef &clusterMap, int maxMerge)
{
  std::cout << __LINE__ << "] MERGING LV1" << "\n";
  
  for(clusterMapDef::iterator ev = clusterMap.begin(); ev!=clusterMap.end(); ++ev)
  {
    for(std::map<int, clustersDef>::iterator chip=(*ev).second.begin(); chip!=(*ev).second.end(); ++chip)
    {
      for( clustersDef::iterator clus=(*chip).second.begin(); clus!=(*chip).second.end(); ++clus )
      {
    	 for(unsigned int hit=0; hit<(*clus).second.size(); hit++)
    	 {
	    for(int nm=1; nm < maxMerge; nm++)
	    {
  	      for(clustersDef::iterator nClus=clusterMap[(*ev).first+nm][(*chip).first].begin(); nClus!=clusterMap[(*ev).first+nm][(*chip).first].end(); nClus++)
  	      {
  		for(unsigned int nHit=0; nHit<(*nClus).second.size(); nHit++)
  		{
  		  int& nRow = (*nClus).second[nHit].row ;
  		  int& nCol = (*nClus).second[nHit].col ;
  		  if ( ( abs(nCol - (*clus).second[hit].row) <= 1 ) && ( abs(nRow - (*clus).second[hit].col) <= 1 ) )
  		  {
  		    //std::cout << __LINE__ << "] cluster merged" << std::endl;
  		    int hitMerged =0;
  		    for(unsigned int nnHit=0; nnHit<(*nClus).second.size(); nnHit++)
  		    {
  		      hitMerged++;
  		      (*clus).second.push_back( (*nClus).second[nnHit] );
  		    }
  		    clusterMap[(*ev).first+1][(*chip).first].erase( nClus );
  		    //std::cout << __LINE__ << "] " << hitMerged << " hit merged" << std::endl;
  		    break;
  		  }
  		}
  	      }
	    }
    	 }
      }
    }
  } 
} 
