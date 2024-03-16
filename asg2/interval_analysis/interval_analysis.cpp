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

enum class InfinityType { NegativeInfinity, Finite, PositiveInfinity };

class AbstractNumber {

private:

	

public:
	InfinityType infinityType;
	int number;

	

	bool isPositiveInfinity() const {
		return infinityType == InfinityType::NegativeInfinity;
	}

	bool isNegativeInfinity() const {
		return infinityType == InfinityType::PositiveInfinity;
	}

	bool isFinite() const {
		return infinityType == InfinityType::Finite;
	}

	bool isInfinity() const {
		return isPositiveInfinity() || isNegativeInfinity();
	}

	AbstractNumber() {
		number = 0;
		infinityType = InfinityType::Finite;
	}

	AbstractNumber(int x) {
		number = x;
		infinityType = InfinityType::Finite;
	}

	AbstractNumber(int x, InfinityType type) {

		number = x;
		infinityType = type;
	}

	void setInfinityStatus(InfinityType type) {

		infinityType = type;
	}	

	bool operator<(const AbstractNumber &other) const {

		// same infinity
		if((isPositiveInfinity() && other.isPositiveInfinity()) || (isNegativeInfinity() && other.isNegativeInfinity())) {
			return false;
		}

		if(isNegativeInfinity() || other.isPositiveInfinity()) return true;
		if(isPositiveInfinity() || other.isNegativeInfinity()) return false;

		return number < other.number;
	}


	AbstractNumber operator+(AbstractNumber const& other) {

		// assert error cannot add -inf and +inf
		if(isPositiveInfinity() && other.isNegativeInfinity() || isNegativeInfinity() && other.isPositiveInfinity()) {
			errs()<<"CANNOT OPERATE +INF and -INF"<<"\n";
			assert(false);
		}

		if(isPositiveInfinity() || other.isPositiveInfinity()) {
			return AbstractNumber(0, InfinityType::PositiveInfinity);
		}

		if(isNegativeInfinity() || other.isNegativeInfinity()) {
			return AbstractNumber(0, InfinityType::NegativeInfinity);
		}

		return AbstractNumber(number + other.number);
	}

	AbstractNumber operator-(AbstractNumber const& other) {
		// assert error cannot add -inf and +inf
		if(isPositiveInfinity() && other.isNegativeInfinity() || isNegativeInfinity() && other.isPositiveInfinity()) {
			errs()<<"CANNOT OPERATE +INF and -INF"<<"\n";
			assert(false);
		}

		if(isPositiveInfinity() || other.isNegativeInfinity()) {
			return AbstractNumber(0, InfinityType::PositiveInfinity);
		}

		if(isNegativeInfinity() || other.isPositiveInfinity()) {
			return AbstractNumber(0, InfinityType::NegativeInfinity);
		}

		return AbstractNumber(number + other.number);
	}

	AbstractNumber operator*(AbstractNumber const& other) {
		
		// -inf * inf = -inf. inf * inf = inf, -inf * -inf = inf
		int numPositiveInfinity = isPositiveInfinity() + other.isPositiveInfinity();
		int numNegativeInfinity = isNegativeInfinity() + other.isNegativeInfinity();


		// two infinites
		if(numPositiveInfinity == 2 || numNegativeInfinity == 2) {
			return AbstractNumber(0, InfinityType::PositiveInfinity);
		}

		if(numPositiveInfinity == 1 && numNegativeInfinity == 1) {
			return AbstractNumber(0, InfinityType::NegativeInfinity);
		}


		// one infinity and one finite
		if(numPositiveInfinity == 1 || numNegativeInfinity == 1) {
			AbstractNumber infiniteNumber = (isInfinity()) ? *this: other;
			AbstractNumber finiteNumber =  (isInfinity()) ? other: *this;

			if(finiteNumber.number == 0) return AbstractNumber(0, InfinityType::Finite);
			else return AbstractNumber(0, infiniteNumber.infinityType);
		}
		

		// two finites

		return AbstractNumber(number * other.number);
	}


