
OBJ_FILES = char_stream.o tokenizer.o parser.o syntax_tree.o symbol.o env.o pyvalues.o native.o debug.o tivi.o vminsns.o translator.o printer.o eval.o

TEST_OBJ_FILES = tests/testRunner.o

LIBS = -lpthread

# CPP_FLAGS = -g -Wall -fno-default-inline -pg
CPP_FLAGS = -g -Wall -fno-default-inline
# CPP_FLAGS = -Wall -O3 -g -DRELEASE_BUILD

all: vminsns.inc bootstrap.inc
	make minipy
	make vmpy

minipy: ${OBJ_FILES} minipy.o
	g++ ${CPP_FLAGS} ${OBJ_FILES} minipy.o -o minipy ${LIBS}
	@echo "Compile \(^O^)/ OK"

bootstrap.inc : etc/gen_bootstrap.rb
	./etc/gen_bootstrap.rb > bootstrap.inc

minipy.o : minipy.cpp minipy.hpp bootstrap.inc
	g++ ${CPP_FLAGS} -c $<

vmpy : ${OBJ_FILES} vmpy.o 
	g++ ${CPP_FLAGS} -o vmpy ${LIBS} ${OBJ_FILES} vmpy.o

dispatchtable.inc : etc/vmcodegen.lisp
	./etc/vmcodegen.lisp dispatchtable 2> /dev/null 1> dispatchtable.inc

vminsns.inc : etc/vmcodegen.lisp
	./etc/vmcodegen.lisp insns 2> /dev/null 1> vminsns.inc

vminsns_print.inc : etc/vmcodegen.lisp
	./etc/vmcodegen.lisp print 2> /dev/null 1> vminsns_print.inc

vminsns.o : vminsns.cpp vminsns.hpp vminsns.inc vminsns_print.inc
	g++ ${CPP_FLAGS} -c $<

translator.o : translator.cpp translator.hpp vminsns.o
	g++ ${CPP_FLAGS} -c $<

tivi.o : tivi.cpp tivi.hpp translator.o bootstrap.inc dispatchtable.inc
	g++ ${CPP_FLAGS} -c $<

%.o : %.cpp %.hpp
	g++ ${CPP_FLAGS} -c $<

clean :
	rm *.o *.cpp~ *.hpp~ *.inc *.inc~ minipy vmpy # minipy vmpy test_tokenizer test_parser

test :
	make tokenizer
	make parser
	make test_run

test_parser: test_parser.cpp test_parser.hpp ${OBJ_FILES}
	g++ ${CPP_FLAGS} test_parser.cpp ${OBJ_FILES} -o test_parser ${LIBS}

test_tokenizer: test_tokenizer.cpp test_tokenizer.hpp ${OBJ_FILES}
	g++ ${CPP_FLAGS} test_tokenizer.cpp ${OBJ_FILES} -o test_tokenizer ${LIBS}

test_run: vmpy minipy
	for file in `find testdata-parser/simple testdata-parser/basics/ -name "*.py"`; do echo $$file; ./vmpy $$file | diff - $$file.right && perl -e "print ' ' x 60;"&& echo "[ \(^O^)/ success ]"; done

test_pyvalues:
	make pyvalues.o
	cd tests; make test_pyvalues

tokenizer :
	make test_tokenizer
