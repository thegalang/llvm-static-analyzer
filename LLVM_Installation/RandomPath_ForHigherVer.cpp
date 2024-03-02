
// This file is compatible for llvm 3.6 and above since 'LLVMContext context' is removed after llvm 3.6.


#include <cstdio>
#include <iostream>
#include <set>
#include <iostream>
#include <cstdlib>
#include <ctime>


#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"


using namespace llvm;

void generatePath(BasicBlock* BB);
std::string getSimpleNodeLabel(const BasicBlock *Node);

// Helper method for converting the name of a LLVM type to a string
static std::string LLVMTypeAsString(const Type *T) {
  std::string TypeName;
  raw_string_ostream N(TypeName);
  T->print(N);
  return N.str();
}

class GVNames : public ModulePass {
public:
  GVNames() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) {
    for (Module::const_global_iterator GI = M.global_begin(),
                                       GE = M.global_end();
         GI != GE; ++GI) {
      outs() << "Found global named \"" << GI->getName()
             << "\": type = " << LLVMTypeAsString(GI->getType()) << "\n";
    }
    return false;
  }

  // The address of this member is used to uniquely identify the class. This is
  // used by LLVM's own RTTI mechanism.
  static char ID;
};

char GVNames::ID = 0;

int main(int argc, char **argv) {
  if (argc < 2) {
    errs() << "Usage: " << argv[0] << " <IR file>\n";
    return 1;
  }

  // Parse the input LLVM IR file into a module.
  SMDiagnostic Err;
  LLVMContext Context;
  std::unique_ptr<Module> Mod(parseIRFile(argv[1], Err, Context));
  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

   for (auto &F: *Mod)
      if (strncmp(F.getName().str().c_str(),"main",4) == 0){
        BasicBlock* BB = dyn_cast<BasicBlock>(F.begin()); 
        llvm::outs() << getSimpleNodeLabel(BB) << "\n";
	//BB->dump();
        generatePath(BB);
      }
    return 0;
}

void generatePath(BasicBlock* BB)
{
  const TerminatorInst *TInst = BB->getTerminator();
  unsigned NSucc = TInst->getNumSuccessors();
  if (NSucc == 1){
      BasicBlock *Succ = TInst->getSuccessor(0);
      llvm::outs() << getSimpleNodeLabel(Succ) << "\n";
      //Succ->dump();
      generatePath(Succ);
  }else if (NSucc>1){
      srand(time(NULL));
      unsigned rnd = std::rand() / (RAND_MAX/NSucc); // rand() return a number between 0 and RAND_MAX
      BasicBlock *Succ = TInst->getSuccessor(rnd);
      llvm::outs() << getSimpleNodeLabel(Succ) << "\n";
      //Succ->dump();
      generatePath(Succ);
  }
}

std::string getSimpleNodeLabel(const BasicBlock *Node) {
    if (!Node->getName().empty())
        return Node->getName().str();
    std::string Str;
    raw_string_ostream OS(Str);
    Node->printAsOperand(OS, false);
    return OS.str();
}
