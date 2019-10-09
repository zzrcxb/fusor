#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/CommandLine.h"
#include "utils.hpp"
#include "inheritance.hpp"
#include "factories.hpp"
#include "json.hpp"

#include <algorithm>
#include <random>
#include <chrono>
#include <set>
#include <cstdlib>
#include <fstream>

using namespace std;
using namespace llvm;

Type* Int1 = NULL;
Type* Int1_ptr = NULL;
Type* Int8 = NULL;
Type* Int8_ptr = NULL;
Type* Int16 = NULL;
Type* Int16_ptr = NULL;
Type* Int32 = NULL;
Type* Int32_ptr = NULL;
Type* Int64 = NULL;
Type* Int64_ptr = NULL;
Type* Float = NULL;
Type* Float_ptr = NULL;
Type* Double = NULL;
Type* Double_ptr = NULL;
Type* Void = NULL;

using json = nlohmann::json;

namespace {
    struct FusorPass : public ModulePass {
        static char ID;

        FusorPass() : ModulePass(ID) {}

        bool runOnModule(Module &M) override {
          Int1 = llvm::Type::getInt1Ty(M.getContext());
          Int1_ptr = llvm::Type::getInt1PtrTy(M.getContext());
          Int8 = llvm::Type::getInt8Ty(M.getContext());
          Int8_ptr = llvm::Type::getInt8PtrTy(M.getContext());
          Int16 = llvm::Type::getInt16Ty(M.getContext());
          Int16_ptr = llvm::Type::getInt16PtrTy(M.getContext());
          Int32 = llvm::Type::getInt32Ty(M.getContext());
          Int32_ptr = llvm::Type::getInt32PtrTy(M.getContext());
          Int64 = llvm::Type::getInt64Ty(M.getContext());
          Int64_ptr = llvm::Type::getInt64PtrTy(M.getContext());
          Float = llvm::Type::getFloatTy(M.getContext());
          Float_ptr = llvm::Type::getFloatPtrTy(M.getContext());
          Double = llvm::Type::getDoubleTy(M.getContext());
          Double_ptr = llvm::Type::getDoublePtrTy(M.getContext());
          Void = llvm::Type::getVoidTy(M.getContext());

          if (!load_config()) {
            errs() << "[Error] Fail to load configuration file, exiting...\n\n";
            exit(1);
          }

          for (auto &F : M) {
            // ignore declaration
            if (F.isDeclaration())
              continue;
            
            vector<Value *> sym_vars;
            deque<BasicBlock *> BBs;

            rand_engine.seed(++seed);

            outs() << "Obfuscating \"" << F.getName() << "\"\n";

            // Initialize
            for (auto &a : F.args())
              sym_vars.emplace_back(&a);

            if (sym_vars.empty()) // nothing will be changed if this function's arg list is empty
              continue;

            for (auto &B : F)
              BBs.emplace_back(&B);

            // move symvar alloca and store instruction into front
            auto *sv_bb = BasicBlock::Create(F.getContext(), "sv_bb", &F, BBs.front());
            BranchInst::Create(BBs.front(), sv_bb);

            auto svs_loc = move_symvar_to_front(sv_bb, sym_vars);
            // after moving, then you can do whatever you want with symvar

            PuzzleBuilderFactory pz_factory;
            FunctionTransformerFactory tr_factory;

            auto pz_builder = pz_factory.get_builder_randomly(pred_conf, &M);
            if (pz_builder == nullptr) {
              errs() << "[Error] Fail to get opaque predicates builder, exiting...\n\n";
              exit(1);
            }
            auto *predicate = pz_builder->build(svs_loc, sv_bb->getTerminator());

            // merge sv bb
            sv_bb->getTerminator()->eraseFromParent();
            auto *merge_point = ISINSTANCE(BBs.front()->getFirstInsertionPt(), Instruction);
            vector<Instruction*> backup_ins_sv;
            for (auto &I : *sv_bb) {
              backup_ins_sv.emplace_back(&I);
            }
            for (auto *I : backup_ins_sv) {
              I->moveBefore(merge_point);
            }
            sv_bb->eraseFromParent();

            auto tr_builder = tr_factory.get_transformer_randomly(trans_conf);
            if (tr_builder == nullptr) {
              errs() << "[Error] Fail to get transformer, exiting...\n\n";
              exit(1);
            }
            tr_builder->transform(&F, predicate);

            outs() << "====== DONE ======\n";
          }

          return True;
        }

    private:
        bool load_config() {
          string config_path = "./config.json";

          if (const char* env_p = getenv("FUSOR_CONFIG")) {
            config_path = env_p;
          }

          ifstream in_file(config_path);

          if (in_file.good()) {
            try {
              in_file >> config;
            }
            catch (json::parse_error) {
              errs() << "--- JSON format ERROR! --- \n";
              return False;
            }
          }
          else {
            errs() << "--- Config file " << config_path << " not found! ---\n";
            return False;
          }

          return config_checker();
        }

        bool config_checker() {
          // check key trans and pred
          string keys[] = {"predicates", "transformations"};

          for (auto &k : keys) {
            if (IN_MAP(k, config)) {
              for (auto &p : config[k]) {
                if (!IN_MAP("name", p)) {
                  errs() << "--- Missing \"name\" field ---\n";
                  return False;
                }
                if (!IN_MAP("code", p)) {
                  errs() << "--- Missing \"code\" field in " << p["name"].get<string>() << " ---\n";
                  return False;
                }

                if (k == "predicates") {
                  pred_conf.insert(pair<string, uint64_t>(p["name"].get<string>(), p["code"].get<uint64_t>()));
                }
                else if (k == "transformations") {
                  trans_conf.insert(pair<string, uint64_t>(p["name"].get<string>(), p["code"].get<uint64_t>()));
                }
              }
            }
            else {
              errs() << "--- Missing key: " << k << " ---\n";
              return False;
            }
          }

          return True;
        }

        unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
        default_random_engine rand_engine;
        json config;
        map<string, uint64_t> trans_conf, pred_conf;
    };
};


char FusorPass::ID = 0;

static RegisterPass<FusorPass> X("fusor", "Fusor Pass", False, False);

// Automatically enable the pass.
static void registerFusorPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
  PM.add(new FusorPass());
}

static RegisterStandardPasses RegisterMyPass1(PassManagerBuilder::EP_EnabledOnOptLevel0, registerFusorPass);
static RegisterStandardPasses RegisterMyPass2(PassManagerBuilder::EP_OptimizerLast, registerFusorPass);
