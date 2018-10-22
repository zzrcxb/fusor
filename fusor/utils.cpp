//
// Created by neil on 10/20/18.
//
#include "utils.hpp"


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
