#!bin/bash

#uncomment to set the LCIO path here
#export LCIO=

if [[ -s ${LCIO} ]]
  then
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${LCIO}/lib/
  else
   unset LCIO
fi

export suffix=`pwd`/bin
export PATH=$PATH:$suffix

#uncomment to use LCIO files
unset LCIO

