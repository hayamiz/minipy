#!/bin/bash

function main(){
local programs
programs=$(find ./pybench -name "*.py")
for program in $programs; do
	python $program > /dev/null
	echo "run: $program"
	time (for i in $(seq 0 19); do python $program > /dev/null; done)
done
}

main

# for i in {0..4}; do ./vmpy testdata-parser/basics/big_dict.py > /dev/null; done
# echo "run: testdata-parser/basics/big_dict.py"; time for i in {0..9}; do ./vmpy testdata-parser/basics/big_dict.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/basics/big_list.py > /dev/null; done
# echo "run: testdata-parser/basics/big_list.py"; time for i in {0..9}; do ./vmpy testdata-parser/basics/big_list.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/basics/test_dict.py > /dev/null; done
# echo "run: testdata-parser/basics/test_dict.py"; time for i in {0..9}; do ./vmpy testdata-parser/basics/test_dict.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/basics/test_list.py > /dev/null; done
# echo "run: testdata-parser/basics/test_list.py"; time for i in {0..9}; do ./vmpy testdata-parser/basics/test_list.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/basics/test_number.py > /dev/null; done
# echo "run: testdata-parser/basics/test_number.py"; time for i in {0..9}; do ./vmpy testdata-parser/basics/test_number.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/basics/test_string.py > /dev/null; done
# echo "run: testdata-parser/basics/test_string.py"; time for i in {0..9}; do ./vmpy testdata-parser/basics/test_string.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/basics/test_tuple.py > /dev/null; done
# echo "run: testdata-parser/basics/test_tuple.py"; time for i in {0..9}; do ./vmpy testdata-parser/basics/test_tuple.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/full/graph.py > /dev/null; done
# echo "run: testdata-parser/full/graph.py"; time for i in {0..9}; do ./vmpy testdata-parser/full/graph.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/full/mst.py > /dev/null; done
# echo "run: testdata-parser/full/mst.py"; time for i in {0..9}; do ./vmpy testdata-parser/full/mst.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/full/nbody.py > /dev/null; done
# echo "run: testdata-parser/full/nbody.py"; time for i in {0..9}; do ./vmpy testdata-parser/full/nbody.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/full/qsort.py > /dev/null; done
# echo "run: testdata-parser/full/qsort.py"; time for i in {0..9}; do ./vmpy testdata-parser/full/qsort.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/full/sor.py > /dev/null; done
# echo "run: testdata-parser/full/sor.py"; time for i in {0..9}; do ./vmpy testdata-parser/full/sor.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/graphics/draw3D.py > /dev/null; done
# echo "run: testdata-parser/graphics/draw3D.py"; time for i in {0..9}; do ./vmpy testdata-parser/graphics/draw3D.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/graphics/drawgraph.py > /dev/null; done
# echo "run: testdata-parser/graphics/drawgraph.py"; time for i in {0..9}; do ./vmpy testdata-parser/graphics/drawgraph.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/graphics/drawmap.py > /dev/null; done
# echo "run: testdata-parser/graphics/drawmap.py"; time for i in {0..9}; do ./vmpy testdata-parser/graphics/drawmap.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/graphics/newton.py > /dev/null; done
# echo "run: testdata-parser/graphics/newton.py"; time for i in {0..9}; do ./vmpy testdata-parser/graphics/newton.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/graphics/taylor.py > /dev/null; done
# echo "run: testdata-parser/graphics/taylor.py"; time for i in {0..9}; do ./vmpy testdata-parser/graphics/taylor.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/simple/fact.py > /dev/null; done
# echo "run: testdata-parser/simple/fact.py"; time for i in {0..9}; do ./vmpy testdata-parser/simple/fact.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/simple/fib.py > /dev/null; done
# echo "run: testdata-parser/simple/fib.py"; time for i in {0..9}; do ./vmpy testdata-parser/simple/fib.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/simple/gcd.py > /dev/null; done
# echo "run: testdata-parser/simple/gcd.py"; time for i in {0..9}; do ./vmpy testdata-parser/simple/gcd.py > /dev/null; done
# for i in {0..4}; do ./vmpy testdata-parser/simple/integral.py > /dev/null; done
# echo "run: testdata-parser/simple/integral.py"; time for i in {0..9}; do ./vmpy testdata-parser/simple/integral.py > /dev/null; done