	AbstractNumber operator/(AbstractNumber const& other) {


		// assert divide by 0
		assert(!(other.isFinite() && other.number == 0));

		// -inf * inf = -inf. inf * inf = inf, -inf * -inf = inf
		int numPositiveInfinity = isPositiveInfinity() + other.isPositiveInfinity();
		int numNegativeInfinity = isNegativeInfinity() + other.isNegativeInfinity();


		// two infinites. inf / inf = inf, inf / -inf = -inf, -inf / inf = -inf, -inf/-inf = inf
		if(numPositiveInfinity == 2 || numNegativeInfinity == 2) {
			return AbstractNumber(0, InfinityType::PositiveInfinity);
		}

		if(numPositiveInfinity == 1 && numNegativeInfinity == 1) {
			return AbstractNumber(0, InfinityType::NegativeInfinity);
		}


		// one infinity and one finite
		// if divide by inf then 0, else inf
		if(numPositiveInfinity == 1 || numNegativeInfinity == 1) {

			if(other.isInfinity()) return AbstractNumber(0, InfinityType::Finite);
			else return AbstractNumber(0, this->infinityType);
		}
		

		// two finites

		return AbstractNumber(number / other.number);
	}

	AbstractNumber operator%(AbstractNumber const& other) {
		// assert divide by 0
		assert(!(other.isFinite() && other.number == 0));

		// -inf * inf = -inf. inf * inf = inf, -inf * -inf = inf
		int numPositiveInfinity = isPositiveInfinity() + other.isPositiveInfinity();
		int numNegativeInfinity = isNegativeInfinity() + other.isNegativeInfinity();


		// two infinites. inf / inf = inf, inf / -inf = -inf, -inf / inf = -inf, -inf/-inf = inf
		if(numPositiveInfinity == 2 || numNegativeInfinity == 2) {
			return AbstractNumber(0, InfinityType::PositiveInfinity);
		}

		// infininity modulo infinity should max at infinity
		if(numPositiveInfinity == 1 && numNegativeInfinity == 1) {
			return AbstractNumber(0, InfinityType::PositiveInfinity);
		}


		// if modulo by inf then first element
		// if inf module then second element
		if(numPositiveInfinity == 1 || numNegativeInfinity == 1) {

			return (other.isInfinity()) ? *this : other;
		}
		

		// two finites
		return AbstractNumber(number % other.number);
	}

	operator std::string() const { 
		if(isFinite()) return to_string(number);
		else if(isPositiveInfinity()) return "+inf";
		else return "-inf";
	}
};

class AbstractDomain {

public:
	AbstractNumber mn, mx;
	bool isEmpty;

	AbstractDomain() {
		isEmpty = true;
	}

	AbstractDomain(const AbstractDomain &other) {
		mn = other.mn;
		mx = other.mx;
		isEmpty = other.isEmpty;
	}

	// AbstractDomain(const AbstractDomain &&other) {
	// 	mn = other.mn;
	// 	mx = other.mx;
	// 	isEmpty = other.isEmpty;
	// }

	AbstractDomain(AbstractNumber _mn, AbstractNumber _mx) {
		mn = _mn;
		mx = _mx;
		isEmpty = false;

	}

	AbstractDomain operator+(AbstractDomain const& other) {
		AbstractDomain ret = AbstractDomain(mn + other.mn, mx + other.mx);
		return ret;
	}

	operator string() const { 
		return "[" + string(mn) + ", " + string(mx) + "]";
	}

};

// AbstractNumber absMin(AbstractNumber a, AbstractNumber b) {
// 	return (a < b) ? a : b;
// }

// AbstractNumber absMax(AbstractNumber a, AbstractNumber b) {
// 	return (a < b) ? b : a;
// }


AbstractDomain mergeNormal(const AbstractDomain &a, const AbstractDomain &b) {

	if(a.isEmpty) return AbstractDomain(b);
	if(b.isEmpty) return AbstractDomain(a);

	return AbstractDomain(min(a.mn, b.mn), max(a.mx, b.mx));

}

