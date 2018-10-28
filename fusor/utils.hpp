//
// Created by neil on 10/4/18.
//

#ifndef PROJECT_UTILS_HPP
#define PROJECT_UTILS_HPP

#include "llvm/Pass.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Type.h"

#include <vector>
#include <deque>
#include <set>
#include <random>

#define True true
#define False false // For Python lovers!
#define IN_SET(ELEM, SET) (SET.find(ELEM) != SET.end())
#define IN_MAP(KEY, MAP) (MAP.find(KEY) != MAP.end())
#define ISINSTANCE(OBJ_P, CLASS) (dyn_cast<CLASS>(OBJ_P))

#define SYMVAR_FUNC "fusor_symvar"

// get common types
extern llvm::Type *Int1;
extern llvm::Type *Int1_ptr;
extern llvm::Type *Int8;
extern llvm::Type *Int8_ptr;
extern llvm::Type *Int16;
extern llvm::Type *Int16_ptr;
extern llvm::Type *Int32;
extern llvm::Type *Int32_ptr;
extern llvm::Type *Int64;
extern llvm::Type *Int64_ptr;
extern llvm::Type *Float;
extern llvm::Type *Float_ptr;
extern llvm::Type *Double;
extern llvm::Type *Double_ptr;

typedef std::map<llvm::Value*, llvm::Instruction*> SymvarLoc;


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


std::vector<llvm::Instruction *> search_symvar_func_call(llvm::Function &F);


std::vector<SymVar> search_symvar_declare(llvm::Function &F);


template <typename T> std::vector<T> range(T start, T end, T step) {
  std::vector<T> res;

  for (T i = start; step > 0 ? i < end : i > end; i += step) {
    res.push_back(i);
  }

  return res;
}


template <typename T> std::vector<T> range(T start, T end) {
  return range<T>(start, end, 1);
}


template <typename T> std::vector<T> range(T end) {
  return range<T>(0, end, 1);
}


std::map<llvm::Value*, llvm::Value*> cast_sv_to_uint8(SymvarLoc &svs_loc, llvm::IRBuilder<> &irbuilder);


llvm::BasicBlock *split_bb_randomly(llvm::BasicBlock *bb, std::set<llvm::Value*> pre_req,
        std::default_random_engine *rand_eng);


llvm::BasicBlock *fake_bb_builder(llvm::BasicBlock *base_bb, std::vector<llvm::Instruction*> seed);


#endif //PROJECT_UTILS_HPP
