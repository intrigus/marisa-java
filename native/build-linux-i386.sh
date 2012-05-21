#!/bin/bash
OUTDIR=.out/Linux/i386
CXX=g++

[ -z "$JAVA_HOME" ] && JAVA_HOME=/usr/java/default

CXXFLAGS="-Wall -I$JAVA_HOME/include -I$JAVA_HOME/include/linux -I./marisa-0.2.0/lib -g -O2 -fPIC -m32"
LINKFLAGS="-shared"

[ -e $OUTDIR ] || mkdir -p $OUTDIR
rm -rf $OUTDIR/* > /dev/null

find src -name '*.cpp' | while read cc
do
  basename $cc
  $CXX $CXXFLAGS -c -o $OUTDIR/$(basename $cc | sed -e 's/\.cpp/\.o/') $cc
done

find marisa-0.2.0/ -name '*.cc' | while read cc
do
  basename $cc
  $CXX $CXXFLAGS -c -o $OUTDIR/$(basename $cc | sed -e 's/\.cc/\.o/') $cc
done

echo libmarisa-java.so
$CXX $CXXFLAGS $LINKFLAGS -o $OUTDIR/libmarisa-java.so $OUTDIR/*.o

echo build
$CXX $CXXFLAGS -o $OUTDIR/build test/build.cpp $OUTDIR/libmarisa-java.so

echo lookup
$CXX $CXXFLAGS -o $OUTDIR/lookup test/lookup.cpp $OUTDIR/libmarisa-java.so

echo predict
$CXX $CXXFLAGS -o $OUTDIR/predict test/predict.cpp $OUTDIR/libmarisa-java.so
