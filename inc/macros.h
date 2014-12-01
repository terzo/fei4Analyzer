/*______________________________________+
|                                       |
| Author: D. Menasce                    |
+______________________________________*/

#ifndef __CUTTREEMACROS__
#define __CUTTREEMACROS__

#include <iostream>
#include <sstream>
#include <algorithm>

#include "ANSIColors.h"

#define STDLINE(MSG,COLOR)    	                             \
        {                                                    \
         std::stringstream msg_ ;                            \
         std::stringstream PFs_ ;                            \
         std::string PF_ = __PRETTY_FUNCTION__ ;             \
         int PFSize = PF_.size() ;                           \
         int maxL = 90 ;                                     \
         if(PFSize > 30) {PFSize = 30 ;}                     \
         for(int i=0; i<PFSize; ++i) {PFs_<<PF_[i];}         \
         if(PFSize<(int)PF_.size())                          \
          {maxL -= 3; PFs_<<ACRed<<"...";}                   \
         msg_ << ACCyan  << ACBold                           \
              << __LINE__                                    \
              << ACPlain << ACYellow                         \
              << "\t] ["                                     \
              << ACPlain                                     \
              << ACWhite << ACBold                           \
              << PFs_.str()                                  \
              << ACPlain << ACYellow                         \
              << "]"  ;                                      \
         int blankSize = maxL - msg_.str().size() + 1 ;      \
         if( blankSize < 0 ) blankSize = 2 ;                 \
         std::string blanks = "" ;                           \
         for(int i=0; i<blankSize; ++i)                      \
         {                                                   \
          blanks += " " ;                                    \
         }                                                   \
         std::cout << msg_.str()                             \
                   << blanks                                 \
                   << COLOR << ACBold                        \
                   << MSG                                    \
                   << ACPlain                                \
                   << std::endl ;                            \
        }

#endif
