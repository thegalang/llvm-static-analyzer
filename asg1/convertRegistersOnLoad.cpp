/*

//

int a; // alloca a

every load instruction, if %2= load i32, i32* %a, align 4
every load instruction, if %target = load i32, i32* %2, align 4

target = source

map<Value*, Value*> registersToVariable

registersToVariable[target] = registersToVariable[source];

*/

#include <cstdio>
#include <iostream>
#include <set>
#include <map>
#include <stack>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/CFG.h"

using namespace std;
using namespace llvm;

// %2 = load i32, i32* %b, align 4
string instructionToString(Instruction &I) {

	string str;
	raw_string_ostream(str) << I;

	return str;

}

string getSimpleNodeLabel(const BasicBlock &Node) {
    if (!Node.getName().empty()){
	//errs()<<Node->getName().str();
        return Node.getName().str();}
    string Str;
    raw_string_ostream OS(Str);
    Node.printAsOperand(OS, false);
    return OS.str();
}

// getting the value of register. %2, source, sink, etc
string getValueName(const Value* Node) {
	if (!Node->getName().empty()){
	//errs()<<Node->getName().str();
        return Node->getName().str();}
    std::string Str;
    raw_string_ostream OS(Str);
    Node->printAsOperand(OS, false);
    return OS.str();
}

// %2 = load i32, i32* %b, align 4. %2 will have same value as %b
// registers can only be initialized once, so once initialized they will be synonimous
map<Value*, Value*> registersToVariable;

Value* getTrueValue(Value* val) {
	if(registersToVariable.count(val) == 0) return val;

	return registersToVariable[val];
}



void replaceInstWithVariable(Instruction &I) {

	for(auto op = I.op_begin(); op != I.op_end(); op++) {
		
		*op = getTrueValue(*op);
	}

}

void convertInstructionsToVariable(BasicBlock &BB) {


	for(Instruction &I: BB) {

		if(isa<AllocaInst>(I)) {
			Value* variableName = llvm::cast<Value>(&I);

			registersToVariable[variableName] = variableName;
		}

		if(isa<LoadInst>(I)) {

			// %2 = load i32, i32* %b, align 4
			Value* source = I.getOperand(0);
			Value* target = llvm::cast<Value>(&I);

			errs()<<"load set reg "<<getValueName(target)<<" "<<getValueName(registersToVariable[source])<<"\n";
			registersToVariable[target] = registersToVariable[source];

			errs()<<"load instruction before replacement: "<<instructionToString(I)<<"\n";

			replaceInstWithVariable(I);
			errs()<<"load instruction after replacement: "<<instructionToString(I)<<"\n";

			errs()<<"\n\n";


		}

		if(isa<BinaryOperator>(I)) {

			Value* op1 = I.getOperand(0);
			Value* op2 = I.getOperand(1);

			Value* target = llvm::cast<Value>(&I);

			errs()<<getValueName(op1)<<" "<<getValueName(getTrueValue(op1))<<"\n";

			errs()<<"load instruction before replacement: "<<instructionToString(I)<<"\n";

			replaceInstWithVariable(I);
			errs()<<"load instruction after replacement: "<<instructionToString(I)<<"\n";

			errs()<<"\n\n";

		}
	}

}

static LLVMContext MyGlobalContext;

int main(int argc, char **argv) {

	LLVMContext &Context = MyGlobalContext;
	SMDiagnostic Err;

	auto M = parseIRFile(argv[1], Err, Context);
	if(M == nullptr) {
		fprintf(stderr, "error: failed to load LLVM IR file \"%s\"", argv[1]);
		return EXIT_FAILURE;
	}

	// extract function main
	Function *F = M->getFunction("main");

	for(auto &BB: *F) {

		errs()<<"==== analyzing block "<<getSimpleNodeLabel(BB)<<"\n";
		convertInstructionsToVariable(BB);
	}
}