//
// Created by neil on 10/11/18.
//

#include "bases.hpp"
#include "utils.hpp"
#include "inheritance.hpp"

#include <random>

using namespace std;
using namespace llvm;


Value* DeepArrayPuzzle::build(Function &F, SymvarLoc &svs_locs, Instruction* insert_point) {
  ArrayType *aint = ArrayType::get(Int8, array_size);
  uniform_int_distribution<uint8_t> i8_generator(0, 127);
  IRBuilder<> irbuilder(insert_point);

  if (!insert_point)
    insert_point = ISINSTANCE(F.begin(), Instruction);

  // do the load job
  map<Value*, vector<Value*>> load_res;
  for (auto &p : svs_locs) {
    auto *sv = p.first;
    auto *loc = p.second;
    Value *fst_index, *scd_index;
    fst_index = irbuilder.CreateLoad(loc, "sv1"); scd_index = fst_index;

    for (size_t _ = 0; _ < fst_depth; _++) {

    }

  }

  return nullptr;
}
