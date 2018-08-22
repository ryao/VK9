#!/bin/bash

if [ -z $(export -p | grep BOOST_INCLUDEDIR\=) ]; then
  export BOOST_INCLUDEDIR=/var/opt/boost_1_67/include
fi
if [ -z $(export -p | grep BOOST_LIBRARYDIR\=) ]; then
  export BOOST_LIBRARYDIR=/var/opt/x86_64-w64-mingw32/lib
fi

