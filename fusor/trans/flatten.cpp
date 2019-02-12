//
// Created by neil on 1/3/19.
//

#include "utils.hpp"
#include "inheritance.hpp"

using namespace std;
using namespace llvm;

const string CFGFlattenTransformer::id = "CFGFlattenTransformer";

Constant *build_log(string name, Module *M) {
  vector<Type *> paramTypes = {Int64,};
  auto *retType = Void;
  FunctionType *logFuncType = FunctionType::get(retType, paramTypes, False);
  Constant *logFunc = M->getOrInsertFunction(name, logFuncType);
  return logFunc;
}

Function *CFGFlattenTransformer::transform(Function *F, Value *predicate) {
  auto func_iter = F->begin();
  auto *first_BB = ISINSTANCE(func_iter, BasicBlock);
  uint64_t switch_id = 1;

  map<BasicBlock *, uint64_t> BB_map;
  if (F->size() == 1)
    return F;

  for (func_iter++; func_iter != F->end(); func_iter++) {
    auto *b = ISINSTANCE(func_iter, BasicBlock);
    BB_map[b] = switch_id;
    switch_id++;
    if (ISINSTANCE(b->getTerminator(), InvokeInst))
      return F;
  }

  // create inf loop entry
  auto *entry_BB = BasicBlock::Create(F->getContext(), "entry", F);
  auto *exit_BB = BasicBlock::Create(F->getContext(), "exit", F);
  BranchInst::Create(entry_BB, exit_BB);
  IRBuilder<> irbuilder(first_BB->getTerminator());
  auto *switch_var = irbuilder.CreateAlloca(Int64);

  irbuilder.CreateStore(ConstantInt::get(Int64, 0), switch_var);
  irbuilder.SetInsertPoint(entry_BB);
  auto *loaded = irbuilder.CreateLoad(switch_var);
  auto *end_switch = irbuilder.CreateSwitch(loaded, exit_BB);

  // check 1st bb
  bool fst_multi = false;
  if (first_BB->getTerminator()->getNumSuccessors() > 1 || ISINSTANCE(first_BB->getTerminator(), ReturnInst)) {
    auto *splitted = first_BB->splitBasicBlock(first_BB->getTerminator(), "ABC");
    first_BB->getTerminator()->eraseFromParent();
    BranchInst::Create(entry_BB, first_BB);
    BB_map[splitted] = switch_id;
    switch_id++;
    fst_multi = true;
  } else {
    first_BB->getTerminator()->eraseFromParent();
    BranchInst::Create(entry_BB, first_BB);
  }

  // build successors tree
  for (auto p : BB_map) {
    auto *bb = p.first;
    auto id = p.second;
    auto *terminator = bb->getTerminator();
    bool avoid = false;
    irbuilder.SetInsertPoint(terminator);

    switch (terminator->getNumSuccessors()) {
      case 0:
        avoid = true;
        break;
      case 1: {
        auto des_id = BB_map[terminator->getSuccessor(0)];
        irbuilder.CreateStore(ConstantInt::get(Int64, des_id), switch_var);
        break;
      }
      default: {
        if (auto *br = ISINSTANCE(terminator, BranchInst)) {
          auto *condition = br->getCondition();
          auto *false_des = ISINSTANCE(br->getOperand(1), BasicBlock), *true_des = ISINSTANCE(br->getOperand(2),
                                                                                              BasicBlock);
          auto false_id = BB_map[false_des], true_id = BB_map[true_des];

          auto *new_sw = irbuilder.CreateSelect(condition, ConstantInt::get(Int64, true_id),
                                                ConstantInt::get(Int64, false_id));
          irbuilder.CreateStore(new_sw, switch_var);
        } else if (auto *sw = ISINSTANCE(terminator, SwitchInst)) {
          auto *condition = sw->getCondition();
          auto default_des_id = BB_map[sw->getDefaultDest()];
          vector<pair<Value *, uint64_t >> cases;
          for (auto &c : sw->cases()) {
            cases.emplace_back(pair<Value *, uint64_t>(c.getCaseValue(), BB_map[c.getCaseSuccessor()]));
          }
          auto *new_sw = irbuilder.CreateSelect(ConstantInt::get(Int1, 1), ConstantInt::get(Int64, default_des_id),
                                                ConstantInt::get(Int64, default_des_id));
          for (auto pp : cases) {
            auto *v = pp.first;
            auto des = pp.second;
            auto *cmp_res = irbuilder.CreateICmpEQ(condition, v);
            new_sw = irbuilder.CreateSelect(cmp_res, ConstantInt::get(Int64, des), new_sw);
          }
          irbuilder.CreateStore(new_sw, switch_var);
        } else {
          errs() << "\n\n\n-=-= Error " << *terminator << "\n\n\n";
        }
        break;
      }
    }

    if (!avoid) {
      terminator->eraseFromParent();
      BranchInst::Create(exit_BB, bb);
    }
    end_switch->addCase(ISINSTANCE(ConstantInt::get(Int64, id), ConstantInt), bb);

    // fix phi
    vector<Instruction*> insts;
    for (auto &I : *bb) {
      insts.emplace_back(&I);
    }

    for (auto *I : insts) {
      if (auto *phi = ISINSTANCE(I, PHINode)) {
        auto *phi_type = phi->getType();
        auto *phi_reload_var = new AllocaInst(phi_type, "reload", first_BB->getTerminator());
        for (auto index : range<uint>(phi->getNumIncomingValues())) {
          auto *incoming_bb = phi->getIncomingBlock(index);
          auto *incoming_v = phi->getIncomingValue(index);
          new StoreInst(incoming_v, phi_reload_var, incoming_bb->getTerminator());
        }
        auto *phi_reload = new LoadInst(phi_reload_var, "reloaded", phi);
        phi->replaceAllUsesWith(phi_reload);
        phi->eraseFromParent();
      }
    }
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
  BranchInst::Create(exit_BB, zero_BB);
  end_switch->addCase(ISINSTANCE(ConstantInt::get(Int64, 0), ConstantInt), zero_BB);

  return F;
}

unique_ptr<Transformer<Function>> CFGFlattenTransformer::clone(uint64_t trans_code) {
  return std::make_unique<CFGFlattenTransformer>(trans_code);
}
