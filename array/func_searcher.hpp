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
#include <utility>

#define SYMVAR_FUNC "fusor_symvar"


std::vector<llvm::Instruction*> search_symvar_func_call(llvm::Function &F) {
  std::vector<llvm::Instruction*> ret_funcs;
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


std::vector<std::pair<llvm::Value*, llvm::Type*>> search_symvar_declare(llvm::Function &F) {
  std::vector<std::pair<llvm::Value*, llvm::Type*>> res;
  std::vector<llvm::Instruction*> toBeErase;

  for (auto *I : search_symvar_func_call(F)) {
    llvm::ImmutableCallSite ICS(I);

    toBeErase.emplace_back(I);

    for (auto &arg : ICS.args()) {
      if (auto *inst = llvm::dyn_cast<llvm::BitCastInst>(arg)) {
        auto *target = inst->getOperand(0);
        auto *type = llvm::dyn_cast<llvm::PointerType>(target->getType());
        res.emplace_back(std::make_pair(target, type));
        toBeErase.emplace_back(inst);
      }
      else if (llvm::dyn_cast<llvm::AllocaInst>(arg)) {
        // For char*, it won't have a bitcast instruction for char*
        auto *target = llvm::dyn_cast<llvm::Value>(arg);
        auto *type = llvm::dyn_cast<llvm::PointerType>(target->getType());
        res.emplace_back(std::make_pair(target, type));
      }
    }
  }

  // Erase function calls related instructions
  while (!toBeErase.empty()) {
    toBeErase.back()->eraseFromParent();
    toBeErase.pop_back();
  }

  return res;
}

#endif //PROJECT_FUNC_SEARCHER_HPP
