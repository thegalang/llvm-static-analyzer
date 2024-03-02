CC = clang++
CFLAGS = $(shell llvm-config --cxxflags)
LDFLAGS = $(shell llvm-config --ldflags)
LIBS = $(shell llvm-config --libs) -lpthread -lncurses -ldl

all: taint_analysis test1 test2_wipetaint test_task3 test_task3_2 test_task4 example1 example2

test1: test1.c
	clang -fno-discard-value-names -emit-llvm -S -o test1.ll test1.c
	opt -dot-cfg test1.ll
	mv .main.dot test1.dot

test2_wipetaint: test2_wipetaint.c
	clang -fno-discard-value-names -emit-llvm -S -o test2_wipetaint.ll test2_wipetaint.c
	opt -dot-cfg test2_wipetaint.ll
	mv .main.dot test2_wipetaint.dot

test_task3: test_task3.c
	clang -fno-discard-value-names -emit-llvm -S -o test_task3.ll test_task3.c
	opt -dot-cfg test_task3.ll
	mv .main.dot test_task3.dot

test_task3_2: test_task3_2.c
	clang -fno-discard-value-names -emit-llvm -S -o test_task3_2.ll test_task3_2.c
	opt -dot-cfg test_task3_2.ll
	mv .main.dot test_task3_2.dot

test_task4: test_task4.c
	clang -fno-discard-value-names -emit-llvm -S -o test_task4.ll test_task4.c
	opt -dot-cfg test_task4.ll
	mv .main.dot test_task4.dot

test_task4: test_task4_2.c
	clang -fno-discard-value-names -emit-llvm -S -o test_task4_2.ll test_task4_2.c
	opt -dot-cfg test_task4_2.ll
	mv .main.dot test_task4_2.dot

example1: example1.c
	clang -fno-discard-value-names -emit-llvm -S -o example1.ll example1.c
	opt -dot-cfg example1.ll
	mv .main.dot example1.dot

example2: example2.c
	clang -fno-discard-value-names -emit-llvm -S -o example2.ll example2.c
	opt -dot-cfg example2.ll
	mv .main.dot example2.dot
	
taint_analysis: taint_analysis.cpp
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(LIBS)
