CC = clang++
CFLAGS = $(shell llvm-config --cxxflags)
LDFLAGS = $(shell llvm-config --ldflags)
LIBS = $(shell llvm-config --libs) -lpthread -lncurses -ldl

all: very_busy_analysis test_vb1 test_vb2 test_vb3 test_vb4 test_vb5 test_vb6

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

very_busy_analysis: very_busy_analysis.cpp
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(LIBS)

