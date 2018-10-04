//
// Created by neil on 7/21/18.
//

#ifndef PROJECT_FUNC_SEARCHER_HPP
#define PROJECT_FUNC_SEARCHER_HPP

#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <vector>
#include <deque>

#define SYMVAR_FUNC "fusor_symvar"


struct SymVar {
    llvm::Instruction *start_point = nullptr;
    llvm::Value *var = nullptr;
    llvm::PointerType *type = nullptr;

    SymVar(llvm::Instruction *sp, llvm::Value *v, llvm::PointerType *t, llvm::Instruction *ref = nullptr)
            : start_point(sp), var(v), type(t) {
      to_be_deleted.emplace_back(sp);
      if (ref)
        to_be_deleted.emplace_back(ref);
    }

    llvm::Instruction* add_useless_inst(llvm::Instruction* inst) {
      to_be_deleted.emplace_front(inst);
    }

    void delete_all() {
      while (!to_be_deleted.empty()) {
        to_be_deleted.back()->eraseFromParent();
        to_be_deleted.pop_back();
      }
    }

private:
    std::deque<llvm::Instruction*> to_be_deleted;
};


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

#endif //PROJECT_FUNC_SEARCHER_HPP
