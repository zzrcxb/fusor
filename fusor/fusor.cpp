#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/CommandLine.h"
#include "utils.hpp"
#include "inheritance.hpp"

#include <algorithm>
#include <random>
#include <chrono>
#include <set>

using namespace std;
using namespace llvm;


cl::opt<int> POOL_SIZE("array_size", cl::desc("Obfuscation fusor's size"), cl::init(63));

namespace {
    struct FusorPass : public ModulePass {
        static char ID;

        FusorPass() : ModulePass(ID) {}

        bool runOnModule(Module &M) override {
          for (auto &F : M) {
            if (F.isDeclaration())
              continue;
            
            vector<Value *> sym_vars;
            deque<BasicBlock *> BBs;

            rand_engine.seed(++seed);

            errs() << "Obfuscating \"" << F.getName() << "\" with fusor size " << (int) ARRAY_SIZE << "\t";

            // Initialize
            for (auto &a : F.args())
              sym_vars.emplace_back(&a);

            if (sym_vars.empty()) // nothing will be changed if this function's arg list is empty
              return False;

            for (auto &B : F)
              BBs.emplace_back(&B);

            // move symvar alloca and store instruction into front
            auto *sv_bb = BasicBlock::Create(F.getContext(), "sv_bb", &F, BBs.front());
            BranchInst::Create(BBs.front(), sv_bb);

            auto svs_loc = move_symvar_to_front(sv_bb, sym_vars);
            // after moving, then you can do whatever you want with symvar

            auto *puzzle = puzzle2(sv_bb->getTerminator(), svs_loc);
            auto *fake = BasicBlock::Create(F.getContext(), "sv_bb", &F);
            auto *tailB = BBs.front()->splitBasicBlock(--BBs.front()->end(), "tail");
            sv_bb->getTerminator()->eraseFromParent();
            BranchInst::Create(BBs.front(), fake, puzzle, sv_bb);

//            ReturnInst::Create(F.getContext(), ConstantInt::get(i32, 1), fake);
            BranchInst::Create(BBs.front(), fake);
            BBs.front()->getTerminator()->eraseFromParent();
            BranchInst::Create(tailB, fake, puzzle, BBs.front());

            errs() << "====== DONE ======\n";
          }
          return True;
        }

    private:
        Value *puzzle3(Instruction *insert_point, map<Value *, Instruction *> svs_loc, size_t sv_cnt = 0) { // 0 for all
          ArrayType *aint = ArrayType::get(i8, ARRAY_SIZE);
          uniform_int_distribution<uint8_t> i8_generator(0, 127);
          vector<Constant *> data1(ARRAY_SIZE), data2(ARRAY_SIZE);

          // setup sv_to_run
          map<Value *, Instruction *> sv_to_run;
          size_t cnt = 0;
          for (auto &p : svs_loc) {
            if (cnt < sv_cnt || sv_cnt == 0)
              sv_to_run.insert(p);
            else
              break;
            cnt++;
          }

          // get truncated symvars
          map<Value *, Instruction *> svs_index;
          for (auto &p : sv_to_run) {
            auto *sv = p.first;
            auto *loc = p.second;
            auto *index = new LoadInst(new BitCastInst(loc, i8p, "casted", insert_point), "trunced.symvar",
                                       insert_point);
            svs_index.insert(pair<Value *, Instruction *>(sv, index));
          }

          Value *result = ConstantInt::get(i1, 1);
          for (auto p : svs_index) {
            auto *sv = p.first;
            auto *val = p.second;
            result = BinaryOperator::Create(Instruction::BinaryOps::And,
                                            new ICmpInst(insert_point, CmpInst::ICMP_SGT, val, ConstantInt::get(i8, 50),
                                                         "cmp"),
                                            result, "res", insert_point);
          }

          return result;
        }


