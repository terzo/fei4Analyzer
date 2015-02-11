/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#include "CosmicEventMaker.h"

CosmicEventMaker::CosmicEventMaker(bool quiet, bool readTimeStamp, bool design25)
{
  quiet_ = quiet;
  readTimeStamp_ = readTimeStamp;
  design25_ = design25;
  std::cout << "Cosmic GUI converter for FE-I4" << "\n";
}

//====================================================================================
EventMaker::hitMapDef CosmicEventMaker::makeEvents(std::string infilename, std::string outfilename, int lv1diff, int nevt)
{
  EventMaker::hitMapDef hitMap;
  
  std::ifstream inputfile;
  std::istream *fs;

  // Handle not given filenames by reading from std::cin and writing to std::cout
  if (infilename == "") fs=&(std::cin); 
  else
  {
    inputfile.open(infilename.c_str(), std::fstream::in);
    fs = &inputfile;
  }
  std::ofstream outputfile;
  std::ostream *fout;	
  
  int timeStampInt=0;
  if (readTimeStamp_) fs->read((char*)&timeStampInt, 4);
  time_t timeStamp = timeStampInt;
  
  //starting and limit event
  int nev=0;
  //const int nevt=100000000;
  int skipcount=0;
  //...
  
  int indx=0;
  int l1id=0;
//  int link=0;
  int bxid=0;
  int firstbxid=0;
  int highesttot=0;
  int maxRow=0;
  int maxCol=0;
  int ntrg=0;
  char block0[9*sizeof(unsigned)] ;
  char* pixblock=0;
  unsigned size, evtno;
//  int modtot[5];
  long int fileNumber=0;
  unsigned long long lasttimestamp=0;
//  unsigned long long lastdeadtime=0;
  bool firstrun = true;

  std::cout << " Reading just " << nevt << std::endl;
  std::cout << " Skipping " << skipcount << std::endl;
  
  while(fs->good())
  {
    fs->read((char*)&size, 4);
    if( fs->eof() ) break;
    if( skipcount == 0 ) nev++; 
    else                 std::cout << " Skipcount is " << skipcount << std::endl;
    if( nevt>0 && nev>nevt ) break; //event limit break

//    for(int i=0;i<5;i++) modtot[i]=0;
    int oldl1id=-1;
    fs->read((char*)&evtno, 4);
    fs->read(block0,9*sizeof(unsigned));
    unsigned long long counter, trgtime, deadtime;
    int bitpos;
    this->triggerDecode(block0, counter, trgtime, deadtime, bitpos);

    //if(bitpos!=-1)tdc->Fill(bitpos);

    if (firstrun || (((trgtime + 65536) - lasttimestamp) > 400000000)) 
    {
       firstrun = false;
       if (!quiet_) std::cout << "Triggered" << std::endl;
       if (outfilename == "-") 
       {
    	  fout = &(std::cout);
    	  (*fout) << "#" << std::endl;
    	  char timeText[100];
    	  struct tm * timeinfo;
    	  timeinfo = localtime ( &timeStamp );
    	  strftime(timeText, 100, "%c", timeinfo);
    	  if (readTimeStamp_) (*fout) << "# " << timeText << std::endl;
    	  (*fout) << "#" << std::endl;  			  
       } 
       else if (outfilename == "") 
       {
    	  fout = NULL;
       } 
       else 
       {
          std::stringstream newname;
    	  newname << outfilename.substr(0, outfilename.find(".")) << "_" << fileNumber++ << outfilename.substr(outfilename.find("."), (outfilename.length() - outfilename.find(".")));
    	  std::cout << "New file to be written: " << newname.str() << std::endl;
    	  outputfile.close();
    	  outputfile.open(newname.str().c_str(), std::fstream::out);
    	  fout = &outputfile;
    	  (*fout) << "#" << std::endl;
    	  char timeText[100];
    	  struct tm * timeinfo;
    	  time_t now = timeStamp + ((int) ((double)trgtime * 25.6e-9));
    	  timeinfo = localtime ( &now );
    	  strftime(timeText, 100, "%c", timeinfo);
    	  if (readTimeStamp_) (*fout) << "# " << timeText << std::endl;
    	  (*fout) << "#" << std::endl;  			  
       }
    }

    //ttr->Fill(trgtime - lasttimestamp);
    //tdt->Fill(deadtime - lastdeadtime);
    //lastdeadtime = deadtime;
    lasttimestamp = trgtime;
    int pixblocksize=size-11*sizeof(unsigned);
    pixblock=new char[pixblocksize];
    fs->read(pixblock,pixblocksize);
    if (skipcount > 0) { skipcount--; continue; }
    indx=0;
    int ndr=0;

    while (indx<pixblocksize)
    {
      unsigned currentu=nextcurrent(&pixblock[0],indx);
      FormattedRecord current(currentu);
      if(current.isData()) ndr++;
    }
    if (fout) 
    {
      //      We're omitting number output...
      //		*(fout) << "0x" << std::setfill('0') << std::setw(8) << std::hex << counter2 << std::dec << std::setfill(' ') << std::endl; // trigger number
      //		*(fout) << "0x" << std::setfill('0') << std::setw(8) << std::hex << counter1 << std::dec << std::setfill(' ') << std::endl; // trigger number
      *(fout) << "TD" << /*std::setw(2)*/" " << "N1" << /*std::setw(3)*/" " << "N2" <<  std::setw(1) <<" " << evtno << std::endl; // trigger mode, error code, trigger number
    }
    indx=0;

    while (indx<pixblocksize)
    {
       unsigned currentu=nextcurrent(&pixblock[0],indx);
       FormattedRecord current(currentu);
       int diffbx;
       if(current.isHeader())
       {
    	  ntrg++;
    	  //link=current.getLink();
    	  //std::cout<<"Data from link "<<link<<std::endl;
    	  l1id=current.getL1id();
    	  bxid=current.getBxid();
    	  if( abs(l1id-oldl1id)< lv1diff )
    	  {
    	    oldl1id=l1id;
    	    firstbxid=bxid;
    	  }
    	  diffbx=bxid-firstbxid;
    	  if (diffbx<0)diffbx+=256;
    	  if (fout) 
    	  {
    	   	  //	  We're omitting number output...
    	   	  //				    *(fout) << "0x" << std::setfill('0') << std::setw(6) << std::hex << (current.getWord() & 0xFFFFFF) << std::dec << std::setfill(' ') << std::endl;
    	   	  *(fout) << "DH" << /*std::setw(2)*/" " << current.getBxid() << /*std::setw(4)*/" " << current.getL1id() << std::setw(1) <<" " << (trgtime + diffbx) << std::endl;
    	  }
       }
       else if(current.isData())
       {
    	  int chip=current.getFE();
    	  int tot=current.getToT();
    	  if(tot>highesttot) highesttot=tot;
    	  int col=current.getCol();
    	  int row=current.getRow();
    	  //tl1a->Fill(diffbx);
    	  if (fout) 
    	  {
    	    //      We're omitting number output...
    	    //  			      *(fout) << "0x" << std::setfill('0') << std::setw(6) << std::hex << current.getWord() << std::dec << std::setfill(' ') << std::endl;
    	    *(fout) << "DR" << /*std::setw(4)*/" " << col << /*std::setw(4)*/" " << row << /*std::setw(3)*/" " << tot << std::endl; // first hit
    	    //  			    *(fout) << "Data: FE#: " << chip << " TOT: " << tot << " COL: " << col << " ROW: " << row << std::endl; 
    	  }
	  //from cell address to space point
/*
	  float x,y;
    	  if (chip<8)
    	  {
    	    x=18*chip+col;
    	    y=row;
    	  } 
    	  else 
    	  {
    	    x=(15-chip)*18+17-col;
    	    y= 319-row;
    	  }
*/
    	  //occ[diffbx]->Fill(x,y);
    	  //toth[0]->Fill(tot, diffbx);
    	  //totmean[diffbx]->fill(Vec2(x,y), tot);
	  
	  EventMaker::hitDef aHit;
	  aHit.tot = tot ;
	  aHit.col = col ;
	  aHit.row = row ;
	  aHit.bcid= bxid;
	  aHit.l1id= l1id;
	  ss_.str("");
	  //ss_ << bxid << oldl1id;
	  ss_ << bxid ;
	  hitMap[string_to_int(ss_.str())][chip].push_back(aHit);
	  if(col>maxCol) maxCol = col;
	  if(row>maxRow) maxRow = row;
	  //std::cout << __LINE__ << "] filling hitMap" << std::endl;
	  //if(hitMap.empty()) std::cout << __LINE__ << "] map empty" << std::endl;
       }
    }
    delete [] pixblock;
  }
  
  ss_.str("");
  ss_ << "highest tot: " << highesttot << " maxCol: " << maxCol << " maxRow: " << maxRow;
  STDLINE(ss_.str(), ACGreen);
  return hitMap;
}

