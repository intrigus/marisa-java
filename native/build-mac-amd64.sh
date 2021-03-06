#!/bin/bash
OUTDIR=./out/Mac/x86_64
CXX=g++

[ -z "$JAVA_HOME" ] && JAVA_HOME=/usr/java/default

CXXFLAGS="-Wall -I/System/Library/Frameworks/JavaVM.framework/Headers -I$JAVA_HOME/include/linux -I./marisa-0.2.0/lib -g -O2 -fPIC -fno-common"
LINKFLAGS="-dynamiclib -undefined suppress -flat_namespace"

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

echo libmarisa-java.dylib
$CXX $CXXFLAGS $LINKFLAGS $OUTDIR/*.o -o $OUTDIR/libmarisa-java.dylib

echo build
$CXX $CXXFLAGS -o $OUTDIR/build test/build.cpp $OUTDIR/libmarisa-java.dylib

echo lookup
$CXX $CXXFLAGS -o $OUTDIR/lookup test/lookup.cpp $OUTDIR/libmarisa-java.dylib

echo predict
$CXX $CXXFLAGS -o $OUTDIR/predict test/predict.cpp $OUTDIR/libmarisa-java.dylib

echo testing libmarisa-java.dylib
java -Djava.library.path=out/Mac/x86_64 LoadJNI
