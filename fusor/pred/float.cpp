//
// Created by neil on 11/19/18.
//

#include "bases.hpp"
#include "utils.hpp"
#include "inheritance.hpp"

#include <limits>

using namespace std;
using namespace llvm;


const string FloatPointPuzzle::id = "FloatPointPuzzle";

Value * FloatPointPuzzle::build(SymvarLoc &svs_locs, Instruction *insert_point) {
  if (!insert_point)
    return nullptr;

  // for debugging
//  Constant* logFunc = module->getOrInsertFunction(
//          "logop", Type::getVoidTy(getGlobalContext()), Type::getInt32Ty(getGlobalContext()), NULL
//  );
  Value* args[2] = {};
  // end of debugging

  uniform_real_distribution<float> fdiv_generator(1E7,
          static_cast<float>(std::min<double>(numeric_limits<float>::max(), 1.0 / numeric_limits<float>::min())));

  uniform_real_distribution<float> feq_generator(1E4, numeric_limits<float>::max());

  IRBuilder<> irbuilder(insert_point);
  vector<Value*> div_res;
  for (auto &p : cast_sv_to_uint8(svs_locs, irbuilder)) {
//    auto *casted = p.second;
    auto *casted = irbuilder.CreateAnd(p.second, ConstantInt::get(Int8, 127));
    casted = irbuilder.CreateOr(casted, ConstantInt::get(Int8, 1));
    auto *toF = irbuilder.CreateSIToFP(casted, Float);

    args[0] = ConstantInt::get(Int32, 1);
    args[1] = toF;
//    irbuilder.CreateCall(logFunc, args);

//    auto *dived = irbuilder.CreateFDiv(toF, ConstantFP::get(Float, fdiv_generator(rand_eng)));
    auto *dived = irbuilder.CreateFDiv(toF, ConstantFP::get(Float, 1E6));

    args[0] = ConstantInt::get(Int32, 2);
    args[1] = dived;
//    irbuilder.CreateCall(logFunc, args);

//    auto eq_base = ConstantFP::get(Float, feq_generator(rand_eng));
    auto eq_base = ConstantFP::get(Float, 1E4);

    args[0] = ConstantInt::get(Int32, 3);
    args[1] = eq_base;
//    irbuilder.CreateCall(logFunc, args);

    auto *left = irbuilder.CreateFAdd(dived, eq_base);

    args[0] = ConstantInt::get(Int32, 4);
    args[1] = left;
//    irbuilder.CreateCall(logFunc, args);

    auto *eq_res = irbuilder.CreateFCmpOEQ(left, eq_base);
    auto *grt = irbuilder.CreateFCmpOGT(dived, ConstantFP::get(Float, 0.0));
    auto *res = irbuilder.CreateAnd(eq_res, grt);

    div_res.emplace_back(res);
  }

  Value *final_res = ConstantInt::get(Int1, 1);
  for (auto r : div_res) {
    final_res = irbuilder.CreateAnd(final_res, r, "res");
  }

  return final_res;
}
