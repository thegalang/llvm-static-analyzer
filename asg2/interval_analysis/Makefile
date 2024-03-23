CC = clang++
CFLAGS = $(shell llvm-config --cxxflags)
LDFLAGS = $(shell llvm-config --ldflags)
LIBS = $(shell llvm-config --libs) -lpthread -lncurses -ldl

all: interval_analysis test1 test2 test3 test4 test5

interval_analysis: interval_analysis.cpp
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(LIBS)

test1: test1.c
	clang -fno-discard-value-names -emit-llvm -S -o test1.ll test1.c
	opt -dot-cfg test1.ll
	mv .main.dot test1.dot

test2: test2.c
	clang -fno-discard-value-names -emit-llvm -S -o test2.ll test2.c
	opt -dot-cfg test2.ll
	mv .main.dot test2.dot

test3: test3.c
	clang -fno-discard-value-names -emit-llvm -S -o test3.ll test3.c
	opt -dot-cfg test3.ll
	mv .main.dot test3.dot

test4: test4.c
	clang -fno-discard-value-names -emit-llvm -S -o test4.ll test4.c
	opt -dot-cfg test4.ll
	mv .main.dot test4.dot

test5: test5.c
	clang -fno-discard-value-names -emit-llvm -S -o test5.ll test5.c
	opt -dot-cfg test5.ll
	mv .main.dot test5.dot
