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
		return infinityType == InfinityType::PositiveInfinity;
	}

	bool isNegativeInfinity() const {
		return infinityType == InfinityType::NegativeInfinity;
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

	bool operator<=(int other) const {
		AbstractNumber ot = AbstractNumber(other);
		return *this <= ot;
	}

	bool operator>=(int other) const {
		AbstractNumber ot = AbstractNumber(other);
		return !(*this < ot);
	}

	bool operator<(int other) const {
		AbstractNumber ot = AbstractNumber(other);
		return *this < ot;
	}

	bool operator>(int other) const {
		AbstractNumber ot = AbstractNumber(other);
		return !(*this <= ot);
	}



	bool operator==(const AbstractNumber &other) const {
		return string(*this) == string(other);
	}

	bool operator<=(const AbstractNumber &other) const {
		return *this < other || *this == other;
	}

	AbstractNumber operator+(AbstractNumber const& other) const {

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

	
	// assume -inf - -inf = -inf
	AbstractNumber operator-(AbstractNumber const& other) const {
		// assume +inf - +inf = +inf and -inf - -inf = +inf (normally undefined)
		if(isPositiveInfinity() && other.isPositiveInfinity() || isNegativeInfinity() && other.isNegativeInfinity()) {
			return AbstractNumber(0, InfinityType::PositiveInfinity);
		}


		if(isPositiveInfinity() || other.isNegativeInfinity()) {
			return AbstractNumber(0, InfinityType::PositiveInfinity);
		}

		if(isNegativeInfinity() || other.isPositiveInfinity()) {
			return AbstractNumber(0, InfinityType::NegativeInfinity);
		}

		return AbstractNumber(number - other.number);
	}

	AbstractNumber operator*(AbstractNumber const& other) const {
		
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
			else if (finiteNumber.number < 0) {

				return (infiniteNumber.infinityType == InfinityType::PositiveInfinity) ? AbstractNumber(0, InfinityType::NegativeInfinity) : AbstractNumber(0, InfinityType::PositiveInfinity);
				
			} else {
				return AbstractNumber(0, infiniteNumber.infinityType);
			}
		}
		

		// two finites

		return AbstractNumber(number * other.number);
	}


	AbstractNumber operator/(AbstractNumber const& other) const {


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

	AbstractDomain(AbstractNumber _mn, AbstractNumber _mx) {
		mn = _mn;
		mx = _mx;
		isEmpty = false;

	}

	AbstractDomain(int _mn, int _mx) {
		mn = AbstractNumber(_mn);
		mx = AbstractNumber(_mx);
		isEmpty = false;
	}

	bool operator==(AbstractDomain const& other) const {
		return mn == other.mn && mx == other.mx;
	}

	AbstractDomain operator+(AbstractDomain const& other) {
		AbstractDomain ret = AbstractDomain(mn + other.mn, mx + other.mx);
		//outs()<<"adding "<<*this<<" "<<other<<" "<<ret<<"\n";
		return ret;
	}

	AbstractDomain operator-(AbstractDomain const& other) {
		AbstractDomain ret = AbstractDomain(mn - other.mx, mx - other.mn);
		return ret;
	}

	AbstractDomain operator*(AbstractDomain const& other) {

		// signs can be switched if multplying by negative
		AbstractDomain ret = AbstractDomain(min(mn * other.mx, mx * other.mn), max(mx * other.mx, mn * other.mn));

		return ret;
	}

	AbstractDomain operator/(AbstractDomain const& other) {

		if(other.mn < 0 && other.mx > 0) {

			AbstractNumber minusOne = AbstractNumber(-1);
			AbstractNumber absoluteMax = max(mn * minusOne, mx);
			return AbstractDomain(minusOne * absoluteMax, absoluteMax);
		} else {
			AbstractNumber retMn = min(mn/other.mn, min(mn/other.mx, min(mx/other.mn, mx/other.mx)));
			AbstractNumber retMx = max(mn/other.mn, max(mn/other.mx, max(mx/other.mn, mx/other.mx)));
			return AbstractDomain(retMn, retMx);
		}
	}

	AbstractDomain operator%(AbstractDomain const& other) {

		AbstractNumber one = AbstractNumber(1);

		AbstractNumber maxPositive = (other.mx > 0) ? (other.mx - one) : AbstractNumber(0);
		AbstractNumber minNegative = (other.mn < 0) ? (other.mn + one) : AbstractNumber(0);


		AbstractDomain ret = AbstractDomain(minNegative, maxPositive);
		return ret;
	}

	operator string() const { 
		return "[" + string(mn) + ", " + string(mx) + "]";
	}


};

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

string instructionToString(const Instruction &I) {

	string str;
	raw_string_ostream(str) << I;

	return str;

}

void mergeVariableIntervals(const VariableInterval &from, VariableInterval &to, function<AbstractDomain(AbstractDomain, AbstractDomain)> mergeFunc ) {
	for(auto &variableData: from) {


		auto variableValue = variableData.first;
		//errs()<<" merging "<<getValueName(variableValue)<<"\n";
		to[variableValue] = mergeFunc(variableData.second, to[variableValue]);
	}
}

using intervalMergeFunct = function<AbstractDomain(const AbstractDomain&, const AbstractDomain&)>;

// find all variables in the program

set<Value*> findAllVariables(Function *F) {

	set<Value*> ret;
	for(auto &BB : *F) {

		for(Instruction &I : BB) {

			if(isa<AllocaInst>(I)) {
				Value* variable = llvm::cast<Value>(&I);

				ret.insert(variable);
			}
		}
	}

	return ret;
}

int getConstantIntValue(Value* var) {
	assert(isa<ConstantInt>(var));

	auto constantInt = llvm::cast<ConstantInt>(var);

	return constantInt->getSExtValue ();

}

AbstractDomain getAbstractDomain(VariableInterval &variableInterval, Value* var) {
	if(isa<ConstantInt>(var)) {

		int constValue = getConstantIntValue(var);
		return AbstractDomain(constValue, constValue);
	}

	return variableInterval[var];
}


void printSingleBlockInterval(VariableInterval a, const set<Value*> &variables) {
	for(auto varInterval: a) {
		if(variables.find(varInterval.first) != variables.end())
			errs()<<getValueName(varInterval.first)<<" "<<varInterval.second<<"\n";
	}
}

void printIntervals(map<string, VariableInterval> intervals, const set<Value*> &variables) {

	for(auto item: intervals) {
		errs()<<"intervals in: "<<item.first<<"\n";

		printSingleBlockInterval(item.second, variables);

		// for(auto varInterval: item.second) {
		// 	if(variables.find(varInterval.first) != variables.end())
		// 		errs()<<getValueName(varInterval.first)<<" "<<varInterval.second<<"\n";
		// }

		errs()<<"\n";
	}

}


// Variables: set of Value* of program variables (for printing purposes only)
map<string, VariableInterval> intervalAnalysisProcess(Function *F, const map<string, VariableInterval> &savedIntervals, set<Value*> variables, intervalMergeFunct mergeFunc)  {

	map<string, VariableInterval> intervalAnalysis(savedIntervals);
	

	auto &entryBlock = F->getEntryBlock();
	string entryBlockName = getSimpleNodeLabel(&entryBlock);

	// parent, child
	set<BasicBlock*> changedNodes;

	// initially insert everything so everything is processed
	for(auto &BB: *F) {
		changedNodes.insert(&BB);
	}
	

	while(!changedNodes.empty()) {

		BasicBlock* BB = *changedNodes.begin();
		changedNodes.erase(changedNodes.begin());


		string blockName = getSimpleNodeLabel(BB);


		VariableInterval intervalInBlock;

		// merge values from parent block
		for(auto Preds: predecessors(BB)) {

			string predBlockName = getSimpleNodeLabel(Preds);
			mergeVariableIntervals(intervalAnalysis[predBlockName], intervalInBlock, mergeNormal);
	
		}

		for(Instruction &I : *BB) {

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


				intervalInBlock[target] = getAbstractDomain(intervalInBlock, source);


			}

			if(isa<StoreInst>(I)) {

				Value* target = I.getOperand(1);

				Value* source = I.getOperand(0);

				intervalInBlock[target] = getAbstractDomain(intervalInBlock, source);

			}

			if(isa<BinaryOperator>(I)) {

				Value* op1 = I.getOperand(0);
				Value* op2 = I.getOperand(1);

				Value* target = llvm::cast<Value>(&I);

				auto op1Domain = getAbstractDomain(intervalInBlock, op1);
				auto op2Domain = getAbstractDomain(intervalInBlock, op2);

				AbstractDomain targetDomain;

				switch(I.getOpcode()) {
				case Instruction::Add:
					targetDomain = op1Domain + op2Domain;
					//outs()<<"MASHOK ADD "<<intervalInBlock"\n";
					break;

				case Instruction::Sub:
					targetDomain = op1Domain - op2Domain;
					break;

				case Instruction::Mul:
					targetDomain = op1Domain * op2Domain;
					break;

				case Instruction::SDiv:
					targetDomain = op1Domain / op2Domain;
					break;

				case Instruction::SRem:
					targetDomain = op1Domain % op2Domain;
					break;
				default:
					errs()<<"ERROR: BINARY OPERATOR NOT SUPPORTED: "<<instructionToString(I)<<"\n";
					assert(false);

				}

				intervalInBlock[target] = targetDomain;

			} 
		
			
		}
		
		// fixpoint not reached
		if(intervalAnalysis[blockName] != intervalInBlock) {

			// errs()<<"FIXPOINT NOT REACHED, previous:\n";
			// printSingleBlockInterval(intervalAnalysis[blockName], variables);
			// errs()<<"\ncurrent:\n";
			// printSingleBlockInterval(intervalInBlock, variables);
			// errs()<<"\n";

			mergeVariableIntervals(intervalAnalysis[blockName], intervalInBlock, mergeFunc);

			intervalAnalysis[blockName] = intervalInBlock;

			for(BasicBlock* succ: successors(BB)) {
				changedNodes.insert(succ);
			}

			
		}



		//printIntervals(intervalAnalysis, variables);
		//exit(0);
	}

	return intervalAnalysis;

}

