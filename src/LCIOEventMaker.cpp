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
  std::cout << "LCIO event converter for FE-I4 (no totcode!)" << "\n";
  evn = 0;
}

//====================================================================================
EventMaker::hitMapDef LCIOEventMaker::makeEvents(std::string infilename, std::string outfilename, int lv1diff, int nevt)
{
  //EventMaker::hitMapDef hitMap;
  
  LCReader* lcReader ;
//  if( dumpNthEvent ) 
//    lcReader = LCFactory::getInstance()->createLCReader() ;
//  else
    lcReader = LCFactory::getInstance()->createLCReader(LCReader::directAccess) ;
    
    
  LCEvent* evt(0) ;
  //int evn = 0;
  
  try
  {
     lcReader->open( infilename.c_str() ) ;
  }
  catch( IOException& e) 
  {
      std::cout << e.what() << std::endl ;
     exit(1) ;
  }
  //if( skip > 1 ) 
  //lcReader->skipNEvents( nevt ) ;
  while( ( evt = lcReader->readNextEvent() ) != 0 )
  {
       //evt = lcReader->readEvent(338,  evn) ;
       if( nevt>-1 && evn<nevt ) break;
       if( !evt ) continue;
       
       const StringVec* colNames = evt->getCollectionNames() ;
       
       for(unsigned int i=0; i < colNames->size(); ++i)
       {
          std::string colName = (*colNames)[i];
          //std::cout<<  colName << "\n";
	  
	  if(colName == "zsdata_apix" || colName == "zsdata_FEI4")
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
		       
		       if( !quiet_ && evn%1000 == 0 )
		            std::cout << "event: "<< evn << "\tsensorID: " << sensorID << "\n";
		       
		       
		       EventMaker::hitDef aHit;
		       aHit.col = static_cast<int> ( zsData->getChargeValues()[k * kElements    ] );
	  	       aHit.row = static_cast<int> ( zsData->getChargeValues()[k * kElements + 1] );
		       aHit.tot = static_cast<int> ( zsData->getChargeValues()[k * kElements + 2] );
		       aHit.chip= static_cast<int> ( zsData->getChargeValues()[k * kElements + 3] );
	  	       aHit.l1id= static_cast<int> ( zsData->getChargeValues()[k * kElements + 4] );
		       aHit.bcid= evn;//bcid;
		       if(design25_) this->design25Encode(aHit);
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
//===============================================================================================================
bool LCIOEventMaker::writeEvents(hitMapDef &hitMap, std::string outfilename, int runNum)
{
  
  // create a writer and open the output file
  LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter();
  lcWrt->setCompressionLevel( 0 );
  lcWrt->open( outfilename.c_str()  , LCIO::WRITE_NEW );
  
  LCRunHeaderImpl* runHdr = new LCRunHeaderImpl() ;
  runHdr->setRunNumber( runNum ) ;      
  
  std::string detName("FEI4Tel")  ;
  runHdr->setDetectorName( detName.c_str() ) ;
  
  //std::stringstream description ; 
  //description << " run: " << runNum <<" just for testing lcio  - no physics !" ;
  //runHdr->setDescription( description.str()  ) ;
  
  //std::string subdetName("PixelTel") ;
  //runHdr->addActiveSubdetector( subdetName ) ;
  int nplanes = 8;
  std::vector<int> MaxX,MaxY,MinX,MinY;
  for(int p=0; p<nplanes; p++)
  {
    MaxX.push_back(335);
    MaxY.push_back(79);
    MinX.push_back(0);
    MinY.push_back(0);
  }
  
  std::vector<std::string> AppliedProcessor;
  AppliedProcessor.push_back("");
  
  runHdr->parameters().setValue ( "GeoID"	     , 0 );
  runHdr->parameters().setValues ( "MaxX"	     , MaxX ) ;
  runHdr->parameters().setValues ( "MaxY"	     , MaxY );
  runHdr->parameters().setValues ( "MinX"	     , MinX ) ;
  runHdr->parameters().setValues ( "MinY"	     , MinY );
  runHdr->parameters().setValue ( "NoOfDetector"     , nplanes );
  runHdr->parameters().setValues ( "AppliedProcessor" , std::vector<std::string>(1,"") );
  runHdr->parameters().setValue ( "DAQHWName"	   , "EUDRB" );
  runHdr->parameters().setValue ( "DAQSWName"	   , "EUDAQ" );
  runHdr->parameters().setValue ( "DataType"	   , "Data" );
  runHdr->parameters().setValue ( "DateTime"	   , "24.12.2000  23:59:59.000000000" );
  runHdr->parameters().setValue ( "EUDRBDet"	   , "MIMOSA26" );
  runHdr->parameters().setValue ( "EUDRBMode"	   , "ZS2" );
  
  lcWrt->writeRunHeader( runHdr ) ;
  int evnum = 0;
  time_t timer;
  for(EventMaker::hitMapDef::iterator ev = hitMap.begin(); ev!=hitMap.end(); ++ev)
  {
     
    //create the event
    LCEventImpl*  evt = new LCEventImpl() ;
    
    // set the event attributes
    evt->setRunNumber(  runNum   ) ;
    evt->setEventNumber( evnum );        
    evt->setDetectorName( detName );
    evt->setRunNumber( runNum );
    evt->setTimeStamp( int(time(&timer) * 1000000000.) );
    evt->parameters().setValue( "EventType", 2 );
    
    //create a collection
    //LCCollectionVec* zsInputCollectionVec = new LCCollectionVec( lcio::LCIO::TRACKERDATA );
    if(evnum == 0)
    {
      //create a collection
      LCCollectionVec* zsInputCollectionVec = new LCCollectionVec( EVENT::LCIO::LCGENERICOBJECT );
    
      zsInputCollectionVec->parameters().setValue( "DataDescription", "type:i,mode:i,spare1:i,spare2:i,spare3:i" );
      zsInputCollectionVec->parameters().setValue( "TypeName", "Setup Description" );

      for(int i=0; i<nplanes; ++i)
      {
        LCGenericObjectImpl *setupObj = 0;
      	setupObj = new LCGenericObjectImpl(5,0,0);
      	setupObj->setIntVal( 0, 102 );
      	setupObj->setIntVal( 1, 101 );    
      	zsInputCollectionVec->addElement( setupObj );
      }
      
      evt->addCollection ( zsInputCollectionVec, "eudrbSetup" );
    }
    
    //ID encoder info
    std::string encodingString = "sensorID:5,sparsePixelType:5";

    //Telescope data collection
    LCCollectionVec *trackerDataColl = new LCCollectionVec( EVENT::LCIO::TRACKERDATA );

    CellIDEncoder< TrackerDataImpl >  zsDataEncoder(encodingString, trackerDataColl);

    for(std::map<int, EventMaker::hitsDef>::iterator chip=ev->second.begin(); chip!=(*ev).second.end(); ++chip)
    { 
      // add hit objects to the collection
      TrackerDataImpl * zsData = new TrackerDataImpl;
      zsDataEncoder["sparsePixelType"] = kEUTelAPIXSparsePixel;
      
      //rappaccio =====================================================
      int fakechipid = (*chip).first;
      if(fakechipid == 0) fakechipid = 2;
      else if(fakechipid == 2) fakechipid = 0;
      //===============================================================
      zsDataEncoder["sensorID"] = fakechipid;
      
      zsDataEncoder.setCellID( zsData );
    
      for(unsigned int h=0; h<(*chip).second.size(); h++)
      {  
        zsData->chargeValues().push_back( (*chip).second[h].col  );
	zsData->chargeValues().push_back( (*chip).second[h].row  );
	zsData->chargeValues().push_back( (*chip).second[h].tot  );
	//zsData->chargeValues().push_back( (*chip).second[h].chip );
	zsData->chargeValues().push_back( fakechipid );
	zsData->chargeValues().push_back( (*chip).second[h].l1id );
	//(*chip).second[h].bcid 
	//zsInputCollectionVec->addElement( zsData ) ;
      } 
      trackerDataColl->addElement( zsData );
    }
    // add the collection with a name
    evt->addCollection(trackerDataColl,"zsdata_FEI4");
    // write the event to the file
    lcWrt->writeEvent( evt ) ;
    //LCTOOLS::dumpEvent( evt ) ;
    //delete zsInputCollectionVec;
    delete trackerDataColl;
    evnum++;
  }
  
  delete runHdr ;
  lcWrt->close() ;
  delete lcWrt ;
  
  return true;
}

