EXELIST = fei4Analyzer 

CC = /usr/local/opt/llvm/bin/clang

SRCDIR  = src
INCDIR  = inc
OBJDIR  = obj
ifdef suffix
 BINDIR  = $(suffix)
else
 BINDIR  = ./bin/
endif

OBJS = obj/EventMaker.o	obj/USBpixEventMaker.o obj/CosmicEventMaker.o obj/Clusterizer.o obj/Plotter.o obj/Calibrator.o obj/Fitter.o obj/fei4TelEventMaker.o obj/tbtrackEventMaker.o

ifdef USE_LCIO
 LCIOINC= -I$(USE_LCIO)/include  -I$(USE_LCIO)/sio/include
 LCIOLIBS= -L$(USE_LCIO)/lib -llcio -L$(USE_LCIO)/sio/lib -lsio -lz
 OBJS += obj/LCIOEventMaker.o
 ENVVAR = -D USE_LCIO
endif

#XERCES-C-INC = /usr/include/xercesc
#XERCES-C-LIB = /usr/lib

BOOSTINC    = /opt/local/include
BOOSTLIB    = /opt/local/lib

OPTIMIZER_FLAGS = -O3 -Wall -Wextra  -fopenmp

INCFLAGS = -I$(INCDIR)            \
           -I$(BOOSTINC)            \
	   $(LCIOINC) 	\
-I/usr/local/opt/llvm/include\
	   `root-config --cflags`
	   
LIBFLAGS = -lc++ -lMinuit -L/usr/local/opt/llvm/lib -lboost_regex-mt -L$(BOOSTLIB) $(LCIOLIBS) `root-config --libs`
	   
CCFLAGS = $(INCFLAGS) $(OPTIMIZER_FLAGS)

HERE   := $(shell pwd)

CPPVERBOSE = 0
		

.PHONY : all 

#---------------------------------------------------------------#
all : CHECK dependencies $(EXELIST)                             # Main sequence ----------------------#
#---------------------------------------------------------------#

#--------------------------------------------------------------------------------------------------------#
CHECK : 
	@clear
	@echo ' '
	@echo '[1;33m[1m>========== [7m' 'Building fei4Analyzer in $(HERE) [0m' '[1;33m[1m================[0m'
	@echo ' '
	@echo ' '

