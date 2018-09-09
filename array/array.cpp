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
#include "llvm/Support/CommandLine.h"
#include "func_searcher.hpp"
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;
using namespace llvm;

#define True true
#define False false // For Python lovers!

cl::opt<int> POOL_SIZE("array_size", cl::desc("Obfuscation array's size"), cl::init(10));


namespace {
    struct SymArrayGroup {
        Value *fst_array, *scd_array, *fst_index, *scd_index, *true_index;
        SymArrayGroup(Value *fst_array, Value *scd_array, Value *fst_index, Value *scd_index, Value *true_index)
        : fst_array(fst_array), scd_array(scd_array), fst_index(fst_index), scd_index(scd_index), true_index(true_index) {}
    };

    struct ArrayPass : public FunctionPass {
        static char ID;

        ArrayPass() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
          rand_engine.seed(++seed);
          vector<Value*> sym_vars;
          deque<BasicBlock*> BBs;
          bool isFirst = true;
          vector<SymArrayGroup> groups;

          DEFAULT_ARRAY_SIZE = POOL_SIZE;
          errs() << "Obfuscating function \"" << F.getName() << "\" with array size " << POOL_SIZE << "\n";

          for (auto &a : F.args())
            sym_vars.emplace_back(&a);

          if (sym_vars.empty())
            return true;

          for (auto &B : F)
            BBs.emplace_back(&B);

          while (!BBs.empty()) {
            BasicBlock *B = BBs.front();
            BasicBlock *originB = B->splitBasicBlock(B->getFirstNonPHIOrDbgOrLifetime(), "originB");

            if (isFirst) {
              groups = alloca_dyn_mem(B, sym_vars);
              isFirst = false;
            }

            BasicBlock *bb = BasicBlock::Create(F.getContext(), "fake", &F);
            B->getTerminator()->eraseFromParent();
            vector<Value*> cmp_res;
            for (auto g : groups) {
              Value* idx[2] = {ConstantInt::get(i8, 0), g.true_index};
              auto af = ArrayRef<Value*>(idx, (size_t)2);
              auto *ptr1 = GetElementPtrInst::CreateInBounds(g.fst_array, af, "ptr1", B);
              auto *index = new LoadInst(ptr1, "index", B);
              idx[1] = index;
              auto *ptr2 = GetElementPtrInst::CreateInBounds(g.scd_array, af, "ptr2", B);
              auto *data = new LoadInst(ptr2, "data", B);
              auto *cmp = new ICmpInst(*B, CmpInst::ICMP_SGT, data, ConstantInt::get(i8, 0), "res");
              cmp_res.emplace_back(cmp);
            }

            auto *cmp = cmp_res.back();
            cmp_res.pop_back();
            while (!cmp_res.empty()) {
              auto *rhs = cmp_res.back();
              cmp_res.pop_back();
              cmp = BinaryOperator::Create(Instruction::And, cmp, rhs, "cmp", B);
            }

            BranchInst::Create(originB, bb, cmp, B);

            auto *tailB = originB->splitBasicBlock(--originB->end(), "tailB");
            originB->getTerminator()->eraseFromParent();
            BranchInst::Create(tailB, bb, cmp, originB);

            BranchInst::Create(originB, bb);

            BBs.pop_front();
          }

          return true;
        }

    protected:
        vector<size_t> awful_cpp_without_range(size_t start, size_t end) {
          // End with end - 1
          vector<size_t> res;
          for (size_t i = start; i < end; i++)
            res.emplace_back(i);
          return res;
        }

        vector<size_t> intrange(size_t end) {
          return awful_cpp_without_range(0, end);
        }

