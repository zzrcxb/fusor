//
// Created by neil on 10/11/18.
//

#ifndef PROJECT_BASES_HPP
#define PROJECT_BASES_HPP

#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "utils.hpp"

#include <random>


enum BuilderType {
    MODULE = 1,
    FUNCTION = 1 << 1,
    BASICBLOCK = 1 << 2
};


class PuzzleBuilder {
public:
    int weight;

    PuzzleBuilder(uint64_t puzzle_code, llvm::Module &M, int weight = 10,
                  std::default_random_engine *rand_eng = nullptr) :
            puzzle_code(puzzle_code), module(M), weight(weight), rand_eng(rand_eng) {}

    virtual llvm::Value *build(SymvarLoc &svs_locs, llvm::Instruction *insert_point) = 0;

protected:
    llvm::Module &module;
    std::default_random_engine *rand_eng;
    uint64_t puzzle_code;
};


template <typename T> class Transformer {
public:
    int weight;

    explicit Transformer(uint64_t trans_code, int weight = 10,
            std::default_random_engine *rand_eng = nullptr) :
            trans_code(trans_code), weight(weight), rand_eng(rand_eng) {}

    virtual T *transform(T *t, llvm::Value *predicate) = 0;

protected:
    uint64_t trans_code;
    std::default_random_engine *rand_eng;
};


#endif //PROJECT_BASES_HPP