//==========================================================================
unsigned CosmicEventMaker::getWord(char *p, int word) 
{
  return *(unsigned*)&p[word*4];
}

//=============================================================================
unsigned CosmicEventMaker::nextcurrent(char* p, int& indx)
{
    unsigned current=*(unsigned*)&p[indx];
    indx+=4;
    return current;
}

//==============================================================================
bool CosmicEventMaker::triggerDecode(char* block, unsigned long long &counter, unsigned long long &trgtime, unsigned long long &deadtime, int &bitpos) 
{
    unsigned counter1=getWord(&block[0],2);
    unsigned counter2=getWord(&block[0],3);
    counter=counter2;
    counter=(counter<<32) | counter1;
    bitpos=-1;
    for (int j=0;j<64;j++)
    {
      if(((counter>>j)&1)==0)
      {
    	bitpos=j;
    	break;
      }
    } 
    unsigned trgtime1=getWord(&block[0],4);
    unsigned trgtime2=getWord(&block[0],5);
    unsigned deadtime1=getWord(&block[0],6);
    unsigned deadtime2=getWord(&block[0],7);
    trgtime=trgtime1;
    trgtime=(trgtime<<32) | trgtime2;
    deadtime=deadtime1;
    deadtime=(deadtime<<32) | deadtime2;
    return true;
}
//============================================================================
