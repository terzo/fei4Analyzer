/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/
#include "LCIOEventMaker.h"


LCIOEventMaker::LCIOEventMaker(bool quiet, bool readTimeStamp, bool design25)
{
  quiet_ = quiet;
  readTimeStamp_ = readTimeStamp;
  design25_ = design25;
  std::cout << "LCIO version! FE-I4" << "\n";
}

//====================================================================================
EventMaker::hitMapDef LCIOEventMaker::makeEvents(std::string infilename, std::string outfilename, int lv1diff, int nevt)
{
  EventMaker::hitMapDef hitMap;
  
  
  LCReader* lcReader ;
//  if( dumpNthEvent ) 
//    lcReader = LCFactory::getInstance()->createLCReader() ;
//  else
    lcReader = LCFactory::getInstance()->createLCReader(LCReader::directAccess) ;
    
    
  LCEvent* evt(0) ;
  int evn = 0;
  
  try
  {
     lcReader->open( infilename.c_str() ) ;
  }
  catch( IOException& e) 
  {
     std::cout << e.what() << endl ;
     exit(1) ;
  }
  //if( skip > 1 ) 
  //lcReader->skipNEvents( nevt ) ;
  
  while( ( evt = lcReader->readNextEvent() ) != 0 && evn<nevt )
  {
       //evt = lcReader->readNextEvent() ; 
       //evt = lcReader->readEvent(338,  evn) ;
       if( !evt ) continue;
       
       const StringVec* colNames = evt->getCollectionNames() ;
       
       for(unsigned int i=0; i < colNames->size(); ++i)
       {
          std::string colName = (*colNames)[i];
          //std::cout<<  colName << "\n";
	  
	  if(colName == "zsdata_apix")
	  {
	      //LCCollection* col = evt->getCollection( (*colNames)[i] ) ;
	  
	      LCCollectionVec * zsInputCollectionVec  = dynamic_cast < LCCollectionVec * > ( evt->getCollection( (*colNames)[i] ) );
              CellIDDecoder<TrackerDataImpl> cellDecoder( zsInputCollectionVec );  
	      
	      for(unsigned int j = 0 ; j < zsInputCollectionVec->size(); j++)
	      {
	         TrackerDataImpl * zsData = dynamic_cast< TrackerDataImpl * > ( zsInputCollectionVec->getElementAt( j ) );
	         int type     = static_cast<int>  ( cellDecoder( zsData )["sparsePixelType"]);
	         int sensorID = static_cast<int>  ( cellDecoder( zsData )["sensorID"] );
	         //std::cout << "type: " <<  type << "\n";
	         
	         if (type == kEUTelAPIXSparsePixel  )
	         {
		   //std::cout << zsData->getChargeValues().size() << "\n";
	           for(unsigned int k=0; k < zsData->getChargeValues().size(); ++k)
	           {
	               //std::cout << k << " : " << zsData->getChargeValues()[k] << "\n";
		       if( (k * kElements + 4)  > zsData->getChargeValues().size() ) continue;
		       //if( evn%10000 == 0 )
		       {
		          std::cout << "event: "<< evn << "\tsensorID: " << sensorID << "\n";
		       }
		       
		       EventMaker::hitDef aHit;
		       aHit.col = static_cast<int> ( zsData->getChargeValues()[k * kElements    ] );
	  	       aHit.row = static_cast<int> ( zsData->getChargeValues()[k * kElements + 1] );
		       aHit.tot = static_cast<int> ( zsData->getChargeValues()[k * kElements + 2] )+1;
		       aHit.chip= static_cast<int> ( zsData->getChargeValues()[k * kElements + 3] );
	  	       aHit.l1id= static_cast<int> ( zsData->getChargeValues()[k * kElements + 4] );
		       aHit.bcid= evn;//bcid;
		       
		       hitMap[evn][sensorID].push_back(aHit);
		       
	           }
	         }
	      }
	  }
       }
       evn++;
  }    

  lcReader->close() ;
  delete lcReader ;
  
  return hitMap;
}