# (minipy-insert-tokenizer-test)
	@echo "** Tokenizer normal test **"
	@echo "Tokenizer test: prob_1.py"
	@./test_tokenizer testdata-tokenizer/prob_1.py | diff -uB - ./testdata-tokenizer/ans_1
	@perl -e "print ' ' x 60;"
	@echo "[ \(^O^)/ success ]"
	@echo "Tokenizer test: prob_2.py"
	@./test_tokenizer testdata-tokenizer/prob_2.py | diff -uB - ./testdata-tokenizer/ans_2
	@perl -e "print ' ' x 60;"
	@echo "[ \(^O^)/ success ]"
	@echo "Tokenizer test: prob_3.py"
	@./test_tokenizer testdata-tokenizer/prob_3.py | diff -uB - ./testdata-tokenizer/ans_3
	@perl -e "print ' ' x 60;"
	@echo "[ \(^O^)/ success ]"
	@echo "Tokenizer test: prob_4.py"
	@./test_tokenizer testdata-tokenizer/prob_4.py | diff -uB - ./testdata-tokenizer/ans_4
	@perl -e "print ' ' x 60;"
	@echo "[ \(^O^)/ success ]"
	@echo "Tokenizer test: prob_5.py"
	@./test_tokenizer testdata-tokenizer/prob_5.py | diff -uB - ./testdata-tokenizer/ans_5
	@perl -e "print ' ' x 60;"
	@echo "[ \(^O^)/ success ]"
	@echo "Tokenizer test: prob_6.py"
	@./test_tokenizer testdata-tokenizer/prob_6.py | diff -uB - ./testdata-tokenizer/ans_6
	@perl -e "print ' ' x 60;"
	@echo "[ \(^O^)/ success ]"
	@echo "** Tokenizer ABNORMAL test **"
	@echo "Tokenizer abnormal test (MUST fail): err_prob_1.py"
	@-./test_tokenizer testdata-tokenizer/err_prob_1.py 1> /dev/null
	@echo "Tokenizer abnormal test (MUST fail): err_prob_2.py"
	@-./test_tokenizer testdata-tokenizer/err_prob_2.py 1> /dev/null
	@echo "Tokenizer abnormal test (MUST fail): err_prob_3.py"
	@-./test_tokenizer testdata-tokenizer/err_prob_3.py 1> /dev/null
	@echo "Tokenizer abnormal test (MUST fail): err_prob_4.py"
	@-./test_tokenizer testdata-tokenizer/err_prob_4.py 1> /dev/null
# ---- tokenizer test end ----

parser : 
	make test_parser
