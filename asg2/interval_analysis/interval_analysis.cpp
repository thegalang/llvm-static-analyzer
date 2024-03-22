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

	bool operator==(const AbstractNumber &other) const {
		return string(*this) == string(other);
	}

	bool operator<=(const AbstractNumber &other) const {
		return *this < other || *this == other;
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

		return AbstractNumber(number - other.number);
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
		AbstractDomain ret = AbstractDomain(mn - other.mn, mx - other.mx);
		return ret;
	}

	AbstractDomain operator*(AbstractDomain const& other) {
		AbstractDomain ret = AbstractDomain(mn * other.mn, mx * other.mx);
		return ret;
	}

	AbstractDomain operator/(AbstractDomain const& other) {
		AbstractDomain ret = AbstractDomain(mn / other.mn, mx / other.mx);
		return ret;
	}

	AbstractDomain operator%(AbstractDomain const& other) {
		AbstractDomain ret = AbstractDomain(mn % other.mn, mx % other.mx);
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

void mergeVariableIntervals(const VariableInterval& from, VariableInterval &to, function<AbstractDomain(AbstractDomain, AbstractDomain)> mergeFunc ) {
	for(auto &variableData: from) {


		auto variableValue = variableData.first;
		//errs()<<" merging "<<getValueName(variableValue)<<"\n";
		to[variableValue] = mergeFunc(to[variableValue], variableData.second);
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

// stores result whether icmp result can be false (0) or true (1), or both
struct IcmpResult {
	bool can0, can1;

	AbstractDomain toAbstractDomain() {

		//either must be true
		assert(can0 || can1);

		int mn = can0 ? 0 : 1;
		int mx = can1 ? 1 : 0;

		return AbstractDomain(mn, mx); 
	}
};

IcmpResult eqInterval(AbstractDomain interval, int C) {

	AbstractNumber cAbs = AbstractNumber(C);

	bool can0 = !(interval.mn == cAbs && interval.mn == cAbs);
	bool can1 = interval.mn <= cAbs && cAbs <= interval.mx;

	return IcmpResult{can0, can1};
}

IcmpResult neInterval(AbstractDomain interval, int C) {

	auto res = eqInterval(interval, C);
	swap(res.can0, res.can1);

	return res;
}

IcmpResult sltInterval(AbstractDomain interval, int C) {
	AbstractNumber cAbs = AbstractNumber(C);

	bool can0 = cAbs <=  interval.mx;
	bool can1 = interval.mn < cAbs;

	return IcmpResult{can0, can1};
}

IcmpResult sgeInterval(AbstractDomain interval, int C) {
	auto res = sltInterval(interval, C);
	swap(res.can0, res.can1);

	return res;
}

IcmpResult sleInterval(AbstractDomain interval, int C) {
	AbstractNumber cAbs = AbstractNumber(C);

	bool can0 = cAbs <  interval.mx;
	bool can1 = interval.mn <= cAbs;

	return IcmpResult{can0, can1};

}

IcmpResult sgtInterval(AbstractDomain interval, int C) {
	auto res = sleInterval(interval, C);
	swap(res.can0, res.can1);

	return res;
}

int numPrint = 21;
void printIntervals(map<string, VariableInterval> intervals, set<Value*> variables) {

	for(auto item: intervals) {
		errs()<<"intervals in: "<<item.first<<"\n";

		for(auto varInterval: item.second) {
			if(variables.find(varInterval.first) != variables.end())
				errs()<<getValueName(varInterval.first)<<" "<<varInterval.second<<"\n";
		}

		errs()<<"\n";
	}

	numPrint--;
	if(numPrint == 0) exit(0);
}


// Variables: set of Value* of program variables (for printing purposes only)
map<string, VariableInterval> intervalAnalysisProcess(Function *F, bool isPathSensitive, const map<string, VariableInterval> &savedIntervals, set<Value*> variables, intervalMergeFunct mergeFunc)  {

	map<string, VariableInterval> intervalAnalysis(savedIntervals);
	

	auto &entryBlock = F->getEntryBlock();
	string entryBlockName = getSimpleNodeLabel(&entryBlock);

	// parent, child
	stack<pair<BasicBlock*, BasicBlock*>> traversalStack;
	traversalStack.push({nullptr, &entryBlock});

	while(!traversalStack.empty()) {

		auto nextVisit = traversalStack.top();
		traversalStack.pop();

		BasicBlock* parent = nextVisit.first;
		BasicBlock* BB = nextVisit.second;


		string blockName = getSimpleNodeLabel(BB);

		errs()<<"processing "<<blockName<<"\n";


		VariableInterval intervalInBlock = intervalAnalysis[blockName];

		// merge values from parent block
		if(parent != nullptr) {

			string predBlockName = getSimpleNodeLabel(parent);
			errs()<<"merging variables from "<<predBlockName<<"\n";
			mergeVariableIntervals(intervalAnalysis[predBlockName], intervalInBlock, mergeFunc);
	
		}

		

		vector<pair<BasicBlock*, BasicBlock*>> nextNodeCandidates;


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
					targetDomain = op2Domain / op2Domain;
					break;

				case Instruction::SRem:
					targetDomain = op1Domain % op2Domain;
					break;
				default:
					errs()<<"ERROR: BINARY OPERATOR NOT SUPPORTED: "<<instructionToString(I)<<"\n";
					assert(false);

				}



				intervalInBlock[target] = targetDomain;

			} if(isa<CmpInst>(I) && isPathSensitive) {

				CmpInst &Ic = llvm::cast<CmpInst>(I);

				Value* op1 = Ic.getOperand(0);
				Value* op2 = Ic.getOperand(1);

				Value* target = llvm::cast<Value>(&I);

				if(!isa<ConstantInt>(op2)) {
					errs()<<"==== ERROR ICMP MUST BE WITH A CONSTANT INT, got: "<<instructionToString(I)<<"\n";
				}

				AbstractDomain op1Domain = getAbstractDomain(intervalInBlock, op1);
				int constValue = getConstantIntValue(op2);

				IcmpResult res;

				switch(Ic.getPredicate()) {
				case CmpInst::ICMP_NE:
					res = neInterval(op1Domain, constValue);
					break;
				case CmpInst::ICMP_EQ:
					res = eqInterval(op1Domain, constValue);
					break;
				case CmpInst::ICMP_SLE:
					res = sleInterval(op1Domain, constValue);
					break;
				case CmpInst::ICMP_SGT:
					res = sgtInterval(op1Domain, constValue);
					break;
				case CmpInst::ICMP_SLT:
					res = sltInterval(op1Domain, constValue);
					break;
				case CmpInst::ICMP_SGE:
					res = sgeInterval(op1Domain, constValue);
					break;
				default:
					errs()<<"ERROR: ICMP INSTRUCTION NOT SUPPORTED: "<<instructionToString(I)<<"\n";
					assert(false);

				}

				intervalInBlock[target] = res.toAbstractDomain();

				
			} else if(isa<BranchInst>(I)) {
				errs()<<"Found BR inst "<<instructionToString(I)<<"\n";

				BranchInst &B = llvm::cast<BranchInst>(I);
				if(B.isUnconditional()) {

					auto nextBlock = B.getSuccessor(0);
					string nextBlockName = getSimpleNodeLabel(nextBlock);
					
					traversalStack.push({BB, nextBlock});

				} else if(B.isConditional()) {
					Value* condition = B.getCondition();

					BasicBlock* truePath = B.getSuccessor(0);
					BasicBlock* falsePath = B.getSuccessor(1);

					string trueBlockName = getSimpleNodeLabel(truePath);
					string falseBlockName = getSimpleNodeLabel(falsePath);

					// just push both if not path sensitive
					if(!isPathSensitive) {
						nextNodeCandidates.push_back({BB, truePath});
						nextNodeCandidates.push_back({BB, falsePath});
						continue;
					}

					// interval must be inside [0, 1]
					AbstractDomain conditionInterval = intervalInBlock[condition];
					if(!(conditionInterval.mn >= 0 && conditionInterval.mx <= 1 && !conditionInterval.isEmpty)) {
						errs()<<"INVALID INTERVAL FOUND in branch, variable: "<<getValueName(condition)<<" "<<conditionInterval<<"\n";
						assert(false);
					}

					int can0 = (conditionInterval.mn == 0);
					int can1 = (conditionInterval.mx == 1);

					if(can0) {
						errs()<<"Added new conditional path: "<<blockName<<" "<<falseBlockName<<"\n";
						nextNodeCandidates.push_back({BB, falsePath});
					}

					if(can1) {
						errs()<<"Added new conditional path: "<<blockName<<" "<<trueBlockName<<"\n";
						nextNodeCandidates.push_back({BB, truePath});
					}
			
				}
			}

			
		}

		// will loop again from the front. will be used in forloops
		
		// fixpoint not reached
		if(intervalAnalysis[blockName] != intervalInBlock) {
			intervalAnalysis[blockName] = intervalInBlock;

			for(auto nextCandidate: nextNodeCandidates)
				traversalStack.push(nextCandidate);
		}
		errs()<<"DONE PROCESSING "<<blockName<<"\n\n";



		//printIntervals(intervalAnalysis, variables);
		//exit(0);
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

	auto variables = findAllVariables(F);

	//errs()<<variables.size()<<"\n";

	intervalMergeFunct mergeAsF = mergeNormal;

	map<string, VariableInterval> intervals;

	intervals = intervalAnalysisProcess(F, true, intervals, variables, widening);
	intervals = intervalAnalysisProcess(F, true, intervals, variables, narrowing);

	for(auto item: intervals) {
		outs()<<"intervals in: "<<item.first<<"\n";

		for(auto varInterval: item.second) {
			if(variables.find(varInterval.first) != variables.end())
				outs()<<getValueName(varInterval.first)<<" "<<varInterval.second<<"\n";
		}

		outs()<<"\n";
	}



	
}