#!bin/bash

#uncomment to set the LCIO path here
#export USE_LCIO=

if [[ -s ${USE_LCIO} ]]
  then
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${USE_LCIO}/lib/
  else
   unset USE_LCIO
fi

export suffix=`pwd`/bin
export PATH=$PATH:$suffix

#uncomment to use LCIO files
unset USE_LCIO

