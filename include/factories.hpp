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
      puzzles[FloatPointPuzzle::id] = std::make_unique<FloatPointPuzzle>();

      rand_eng.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    }

    std::unique_ptr<PuzzleBuilder> get_builder(std::string id, uint64_t puzzle_code, llvm::Module *M) {
      return puzzles[id]->clone(puzzle_code, M);
    }

    std::unique_ptr<PuzzleBuilder> get_builder_randomly(std::map<std::string, uint64_t> pc_map, llvm::Module *M) {
      std::uniform_int_distribution<ulong > ulong_gen(0, pc_map.size() - 1);
      auto it = pc_map.begin();
      std::advance(it, ulong_gen(rand_eng));
      auto key = it->first;
      auto puzzle_code = it->second;

      if (!IN_MAP(key, puzzles)) {
        llvm::errs() << "--- Puzzle \"" << key <<"\" not found in PuzzleBuilder ---\n";
        return nullptr;
      }

      return get_builder(key, puzzle_code, M);
    }

private:
    std::map<std::string, std::unique_ptr<PuzzleBuilder>> puzzles;
    std::default_random_engine rand_eng;
};


class FunctionTransformerFactory {
public:
    FunctionTransformerFactory() {
      transes[BogusCFGTransformer::id] = std::make_unique<BogusCFGTransformer>();
      transes[SecondOpaqueTransformer::id] = std::make_unique<SecondOpaqueTransformer>();
      transes[CFGFlattenTransformer::id] = std::make_unique<CFGFlattenTransformer>();
      transes[DataFlowTransformer::id] = std::make_unique<DataFlowTransformer>();

      rand_eng.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    }

    std::unique_ptr<Transformer<llvm::Function>> get_transformer(std::string id, uint64_t trans_code) {
      return transes[id]->clone(trans_code);
    }

    std::unique_ptr<Transformer<llvm::Function>> get_transformer_randomly(std::map<std::string, uint64_t> tc_map) {
      std::uniform_int_distribution<ulong > ulong_gen(0, tc_map.size() - 1);
      auto it = tc_map.begin();
      std::advance(it, ulong_gen(rand_eng));
      auto key = it->first;
      auto puzzle_code = it->second;

      if (!IN_MAP(key, transes)) {
        llvm::errs() << "--- Transformer \"" << key <<"\" not found in TransformerFactory ---\n";
        return nullptr;
      }

      return get_transformer(key, puzzle_code);
    }

private:
    std::map<std::string, std::unique_ptr<Transformer<llvm::Function>>> transes;
    std::default_random_engine rand_eng;
};


#endif //PROJECT_FACTORIES_HPP
