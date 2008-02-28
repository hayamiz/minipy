for i in {0..4}; do python pybench/graphics/newton.py > /dev/null; done
echo "run: pybench/graphics/newton.py"; time for i in {0..9}; do python pybench/graphics/newton.py > /dev/null; done
for i in {0..4}; do python pybench/graphics/taylor.py > /dev/null; done
echo "run: pybench/graphics/taylor.py"; time for i in {0..9}; do python pybench/graphics/taylor.py > /dev/null; done
for i in {0..4}; do python pybench/graphics/drawmap.py > /dev/null; done
echo "run: pybench/graphics/drawmap.py"; time for i in {0..9}; do python pybench/graphics/drawmap.py > /dev/null; done
for i in {0..4}; do python pybench/graphics/drawgraph.py > /dev/null; done
echo "run: pybench/graphics/drawgraph.py"; time for i in {0..9}; do python pybench/graphics/drawgraph.py > /dev/null; done
for i in {0..4}; do python pybench/graphics/draw3D.py > /dev/null; done
echo "run: pybench/graphics/draw3D.py"; time for i in {0..9}; do python pybench/graphics/draw3D.py > /dev/null; done
for i in {0..4}; do python pybench/full/qsort.py > /dev/null; done
echo "run: pybench/full/qsort.py"; time for i in {0..9}; do python pybench/full/qsort.py > /dev/null; done
for i in {0..4}; do python pybench/full/nbody.py > /dev/null; done
echo "run: pybench/full/nbody.py"; time for i in {0..9}; do python pybench/full/nbody.py > /dev/null; done
for i in {0..4}; do python pybench/full/graph.py > /dev/null; done
echo "run: pybench/full/graph.py"; time for i in {0..9}; do python pybench/full/graph.py > /dev/null; done
for i in {0..4}; do python pybench/full/mst.py > /dev/null; done
echo "run: pybench/full/mst.py"; time for i in {0..9}; do python pybench/full/mst.py > /dev/null; done
for i in {0..4}; do python pybench/full/sor.py > /dev/null; done
echo "run: pybench/full/sor.py"; time for i in {0..9}; do python pybench/full/sor.py > /dev/null; done
for i in {0..4}; do python pybench/basics/test_tuple.py > /dev/null; done
echo "run: pybench/basics/test_tuple.py"; time for i in {0..9}; do python pybench/basics/test_tuple.py > /dev/null; done
for i in {0..4}; do python pybench/basics/test_list.py > /dev/null; done
echo "run: pybench/basics/test_list.py"; time for i in {0..9}; do python pybench/basics/test_list.py > /dev/null; done
for i in {0..4}; do python pybench/basics/big_list.py > /dev/null; done
echo "run: pybench/basics/big_list.py"; time for i in {0..9}; do python pybench/basics/big_list.py > /dev/null; done
for i in {0..4}; do python pybench/basics/test_dict.py > /dev/null; done
echo "run: pybench/basics/test_dict.py"; time for i in {0..9}; do python pybench/basics/test_dict.py > /dev/null; done
for i in {0..4}; do python pybench/basics/big_dict.py > /dev/null; done
echo "run: pybench/basics/big_dict.py"; time for i in {0..9}; do python pybench/basics/big_dict.py > /dev/null; done
for i in {0..4}; do python pybench/basics/test_string.py > /dev/null; done
echo "run: pybench/basics/test_string.py"; time for i in {0..9}; do python pybench/basics/test_string.py > /dev/null; done
for i in {0..4}; do python pybench/basics/test_number.py > /dev/null; done
echo "run: pybench/basics/test_number.py"; time for i in {0..9}; do python pybench/basics/test_number.py > /dev/null; done
for i in {0..4}; do python pybench/simple/fact.py > /dev/null; done
echo "run: pybench/simple/fact.py"; time for i in {0..9}; do python pybench/simple/fact.py > /dev/null; done
for i in {0..4}; do python pybench/simple/gcd.py > /dev/null; done
echo "run: pybench/simple/gcd.py"; time for i in {0..9}; do python pybench/simple/gcd.py > /dev/null; done
for i in {0..4}; do python pybench/simple/fib.py > /dev/null; done
echo "run: pybench/simple/fib.py"; time for i in {0..9}; do python pybench/simple/fib.py > /dev/null; done
for i in {0..4}; do python pybench/simple/integral.py > /dev/null; done
echo "run: pybench/simple/integral.py"; time for i in {0..9}; do python pybench/simple/integral.py > /dev/null; done