        Value *puzzle2(Instruction *insert_point, map<Value *, Instruction *> svs_loc, size_t sv_cnt = 0) { // 0 for all
          ArrayType *aint = ArrayType::get(i8, ARRAY_SIZE);
          uniform_int_distribution<uint8_t> i8_generator(0, 127);
          vector<Constant *> data1(ARRAY_SIZE), data2(ARRAY_SIZE);

          // setup sv_to_run
          map<Value *, Instruction *> sv_to_run;
          size_t cnt = 0;
          for (auto &p : svs_loc) {
            if (cnt < sv_cnt || sv_cnt == 0)
              sv_to_run.insert(p);
            else
              break;
            cnt++;
          }

          // setup mapping
          vector<uint8_t> mapping;
          for (uint8_t i = 0; i < ARRAY_SIZE; i++)
            mapping.push_back(i);
          shuffle(mapping.begin(), mapping.end(), rand_engine);

          // generate arrays
          for (uint8_t i = 0; i < ARRAY_SIZE; i++) {
            auto tmp1 = (i8_generator(rand_engine) / ARRAY_SIZE) * ARRAY_SIZE + mapping[i];
            auto tmp2 = (i8_generator(rand_engine) / ARRAY_SIZE) * ARRAY_SIZE + i;
            if (tmp1 % ARRAY_SIZE != mapping[i] || tmp2 % ARRAY_SIZE != i)
              errs() << "=== ERROR! ===\n";
            data1[i] = ConstantInt::get(i8, tmp1);
            data2[mapping[i]] = ConstantInt::get(i8, tmp2);
          }

          auto *module = insert_point->getParent()->getParent()->getParent();
          auto *array1 = new GlobalVariable(*module, aint, True, GlobalValue::InternalLinkage,
                                            ConstantArray::get(aint, data1), "fusor");
          auto *array2 = new GlobalVariable(*module, aint, True, GlobalValue::InternalLinkage,
                                            ConstantArray::get(aint, data2), "fusor");
          Value *result = ConstantInt::get(i1, 1);

          // get truncated symvars
          map<Value *, Instruction *> svs_index;
          for (auto &p : sv_to_run) {
            auto *sv = p.first;
            auto *loc = p.second;
            auto *index = BinaryOperator::Create(Instruction::BinaryOps::URem,
                                                 new LoadInst(
                                                         new BitCastInst(loc, i8p, "casted", insert_point),
                                                         "trunced.symvar", insert_point),
                                                 ConstantInt::get(i8, ARRAY_SIZE), "index", insert_point);
            svs_index.insert(pair<Value *, Instruction *>(sv, index));
          }

          // load from fusor
          map<Value *, Instruction *> svs_index_loaded;
          for (auto &p : svs_index) {
            auto *sv = p.first;
            auto *index = p.second;
            Value *under[2] = {ConstantInt::get(i8, 0), nullptr};
            under[1] = index;
            auto *fst_data = new LoadInst(
                    GetElementPtrInst::CreateInBounds(array1, ArrayRef<Value *>(under, 2), "ptr", insert_point),
                    "a1_data", insert_point);
            auto *index2 = BinaryOperator::Create(Instruction::BinaryOps::URem,
                                                  fst_data, ConstantInt::get(i8, ARRAY_SIZE), "index2", insert_point);

            under[1] = index2;
            auto *scd_data = new LoadInst(
                    GetElementPtrInst::CreateInBounds(array2, ArrayRef<Value *>(under, 2), "ptr", insert_point),
                    "a2_data", insert_point);
            auto *index3 = BinaryOperator::Create(Instruction::BinaryOps::URem,
                                                  scd_data, ConstantInt::get(i8, ARRAY_SIZE), "index3", insert_point);
            svs_index_loaded.insert(pair<Value *, Instruction *>(sv, index3));
          }

          // calculate results
          for (auto &p : svs_index_loaded) {
            auto *sv = p.first;
            auto *index = svs_index[sv];
            auto *index_loaded = p.second;
            result = BinaryOperator::Create(Instruction::BinaryOps::And,
                                            new ICmpInst(insert_point, CmpInst::ICMP_EQ, index_loaded, index, "cmp"),
                                            result, "res", insert_point);
          }

          return result;
        }


