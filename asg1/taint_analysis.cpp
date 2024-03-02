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
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace std;

using singlePathType = set<vector<string>>;

using pathType = map<Value*, singlePathType>;

string instructionToString(Instruction &I) {

	string str;
	raw_string_ostream(str) << I;

	return str;

}

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

string getValueName(const Value* Node) {
	if (!Node->getName().empty()){
	//errs()<<Node->getName().str();
        return Node->getName().str();}
    std::string Str;
    raw_string_ostream OS(Str);
    Node->printAsOperand(OS, false);
    return OS.str();
}

static LLVMContext MyGlobalContext;

pathType union_map(pathType a, pathType b) {
	for(auto &item : b) {
		if(a.find(item.first) == a.end()) {
			a.emplace(item.first, item.second);
		} else {
			auto bkey = item.first;
			for(auto vecs: item.second) {
				a[bkey].insert(vecs);
			}
		}
	}

	return a;
}

bool isTainted(Value* var, pathType &taintedVars) {

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

void printTaintedVarsPath(pathType taintedPath) {
	errs()<<"TAINTED VARS PATH:\n";

	for(const auto& kv: taintedPath) {
		errs()<<kv.first->getName()<<":\n";
		for(auto &v: kv.second) {
			for(auto block: v) errs()<<block<<" "; errs()<<"\n";
		}
		errs()<<"\n";
	}

	errs()<<"\n\n";
}


void copyTaintedFlow(const singlePathType &from, singlePathType &to, string currentBlock) {

	for(auto v: from) {
		if(v.back() != currentBlock) v.push_back(currentBlock);

		to.insert(v);
	}
}

string TAINTED_INIT_NAME = "source";

int TAINTED_INPUT = 42;

pathType findTaintedVars(BasicBlock *BB, const pathType &entryTaintedPath) {


	pathType updatedTaintedPath(entryTaintedPath);

	string currentBlock = getSimpleNodeLabel(BB);

	if(currentBlock == "entry") {
		for(Instruction &I: *BB) {
			for(Instruction &I2: *BB) {
				if(I.isIdenticalTo(&I2))
					errs()<<instructionToString(I)<<" | "<<instructionToString(I2)<<" "<<"\n";
			}
		}
	}
	//errs()<<"analyzing "<<currentBlock<< " START\n";

	for(auto &I: *BB) {


		if(isa<AllocaInst>(I)) {

			// DO NOTHING

			// Value *allocatedVar = llvm::cast<Value>(&I);

			// if(allocatedVar->getName() == TAINTED_INIT_NAME) {
			// 	errs()<<"tainted variable initialized: "<<allocatedVar->getName()<<"\n";

			// 	vector<string> singleBlock = {currentBlock};
			// 	updatedTaintedPath[allocatedVar] = singlePathType({singleBlock});
			// }
		}
		if(isa<StoreInst>(I)) {

			// if arg0 is tainted, that arg1 is tainted
			// if arg0 is immediate, then arg1 is untainted
			Value* target = I.getOperand(1);

			Value* source = I.getOperand(0);
			
			bool isTaintedStore = false;
			if(isa<ConstantInt>(source)) {
				ConstantInt* CI = dyn_cast<ConstantInt>(source);
				if(CI->getSExtValue() == TAINTED_INPUT) {
					isTaintedStore = true;
				}
			}

			if(isTaintedStore) {
				//errs()<<"==== Tainted Variable Input: "<<target->getName()<<"\n";
				vector<string> singleBlock = {currentBlock};
				updatedTaintedPath[target] = singlePathType({singleBlock});
			}
			else if(isTainted(source, updatedTaintedPath)) {
				 // gen
				copyTaintedFlow(updatedTaintedPath[source], updatedTaintedPath[target], currentBlock);

			} else {
				 // kill
				updatedTaintedPath.erase(source);
			}
		} else if(isa<LoadInst>(I)) {

			// if loaded var is tainted, then target is tainted
			//Instruction* lhsPointer = &I;
			//LoadInst *inst = llvm::dyn_cast<llvm::LoadInst>(&I);
			Value *target= llvm::cast<Value>(&I);
			Value *source = I.getOperand(0);

			//errs()<<"load var names:"<<source->getName()<<" "<<target->getName()<<"\n";
			if(isTainted(source, updatedTaintedPath)) {
				// gen
				copyTaintedFlow(updatedTaintedPath[source], updatedTaintedPath[target], currentBlock);
			} else {
				updatedTaintedPath.erase(target);
			}


		} else if(isa<BinaryOperator>(I)) {
			//Instruction* lhsPointer = &I;
			Value *target = llvm::cast<Value>(&I);

			Value* left = I.getOperand(0);
			Value* right = I.getOperand(1);
			//errs()<< "found binary " <<target->getName()<<" : "<<left->getName()<<"-"<<right->getName()<<"\n";
			if(isTainted(left, updatedTaintedPath) || isTainted(right, updatedTaintedPath)) {

				if(isTainted(left, updatedTaintedPath)) copyTaintedFlow(updatedTaintedPath[left], updatedTaintedPath[target], currentBlock);
				if(isTainted(right, updatedTaintedPath)) copyTaintedFlow(updatedTaintedPath[right], updatedTaintedPath[target], currentBlock);

			} else {
				updatedTaintedPath.erase(target);
			}
		}
	}

	//errs()<<"\n";



	//printTaintedVarsPath(updatedTaintedPath);
	return updatedTaintedPath;

}

void printAnalysisMap(map<string, pathType> analysisMap) {
	errs() << "TAINTED VARIABLES:\n";
    for (auto& row : analysisMap){
    	pathType taintedPath = row.second;
    	string BBLabel = row.first;
    	outs() << "label "<<BBLabel << ":\n";

    	int id = 1;
    	for(const auto& kv: taintedPath) {
			outs()<<id++<<". "<<getValueName(kv.first)<<":\n";
			outs()<<"tainted data flow paths:\n";

			
			for(auto &v: kv.second) {
				outs()<<"- ";

				bool isFirstElem = true;
				for(auto block: v) {
					outs()<<((isFirstElem) ? "" : " -> ")<<block; 
					isFirstElem = false;
				}

				outs()<<"\n";
				
			}
			outs()<<"\n";
		}
    	outs() << "\n";
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

	map<string, pathType> taintAnalysisMap;
	for(auto &BB: *F) {
		taintAnalysisMap[getSimpleNodeLabel(&BB)] = {};
		//std::cout<<getSimpleNodeLabel(&BB)<<"\n";
	}

	stack<pair<BasicBlock*, pathType>> traversalStack;
	BasicBlock* entryBB = &F->getEntryBlock();
	traversalStack.push({entryBB, {}});

	while(!traversalStack.empty()) {
		auto analysisNode = traversalStack.top();
		traversalStack.pop();

		auto BB = analysisNode.first;
		auto entryTaintedVars = analysisNode.second;
		auto blockLabel = getSimpleNodeLabel(BB);


		pathType updatedTaintedVars = findTaintedVars(BB, entryTaintedVars);

		pathType exitTaintedVars = (taintAnalysisMap[blockLabel].empty()) ? 
											updatedTaintedVars : 
											union_map(updatedTaintedVars, taintAnalysisMap[blockLabel]); 

		taintAnalysisMap[blockLabel] = exitTaintedVars;



		for(auto* Succ: successors(BB)) {

			string sucName = getSimpleNodeLabel(Succ);

			if(exitTaintedVars != taintAnalysisMap[sucName])
				traversalStack.push({Succ, exitTaintedVars});
		}

	}

	printAnalysisMap(taintAnalysisMap);

}