        vector<SymArrayGroup> alloca_dyn_mem(BasicBlock* BB, const vector<Value*> &sym_var) {
          Instruction *insert_point = dyn_cast<Instruction>(BB->begin());
          ArrayType *aint = ArrayType::get(i8, DEFAULT_ARRAY_SIZE);
          vector<SymArrayGroup> res;
          vector<Value*> real_sym_var_loc;

          // Move argument allocation and initialization
          for (auto *sv : sym_var) {
            bool found = false;
            for (auto &u : sv->uses()) {
              if (auto *storeI = dyn_cast<StoreInst>(u.getUser())) {
                if (auto *allocaI = dyn_cast<AllocaInst>(storeI->getOperand(1))) {
                  allocaI->moveBefore(insert_point);
                  storeI->moveBefore(insert_point);
                  real_sym_var_loc.emplace_back(allocaI);
                  found = true;
                }
              }
            }
            if (!found) { // Allocate arguments in case compiler gets some of them optimized. Not be tested!!!
              auto *addr = new AllocaInst(sv->getType(), sv->getName(), insert_point);
              real_sym_var_loc.emplace_back(addr);
              new StoreInst(sv, addr, insert_point);
            }
          }

          for (auto *sv : real_sym_var_loc) {
            auto *fst_array = new AllocaInst(aint, "fst_array", insert_point);
            auto *scd_array = new AllocaInst(aint, "scd_array", insert_point);
            vector<size_t> fst_index = intrange(DEFAULT_ARRAY_SIZE);
            shuffle(fst_index.begin(), fst_index.end(), rand_engine); // Shuffle it
            vector<size_t> scd_index = intrange(DEFAULT_ARRAY_SIZE);
            shuffle(scd_index.begin(), scd_index.end(), rand_engine); // Shuffle it

            // These can be replaced by llvm.memset
            for (u_int64_t i = 0; i < DEFAULT_ARRAY_SIZE; i++) {
              Value* idx[2] = {ConstantInt::get(i8, 0), ConstantInt::get(i8, i)};
              auto af = ArrayRef<Value*>(idx, (size_t)2);
              auto *ptr1 = GetElementPtrInst::CreateInBounds(fst_array, af, "ptr1", insert_point);
              new StoreInst(ConstantInt::get(i8, fst_index[i], true), ptr1, insert_point);
              auto *ptr2 = GetElementPtrInst::CreateInBounds(scd_array, af, "ptr2", insert_point);
              new StoreInst(ConstantInt::get(i8, -scd_index[i], true), ptr2, insert_point);
            }

            // Trunc symvar to char
            auto *trunc_p = new BitCastInst(sv, i8p, "casted", insert_point);
            auto *trunced = new LoadInst(trunc_p, "trunced.symvar", insert_point);
            auto *fixed = BinaryOperator::Create(Instruction::BinaryOps::And, trunced, ConstantInt::get(i8, 0x7F), "bug_fix", insert_point);
            auto *true_index = BinaryOperator::Create(Instruction::BinaryOps::SRem, fixed, ConstantInt::get(i8, DEFAULT_ARRAY_SIZE), "truci", insert_point);
            Value* idx[2] = {ConstantInt::get(i8, 0), true_index};
            auto af = ArrayRef<Value*>(idx, (size_t)2);
            auto *ptr1 = GetElementPtrInst::CreateInBounds(fst_array, af, "ptr1", insert_point);
            auto *index2 = new LoadInst(ptr1, "index2", insert_point);

            idx[1] = index2;
            auto *ptr2 = GetElementPtrInst::CreateInBounds(scd_array, af, "ptr2", insert_point);
            auto *original = new LoadInst(ptr2, "orgi", insert_point);
            auto *final_val = BinaryOperator::Create(Instruction::BinaryOps::Add, original, ConstantInt::get(i8, DEFAULT_ARRAY_SIZE), "res", insert_point);
            auto *store = new StoreInst(final_val, ptr2, insert_point);

            res.emplace_back(SymArrayGroup(fst_array, scd_array, ptr1, ptr2, true_index));
          }

          return res;
        }

    private:
        unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
        default_random_engine rand_engine;

        uint64_t DEFAULT_ARRAY_SIZE = 10;
        Type *i32 = Type::getInt32Ty(getGlobalContext());
        Type *i1 = Type::getInt1Ty(getGlobalContext());
        Type *i8 = Type::getInt8Ty(getGlobalContext());
        Type *i8p = Type::getInt8PtrTy(getGlobalContext());
    };
}

char ArrayPass::ID = 0;

static RegisterPass<ArrayPass> X("array", "Array Pass", false, false);

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerArrayPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new ArrayPass());
}

static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerArrayPass);
