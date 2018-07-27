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
          auto *DT = new DataLayout(F.getParent());
          Type *i32 = Type::getInt32Ty(getGlobalContext());
          for (auto &sv : search_symvar_declare(F)) {
            ArrayType *aint = ArrayType::get(i32, 5);
            ArrayType *at = ArrayType::get(sv.type->getElementType(), 5);
            auto *fst_block = new AllocaInst(aint, "fst_array", sv.start_point);
            auto *scd_block = new AllocaInst(at, "scd_array", sv.start_point);

            for (u_int64_t i = 0; i < 5; i++) {
              Value* idx[2] = {ConstantInt::get(i32, 0), ConstantInt::get(i32, i)};
              auto af = ArrayRef<Value*>(idx, (size_t)2);
              auto *ptr = GetElementPtrInst::CreateInBounds(fst_block, af, "tt", sv.start_point);
              auto *li = new StoreInst(ConstantInt::get(i32, i, true), ptr, sv.start_point);
            }

            errs() << "Size: " << DT->getTypeAllocSize(sv.type->getElementType()) << "\n";

            Value* idx[2] = {ConstantInt::get(i32, 0), ConstantInt::get(i32, 2)};
            auto af = ArrayRef<Value*>(idx, (size_t)2);
            auto *ptr = GetElementPtrInst::CreateInBounds(scd_block, af, "ts", sv.start_point);
            auto *val = new LoadInst(sv.var, "val", sv.start_point);
            auto *store = new StoreInst(val, ptr, sv.start_point);

            errs() << scd_block->getName() << "\t" << *scd_block->getType() << "\n";
          }

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
