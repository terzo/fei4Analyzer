#!bin/bash

export USE_LCIO=/home/iwsatlas1/terzo/utility/ClusterAnalysis/lcio/v01-60

if [[ -s ${USE_LCIO} ]]
  then
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${USE_LCIO}/lib/
  else
   unset USE_LCIO
fi

export suffix=`pwd`/bin
export PATH=$PATH:$suffix