        Value *build_puzzle(Instruction *insert_point, map<Value *, Instruction *> svs_loc, size_t num_nested = 2) {
          ArrayType *aint = ArrayType::get(i8, ARRAY_SIZE);
          uniform_int_distribution<uint8_t> i8_generator(0, 255);
          map<Value *, Instruction *> svs_index;
          vector<AllocaInst *> allocated;
          Value *result = ConstantInt::get(i1, 1);

          for (size_t _ = 0; _ < num_nested; _++) {
            auto *array = new AllocaInst(aint, "fusor", insert_point);
            allocated.push_back(array);

            for (uint64_t i = 0; i < ARRAY_SIZE; i++) {
              Value *under[2] = {ConstantInt::get(i8, 0), ConstantInt::get(i8, i)};
              auto index = ArrayRef<Value *>(under, 2);
              auto *ptr = GetElementPtrInst::CreateInBounds(array, index, "ptr", insert_point);
              new StoreInst(ConstantInt::get(i8, i8_generator(rand_engine)), ptr, insert_point);
            }
          }

          // generate puzzle
          for (auto p : svs_loc) {
            auto *sv = p.first;
            auto *loc = p.second;
            auto *index = BinaryOperator::Create(Instruction::BinaryOps::SRem,
                                                 BinaryOperator::Create(Instruction::BinaryOps::And,
                                                                        new LoadInst(
                                                                                new BitCastInst(loc, i8p, "casted",
                                                                                                insert_point),
                                                                                "trunced.symvar", insert_point),
                                                                        ConstantInt::get(i8, 0x7F), "fxxk_srem",
                                                                        insert_point),
                                                 ConstantInt::get(i8, ARRAY_SIZE), "index", insert_point);
            svs_index.insert(pair<Value *, Instruction *>(sv, index)); // stored for later use, in case symvar changed

            for (size_t layer = 0; layer < num_nested - 1; layer++) {
              auto *data = new LoadInst(allocated[layer], "data", insert_point);
              Value *under[2] = {ConstantInt::get(i8, 0), index};
              auto af_index = ArrayRef<Value *>(under, 2);
              index = BinaryOperator::Create(Instruction::BinaryOps::SRem,
                                             BinaryOperator::Create(Instruction::BinaryOps::And,
                                                                    new LoadInst(
                                                                            GetElementPtrInst::CreateInBounds(
                                                                                    allocated[layer], af_index, "ptr",
                                                                                    insert_point),
                                                                            "data", insert_point),
                                                                    ConstantInt::get(i8, 0x7F), "fxxk_srem",
                                                                    insert_point),
                                             ConstantInt::get(i8, ARRAY_SIZE), "index", insert_point);
            }
            Value *under[2] = {ConstantInt::get(i8, 0), index};
            auto af_index = ArrayRef<Value *>(under, 2);
            new StoreInst(ConstantInt::get(i8, 0xFF),
                          GetElementPtrInst::CreateInBounds(allocated.back(), af_index, "ptr", insert_point),
                          insert_point);
          }

          // load the data
          for (auto p : svs_index) {
            auto *sv = p.first;
            auto *index = p.second;
            for (size_t layer = 0; layer < num_nested - 1; layer++) {
              auto *data = new LoadInst(allocated[layer], "data", insert_point);
              Value *under[2] = {ConstantInt::get(i8, 0), index};
              auto af_index = ArrayRef<Value *>(under, 2);
              index = BinaryOperator::Create(Instruction::BinaryOps::SRem,
                                             BinaryOperator::Create(Instruction::BinaryOps::And,
                                                                    new LoadInst(
                                                                            GetElementPtrInst::CreateInBounds(
                                                                                    allocated[layer], af_index, "ptr",
                                                                                    insert_point),
                                                                            "data", insert_point),
                                                                    ConstantInt::get(i8, 0x7F), "fxxk_srem",
                                                                    insert_point),
                                             ConstantInt::get(i8, ARRAY_SIZE), "index", insert_point);
            }
            Value *under[2] = {ConstantInt::get(i8, 0), index};
            auto af_index = ArrayRef<Value *>(under, 2);
            auto *data = new LoadInst(
                    GetElementPtrInst::CreateInBounds(allocated.back(), af_index, "ptr", insert_point),
                    "data", insert_point);
            result = BinaryOperator::Create(Instruction::BinaryOps::And,
                                            new ICmpInst(insert_point, CmpInst::ICMP_EQ, data,
                                                         ConstantInt::get(i8, 255), "cmp"),
                                            result, "res", insert_point);
          }
          return result;
        }


