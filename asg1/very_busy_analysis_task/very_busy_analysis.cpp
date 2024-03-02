/*
Name: Galangkangin Gotera
Matric number: A0274760Y
Email: galangkangin@u.nus.edu
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

using namespace llvm;
using namespace std;

static LLVMContext MyGlobalContext;

string instructionToString(const Instruction &I) {

	string str;
	raw_string_ostream(str) << I;

	return str;

}

string getSimpleNodeLabel(const BasicBlock *Node) {
    if (!Node->getName().empty()){
	//errs()<<Node->getName().str();
        return Node->getName().str();}
    string Str;
    raw_string_ostream OS(Str);
    Node->printAsOperand(OS, false);
    return OS.str();
}


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
	if(registersToVariable.find(val) == registersToVariable.end()) {
		registersToVariable[val] = val;
		return val;
	}
	return registersToVariable[val];
}

bool isSameExpression(const Instruction *a, const Instruction *b) {
	if(!a->isSameOperationAs(b)) return false;

	for(auto opA = a->op_begin(), opB = b->op_begin(); opA != a->op_end() && opB != b->op_end(); opA++, opB++) {

		Value* tA = getTrueValue(*opA);
		Value* tB = getTrueValue(*opB);
		if(getValueName(tA) != getValueName(tB)) return false;
	}

	return true;
}


// deque because we can insert the instruction from the front
using veryBusyData = deque<Instruction*>;


bool findInstruction(const veryBusyData &data, const Instruction* a) {

	for(auto &I : data) {
		if(isSameExpression(I, a)) return true;
	}

	return false;
}


void printVb(veryBusyData a) {
	for(auto &I: a) {
		errs()<<instructionToString(*I)<<"\n";
	}

}

veryBusyData set_intersection(veryBusyData a, veryBusyData b) {

	for(auto it = a.begin(); it!=a.end(); )  {
		if(!findInstruction(b, *it)) it = a.erase(it);
		else it++;
	}


	return a;
}

void replaceInstWithVariable(Instruction &I) {

	for(auto op = I.op_begin(); op != I.op_end(); op++) {
		
		*op = getTrueValue(*op);
	}

}

void kill(veryBusyData &a, Value* killedVar) {
	for(auto it = a.begin(); it != a.end(); ) {

		bool existKilledVar = false;
		for(auto op = (*it)->op_begin(); op != (*it)->op_end(); op++) {
			if(getTrueValue(*op) == getTrueValue(killedVar)) {
				existKilledVar = true;
			}
		}

		if(existKilledVar) it = a.erase(it);
		else it++;

	}

}

bool isSameSet(veryBusyData &a, veryBusyData &b) {

	for(auto &I: a) if(!findInstruction(b, I)) return false;

	for(auto &I: b) if(!findInstruction(a, I)) return false;

	return true;
}


veryBusyData veryBusyAnalysis(BasicBlock &BB, const veryBusyData &lastExpressions) {

	veryBusyData entry(lastExpressions);

	// alloca, load is ignored here because it does not affect veryBusy and is already handled in convertInstructionsToVariable
	for(Instruction &I: reverse(BB)) {


		// equivalent to target = source. Therefore killing all expressions with target
		if(isa<StoreInst>(I)) {

			Value* source = I.getOperand(0);
			Value* target = I.getOperand(1);

			kill(entry, target);


		}

		// equivalent to target = op1 ? op2
		// killing all expressions with target and generating I
		if(isa<BinaryOperator>(I)) {

			Value* op1 = I.getOperand(0);
			Value* op2 = I.getOperand(1);

			Value* target = llvm::cast<Value>(&I);

			kill(entry, target);

			if(!findInstruction(entry, &I)) {
				entry.push_front(&I);
			}


			replaceInstWithVariable(I);


		}
	}

	return entry;
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

			registersToVariable[target] = getTrueValue(source);


		}

		if(isa<BinaryOperator>(I)) {

			Value* op1 = I.getOperand(0);
			Value* op2 = I.getOperand(1);

			Value* target = llvm::cast<Value>(&I);
			registersToVariable[target] = target;


		}
	}

}


void convertInstructionToVariable(Function *F) {
	BasicBlock* BB = &F->getEntryBlock();

	stack<BasicBlock*> traversalStack;
	set<BasicBlock*> visitedBasicBlock;


	traversalStack.push(BB);

	while(!traversalStack.empty()) {

		auto BB = traversalStack.top();
		traversalStack.pop();

		visitedBasicBlock.insert(BB);

		convertInstructionsToVariable(*BB);

		for(auto *Succ: successors(BB)) {

			// in a while loop, only need to visit each block once
			if(visitedBasicBlock.find(Succ) == visitedBasicBlock.end())
				traversalStack.push(Succ);
		}
	}
}

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

	convertInstructionToVariable(F);

	veryBusyData allInst;

	map<string, veryBusyData> entryVeryBusyExpressions;
	map<string, veryBusyData> exitVeryBusyExpressions;

	stack<pair<BasicBlock*, veryBusyData>> traversalStack;

	for(auto &BB: *F) {

		// we are doing backwards analysis, therefore starting from all end blocks (they will have no successors)
		if(BB.getTerminator()->getNumSuccessors() == 0) {
			errs()<<"terminating block: "<<getSimpleNodeLabel(&BB)<<"\n";
			traversalStack.push({&BB, {}});
		}
	}

	errs()<<"\n";

	while(!traversalStack.empty()) {
		auto analysisNode = traversalStack.top();
		traversalStack.pop();

		auto BB = analysisNode.first;
		auto blockLabel = getSimpleNodeLabel(BB);

		//errs()<<blockLabel<<"\n";

		exitVeryBusyExpressions[blockLabel] = (exitVeryBusyExpressions[blockLabel].empty()) ? 
												analysisNode.second : 
												set_intersection(exitVeryBusyExpressions[blockLabel], analysisNode.second);
		

		entryVeryBusyExpressions[blockLabel] = veryBusyAnalysis(*BB, exitVeryBusyExpressions[blockLabel]);

		for(auto *Preds: predecessors(BB)) {

			string predLabel = getSimpleNodeLabel(Preds);

			bool foundNewBlock = exitVeryBusyExpressions.find(predLabel) == exitVeryBusyExpressions.end();

			auto newExitVeryBusyExpression = (foundNewBlock) ? 
												entryVeryBusyExpressions[blockLabel] : 
												set_intersection(exitVeryBusyExpressions[predLabel], entryVeryBusyExpressions[blockLabel]);

			//errs()<<blockLabel<<" "<<predLabel<<" "<<newExitVeryBusyExpression.size()<<" "<<exitVeryBusyExpressions[predLabel].size()<<"\n";
			// 	we reach fixpoint if after intersecting the values, exit expressions stay the same because traverse again does not make a difference
			if(foundNewBlock || !isSameSet(newExitVeryBusyExpression, exitVeryBusyExpressions[predLabel])) { 
				traversalStack.push({Preds, entryVeryBusyExpressions[blockLabel]});
			}
		}

	}

	outs()<<"VERY BUSY EXPRESSIONS:\n";
	for(auto &BB: *F) {
		auto blockLabel = getSimpleNodeLabel(&BB);
		outs()<<"===== "<<blockLabel<<" =====\n";
		outs()<<"entry very busy expressions:\n";
		for(auto &I : entryVeryBusyExpressions[blockLabel]) {
			outs()<<instructionToString(*I)<<"\n";
		}
		outs()<<"\n";
		outs()<<"exit very busy expressions:\n";
		for(auto &I : exitVeryBusyExpressions[blockLabel]) {
			outs()<<instructionToString(*I)<<"\n";
		}
		outs()<<"\n";

	}

}
