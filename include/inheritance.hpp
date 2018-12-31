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
    const static std::string id;

    DeepArrayPuzzle() {
      array_size = 64; fst_depth = 3; scd_depth = 5;
    }

    DeepArrayPuzzle(uint64_t puzzle_code, llvm::Module *M) : PuzzleBuilder(puzzle_code, M) {
      array_size = static_cast<uint8_t>(puzzle_code % 256);
      puzzle_code = puzzle_code >> 8;
      fst_depth = static_cast<uint8_t>(puzzle_code % 256);
      puzzle_code = puzzle_code >> 8;
      scd_depth = static_cast<uint8_t>(puzzle_code % 256);

      rand_eng.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
//      llvm::errs() << (int)array_size << "\t" << (int)fst_depth << "\t" << (int)scd_depth << "\n";
    }

    llvm::Value *build(SymvarLoc &svs_locs, llvm::Instruction *insert_point) override;

    std::unique_ptr<PuzzleBuilder> clone(uint64_t, llvm::Module*) override;

private:
    uint8_t array_size, fst_depth, scd_depth;
    std::default_random_engine rand_eng;
};


class FloatPointPuzzle : public PuzzleBuilder {
public:
    const static std::string id;

    FloatPointPuzzle() = default;

    FloatPointPuzzle(uint64_t puzzle_code, llvm::Module* M) : PuzzleBuilder(puzzle_code, M) {
      rand_eng.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    }

    llvm::Value *build(SymvarLoc &svs_locs, llvm::Instruction *insert_point) override;

    std::unique_ptr<PuzzleBuilder> clone(uint64_t puzzle_code, llvm::Module* M) override {
      return std::make_unique<FloatPointPuzzle>(puzzle_code, M);
    }

private:
    std::default_random_engine rand_eng;
};


class BogusCFGTransformer : public Transformer<llvm::Function> {
public:
    const static std::string id;

    BogusCFGTransformer() {
      obf_times = 1; obf_prob = 50;
    }

    explicit BogusCFGTransformer(uint64_t trans_code) : Transformer(trans_code) {
      obf_prob = static_cast<uint8_t>(trans_code % 256);
      obf_times = static_cast<uint8_t>((trans_code >> 8) % 256);

      rand_eng.seed(static_cast<unsigned >(std::chrono::system_clock::now().time_since_epoch().count()));
//      llvm::errs() << int(obf_prob) << "\t" << int(obf_times) << "\n";
    }

    llvm::Function *transform(llvm::Function *F, llvm::Value *predicate) override;

    std::unique_ptr<Transformer<llvm::Function>> clone(uint64_t) override;

private:
    uint8_t obf_prob, obf_times;
    std::default_random_engine rand_eng;
};


class SecondOpaqueTransformer : public Transformer<llvm::Function> {
public:
    const static std::string id;

    SecondOpaqueTransformer() = default;

    explicit SecondOpaqueTransformer(uint64_t trans_code) : Transformer(trans_code) {}

    llvm::Function *transform(llvm::Function *F, llvm::Value *predicate) override;

    std::unique_ptr<Transformer<llvm::Function>> clone(uint64_t) override;
};

#endif //PROJECT_INHERITANCE_HPP
