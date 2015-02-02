#!bin/bash

export LCIO=/home/iwsatlas1/terzo/utility/ClusterAnalysis/lcio/v01-60

if [[ -s ${LCIO} ]]
  then
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LCIO/lib/
  else
   unset LCIO
fi

export suffix=`pwd`/bin
export PATH=$PATH:$suffix

