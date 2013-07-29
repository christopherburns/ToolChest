
TOP=$(shell pwd)

BIN = bin
BUILD = build
ASM = asm

CC = clang++ 

CFLAGS = -std=c++11 -O3 -D__APPLE__ -Wunused-value
LDFLAGS = -lstdc++

EXES = testunit profilelinkedlist profilesort profilearray profiletreemap profiletreeset delaunay

delaunay: test/Delaunay.cpp test/Delaunay.h test/PeriodicDelaunay.h test/Bounds.h
	${CC} -o $@ ${CFLAGS} ${LDFLAGS} $<
	mkdir -vp bin; mv -f $@ $(BIN)/;

testunit: test/UnitTests.cpp
	${CC} -o $@ ${CFLAGS} ${LDFLAGS} $<
	mkdir -vp bin; mv -f $@ $(BIN)/;

profilelinkedlist: test/ProfileLinkedList.cpp
	${CC} -o $@ ${CFLAGS} ${LDFLAGS} $<
	mkdir -vp bin; mv -f $@ $(BIN)/;

profilesort: test/ProfileSort.cpp
	${CC} -o $@ ${CFLAGS} ${LDFLAGS} $<
	mkdir -vp bin; mv -f $@ $(BIN)/;

profilearray: test/ProfileArray.cpp
	${CC} -o $@ ${CFLAGS} ${LDFLAGS} $<
	mkdir -vp bin; mv -f $@ $(BIN)/;

profiletreemap: test/ProfileTreeMap.cpp
	${CC} -o $@ ${CFLAGS} ${LDFLAGS} $<
	mkdir -vp bin; mv -f $@ $(BIN)/;

profiletreeset: test/ProfileTreeSet.cpp
	${CC} -o $@ ${CFLAGS} ${LDFLAGS} $<
	mkdir -vp bin; mv -f $@ $(BIN)/;



clean:
	rm -rf $(BIN)/*
