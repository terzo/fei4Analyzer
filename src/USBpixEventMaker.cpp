/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#include "USBpixEventMaker.h"

USBpixEventMaker::USBpixEventMaker(bool quiet, bool readTimeStamp, bool design25)
{
  quiet_ = quiet;
  readTimeStamp_ = readTimeStamp;
  design25_ = design25;
  std::cout << "USBPix version!FE-I4" << "\n";
}

//====================================================================================
EventMaker::hitMapDef USBpixEventMaker::makeEvents(std::string infilename, std::string outfilename, int lv1diff, int nevt)
{
  EventMaker::hitMapDef hitMap;
  
  	  ////////// input raw-file
  FILE *in = fopen(infilename.c_str(),"r");
  if(in==0){
  	  printf("Can't open data file %s\n", infilename.c_str());
  	  exit(0);
  }
  char line[2000];

  ofstream logFile;
  TString logFileName = outfilename.c_str();
  logFile.open(logFileName.Data());
  std::string logString;

  int BCIDnumber = 0;		  // number of trigger window
  float lvl1 = -1;		  // virtual level 1ID
  int lv1ID = -1;		  // lv1ID = Level 1ID of one Trigge
  int bcid = -1;		  // bcid = BunchCrossing ID in one Trigger Window
  int hitNumTrigger = 0;  // number of hits in current TriggerWindow
  int hitNum = 0;		  // number of hits in run
  int errorFlag = 0;		  // = 1 if there is an error in one trigger window
  int chip=0;
  int errorNumber = 0;
  int nev = 0;
  
  // read till end of .raw-file
  while(fgets(line,2000,in)!=0)
  {
  	  std::string sLine =line;
	  
	  if(nevt>0 && nev > nevt) break;
	  
  	  if(strcmp(sLine.substr(0,2).c_str(),"0x")==0)
	  {
  		  int rawData = 0;
  		  sscanf(sLine.c_str(), "%x", &rawData);
  		  //check Field 1
  		  int checkField1 = rawData & 16252928;
  		  if(checkField1==16252928)
		  { //new Trigger Window
  			  if ( fgets(line,2000,in) == 0 ) break;
			  
  			  sLine =line;
  			  if(errorFlag==0)
			  { //accept BCID Window
  			  //if(lvl1==15 && errorFlag==0){ // only accepted raw data with 16 LVL1 trigger
  				  //std::cout << "    BCID Window accepted " << std::endl;
  				  logString = "";
  				  lvl1 = -1;
  				  lv1ID = -1;
  				  bcid = -1;
  				  BCIDnumber++;
  				  hitNum = hitNum + hitNumTrigger;
				  if(hitNumTrigger>0) nev++;
  				  hitNumTrigger = 0;
  			  }
  			  else
			  { // rejecte BCID Window
  				  //std::cout << "    BCID Window rejected " << std::endl;
  				  std::string logHeaderString = "\nErrorFlag high \n ";
  				  logString = logHeaderString + logString + "--------------------------------------------------------\n\n";
  				  logFile << logString.c_str();
  				  logString = "";
  				  lvl1 = -1;
  				  lv1ID = -1;
  				  bcid = -1;
  				  errorFlag = 0;
  				  BCIDnumber++;
				  if(hitNumTrigger>0) nev++;
  				  hitNumTrigger = 0;
  				  errorNumber++;
  			  }
  		  }
  		  else if(checkField1==15204352){ // DH, AR, VR, SR
  		  //if(checkField1==15204352 || checkField1==14680064 || checkField1==16252928 || checkField1==13107200 || checkField1==11010048 || checkField1==6815744){ // DH, AR, VR, SR
  			  int checkField2 = rawData & 458752;
  			  if(checkField2==65536){
  				  // check error flag in raw data
  				  int checkFlag = rawData & 32768;
  				  if(checkFlag==32768){
  					  //std::cout << " error in DataHeader" << std::endl;
  					  errorFlag=1;
  				  }
  				  // new DataHeader line
  				  if(lvl1==-1){   // new trigger window
  					  lv1ID = rawData & 32512;
  					  lv1ID = lv1ID >> 8;
  					  bcid = rawData & 255;
  					  lvl1++;
  					  //this->lvl1TriggerCounter++;
  				  }
  				  else{
  					  int templv1ID =  rawData & 32512;
  					  templv1ID = templv1ID >> 8;
  					  int tempBcid =  rawData & 255;
  					  if(bcid==255){
  						  bcid=-1;
  					  }
  					  if(templv1ID==lv1ID ||tempBcid==bcid+1){
  						  // everything is fine
  						  bcid=tempBcid;
  						  lvl1++;
  						  //this->lvl1TriggerCounter++;
  					  }
  					  else{
  						  // there is a problem
  						  // dismiss BCID window
  						  //std::cout << " no clustering due to missing level 1 trigger: see logFile " << BCIDnumber << std::endl;
  						  logFile << "\nLVL1ID or BCID missmatch \n lvl1 " << lvl1 << " bcid " << bcid << " templv1ID " << templv1ID << " tempBcid " << tempBcid << "\n";
  						  logFile << logString.c_str() << "--------------------------------------------------------\n\n";
  						  logString = "";
  						  lvl1 = 0;
  						  lv1ID =  rawData & 32512;
  						  lv1ID = lv1ID >> 8;
  						  bcid = rawData & 255;
  						  errorFlag = 0;
  						  BCIDnumber++;
						  if(hitNumTrigger>0) nev++;
  						  hitNumTrigger = 0;
  						  errorNumber++;
  					  }
  				  }
  			  //std::cout << " lvl1: " << lvl1 << " lv1ID: " << lv1ID << " bcid " << bcid << " errorFlag " << errorFlag << std::endl;
  			  }
  			  else if(checkField2==131072){
  				  //Address Record
  				  //std::cout << " Address Record " << sLine.c_str();
  			  }
  			  else if(checkField2==262144){
  				  //Value Record
  				  //std::cout << " Value Record " << sLine.c_str();
  			  }
  			  else if(checkField2==458752){
  				  //Service Record
  				  //std::cout << " Service Record " << sLine.c_str();
  			  }
  			  else{
  				  //std::cout << " Something unknown " << sLine.c_str();
  			  }
  		  }
  		  else{
  			  //std::cout << " Data Record " << sLine.c_str();
  			  //if ( ( hitNumTrigger % 1000 ) == 0 ) {
  			  //  std::cout << " Data Record " << sLine.c_str();
  			  //}
  			  int colData = rawData & 16646144;
  			  colData = colData >> 17;
  			  int rowData = rawData & 130816;
  			  rowData = rowData >> 8;
  			  int tot0Data = rawData & 240;
  			  tot0Data = tot0Data >> 4;
  			  int tot1Data = rawData & 15;
  			  if ( ( hitNumTrigger % 1000 ) == 0 && !quiet_) {
  			    std::cout << " col: " << colData  << " row: " << rowData << " tot0: " << tot0Data << " tot1: " << tot1Data << "\n";
  			  }
  			  //std::cout << " col: " << colData  << " row: " << rowData << " tot0: " << tot0Data << " tot1: " << tot1Data << "\n";
  			  // M. Bomben Tue May 27 17:17:52 CEST 2014
  			  if(tot0Data<14){
  				  tot0Data++;
  			  }
  			  else if(tot0Data==14){
  				  tot0Data=0;
  			  }
  			  if(tot1Data<14){
  				  tot1Data++;
  			  }
  			  else if(tot1Data==14){
  				  tot1Data=0;
  			  } // 
				
  			  // if(tot0Data>0 && tot0Data<14 && tot1Data>0 && tot1Data<14){ // M. Bomben Tue May 27 17:17:52 CEST 2014
  			  if(tot0Data>0 && tot0Data<14){ // cut only on tot0
  				  //std::cout << " hit " << colData << "\t" << rowData << "\t" << tot0Data << std::endl;  
	  			  EventMaker::hitDef aHit;
	  			  aHit.tot = tot0Data ;
	  			  aHit.col = colData-1 ;
	  			  aHit.row = rowData-1 ;
	  			  aHit.bcid= nev;
	  			  aHit.l1id= lvl1;
				  if(design25_) this->design25Encode(aHit);
	  			  ss_.str("");
	  			  //ss_ << bxid << oldl1id;
	  			  ss_ <<  nev;
	  			  hitMap[EventMaker::string_to_int(ss_.str())][chip].push_back(aHit);
//	  			  if(aHit.col>maxCol) maxCol = col;
//	  			  if(Hit.row>maxRow ) maxRow = row;
  				  hitNumTrigger++;
  			  }
  			  //if(tot0Data>0 && tot0Data<14 && tot1Data>0 && tot1Data<14){ // M. Bomben Tue May 27 17:17:52 CEST 2014
  			  if(tot1Data>0 && tot1Data<14){   // cut only on tot1  
  				  //std::cout << " hit " << colData << "\t" << rowData+1 << "\t" << tot1Data << std::endl;
				  EventMaker::hitDef aHit;
	  			  aHit.tot = tot1Data ;
	  			  aHit.col = colData-1 ;
	  			  aHit.row = rowData ;
	  			  aHit.bcid= nev;//bcid;
	  			  aHit.l1id= lvl1;
				  if(design25_) this->design25Encode(aHit);
	  			  ss_.str("");
	  			  //ss_ << bxid << oldl1id;
	  			  ss_ <<  nev;
	  			  hitMap[EventMaker::string_to_int(ss_.str())][chip].push_back(aHit);
//	  			  if(aHit.col>maxCol) maxCol = col;
//	  			  if(Hit.row>maxRow ) maxRow = row;
  				  hitNumTrigger++;
  			  }
  		  }
  	  }
	  else if( strcmp(sLine.substr(0,7).c_str(),"CHANNEL")==0 )
	  {
	    chip = EventMaker::string_to_int( sLine.substr(8,9) );
	    if(!quiet_) std::cout<< "CHIP " << chip << "\n";
	  }
  	  logString = logString + sLine;
  } // end while

  if(errorFlag==0){
  	  //std::cout << " last BCID window, no error " << BCIDnumber << std::endl;
  	  hitNum = hitNum + hitNumTrigger;
  }
  else{
  	  //std::cout << " last BCID window, error " << BCIDnumber << std::endl;
  	  errorFlag=0;
  	  errorNumber++;
  }

  //this->triggers = BCIDnumber;
  //this->hits = hitNum;
  //this->errors = errorNumber;
  if(!quiet_) std::cout << "errors: " << errorNumber << "\n";

  logFile.close();
  
  //ss_.str("");
  //ss_ << "highest tot: " << highesttot << " maxCol: " << maxCol << " maxRow: " << maxRow;
  //STDLINE(ss_.str(), ACGreen);
  return hitMap;
}
//=========================================
void USBpixEventMaker::design25Encode(EventMaker::hitDef &aHit)
{
  if (aHit.col % 2!=0)
  {
    aHit.row *= 2;
    aHit.col = (aHit.col-1)/2;
  }
  else 
  {
    aHit.row *= 2;
    aHit.row++;
    aHit.col = aHit.col/2;
  }
}
