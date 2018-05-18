fei4Analyzer  
============  
Download the fei4analyzer from git: https://github.com/terzo/fei4Analyzer  

If you want to analyse EUDET data after the Eutelescope conversion in ".slcio" files you also need to install this:  
http://lcio.desy.de/  

Check the file setup.sh if you need LCIO file support uncomment the following line and put the path of your LCIO installation:  
export LCIO=where/did/you/install/lcio.h  

The program uses BOOST, check that the path of your BOOST installation are correct in the Makefile, for ex.:  
BOOSTINC = /usr/include/  
BOOSTLIB = /usr/lib/  

For ROOT any version should be fine. Check that the command root-config exists (but it should if root works).

Once everything is set up (no more supported):  
$ source setup.sh  
$ make  
$ make install  

or with CMAKE (officially supported):  
$ source setup.sh  
$ mkdir build  
$ cd build  
$ cmake ..  
$ make -j4  
$ make install  

You can explore the options calling the help:  
$ fei4analyzer -h  

To install on MAC OSX you need to get _llvm_. Try with homebrew (https://brew.sh/):  
$ brew install llvm
 

If you are a Windows user you are not supported.  