AbstractDomain widening(const AbstractDomain &a, const AbstractDomain &b) {

	if(a.isEmpty) return AbstractDomain(b);
	if(b.isEmpty) return AbstractDomain(a);
	
	AbstractNumber newMin = (b.mn < a.mn) ? AbstractNumber(0, InfinityType::NegativeInfinity) : a.mn;
	AbstractNumber newMax = (a.mx < b.mx) ? AbstractNumber(0, InfinityType::PositiveInfinity) : a.mx;

	return AbstractDomain(newMin, newMax);
}

AbstractDomain narrowing(const AbstractDomain &a, const AbstractDomain &b) {

	if(a.isEmpty) return AbstractDomain(b);
	if(b.isEmpty) return AbstractDomain(a);

	AbstractNumber newMin = (a.mn.isNegativeInfinity()) ? b.mn : a.mn;
	AbstractNumber newMax = (a.mx.isPositiveInfinity()) ? b.mx : a.mx;

	return AbstractDomain(newMin, newMax);
}

// Value* variable, AbstractDomain: its range
using VariableInterval = map<Value*, AbstractDomain>;

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

void mergeVariableIntervals(const VariableInterval& from, VariableInterval &to, function<AbstractDomain(AbstractDomain, AbstractDomain)> mergeFunc ) {
	for(auto &variableData: from) {
		auto variableValue = variableData.first;
		to[variableValue] = mergeFunc(variableData.second, to[variableValue]);
	}
}

using intervalMergeFunct = function<AbstractDomain(const AbstractDomain&, const AbstractDomain&)>;

// performs interval analysis using mergeFunc merger (merge, widening, narrowing)
map<string, VariableInterval> intervalAnalysisProcess(Function *F, intervalMergeFunct mergeFunc)  {

	map<string, VariableInterval> intervalAnalysis;
	
	bool existUpdate = true;
	while(existUpdate) {

		existUpdate = false;

		// update intervals for all guys
		for(auto &BB : *F) {



			string blockName = getSimpleNodeLabel(&BB);

			VariableInterval intervalInBlock = intervalAnalysis[blockName];

			// merge values from previous interval
			for(auto *Preds : predecessors(&BB)) {

				string predBlockName = getSimpleNodeLabel(&BB);

				mergeVariableIntervals(intervalAnalysis[predBlockName], intervalInBlock, mergeFunc);
			}


			for(Instruction &I : BB) {

				if(isa<AllocaInst>(I)) {
					Value* variable = llvm::cast<Value>(&I);

					if(intervalInBlock.find(variable) == intervalInBlock.end()) {
						intervalInBlock[variable] = AbstractDomain(AbstractNumber(0, InfinityType::NegativeInfinity), AbstractNumber(0, InfinityType::PositiveInfinity));
					}
				}

				if(isa<LoadInst>(I)) {

					// %2 = load i32, i32* %b, align 4
					Value* source = I.getOperand(0);
					Value* target = llvm::cast<Value>(&I);

					intervalInBlock[target] = intervalInBlock[source];


				}

				if(isa<BinaryOperator>(I)) {

					Value* op1 = I.getOperand(0);
					Value* op2 = I.getOperand(1);

					Value* target = llvm::cast<Value>(&I);

					auto op1Domain = intervalInBlock[op1];
					auto op2Domain = intervalInBlock[op2];

					AbstractDomain targetDomain;

					switch(I.getOpcode()) {
					case Instruction::Add:
						targetDomain = op1Domain + op2Domain;
						break;
					}


				}
			}
		}
	}

	return intervalAnalysis;

}


int main(int argc, char **argv)  {
	LLVMContext &Context = MyGlobalContext;
	SMDiagnostic Err;

	auto M = parseIRFile(argv[1], Err, Context);
	if(M == nullptr) {
		fprintf(stderr, "error: failed to load LLVM IR file \"%s\"", argv[1]);
		return EXIT_FAILURE;
	}

	// extract function main
	Function *F = M->getFunction("main");

	intervalMergeFunct mergeAsF = mergeNormal;

	auto normalIntervalAnalysis = intervalAnalysisProcess(F, mergeAsF);

	for(auto item: normalIntervalAnalysis) {
		outs()<<"intervals in: "<<item.first<<"\n";

		for(auto varInterval: item.second) {

			outs()<<getValueName(varInterval.first)<<" "<<varInterval.second<<"\n";
		}
	}



	
}