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

using namespace llvm;
using namespace std;

// Printing Basic Block Label
std::string getSimpleNodeLabel(const BasicBlock *Node) {
    if (!Node->getName().empty()){
	//errs()<<Node->getName().str();
        return Node->getName().str();}
    std::string Str;
    raw_string_ostream OS(Str);
    Node->printAsOperand(OS, false);
    return OS.str();
}

static LLVMContext MyGlobalContext;

template <typename T>
set<T> union_sets(set<T> a, set<T> b) {
	for(auto item : b) a.insert(item);

	return a;
}

bool isTainted(Value* var, set<Value*> &taintedVars) {

	if(isa<ConstantInt>(var) || isa<ConstantFP>(var) || isa<ConstantExpr>(var)) {
		return false;
	}

	return (taintedVars.count(var) != 0);
}

void printTaintedVarsSet(set<Value*> taintedVars) {

	errs()<<" TAINTED VARS: ";
	for(auto c: taintedVars) {
		errs()<<c->getName()<<" ";
	}
	errs()<<"\n";
}

void printTaintedVarsPath(map<Value*, vector<string>> taintedPath) {
	errs()<<" TAINTED VARS PATH: "

	for(const auto& kv: taintedPath) {
		errs()<<kv.first<<":\n";
		for(auto &v: kv.second) {
			for(auto block: v) errs()<<block<<" "; errs()<<"\n";
		}
		errs()<<"\n";
	}

	errs()<<"\n\n";
}


void copyTaintedFlow(const set<vector<string>> &from, set<vector<string>> &to, string currentBlock) {

	for(auto v: from) {
		if(v.back() != currentBlock) v.push_back(currentBlock);

		to.insert(v);
	}
}

string TAINTED_INIT_NAME = "source";

pair<set<Value*>, map<v findTaintedVars(BasicBlock *BB, const set<Value*> &entryTaintedVars, const map<Value*, vector<string>> &entryTaintedPath) {

	set<Value*> updatedTaintedVars(entryTaintedVars);
	map<Value*, vector<string>> updatedTaintedPath(entryTaintedPath);

	string currentBlock = getSimpleNodeLabel(BB);
	errs()<<"analyzing "<<currentBlock<< " START\n";

	for(auto &I: *BB) {

		if(isa<AllocaInst>(I)) {

			Value *allocatedVar = llvm::cast<Value>(&I);

			if(allocatedVar->getName() == TAINTED_INIT_NAME) {
				errs()<<"tainted variable initialized: "<<allocatedVar->getName()<<"\n";

				updatedTaintedVars.insert(allocatedVar);
				updatedTaintedPath[allocatedVar] = {{allocatedVar}};
			}
		}
		if(isa<StoreInst>(I)) {

			// if arg0 is tainted, that arg1 is tainted
			// if arg0 is immediate, then arg1 is untainted
			Value* targetVar = I.getOperand(1);

			Value* sourceVar = I.getOperand(0);
			
			// bool isTaintedStore = false;
			// if(isa<ConstantInt>(sourceVar)) {
			// 	ConstantInt* CI = dyn_cast<ConstantInt>(sourceVar);
			// 	if(CI->getSExtValue() == 42) {
			// 		isTaintedStore = true;
			// 	}
			// }

			// if(isTaintedStore) {
			// 	errs()<<"==== Tainted Variable Input: "<<targetVar->getName()<<"\n";
			// }

			if(isTainted(sourceVar, updatedTaintedVars)) {
				updatedTaintedVars.insert(targetVar); // gen
				copyTaintedFlow(updatedTaintedPath[sourceVar], updatedTaintedPath[targetVar], currentBlock);

			} else {
				updatedTaintedVars.erase(targetVar); // kill
				updatedTaintedVars[targetVar].clear();
			}
		} else if(isa<LoadInst>(I)) {

			// if loaded var is tainted, then target is tainted
			//Instruction* lhsPointer = &I;
			//LoadInst *inst = llvm::dyn_cast<llvm::LoadInst>(&I);
			Value *target= llvm::cast<Value>(&I);
			Value *source = I.getOperand(0);

			errs()<<"load var names:"<<source->getName()<<" "<<inst->getName()<<"\n";
			if(isTainted(source, updatedTaintedVars)) {
				updatedTaintedVars.insert(inst); // gen
				copyTaintedFlow(updatedTaintedPath[source], updatedTaintedPath[target], currentBlock);
			} else {
				updatedTaintedVars.erase(inst);
				updatedTaintedPath[target].clear();
			}


		} else if(isa<BinaryOperator>(I)) {
			//Instruction* lhsPointer = &I;
			Value *target = llvm::cast<Value>(&I);

			Value* left = I.getOperand(0);
			Value* right = I.getOperand(1);
			errs()<< "found binary " <<target->getName()<<" : "<<left->getName()<<"-"<<right->getName()<<"\n";
			if(isTainted(left, updatedTaintedVars) || isTainted(right, updatedTaintedVars)) {
				updatedTaintedVars.insert(target);

				if(isTainted(left, updatedTaintedVars)) copyTaintedFlow(updatedTaintedPath[left], updatedTaintedPath[target], currentBlock);
				if(isTainted(right, updatedTaintedVars)) copyTaintedFlow(updatedTaintedPath[right], updatedTaintedPath[target], currentBlock);

			} else {
				updatedTaintedVars.erase(target);
			}
		}
	}

	errs()<<"\n";


	printTaintedVarsSet(updatedTaintedVars);

	printTaintedVarsPath(updatedTaintedPath);
	return updatedTaintedVars;

}

void printAnalysisMap(std::map<std::string,std::set<Value*>> analysisMap) {
	errs() << "TAINTED VARIABLES:\n";
    for (auto& row : analysisMap){
    	std::set<Value*> taintedVars = row.second;
    	std::string BBLabel = row.first;
    	outs() << BBLabel << ":\n";
    	for (Value* var : taintedVars){
    		outs() << " ";
    		outs() << var->getName()<<" ";
    	}
    	outs() << "\n\n";
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

	map<string, set<Value*>> taintAnalysisMap;
	map<string, vector<string>> taintAnalysisEdge;
	for(auto &BB: *F) {
		taintAnalysisMap[getSimpleNodeLabel(&BB)] = {};
		//std::cout<<getSimpleNodeLabel(&BB)<<"\n";
	}

	stack<pair<BasicBlock*, set<Value*>>> traversalStack;
	BasicBlock* entryBB = &F->getEntryBlock();
	traversalStack.push({entryBB, {}});

	while(!traversalStack.empty()) {
		auto analysisNode = traversalStack.top();
		traversalStack.pop();

		auto BB = analysisNode.first;
		auto entryTaintedVars = analysisNode.second;
		auto blockLabel = getSimpleNodeLabel(BB);


		set<Value*> updatedTaintedVars = findTaintedVars(BB, entryTaintedVars);

		set<Value*> exitTaintedVars = (taintAnalysisMap[blockLabel].empty()) ? 
											updatedTaintedVars : 
											union_sets(updatedTaintedVars, taintAnalysisMap[blockLabel]); 

		taintAnalysisMap[blockLabel] = exitTaintedVars;


		const auto *TInst = BB->getTerminator();
		const int nSucc = TInst->getNumSuccessors();

		for(int i = 0; i < nSucc; ++i) {

			BasicBlock *Succ = TInst->getSuccessor(i);

			// todo: handle while
			traversalStack.push({Succ, exitTaintedVars});
		}

	}

	printAnalysisMap(taintAnalysisMap);

}

