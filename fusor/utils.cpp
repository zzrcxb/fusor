//
// Created by neil on 10/20/18.
//
#include "utils.hpp"


using namespace std;
using namespace llvm;


std::vector<llvm::Instruction *> search_symvar_func_call(llvm::Function &F) {
  std::vector<llvm::Instruction *> ret_funcs;
  for (auto &B : F) {
    for (auto &I : B) {
      if (auto *call = llvm::dyn_cast<llvm::CallInst>(&I)) {
        if (call) {
          llvm::Function *func = call->getCalledFunction();
          if (func->getName() == SYMVAR_FUNC) {
            ret_funcs.emplace_back(call);
          }
        }
      }
    }
  }
  return ret_funcs;
}


std::vector<SymVar> search_symvar_declare(llvm::Function &F) {
  std::vector<SymVar> res;

  for (auto *I : search_symvar_func_call(F)) {
    llvm::ImmutableCallSite ICS(I);

    for (auto &arg : ICS.args()) {
      if (auto *inst = llvm::dyn_cast<llvm::BitCastInst>(arg)) {
        auto *target = inst->getOperand(0);
        auto *type = llvm::dyn_cast<llvm::PointerType>(target->getType());
        res.emplace_back(SymVar(I, target, type, inst));
      } else if (llvm::dyn_cast<llvm::AllocaInst>(arg)) {
        // For char*, it won't have a bitcast instruction for char*
        auto *target = llvm::dyn_cast<llvm::Value>(arg);
        auto *type = llvm::dyn_cast<llvm::PointerType>(target->getType());
        res.emplace_back(SymVar(I, target, type));
      }
    }
  }

  return res;
}


std::map<llvm::Value*, llvm::Value*> cast_sv_to_uint8(SymvarLoc &svs_loc, llvm::IRBuilder<> &irbuilder) {
  std::map<llvm::Value*, llvm::Value*> res;

  for (auto &p : svs_loc) {
    auto *sv = p.first;
    auto *loc = p.second;

    auto *btc = irbuilder.CreateBitCast(loc, Int8_ptr, "btc");
    btc = irbuilder.CreateLoad(btc, "loaded");
    res.insert(std::pair<llvm::Value*, llvm::Value*>(sv, btc));
  }

  return res;
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
