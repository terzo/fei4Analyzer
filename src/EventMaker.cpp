/****************************************************************************
** Author: Stefano Terzo
**
** Max-Planck-instiut fuer Physik (Werner-Heisenberg-Institut)
** Foehringer Ring 6, D-80805 Muenchen, Germany
**
****************************************************************************/

#include "EventMaker.h"

EventMaker::EventMaker(bool quiet, bool readTimeStamp, bool design25)
{
  quiet_ = quiet;
  design25_= design25;
  readTimeStamp_ = readTimeStamp;
  evn = 0;
}

//============================================================================
int EventMaker::string_to_int(std::string theString)
{
  std::istringstream data_s;
  data_s.str(theString)	  ;
  int n			  ;
  data_s >> n		  ;
  
  return n                ;
}
//=========================================
void EventMaker::design25Encode(EventMaker::hitDef &aHit)
{
  if (aHit.col % 2!=0)
  {
    aHit.row *= 2;
    aHit.row++;
    aHit.col = (aHit.col-1)/2;
  }
  else 
  {
    aHit.row *= 2;
    aHit.col = aHit.col/2;
  }
}
