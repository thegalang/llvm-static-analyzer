#include <cstdio>
#include <iostream>
#include <set>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

void generatePath(BasicBlock* BB);
std::string getSimpleNodeLabel(const BasicBlock *Node);

int main(int argc, char **argv)
{
    // Read the IR file.
    LLVMContext &Context = getGlobalContext(); //A reference to the core LLVM "engine" that you should pass to the various methods that require a LLVMContext.
    SMDiagnostic Err;
    
    // Extract Module M from IR (assuming only one Module exists)
    // Modules are the top level container of all other LLVM Intermediate Representation (IR) objects.
    Module *M = ParseIRFile(argv[1], Err, Context);  
    if (M == nullptr)
    {
      fprintf(stderr, "error: failed to load LLVM IR file \"%s\"", argv[1]);
      return EXIT_FAILURE;
    }

    // 1.Extract Function main from Module M
    for (auto &F: *M)
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
    raw_string_ostream OS(Str);  //A raw_ostream that writes to an std::string. 
    Node->printAsOperand(OS, false);
    return OS.str();
}
