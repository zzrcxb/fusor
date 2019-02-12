//
// Created by neil on 1/5/19.
//

#include "inheritance.hpp"
#include "utils.hpp"
#include "llvm/IR/Constant.h"


using namespace std;
using namespace llvm;

const string DataFlowTransformer::id = "DataFlowTransformer";

Function *DataFlowTransformer::transform(Function *F, Value *predicate) {
  vector<GlobalVariable *> gvs;
  vector<Constant *> gv_ptrs;
  map<Constant *, GlobalVariable *> ptr_gv_mapper;
  for (auto &gl : F->getParent()->getGlobalList()) {
    if (gl.hasInitializer()) {
//      for (auto &u : gl.uses()) {
//        auto *usr = u.getUser();
//        for (auto &u2 : usr->uses())
//          errs() << "\t" << *u.get() << "\t" << *u.getUser() << "\n\t\t" << *u2.getUser() << "\n\n";
//      }
      if (auto *data = ISINSTANCE(gl.getInitializer(), ConstantDataArray)) {
        if (data->isString()) {
          Value *idx[2] = {ConstantInt::get(Int32, 0), ConstantInt::get(Int32, 0)};
          auto *ptr = ConstantExpr::getInBoundsGetElementPtr(data->getType(), &gl, ArrayRef<Value *>(idx, 2));
          gvs.emplace_back(&gl);
          gv_ptrs.emplace_back(ptr);
          ptr_gv_mapper[ptr] = &gl;
        }
      }
    }
  }

  shuffle(gv_ptrs.begin(), gv_ptrs.end(), rand_eng);
  ArrayType *a_type = ArrayType::get(Int8_ptr, gv_ptrs.size());
  map<Value *, size_t> gv_map;
  for (size_t i = 0; i < gv_ptrs.size(); i++) {
    gv_map[ptr_gv_mapper[gv_ptrs[i]]] = i;
  }

  auto *str_mapper = new GlobalVariable(*F->getParent(), a_type, True, GlobalVariable::InternalLinkage,
                                        ConstantArray::get(a_type, gv_ptrs), F->getName());

  uniform_int_distribution<uint32_t> i32_generator(0, static_cast<uint32_t>(gvs.size()));
  uint32_t true_offset = i32_generator(rand_eng), false_offset = i32_generator(rand_eng);

  auto *pred_bb = ISINSTANCE(predicate, Instruction)->getParent();
  auto iter = pred_bb->begin();
  for (; ISINSTANCE(iter, Value) != predicate && iter != pred_bb->end(); iter++);
  iter++;
  IRBuilder<> irbuilder(ISINSTANCE(iter, Instruction));
  auto *offset_ptr = irbuilder.CreateAlloca(Int32);
  auto *select = irbuilder.CreateSelect(predicate, ConstantInt::get(Int32, true_offset),
                                        ConstantInt::get(Int32, false_offset));
  irbuilder.CreateStore(select, offset_ptr);
  for (auto *gv : gvs) {
    for (auto u : gv->users()) {
      if (auto *expr = ISINSTANCE(u, ConstantExpr)) {
        if (expr->isGEPWithNoNotionalOverIndexing()) {
          for (auto u2 : expr->users()) {
            if (auto *ins = ISINSTANCE(u2, Instruction)) {
              if (ins->getParent()->getParent() == F && expr->getNumOperands() == 3 && ISINSTANCE(ins, PHINode) ==
                                                                                               nullptr) {
                irbuilder.SetInsertPoint(ins);
                auto actual_index = static_cast<uint32_t >(gv_map[gv]) ^ true_offset;
                auto *saved_select = irbuilder.CreateLoad(offset_ptr);

                auto *load_index = irbuilder.CreateXor(ConstantInt::get(Int32, actual_index), saved_select);
                Value *idx[2] = {ConstantInt::get(Int32, 0), load_index};
                auto *str_ptr_ptr = irbuilder.CreateInBoundsGEP(str_mapper, ArrayRef<Value *>(idx, 2));
                auto *str_ptr = irbuilder.CreateLoad(str_ptr_ptr);
                idx[0] = expr->getOperand(2);
//                errs() << *expr->getOperand(1) << "\n";
                auto *moved_ptr = irbuilder.CreateInBoundsGEP(str_ptr, ArrayRef<Value *>(idx, 1));

                ins->replaceUsesOfWith(u, moved_ptr);
              }
            }
          }
        }
      } // else if (auto *ins = ISINSTANCE(u, GetElementPtrInst)) {
//        if (ins->getNumIndices() == 2 && ins->getParent()->getParent() == F) {
//          irbuilder.SetInsertPoint(ins);
//          auto actual_index = static_cast<uint32_t >(gv_map[gv]) ^true_offset;
//          auto *saved_select = irbuilder.CreateLoad(offset_ptr);
//          auto *load_index = irbuilder.CreateXor(ConstantInt::get(Int32, actual_index), saved_select);
//          Value *idx[2] = {ConstantInt::get(Int32, 0), load_index};
//          auto *str_ptr_ptr = irbuilder.CreateInBoundsGEP(str_mapper, ArrayRef<Value *>(idx, 2));
//          auto *str_ptr = irbuilder.CreateLoad(str_ptr_ptr);
//          idx[0] = ins->getOperand(2);
//          auto *moved_ptr = irbuilder.CreateInBoundsGEP(str_ptr, ArrayRef<Value *>(idx, 1));
//          ins->replaceAllUsesWith(moved_ptr);
//          errs() << *ins->getOperand(1) << "\n";
//        }
//      }
    }
  }

//  F->dump();
  return F;
}


unique_ptr<Transformer<Function>> DataFlowTransformer::clone(uint64_t trans_code) {
  return std::make_unique<DataFlowTransformer>(trans_code);
}
