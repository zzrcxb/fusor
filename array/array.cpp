#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/CallSite.h"
#include "func_searcher.hpp"

using namespace std;
using namespace llvm;

namespace {
    struct ArrayPass : public FunctionPass {
        static char ID;

        ArrayPass() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
          vector<Instruction*> toBeDeleted;
          Instruction *tobe = nullptr;

          for (auto p : search_symvar_declare(F)) {
            errs() << p.first->getName() << "\t" << *p.second << "\n";
          }

//          for (auto p : search_symvar_func_call(F)) {
//            Function *func = p.first;
//            Instruction *I = p.second;
//
//            ImmutableCallSite ICS(I);
//
//            for (auto &arg : ICS.args()) {
//              if (auto *inst = dyn_cast<BitCastInst>(arg)) {
//                auto *target = inst->getOperand(0);
//                auto *type = dyn_cast<PointerType>(target->getType());
//                errs() << target->getName() << "\t" << *type << "\n";
//              }
//              else if (auto *inst = dyn_cast<AllocaInst>(arg)) {
//                auto *target = dyn_cast<Value>(arg);
//                auto *type = dyn_cast<PointerType>(target->getType());
//                errs() << *target << "\t" << *type << "\n";
//              }
//            }
//          }

//          for (auto &B : F) {
//            for (auto I = B.begin(); I != B.end(); ++I) {
//              if (auto *ct = dyn_cast<CallInst>(I)) {
//                if (ct->getCalledFunction()->getName() == SYMVAR_FUNC) {
//
//                  Function *func = ct->getCalledFunction();
//                  auto *arg = dyn_cast<Value>(func->arg_begin());
//
//                  --I;
//                  auto *ui = dyn_cast<UnaryInstruction>(I);
//                  auto *target = ui->getOperand(0);
//                  auto *t_t = dyn_cast<PointerType>(ui->getOperand(0)->getType());
//
//                  for (auto &u : arg->uses())
//                    errs() << *u.getUser() << "\n=============\n";
//
//                  for (auto &u : target->uses())
//                    errs() << *u.getUser() << "\n";
//
//                  errs() << target->getName() << "\t" << "\n";
//                  ++I;
//
//                  Type *type = Type::getInt32Ty(getGlobalContext());
//                  Instruction *ni = BinaryOperator::Create(Instruction::Add, ConstantInt::get(type, 4), ConstantInt::get(type, 4));
//                  Instruction *ni2 = new StoreInst(ConstantInt::get(t_t->getElementType(), 11), target);
//
////                  I->getParent()->getInstList().insert(BasicBlock::iterator(*I), ni);
//                  I->getParent()->getInstList().insert(BasicBlock::iterator(*I), ni2);
//                  break;
//                }
//              }
//            }
//            if (tobe)
//              break;
//          }
//
//          if (tobe)
//            tobe->eraseFromParent();

          errs() << "--------------\n" << F.getName() << "\n";
          F.dump();
          return true;
        }
    };
}

char ArrayPass::ID = 0;

static RegisterPass<ArrayPass> X("array", "Array Pass", false, false);

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
// static void registerArrayPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
//     PM.add(new ArrayPass());
// }
//
// static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerArrayPass);
