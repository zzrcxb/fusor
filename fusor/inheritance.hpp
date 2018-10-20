//
// Created by neil on 10/19/18.
//

#ifndef PROJECT_INHERITANCE_HPP
#define PROJECT_INHERITANCE_HPP

#include "bases.hpp"

class DeepArrayPuzzle : PuzzleBuilder {
public:
    explicit DeepArrayPuzzle(size_t array_size, int weight = 10, size_t fst_depth=2, size_t scd_depth=6)
            : PuzzleBuilder(BuilderType::BASICBLOCK, weight), array_size(array_size), fst_depth(fst_depth),
              scd_depth(scd_depth) {}

    llvm::Value *build(llvm::Function &F, SymvarLoc &svs_locs, llvm::Instruction *insert_point) override;

private:
    const size_t array_size, fst_depth, scd_depth;
};

#endif //PROJECT_INHERITANCE_HPP
