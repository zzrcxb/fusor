//
// Created by neil on 10/21/18.
//

#include "../utils.hpp"
#include "../inheritance.hpp"


using namespace std;
using namespace llvm;


Function* BogusCFGTransformer::transform(llvm::Function *F, llvm::Value *predicate) {
  uniform_int_distribution<uint8_t> i8_generator(0, 100);
  size_t types = 1;

  for (auto _ : range<int>(obf_times)) {
    for (auto &B : *F) {
      // obfuscate
      if (i8_generator(*rand_eng) < obf_prob) {
        switch (i8_generator(*rand_eng) % types) {
          case 0: {
            auto *new_bb = split_bb_randomly(&B, set<Value*>{predicate}, rand_eng);
            
          }
            break;
          case 1: {

          }
            break;
          default: {}
        }
      }
    }
  }

  return F;
}

BasicBlock *split_bb_randomly(BasicBlock *bb, set<Value*> pre_req, default_random_engine *rand_eng) {
  auto iter_p = bb->begin();
  size_t cnt = 0;
  while (!pre_req.empty() && iter_p != bb->end()) {
    auto *ins = dyn_cast<Instruction>(iter_p);
    if (IN_SET(ins, pre_req)) {
      pre_req.erase(ins);
    }
    iter_p++;
    cnt++;
  }

  auto prob = 100.0f / (bb->size() - cnt);
  uniform_real_distribution<float> d_generator(0, 100.0);
  Instruction *split_ins = nullptr;
  while (split_ins == nullptr && iter_p != bb->end()) {
    if (d_generator(*rand_eng) < prob) {
      split_ins = dyn_cast<Instruction>(iter_p);
    }
  }
  if (split_ins == nullptr) {
    split_ins = bb->getTerminator();
  }
  return bb->splitBasicBlock(split_ins, "split");
}
