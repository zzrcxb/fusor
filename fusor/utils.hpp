//
// Created by neil on 10/4/18.
//

#ifndef PROJECT_UTILS_HPP
#define PROJECT_UTILS_HPP

#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <vector>
#include <deque>

#define True true
#define False false // For Python lovers!
#define IN_SET(ELEM, SET) (SET.find(ELEM) != SET.end())
#define IN_MAP(KEY, MAP) (MAP.find(KEY) != MAP.end())
#define ISINSTANCE(OBJ_P, CLASS) (dyn_cast<CLASS>(OBJ_P))

#define SYMVAR_FUNC "fusor_symvar"


const auto *Int1 = llvm::Type::getInt1Ty(llvm::getGlobalContext());
const auto *Int1_ptr = llvm::Type::getInt1PtrTy(llvm::getGlobalContext());
const auto *Int8 = llvm::Type::getInt8Ty(llvm::getGlobalContext());
const auto *Int8_ptr = llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
const auto *Int16 = llvm::Type::getInt16Ty(llvm::getGlobalContext());
const auto *Int16_ptr = llvm::Type::getInt16PtrTy(llvm::getGlobalContext());
const auto *Int32 = llvm::Type::getInt32Ty(llvm::getGlobalContext());
const auto *Int32_ptr = llvm::Type::getInt32PtrTy(llvm::getGlobalContext());
const auto *Int64 = llvm::Type::getInt64Ty(llvm::getGlobalContext());
const auto *Int64_ptr = llvm::Type::getInt64PtrTy(llvm::getGlobalContext());
const auto *Float = llvm::Type::getFloatTy(llvm::getGlobalContext());
const auto *Float_ptr = llvm::Type::getFloatPtrTy(llvm::getGlobalContext());
const auto *Double = llvm::Type::getDoubleTy(llvm::getGlobalContext());
const auto *Double_ptr = llvm::Type::getDoublePtrTy(llvm::getGlobalContext());


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


#endif //PROJECT_UTILS_HPP
