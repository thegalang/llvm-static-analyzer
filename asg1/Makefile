CC = clang++
CFLAGS = $(shell llvm-config --cxxflags)
LDFLAGS = $(shell llvm-config --ldflags)
LIBS = $(shell llvm-config --libs) -lpthread -lncurses -ldl

all: taint_analysis initializedVars test1 test2_wipetaint test_task3 test_task4 test_vb1 convertRegistersOnLoad very_busy_analysis test_vb2 test_vb3 test_vb4 test_vb5 test_vb6

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

test_task4: test_task4.c
	clang -fno-discard-value-names -emit-llvm -S -o test_task4.ll test_task4.c
	opt -dot-cfg test_task4.ll
	mv .main.dot test_task4.dot

test_vb1: test_vb1.c
	clang -fno-discard-value-names -emit-llvm -S -o test_vb1.ll test_vb1.c
	opt -dot-cfg test_vb1.ll
	mv .main.dot test_vb1.dot

test_vb2: test_vb2.c
	clang -fno-discard-value-names -emit-llvm -S -o test_vb2.ll test_vb2.c
	opt -dot-cfg test_vb2.ll
	mv .main.dot test_vb2.dot

test_vb3: test_vb3.c
	clang -fno-discard-value-names -emit-llvm -S -o test_vb3.ll test_vb3.c
	opt -dot-cfg test_vb3.ll
	mv .main.dot test_vb3.dot

test_vb4: test_vb4.c
	clang -fno-discard-value-names -emit-llvm -S -o test_vb4.ll test_vb4.c
	opt -dot-cfg test_vb4.ll
	mv .main.dot test_vb4.dot

test_vb5: test_vb5.c
	clang -fno-discard-value-names -emit-llvm -S -o test_vb5.ll test_vb5.c
	opt -dot-cfg test_vb5.ll
	mv .main.dot test_vb5.dot

test_vb6: test_vb6.c
	clang -fno-discard-value-names -emit-llvm -S -o test_vb6.ll test_vb6.c
	opt -dot-cfg test_vb6.ll
	mv .main.dot test_vb6.dot


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

convertRegistersOnLoad: convertRegistersOnLoad.cpp
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(LIBS)

very_busy_analysis: very_busy_analysis.cpp
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(LIBS)
