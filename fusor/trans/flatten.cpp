//
// Created by neil on 1/3/19.
//

#include "utils.hpp"
#include "inheritance.hpp"

using namespace std;
using namespace llvm;

const string CFGFlattenTransformer::id = "CFGFlattenTransformer";

Constant *build_log(string name, Module *M) {
  vector<Type*> paramTypes = {Int64, };
  auto *retType = Void;
  FunctionType *logFuncType = FunctionType::get(retType, paramTypes, False);
  Constant *logFunc = M->getOrInsertFunction(name, logFuncType);
  return logFunc;
}

Function *CFGFlattenTransformer::transform(Function *F, Value *predicate) {
  auto func_iter = F->begin();
  func_iter++;
  auto *first_BB = ISINSTANCE(func_iter, BasicBlock);
  uint64_t switch_id = 1;

  map<BasicBlock *, uint64_t> BB_map;
  if (F->size() == 1)
    return F;

  for (func_iter++; func_iter != F->end(); func_iter++) {
    auto *b = ISINSTANCE(func_iter, BasicBlock);
    b->setName("dbg");
    BB_map[b] = switch_id;
    switch_id++;
  }

  // create inf loop entry
  auto *entry_BB = BasicBlock::Create(F->getContext(), "entry", F);
  IRBuilder<> irbuilder(first_BB->getTerminator());
  auto *switch_var = irbuilder.CreateAlloca(Int64);

  irbuilder.CreateStore(ConstantInt::get(Int64, 0), switch_var);
  irbuilder.SetInsertPoint(entry_BB);
  auto *loaded = irbuilder.CreateLoad(switch_var);
  auto *end_switch = irbuilder.CreateSwitch(loaded, entry_BB);

  // check 1st bb
  bool fst_multi = false;
  if (first_BB->getTerminator()->getNumSuccessors() > 1 || ISINSTANCE(first_BB->getTerminator(), ReturnInst)) {
    auto *splitted = first_BB->splitBasicBlock(first_BB->getTerminator(), "ABC");
    first_BB->getTerminator()->eraseFromParent();
    BranchInst::Create(entry_BB, first_BB);
    BB_map[splitted] = switch_id;
    switch_id++;
    fst_multi = true;
  }
  else {
    first_BB->getTerminator()->eraseFromParent();
    BranchInst::Create(entry_BB, first_BB);
  }

  // build successors tree
  for (auto p : BB_map) {
    auto *bb = p.first;
    auto id = p.second;
    errs() << bb->getName() << "\t" << id << "\n";
    auto *terminator = bb->getTerminator();
    bool avoid = false;
    irbuilder.SetInsertPoint(terminator);

    if (auto *br = ISINSTANCE(terminator, BranchInst)) {
      if (br->isConditional()) {
        auto *condition = br->getCondition();
        auto *false_des = ISINSTANCE(br->getOperand(1), BasicBlock), *true_des = ISINSTANCE(br->getOperand(2),
                                                                                            BasicBlock);
        auto false_id = BB_map[false_des], true_id = BB_map[true_des];

        auto *new_sw = irbuilder.CreateSelect(condition, ConstantInt::get(Int64, true_id),
                                              ConstantInt::get(Int64, false_id));
        irbuilder.CreateStore(new_sw, switch_var);
      } else {
        auto des_id = BB_map[ISINSTANCE(br->getOperand(0), BasicBlock)];
        irbuilder.CreateStore(ConstantInt::get(Int64, des_id), switch_var);
      }
    } else if (ISINSTANCE(terminator, ReturnInst)) {
      avoid = true;
    } else {
      errs() << "\n\nError!\n\n";
    }
    if (!avoid) {
      terminator->eraseFromParent();
      BranchInst::Create(entry_BB, bb);
    }
    end_switch->addCase(ISINSTANCE(ConstantInt::get(Int64, id), ConstantInt), bb);
  }

  // create 0 block
  auto *zero_BB = BasicBlock::Create(F->getContext(), "zero", F);
  irbuilder.SetInsertPoint(zero_BB);
  Value *new_sw;
  if (fst_multi)
    new_sw = irbuilder.CreateSelect(predicate, ConstantInt::get(Int64, switch_id - 1), ConstantInt::get(Int64, 0));
  else
    new_sw = irbuilder.CreateSelect(predicate, ConstantInt::get(Int64, 1), ConstantInt::get(Int64, BB_map.size()));
  irbuilder.CreateStore(new_sw, switch_var);
  BranchInst::Create(entry_BB, zero_BB);
  end_switch->addCase(ISINSTANCE(ConstantInt::get(Int64, 0), ConstantInt), zero_BB);
//  F->dump();
  return F;
}

unique_ptr<Transformer<Function>> CFGFlattenTransformer::clone(uint64_t trans_code) {
  return std::make_unique<CFGFlattenTransformer>(trans_code);
}