        deque<Instruction *> around_half_div(BasicBlock *bdiv, float threshold = .9) {
          set<Instruction *> cannot_move;
          deque<Instruction *> gonna_move;
          auto total_size = bdiv->size();
          auto start_point = bdiv->getFirstInsertionPt();
          auto end_point = bdiv->getTerminator();
          errs() << bdiv->getName() << "\t" << *ISINSTANCE(start_point, Instruction) << "\n";
          // search for all instructions before
          for (auto &I : *bdiv) {
            if (&I == start_point)
              break;
            for (User *u : I.users()) {
              if (auto *ui = ISINSTANCE(u, Instruction)) {
                if (ui->getParent() == bdiv)
                  cannot_move.insert(ui);
              }
            }
          }

          uniform_real_distribution<float> gen(.0, 1.0);

          auto cit = start_point;
          for (auto stop_point = bdiv->end()--; cit != stop_point; cit++) {
            if (auto *ci = ISINSTANCE(cit, Instruction)) {
              if (!IN_SET(ci, cannot_move) && gen(rand_engine) < threshold)
                gonna_move.push_back(ci);
              else
                break;
            }
          }

          return gonna_move;
        }


        bool insert_randomly(BasicBlock *bins, float threshold, deque<Instruction *> &gonna_move) {
          set<Instruction *> scanned;
          map<Instruction *, Instruction *> iips; // instruction insert points

          auto insert_point = bins->getFirstInsertionPt();
          // skip instructions before insert point
          for (auto &I : *bins) {
            if (&I != ISINSTANCE(insert_point, Instruction)) {
              scanned.insert(&I);
            } else
              break;
          }

          for (auto *I : gonna_move) {
            for (Use &u : I->operands()) { // check all uses
              if (auto *ui = ISINSTANCE(u.get(), Instruction)) {
                if (!IN_SET(ui, scanned) && ui->getParent() == bins) {
                  while (ui != ISINSTANCE(insert_point, Instruction) && insert_point != bins->end()) {
                    insert_point++;
                    scanned.insert(ISINSTANCE(insert_point, Instruction));
                  }
                }
                iips.insert(pair<Instruction *, Instruction *>(I, insert_point));
              }
            } // end of uses check
          }

          return True;
        }


        vector<Constant *> rand_index_generator(size_t size) {
          vector<Constant *> results(size);
          uniform_int_distribution<uint8_t> generator(0, 255);
          for (size_t i = 0; i < size; i++)
            results.push_back(ConstantInt::get(i8, generator(rand_engine)));

          return results;
        }


        const SymvarLoc move_symvar_to_front(BasicBlock *BB, const vector<Value *> &sym_var) {
          Instruction *insert_point = dyn_cast<Instruction>(BB->begin());
          map<Value *, Instruction *> locations;

          for (auto *sv : sym_var) {
            bool found = False;
            for (auto &u : sv->uses()) {
              if (auto *storeI = dyn_cast<StoreInst>(u.getUser())) {
                if (auto *allocaI = dyn_cast<AllocaInst>(storeI->getOperand(1))) {
                  allocaI->moveBefore(insert_point);
                  storeI->moveBefore(insert_point);
                  locations.insert(pair<Value *, Instruction *>(sv, allocaI));
                  found = true;
                }
              }
            }
            if (!found) { // Allocate arguments in case compiler gets some of them optimized. Not be tested!!!
              auto *addr = new AllocaInst(sv->getType(), sv->getName(), insert_point);
              locations.insert(pair<Value *, Instruction *>(sv, addr));
              new StoreInst(sv, addr, insert_point);
            }
          }

          return locations;
        }


        unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
        default_random_engine rand_engine;

        uint8_t ARRAY_SIZE = (uint8_t)POOL_SIZE;
        Type *i32 = Type::getInt32Ty(getGlobalContext());
        Type *i1 = Type::getInt1Ty(getGlobalContext());
        Type *i8 = Type::getInt8Ty(getGlobalContext());
        Type *i8p = Type::getInt8PtrTy(getGlobalContext());
        Type *i32p = Type::getInt32PtrTy(getGlobalContext());
    };
};


char FusorPass::ID = 0;

static RegisterPass<FusorPass> X("fusor", "Fusor Pass", False, False);

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerFusorPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
  PM.add(new FusorPass());
}

//static RegisterStandardPasses RegisterMyPass2(PassManagerBuilder::EP_EarlyAsPossible, registerFusorPass);
static RegisterStandardPasses RegisterMyPass1(PassManagerBuilder::EP_EnabledOnOptLevel0, registerFusorPass);
static RegisterStandardPasses RegisterMyPass2(PassManagerBuilder::EP_OptimizerLast, registerFusorPass);
