
TEST_OBJ_FILES = testRunner.o

CPP_FLAGS = -g -Wall -ansi

all :
	./generate.lisp all 2> /dev/null
	make test_all.out
	./test_all.out


eval :
	./generate.lisp runner eval 2> /dev/null
	./generate.lisp suite eval 2> /dev/null
	@make test_eval.out
	./test_eval.out

test_eval.out : evalTest.o ../eval.o  ${TEST_OBJ_FILES}
	cd ../ ; make
	g++ ${CPP_FLAGS} ${TEST_OBJ_FILES} evalTest.o ../eval.o  -o test_eval.out -lcppunit

foo :
	./generate.lisp runner foo 2> /dev/null
	./generate.lisp suite foo 2> /dev/null
	@make test_foo.out
	./test_foo.out

test_foo.out : fooTest.o ../foo.o ../baz.o ../bar.o  ${TEST_OBJ_FILES}
	cd ../ ; make
	g++ ${CPP_FLAGS} ${TEST_OBJ_FILES} fooTest.o ../foo.o ../baz.o ../bar.o  -o test_foo.out -lcppunit

native :
	./generate.lisp runner native 2> /dev/null
	./generate.lisp suite native 2> /dev/null
	@make test_native.out
	./test_native.out

test_native.out : nativeTest.o ../native.o ../env.o ../symbol.o ../pyvalues.o  ${TEST_OBJ_FILES}
	cd ../ ; make
	g++ ${CPP_FLAGS} ${TEST_OBJ_FILES} nativeTest.o ../native.o ../env.o ../symbol.o ../pyvalues.o  -o test_native.out -lcppunit

pyvalues :
	./generate.lisp runner pyvalues 2> /dev/null
	./generate.lisp suite pyvalues 2> /dev/null
	@make test_pyvalues.out
	./test_pyvalues.out

test_pyvalues.out : pyvaluesTest.o ../pyvalues.o  ${TEST_OBJ_FILES}
	cd ../ ; make
	g++ ${CPP_FLAGS} ${TEST_OBJ_FILES} pyvaluesTest.o ../pyvalues.o  -o test_pyvalues.out -lcppunit

symbol :
	./generate.lisp runner symbol 2> /dev/null
	./generate.lisp suite symbol 2> /dev/null
	@make test_symbol.out
	./test_symbol.out

test_symbol.out : symbolTest.o ../symbol.o  ${TEST_OBJ_FILES}
	cd ../ ; make
	g++ ${CPP_FLAGS} ${TEST_OBJ_FILES} symbolTest.o ../symbol.o  -o test_symbol.out -lcppunit

translator :
	./generate.lisp runner translator 2> /dev/null
	./generate.lisp suite translator 2> /dev/null
	@make test_translator.out
	./test_translator.out

test_translator.out : translatorTest.o ../translator.o ../symbol.o ../vminsns.o  ${TEST_OBJ_FILES}
	cd ../ ; make
	g++ ${CPP_FLAGS} ${TEST_OBJ_FILES} translatorTest.o ../translator.o ../symbol.o ../vminsns.o  -o test_translator.out -lcppunit


test_all.out: evalTest.o ../eval.o fooTest.o ../foo.o ../baz.o ../bar.o nativeTest.o ../native.o ../env.o ../symbol.o ../pyvalues.o pyvaluesTest.o ../pyvalues.o symbolTest.o ../symbol.o translatorTest.o ../translator.o ../symbol.o ../vminsns.o  ${TEST_OBJ_FILES}
	g++ ${CPP_FLAGS} evalTest.o ../eval.o fooTest.o ../foo.o ../baz.o ../bar.o nativeTest.o ../native.o ../env.o ../symbol.o ../pyvalues.o pyvaluesTest.o ../pyvalues.o symbolTest.o ../symbol.o translatorTest.o ../translator.o ../symbol.o ../vminsns.o  -o test_all.out -lcppunit

testRunner.o : testRunner.cpp
	g++ ${CPP_FLAGS} -c testRunner.cpp

%.o : %.cpp %.hpp 
	g++ ${CPP_FLAGS} -c $<

clean :
	rm *.o *.out evalTest.hpp evalTest.cpp fooTest.hpp fooTest.cpp nativeTest.hpp nativeTest.cpp pyvaluesTest.hpp pyvaluesTest.cpp symbolTest.hpp symbolTest.cpp translatorTest.hpp translatorTest.cpp 

file :
	./generate.lisp makefile 2> /dev/null
