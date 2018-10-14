COMPILER=gcc
BINARIES=depth_map quadtree
CFLAGS=-Wall -g -std=c99
CUNIT=-L/home/ff/cs61c/cunit/install/lib -I/home/ff/cs61c/cunit/install/include -lcunit

.PHONY: clean all run-unit-tests

default: all

all: ${BINARIES}

depth_map: calc_depth.c depth_map.c utils.c
	${COMPILER} ${CFLAGS} -o $@ $^ -lm

quadtree: quadtree.c make_qtree.c utils.c
	${COMPILER} ${CFLAGS} -o $@ $^ -lm

unit-tests: calc_depth.c make_qtree.c test/cunit/unit-tests.c
	${COMPILER} ${CFLAGS} -o $@ test/cunit/unit-tests.c -lm ${CUNIT}

run-unit-tests:	unit-tests
	./$^

check: ${BINARIES}
	python2 check.py

clean:
	rm -rf ${BINARIES} test/output/* unit-tests