#--------------------------------------------------------------------------------------------------------#
dependencies : 
	@echo '            [1;32m[1m>>********** Making main sequence dependencies ***********<<[0m'
	@if [ ! -e dependencies ] ; then touch dependencies ;fi
        ifdef CPPVERBOSE
	  ${CC} -MM $(SRCDIR)/*.cpp $(CCFLAGS) | sed 's/.*\.o:/$(OBJDIR)\/&/' >  dependencies
        else
	 @${CC} -MM $(SRCDIR)/*.cpp $(CCFLAGS) | sed 's/.*\.o:/$(OBJDIR)\/&/' >  dependencies
        endif
#--------------------------------------------------------------------------------------------------------#
fei4Analyzer : fei4Analyzer.C                                            \
	       $(OBJS)  					      \
               inc/ANSIColors.h 				 \
               inc/macros.h
	@      echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC)    -o $@					   \
	           $<                                      \
		   $(OBJS)					\
		   $(CCFLAGS)		                   \
		   $(LIBFLAGS)  			\
		   $(ENVVAR)
        else
	 @$(CC) -o $@					   \
	           $<                                      \
		   $(OBJS)					\
		   $(CCFLAGS)		                   \
		   $(LIBFLAGS)		   
        endif
	
#--------------------------------------------------------------------------------------------------------#
obj/EventMaker.o : src/EventMaker.cpp     \
                             inc/EventMaker.h     \
                             inc/macros.h            \
			     inc/ANSIColors.h     \
			     inc/FormattedRecord.hh
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        else
	 @$(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        endif
#--------------------------------------------------------------------------------------------------------#
obj/USBpixEventMaker.o : src/USBpixEventMaker.cpp     \
                             inc/USBpixEventMaker.h     \
                             inc/EventMaker.h     \
                             inc/macros.h            \
			     inc/ANSIColors.h     \
			     inc/FormattedRecord.hh
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        else
	 @$(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        endif
#--------------------------------------------------------------------------------------------------------#
obj/CosmicEventMaker.o : src/CosmicEventMaker.cpp     \
                             inc/CosmicEventMaker.h     \
                             inc/EventMaker.h     \
                             inc/macros.h            \
			     inc/ANSIColors.h     \
			     inc/FormattedRecord.hh
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        else
	 @$(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        endif
#--------------------------------------------------------------------------------------------------------#
obj/LCIOEventMaker.o : src/LCIOEventMaker.cpp     \
                       inc/LCIOEventMaker.h     \
                       inc/EventMaker.h     \
                       inc/macros.h	       \
		       inc/ANSIColors.h     \
		       inc/FormattedRecord.hh
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        else
	 @$(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        endif
#--------------------------------------------------------------------------------------------------------#
obj/fei4TelEventMaker.o : src/fei4TelEventMaker.cpp     \
                          inc/fei4TelEventMaker.h     \
                          inc/EventMaker.h     \
                          inc/macros.h  	  \
		          inc/ANSIColors.h     \
		          inc/FormattedRecord.hh
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        else
	 @$(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        endif
#--------------------------------------------------------------------------------------------------------#
obj/tbtrackEventMaker.o : src/tbtrackEventMaker.cpp     \
                          inc/tbtrackEventMaker.h     \
                          inc/EventMaker.h     \
                          inc/macros.h  	  \
		          inc/ANSIColors.h     \
		          inc/FormattedRecord.hh
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        else
	 @$(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        endif
#--------------------------------------------------------------------------------------------------------#
obj/Clusterizer.o : src/Clusterizer.cpp     \
                   inc/Clusterizer.h	\
		   inc/EventMaker.h \
                   inc/macros.h 	   \
		   inc/ANSIColors.h	
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        else
	 @$(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        endif
	
#--------------------------------------------------------------------------------------------------------#
obj/Plotter.o :    src/Plotter.cpp     \
                   inc/Plotter.h	\
		   inc/Clusterizer.h \
		   inc/Calibrator.h \
		   inc/Fitter.h \
                   inc/macros.h 	   \
		   inc/ANSIColors.h	
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@              \
	              $<  	      \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        else
	 @$(CC) -c -o $@              \
	              $<  	      \
		      obj/Clusterizer.o              \
		      $(CCFLAGS)                              \
		      `root-config --cflags`
        endif
#--------------------------------------------------------------------------------------------------------#
obj/Calibrator.o : src/Calibrator.cpp \
		   inc/Calibrator.h \
		   inc/EventMaker.h \
                   inc/macros.h 	   \
		   inc/ANSIColors.h
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@					   \
	           $<                                      \
		   $(CCFLAGS)		                   \
		   $(LIBFLAGS)
        else
	 @$(CC) -c -o $@					   \
	           $<                                      \
		   $(CCFLAGS)		                   \
		   $(LIBFLAGS)		   
        endif
#--------------------------------------------------------------------------------------------------------#
obj/Fitter.o : src/Fitter.cpp \
		   inc/Fitter.h \
                   inc/macros.h 	   \
		   inc/ANSIColors.h
	@echo " "
	@echo '            [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) -c -o $@					   \
	           $<                                      \
		   $(CCFLAGS)		                   \
		   $(LIBFLAGS)
        else
	 @$(CC) -c -o $@					   \
	           $<                                      \
		   $(CCFLAGS)		                   \
		   $(LIBFLAGS)		   
        endif
#--------------------------------------------------------------------------------------------------------#
clean:
	@echo ' '
	@echo '[1;32m[1m----------- Main sequence clean up --------------------------------------------------[0m'
        ifdef CPPVERBOSE
	  rm -rf $(OBJDIR)/*.o *~  $(EXELIST) $(BINDIR)/$(EXELIST) $(dependencies)
        else
	 @rm -rf $(OBJDIR)/*.o *~  $(EXELIST) dependencies
        endif
	@echo ' '
	@echo ' '
#--------------------------------------------------------------------------------------------------------#
install:
	@echo ' '
	@echo '[1;32m[1m----------- Main sequence install --------------------------------------------------[0m'
        ifdef CPPVERBOSE
	  ln -fv $(EXELIST) $(BINDIR)
        else
	 @ln -fv $(EXELIST) $(BINDIR)
        endif
	@echo ' '
	@echo ' '
