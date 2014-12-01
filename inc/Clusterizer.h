#ifndef __CLUSTERIZER_H__
#define __CLUSTERIZER_H__

#include <vector>
#include <map>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <string>

#include "EventMaker.h"

class Clusterizer
{
 public :
 
   //          #clusterID
   typedef std::map<int, EventMaker::hitsDef>             clustersDef   ;
   //              #l1id         #chip
   typedef std::map<int , std::map<int, clustersDef> >    clusterMapDef ;
 
   Clusterizer(void                    ) {;}
  ~Clusterizer(void                    ) {;}
  
   clusterMapDef makeCluster(EventMaker::hitMapDef hitMap, int lv1diff = 1);
   void          mergeLv1(Clusterizer::clusterMapDef &clusterMap, int maxMerge);
  
 private:
   
   std::stringstream  ss_;
 
} ;

#endif
