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

		if(numPositiveInfinity == 1 && numNegativeInfinity == 1) {
			return AbstractNumber(0, InfinityType::NegativeInfinity);
		}


		// if modulo by inf then first element
		// if inf module then second element
		if(numPositiveInfinity == 1 || numNegativeInfinity == 1) {

			if(other.isInfinity()) return *this;
			else return other;
		}
		

		// two finites
		return AbstractNumber(number % other.number);
	}
};

int main() {
	
}