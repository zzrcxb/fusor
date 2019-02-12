//
// Created by neil on 10/21/18.
//

#include "utils.hpp"
#include "inheritance.hpp"


using namespace std;
using namespace llvm;


const string BogusCFGTransformer::id = "BogusCFGTransformer";

Function *BogusCFGTransformer::transform(llvm::Function *F, llvm::Value *predicate) {
  uniform_int_distribution<uint8_t> i8_generator(0, 99);
  unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
  default_random_engine def_rand;
  def_rand.seed(seed);

  deque<BasicBlock *> bbs;
  size_t types = 1;

  for (auto _ : range<int>(obf_times)) {
    bbs.clear();
    for (auto &B : *F) {
      bbs.emplace_back(&B);
      B.setName("dbg");
    }

    while (!bbs.empty()) {
      auto *B = bbs.front();
      bbs.pop_front();
      // obfuscate
      if (i8_generator(def_rand) < obf_prob) {
        switch (i8_generator(def_rand) % types) {
          case 0: {
            /*    |
             * /-----\
             * | orig|
             * \-----/
             *    |
             *    +->-------->----+
             *    +-<--+  +-->-+  |
             *   \|/   |  |    |  |
             * /-----\ |  |   /-----\
             * | body| |  |   | fake|
             * \-----/ |  |   \-----/
             *    |    |  |     |
             *    |    ---+-<---+
             *    +--->---|
             *    |
             * /-----\
             * | tail|
             * \-----/
             *    |
             **/
            auto *body_bb = split_bb_randomly(B, set<Value *>{predicate}, &def_rand);
//            auto *body_bb = B->splitBasicBlock(B->getFirstNonPHIOrDbgOrLifetime());
            auto *fake_bb = fake_bb_builder(BasicBlock::Create(F->getContext(), "f", F), vector<Instruction *>{});
            auto *tail_bb = split_bb_randomly(body_bb, set<Value *>{}, &def_rand);
//            auto *tail_bb = body_bb->splitBasicBlock(body_bb->getTerminator());
            tail_bb->setName("tail");
            body_bb->setName("body");

            B->getTerminator()->eraseFromParent();
            BranchInst::Create(body_bb, fake_bb, predicate, B);

            BranchInst::Create(body_bb, fake_bb);
            body_bb->getTerminator()->eraseFromParent();
            BranchInst::Create(tail_bb, fake_bb, predicate, body_bb);
          }
            break;
          case 1: {

          }
            break;
          default: {
          }
        }
      }
    }
  }

  return F;
}


unique_ptr<Transformer<llvm::Function>> BogusCFGTransformer::clone(uint64_t trans_code) {
  return std::make_unique<BogusCFGTransformer>(trans_code);
}


BasicBlock *split_bb_randomly(BasicBlock *bb, set<Value *> pre_req, default_random_engine *rand_eng) {
  auto iter_p = bb->begin();
  auto fst_i = bb->getFirstNonPHIOrDbgOrLifetime();
  set<Value *> intersected, all_insts;

//  errs() << " =========" << bb->getName() << "========= \n";
//  errs() << "First instruction: " << *iter_p << "\n";

  for (auto &I : *bb)
    all_insts.insert(&I);

  set_intersection(all_insts.begin(), all_insts.end(), pre_req.begin(), pre_req.end(),
                   inserter(intersected, intersected.begin()));

  size_t cnt = 0;
  auto found_fst = False;
  while ((!intersected.empty() && iter_p != bb->end()) || !found_fst) {
    auto *ins = dyn_cast<Instruction>(iter_p);
    if (ins == fst_i && !found_fst) {
      found_fst = True;
      continue;
    }
    if (IN_SET(ins, pre_req)) {
      intersected.erase(ins);
    }
    iter_p++;
    cnt++;
  }

  uniform_int_distribution<size_t > i_generator(0, bb->size() - cnt - 1);
  Instruction *split_ins = bb->getTerminator();

  size_t cnt_finder = 0;
  auto stop_point = i_generator(*rand_eng);
//  errs() << "Start of possible split point: " << *iter_p << ";\t choose #" << stop_point << "\n";

  while (iter_p != bb->end()) {
    if (cnt_finder == stop_point) {
      split_ins = dyn_cast<Instruction>(iter_p);
      break;
    }
    iter_p++;
    cnt_finder++;
  }

//  errs() << "** Split Point: " << *split_ins << "\n";
  return bb->splitBasicBlock(split_ins, "split");
}


BasicBlock *fake_bb_builder(BasicBlock *base_bb, vector<Instruction *> seed) {
  return base_bb;
}
