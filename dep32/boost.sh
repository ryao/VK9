#!/bin/bash

if [ -z "$(export -p | grep BOOST_INCLUDEDIR\=)" ]; then
  export BOOST_INCLUDEDIR=/var/opt/boost_1_67/include
  BOOST_INCLUDEDIR_SET=false
fi
if [ -z "$(export -p | grep BOOST_LIBRARYDIR\=)" ]; then
  export BOOST_LIBRARYDIR=/var/opt/i686-w64-mingw32/lib
  BOOST_LIBRARYDIR_SET=false
fi

