//
// Created by neil on 10/19/18.
//

#ifndef PROJECT_INHERITANCE_HPP
#define PROJECT_INHERITANCE_HPP

#include "llvm/Support/raw_ostream.h"
#include "bases.hpp"
#include <random>


class DeepArrayPuzzle : public PuzzleBuilder {
public:
    DeepArrayPuzzle(uint64_t puzzle_code, llvm::Module &M, std::default_random_engine* rand_eng, int weight = 10)
            : PuzzleBuilder(puzzle_code, M, weight, rand_eng) {
      array_size = static_cast<uint8_t>(puzzle_code % 256);
      puzzle_code = puzzle_code >> 8;
      fst_depth = static_cast<uint8_t>(puzzle_code % 256);
      puzzle_code = puzzle_code >> 8;
      scd_depth = static_cast<uint8_t>(puzzle_code % 256);

      llvm::errs() << (int)array_size << "\t" << (int)fst_depth << "\t" << (int)scd_depth << "\n";
    }

    llvm::Value *build(SymvarLoc &svs_locs, llvm::Instruction *insert_point) override;

private:
    uint8_t array_size, fst_depth, scd_depth;
};


class BogusCFGTransformer : public Transformer<llvm::Function> {
public:
    explicit BogusCFGTransformer(uint64_t trans_code, int weight=10,
            std::default_random_engine *rand_eng = nullptr) :
            Transformer(trans_code, weight) {
//      obf_prob = static_cast<uint8_t>(trans_code % 100);
//      obf_times = static_cast<uint8_t>((trans_code >> 8) % 256);
      obf_prob = 30;
      obf_times = 3;
    }

    llvm::Function *transform(llvm::Function *F, llvm::Value *predicate) override;

private:
    uint8_t obf_prob, obf_times;
};

#endif //PROJECT_INHERITANCE_HPP
