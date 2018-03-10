#! /bin/bash

srcdir=$PWD
bindir=${srcdir}/.build-and-test

function cleanup ()
{
  [[ -d ${bindir} ]] && rm -r ${bindir}
  exit 1
}
set -e 
trap cleanup EXIT

mkdir ${bindir}
cd ${bindir}
cmake ..
cmake --build .
cmake --build . --target test

echo "PASSED"

