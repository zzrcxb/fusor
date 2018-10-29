//
// Created by neil on 10/26/18.
//

#include "utils.hpp"
#include "bases.hpp"
#include "inheritance.hpp"


#ifndef PROJECT_FACTORIES_HPP
#define PROJECT_FACTORIES_HPP


class PuzzleBuilderFactory {
public:
    PuzzleBuilderFactory() {
      puzzles[DeepArrayPuzzle::id] = std::make_unique<DeepArrayPuzzle>();

      rand_eng.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    }

    std::unique_ptr<PuzzleBuilder> get_builder(std::string &id, uint64_t puzzle_code, llvm::Module *M) {
      return puzzles[id]->clone(puzzle_code, M);
    }

    std::unique_ptr<PuzzleBuilder> get_builder_randomly(uint64_t, llvm::Module&);

private:
    std::map<std::string, std::unique_ptr<PuzzleBuilder>> puzzles;
    std::default_random_engine rand_eng;
};


class FunctionTransformerFactory {
public:
    FunctionTransformerFactory() {
      transes[BogusCFGTransformer::id] = std::make_unique<BogusCFGTransformer>();

      rand_eng.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    }

    std::unique_ptr<Transformer<llvm::Function>> get_transformer(std::string &id, uint64_t trans_code) {
      return transes[id]->clone(trans_code);
    }

    std::unique_ptr<Transformer<llvm::Function>> get_transformer_randomly();

private:
    std::map<std::string, std::unique_ptr<Transformer<llvm::Function>>> transes;
    std::default_random_engine rand_eng;
};


#endif //PROJECT_FACTORIES_HPP