// sep [x1, x2] [y1, y2]. max difference is max(|y2 - x1|, |x2 - y1|)
AbstractNumber getSep(AbstractDomain x, AbstractDomain y) {
	
	// dont need abs here because if a value is negative, then that means the interval does not overlap and the other value will be the largest
	return max(x.mx - y.mn, y.mx - x.mn);
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

	auto variables = findAllVariables(F);

	//errs()<<variables.size()<<"\n";

	intervalMergeFunct mergeAsF = mergeNormal;

	map<string, VariableInterval> intervals;

	intervals = intervalAnalysisProcess(F, intervals, variables, widening);

	intervals = intervalAnalysisProcess(F, intervals, variables, narrowing);

	//outs()<<"intervals after narrowing:\n";
	for(auto item: intervals) {
		outs()<<"difference analysis in: "<<item.first<<"\n";

		for(auto varInterval: item.second) {
			for(auto varInterval2: item.second) {

				Value* var1 = varInterval.first, *var2 = varInterval2.first;

				if(variables.find(var1) == variables.end() || variables.find(var2) == variables.end()) continue;

				if(var1 >= var2) continue; // to not double print sep(x, y) and sep(y, x)

				string var1Name = getValueName(var1), var2Name = getValueName(var2);
				//outs()<<varInterval.second<<" "<<varInterval2.second<<"\n";
				outs()<<"sep("<<var1Name<<", "<<var2Name<<") = "<<getSep(varInterval.second, varInterval2.second)<<"\n";
			}	
		}

		outs()<<"\n";
	}



	
}