# (minipy-insert-parser-test)
	@echo "Parser test: ./testdata-parser/basics/big_list.py"
	@./test_parser ./testdata-parser/basics/big_list.py > ./testdata-parser/basics/big_list.py.parsed
	@./test_parser ./testdata-parser/basics/big_list.py.parsed > ./testdata-parser/basics/big_list.py.parsed.parsed
	@diff -uB ./testdata-parser/basics/big_list.py.parsed ./testdata-parser/basics/big_list.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/basics/big_list.py.parsed ./testdata-parser/basics/big_list.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/basics/test_dict.py"
	@./test_parser ./testdata-parser/basics/test_dict.py > ./testdata-parser/basics/test_dict.py.parsed
	@./test_parser ./testdata-parser/basics/test_dict.py.parsed > ./testdata-parser/basics/test_dict.py.parsed.parsed
	@diff -uB ./testdata-parser/basics/test_dict.py.parsed ./testdata-parser/basics/test_dict.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/basics/test_dict.py.parsed ./testdata-parser/basics/test_dict.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/basics/test_tuple.py"
	@./test_parser ./testdata-parser/basics/test_tuple.py > ./testdata-parser/basics/test_tuple.py.parsed
	@./test_parser ./testdata-parser/basics/test_tuple.py.parsed > ./testdata-parser/basics/test_tuple.py.parsed.parsed
	@diff -uB ./testdata-parser/basics/test_tuple.py.parsed ./testdata-parser/basics/test_tuple.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/basics/test_tuple.py.parsed ./testdata-parser/basics/test_tuple.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/basics/test_list.py"
	@./test_parser ./testdata-parser/basics/test_list.py > ./testdata-parser/basics/test_list.py.parsed
	@./test_parser ./testdata-parser/basics/test_list.py.parsed > ./testdata-parser/basics/test_list.py.parsed.parsed
	@diff -uB ./testdata-parser/basics/test_list.py.parsed ./testdata-parser/basics/test_list.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/basics/test_list.py.parsed ./testdata-parser/basics/test_list.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/basics/test_string.py"
	@./test_parser ./testdata-parser/basics/test_string.py > ./testdata-parser/basics/test_string.py.parsed
	@./test_parser ./testdata-parser/basics/test_string.py.parsed > ./testdata-parser/basics/test_string.py.parsed.parsed
	@diff -uB ./testdata-parser/basics/test_string.py.parsed ./testdata-parser/basics/test_string.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/basics/test_string.py.parsed ./testdata-parser/basics/test_string.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/basics/test_number.py"
	@./test_parser ./testdata-parser/basics/test_number.py > ./testdata-parser/basics/test_number.py.parsed
	@./test_parser ./testdata-parser/basics/test_number.py.parsed > ./testdata-parser/basics/test_number.py.parsed.parsed
	@diff -uB ./testdata-parser/basics/test_number.py.parsed ./testdata-parser/basics/test_number.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/basics/test_number.py.parsed ./testdata-parser/basics/test_number.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/basics/big_dict.py"
	@./test_parser ./testdata-parser/basics/big_dict.py > ./testdata-parser/basics/big_dict.py.parsed
	@./test_parser ./testdata-parser/basics/big_dict.py.parsed > ./testdata-parser/basics/big_dict.py.parsed.parsed
	@diff -uB ./testdata-parser/basics/big_dict.py.parsed ./testdata-parser/basics/big_dict.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/basics/big_dict.py.parsed ./testdata-parser/basics/big_dict.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/simple/integral.py"
	@./test_parser ./testdata-parser/simple/integral.py > ./testdata-parser/simple/integral.py.parsed
	@./test_parser ./testdata-parser/simple/integral.py.parsed > ./testdata-parser/simple/integral.py.parsed.parsed
	@diff -uB ./testdata-parser/simple/integral.py.parsed ./testdata-parser/simple/integral.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/simple/integral.py.parsed ./testdata-parser/simple/integral.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/simple/gcd.py"
	@./test_parser ./testdata-parser/simple/gcd.py > ./testdata-parser/simple/gcd.py.parsed
	@./test_parser ./testdata-parser/simple/gcd.py.parsed > ./testdata-parser/simple/gcd.py.parsed.parsed
	@diff -uB ./testdata-parser/simple/gcd.py.parsed ./testdata-parser/simple/gcd.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/simple/gcd.py.parsed ./testdata-parser/simple/gcd.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/simple/fib.py"
	@./test_parser ./testdata-parser/simple/fib.py > ./testdata-parser/simple/fib.py.parsed
	@./test_parser ./testdata-parser/simple/fib.py.parsed > ./testdata-parser/simple/fib.py.parsed.parsed
	@diff -uB ./testdata-parser/simple/fib.py.parsed ./testdata-parser/simple/fib.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/simple/fib.py.parsed ./testdata-parser/simple/fib.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/simple/fact.py"
	@./test_parser ./testdata-parser/simple/fact.py > ./testdata-parser/simple/fact.py.parsed
	@./test_parser ./testdata-parser/simple/fact.py.parsed > ./testdata-parser/simple/fact.py.parsed.parsed
	@diff -uB ./testdata-parser/simple/fact.py.parsed ./testdata-parser/simple/fact.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/simple/fact.py.parsed ./testdata-parser/simple/fact.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/full/qsort.py"
	@./test_parser ./testdata-parser/full/qsort.py > ./testdata-parser/full/qsort.py.parsed
	@./test_parser ./testdata-parser/full/qsort.py.parsed > ./testdata-parser/full/qsort.py.parsed.parsed
	@diff -uB ./testdata-parser/full/qsort.py.parsed ./testdata-parser/full/qsort.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/full/qsort.py.parsed ./testdata-parser/full/qsort.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/full/mst.py"
	@./test_parser ./testdata-parser/full/mst.py > ./testdata-parser/full/mst.py.parsed
	@./test_parser ./testdata-parser/full/mst.py.parsed > ./testdata-parser/full/mst.py.parsed.parsed
	@diff -uB ./testdata-parser/full/mst.py.parsed ./testdata-parser/full/mst.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/full/mst.py.parsed ./testdata-parser/full/mst.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/full/nbody.py"
	@./test_parser ./testdata-parser/full/nbody.py > ./testdata-parser/full/nbody.py.parsed
	@./test_parser ./testdata-parser/full/nbody.py.parsed > ./testdata-parser/full/nbody.py.parsed.parsed
	@diff -uB ./testdata-parser/full/nbody.py.parsed ./testdata-parser/full/nbody.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/full/nbody.py.parsed ./testdata-parser/full/nbody.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/full/sor.py"
	@./test_parser ./testdata-parser/full/sor.py > ./testdata-parser/full/sor.py.parsed
	@./test_parser ./testdata-parser/full/sor.py.parsed > ./testdata-parser/full/sor.py.parsed.parsed
	@diff -uB ./testdata-parser/full/sor.py.parsed ./testdata-parser/full/sor.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/full/sor.py.parsed ./testdata-parser/full/sor.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/full/graph.py"
	@./test_parser ./testdata-parser/full/graph.py > ./testdata-parser/full/graph.py.parsed
	@./test_parser ./testdata-parser/full/graph.py.parsed > ./testdata-parser/full/graph.py.parsed.parsed
	@diff -uB ./testdata-parser/full/graph.py.parsed ./testdata-parser/full/graph.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/full/graph.py.parsed ./testdata-parser/full/graph.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/graphics/taylor.py"
	@./test_parser ./testdata-parser/graphics/taylor.py > ./testdata-parser/graphics/taylor.py.parsed
	@./test_parser ./testdata-parser/graphics/taylor.py.parsed > ./testdata-parser/graphics/taylor.py.parsed.parsed
	@diff -uB ./testdata-parser/graphics/taylor.py.parsed ./testdata-parser/graphics/taylor.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/graphics/taylor.py.parsed ./testdata-parser/graphics/taylor.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/graphics/drawmap.py"
	@./test_parser ./testdata-parser/graphics/drawmap.py > ./testdata-parser/graphics/drawmap.py.parsed
	@./test_parser ./testdata-parser/graphics/drawmap.py.parsed > ./testdata-parser/graphics/drawmap.py.parsed.parsed
	@diff -uB ./testdata-parser/graphics/drawmap.py.parsed ./testdata-parser/graphics/drawmap.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/graphics/drawmap.py.parsed ./testdata-parser/graphics/drawmap.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/graphics/drawgraph.py"
	@./test_parser ./testdata-parser/graphics/drawgraph.py > ./testdata-parser/graphics/drawgraph.py.parsed
	@./test_parser ./testdata-parser/graphics/drawgraph.py.parsed > ./testdata-parser/graphics/drawgraph.py.parsed.parsed
	@diff -uB ./testdata-parser/graphics/drawgraph.py.parsed ./testdata-parser/graphics/drawgraph.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/graphics/drawgraph.py.parsed ./testdata-parser/graphics/drawgraph.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/graphics/draw3D.py"
	@./test_parser ./testdata-parser/graphics/draw3D.py > ./testdata-parser/graphics/draw3D.py.parsed
	@./test_parser ./testdata-parser/graphics/draw3D.py.parsed > ./testdata-parser/graphics/draw3D.py.parsed.parsed
	@diff -uB ./testdata-parser/graphics/draw3D.py.parsed ./testdata-parser/graphics/draw3D.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/graphics/draw3D.py.parsed ./testdata-parser/graphics/draw3D.py.parsed.parsed
	@echo "Parser test: ./testdata-parser/graphics/newton.py"
	@./test_parser ./testdata-parser/graphics/newton.py > ./testdata-parser/graphics/newton.py.parsed
	@./test_parser ./testdata-parser/graphics/newton.py.parsed > ./testdata-parser/graphics/newton.py.parsed.parsed
	@diff -uB ./testdata-parser/graphics/newton.py.parsed ./testdata-parser/graphics/newton.py.parsed.parsed
	@echo "                                                            [ \(^O^)/ success ]"
	@rm ./testdata-parser/graphics/newton.py.parsed ./testdata-parser/graphics/newton.py.parsed.parsed
# ---- end of minipy-insert-parser-test ----

check-syntax:
	gcc -Wall -fsyntax-only $(CHK_SOURCES)
