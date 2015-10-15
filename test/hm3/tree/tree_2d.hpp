#pragma once
/// \file tree_2d.hpp Tree 2D tests
#include "tree.hpp"

struct uniform_tree {
  std::vector<node> nodes{
   {idx{0}, lvl{0}, pn{i}, cs{1, 2, 3, 4}, pip{}, fn{i, i, i, i},
    en{i, i, i, i}, cn{}, an{}},
   {idx{1}, lvl{1}, pn{0}, cs{5, 6, 7, 8}, pip{0}, fn{i, 2, i, 3},
    en{i, i, i, 4}, cn{}},
   {idx{2}, lvl{1}, pn{0}, cs{9, 10, 11, 12}, pip{1}, fn{1, i, i, 4},
    en{i, i, 3, i}, cn{}},
   {idx{3}, lvl{1}, pn{0}, cs{13, 14, 15, 16}, pip{2}, fn{i, 4, 1, i},
    en{i, 2, i, i}, cn{}},
   {idx{4}, lvl{1}, pn{0}, cs{17, 18, 19, 20}, pip{3}, fn{3, i, 2, i},
    en{1, i, i, i}, cn{}},
   //
   {idx{5}, lvl{2}, pn{1}, cs{}, pip{0, 0}, fn{i, 6, i, 7}, en{i, i, i, 8},
    cn{}},
   {idx{6}, lvl{2}, pn{1}, cs{}, pip{0, 1}, fn{5, 9, i, 8}, en{i, i, 7, 11},
    cn{}},
   {idx{7}, lvl{2}, pn{1}, cs{}, pip{0, 2}, fn{i, 8, 5, 13}, en{i, 6, i, 14},
    cn{}},
   {idx{8}, lvl{2}, pn{1}, cs{}, pip{0, 3}, fn{7, 11, 6, 14}, en{5, 9, 13, 17},
    cn{}},
   {idx{9}, lvl{2}, pn{2}, cs{}, pip{1, 0}, fn{6, 10, i, 11}, en{i, i, 8, 12},
    cn{}},
   {idx{10}, lvl{2}, pn{2}, cs{}, pip{1, 1}, fn{9, i, i, 12}, en{i, i, 11, i},
    cn{}},
   {idx{11}, lvl{2}, pn{2}, cs{}, pip{1, 2}, fn{8, 12, 9, 17},
    en{6, 10, 14, 18}, cn{}},
   {idx{12}, lvl{2}, pn{2}, cs{}, pip{1, 3}, fn{11, i, 10, 18}, en{9, i, 17, i},
    cn{}},
   {idx{13}, lvl{2}, pn{3}, cs{}, pip{2, 0}, fn{i, 14, 7, 15},
    en{i, 8, i, 16_u}, cn{}},
   {idx{14}, lvl{2}, pn{3}, cs{}, pip{2, 1}, fn{13, 17, 8, 16},
    en{7, 11, 15, 19}, cn{}},
   {idx{15}, lvl{2}, pn{3}, cs{}, pip{2, 2}, fn{i, 16, 13, i}, en{i, 14, i, i},
    cn{}},
   {idx{16}, lvl{2}, pn{3}, cs{}, pip{2, 3}, fn{15, 19, 14, i},
    en{13, 17, i, i}, cn{}},
   {idx{17}, lvl{2}, pn{4}, cs{}, pip{3, 0}, fn{14, 18, 11, 19},
    en{8, 12, 16, 20}, cn{}},
   {idx{18}, lvl{2}, pn{4}, cs{}, pip{3, 1}, fn{17, i, 12, 20},
    en{11, i, 19, i}, cn{}},
   {idx{19}, lvl{2}, pn{4}, cs{}, pip{3, 2}, fn{16, 20, 17, i},
    en{14, 18, i, i}, cn{}},
   {idx{20}, lvl{2}, pn{4}, cs{}, pip{3, 3}, fn{19, i, 18, i}, en{17, i, i, i},
    cn{}},
   //
  };
};

struct tree_after_refine {
  std::vector<node> nodes{
   {idx{0}, lvl{0}, pn{i}, cs{1, 2, 3, 4}, pip{}, fn{i, i, i, i},
    en{i, i, i, i}, cn{}, an{}},
   {idx{1}, lvl{1}, pn{0}, cs{5, 6, 7, 8}, pip{0}, fn{i, 2, i, 3},
    en{i, i, i, 4}, cn{}},
   {idx{2}, lvl{1}, pn{0}, cs{9, 10, 11, 12}, pip{1}, fn{1, i, i, 4},
    en{i, i, 3, i}, cn{}},
   {idx{3}, lvl{1}, pn{0}, cs{13, 14, 15, 16}, pip{2}, fn{i, 4, 1, i},
    en{i, 2, i, i}, cn{}},
   {idx{4}, lvl{1}, pn{0}, cs{17, 18, 19, 20}, pip{3}, fn{3, i, 2, i},
    en{1, i, i, i}, cn{}},
   //

   {idx{5}, lvl{2}, pn{1}, cs{}, pip{0, 0}, fn{i, 6, i, 7}, en{i, i, i, 8},
    cn{}, an{6, 7, 21}},
   {idx{6}, lvl{2}, pn{1}, cs{}, pip{0, 1}, fn{5, 9, i, 8}, en{i, i, 7, 11},
    cn{}, an{5, 7, 9, 11, 21, 22}},
   {idx{7}, lvl{2}, pn{1}, cs{}, pip{0, 2}, fn{i, 8, 5, 13}, en{i, 6, i, 14},
    cn{}, an{5, 6, 13, 14, 21, 23}},
   {idx{8}, lvl{2}, pn{1}, cs{21, 22, 23, 24}, pip{0, 3}, fn{7, 11, 6, 14},
    en{5, 9, 13, 17}, cn{}, an{5, 6, 7, 9, 11, 13, 14, 25}},
   {idx{9}, lvl{2}, pn{2}, cs{}, pip{1, 0}, fn{6, 10, i, 11}, en{i, i, 8, 12},
    cn{}, an{6, 10, 11, 12, 22}},
   {idx{10}, lvl{2}, pn{2}, cs{}, pip{1, 1}, fn{9, i, i, 12}, en{i, i, 11, i},
    cn{}, an{9, 11, 12}},
   {idx{11}, lvl{2}, pn{2}, cs{}, pip{1, 2}, fn{8, 12, 9, 17},
    en{6, 10, 14, 18}, cn{}, an{6, 9, 10, 12, 14, 18, 22, 24, 25, 26}},
   {idx{12}, lvl{2}, pn{2}, cs{}, pip{1, 3}, fn{11, i, 10, 18}, en{9, i, 17, i},
    cn{}, an{9, 10, 11, 18, 26}},
   {idx{13}, lvl{2}, pn{3}, cs{}, pip{2, 0}, fn{i, 14, 7, 15},
    en{i, 8, i, 16_u}, cn{}, an{7, 14, 15, 16, 23}},
   {idx{14}, lvl{2}, pn{3}, cs{}, pip{2, 1}, fn{13, 17, 8, 16},
    en{7, 11, 15, 19}, cn{}, an{7, 11, 13, 15, 16, 19, 23, 24, 25, 27}},
   {idx{15}, lvl{2}, pn{3}, cs{}, pip{2, 2}, fn{i, 16, 13, i}, en{i, 14, i, i},
    cn{}, an{13, 14, 16}},
   {idx{16}, lvl{2}, pn{3}, cs{}, pip{2, 3}, fn{15, 19, 14, i},
    en{13, 17, i, i}, cn{}, an{13, 14, 15, 19, 27}},
   {idx{17}, lvl{2}, pn{4}, cs{25, 26, 27, 28}, pip{3, 0}, fn{14, 18, 11, 19},
    en{8, 12, 16, 20}, cn{}, an{11, 12, 14, 16, 18, 19, 20, 24}},
   {idx{18}, lvl{2}, pn{4}, cs{}, pip{3, 1}, fn{17, i, 12, 20},
    en{11, i, 19, i}, cn{}, an{11, 12, 19, 20, 26, 28}},
   {idx{19}, lvl{2}, pn{4}, cs{}, pip{3, 2}, fn{16, 20, 17, i},
    en{14, 18, i, i}, cn{}, an{14, 16, 18, 20, 27, 28}},
   {idx{20}, lvl{2}, pn{4}, cs{}, pip{3, 3}, fn{19, i, 18, i}, en{17, i, i, i},
    cn{}, an{18, 19, 28}},
   {idx{21}, lvl{3}, pn{8}, cs{}, pip{0, 3, 0}, fn{i, 22, i, 23},
    en{i, i, i, 24}, cn{}},  //
   {idx{22}, lvl{3}, pn{8}, cs{}, pip{0, 3, 1}, fn{21, i, i, 24},
    en{i, i, 23, i}, cn{}},  //
   {idx{23}, lvl{3}, pn{8}, cs{}, pip{0, 3, 2}, fn{i, 24, 21, i},
    en{i, 22, i, i}, cn{}},  //
   {idx{24}, lvl{3}, pn{8}, cs{}, pip{0, 3, 3}, fn{23, i, 22, i},
    en{21, i, i, 25}, cn{}, an{11, 14, 21, 22, 23, 25}},
   {idx{25}, lvl{3}, pn{17}, cs{}, pip{3, 0, 0}, fn{i, 26, i, 27},
    en{24, i, i, 28}, cn{}, an{11, 14, 24, 26, 27, 28}},  //
   {idx{26}, lvl{3}, pn{17}, cs{}, pip{3, 0, 1}, fn{25, i, i, 28},
    en{i, i, 27, i}, cn{}},  //
   {idx{27}, lvl{3}, pn{17}, cs{}, pip{3, 0, 2}, fn{i, 28, 25, i},
    en{i, 26, i, i}, cn{}},  //
   {idx{28}, lvl{3}, pn{17}, cs{}, pip{3, 0, 3}, fn{27, i, 26, i},
    en{25, i, i, i}, cn{}}  //

   //
  };
};

struct tree_after_coarsen {
  std::vector<node> nodes{
   {idx{0}, lvl{0}, pn{i}, cs{1, 2, 3, 4}, pip{}, fn{i, i, i, i},
    en{i, i, i, i}, cn{}, an{}},
   {idx{1}, lvl{1}, pn{0}, cs{5, 6, 7, 8}, pip{0}, fn{i, 2, i, 3},
    en{i, i, i, 4}, cn{}},
   {idx{2}, lvl{1}, pn{0}, cs{}, pip{1}, fn{1, i, i, 4}, en{i, i, 3, i}, cn{},
    an{3, 6, 8, 17, 18}},
   {idx{3}, lvl{1}, pn{0}, cs{}, pip{2}, fn{i, 4, 1, i}, en{i, 2, i, i}, cn{},
    an{2, 7, 8, 17, 19}},
   {idx{4}, lvl{1}, pn{0}, cs{17, 18, 19, 20}, pip{3}, fn{3, i, 2, i},
    en{1, i, i, i}, cn{}},
   //
   {idx{5}, lvl{2}, pn{1}, cs{}, pip{0, 0}, fn{i, 6, i, 7}, en{i, i, i, 8},
    cn{}, an{6, 7, 21}},
   {idx{6}, lvl{2}, pn{1}, cs{}, pip{0, 1}, fn{5, i, i, 8}, en{i, i, 7, i},
    cn{}, an{2, 5, 7, 21, 22}},
   {idx{7}, lvl{2}, pn{1}, cs{}, pip{0, 2}, fn{i, 8, 5, i}, en{i, 6, i, i},
    cn{}, an{3, 5, 6, 21, 23}},
   {idx{8}, lvl{2}, pn{1}, cs{21, 22, 23, 24}, pip{0, 3}, fn{7, i, 6, i},
    en{5, i, i, 17}, cn{}, an{2, 3, 5, 6, 7, 25}},
   {idx{17}, lvl{2}, pn{4}, cs{25, 26, 27, 28}, pip{3, 0}, fn{i, 18, i, 19},
    en{8, i, i, 20}, cn{}, an{2, 3, 18, 19, 20, 24}},
   {idx{18}, lvl{2}, pn{4}, cs{}, pip{3, 1}, fn{17, i, i, 20}, en{i, i, 19, i},
    cn{}, an{2, 19, 20, 26, 28}},
   {idx{19}, lvl{2}, pn{4}, cs{}, pip{3, 2}, fn{i, 20, 17, i}, en{i, 18, i, i},
    cn{}, an{3, 18, 20, 27, 28}},
   {idx{20}, lvl{2}, pn{4}, cs{}, pip{3, 3}, fn{19, i, 18, i}, en{17, i, i, i},
    cn{}, an{18, 19, 28}},
   {idx{21}, lvl{3}, pn{8}, cs{}, pip{0, 3, 0}, fn{i, 22, i, 23},
    en{i, i, i, 24}, cn{}},  //
   {idx{22}, lvl{3}, pn{8}, cs{}, pip{0, 3, 1}, fn{21, i, i, 24},
    en{i, i, 23, i}, cn{}},  //
   {idx{23}, lvl{3}, pn{8}, cs{}, pip{0, 3, 2}, fn{i, 24, 21, i},
    en{i, 22, i, i}, cn{}},  //
   {idx{24}, lvl{3}, pn{8}, cs{}, pip{0, 3, 3}, fn{23, i, 22, i},
    en{21, i, i, 25}, cn{}},
   {idx{25}, lvl{3}, pn{17}, cs{}, pip{3, 0, 0}, fn{i, 26, i, 27},
    en{24, i, i, 28}, cn{}},  //
   {idx{26}, lvl{3}, pn{17}, cs{}, pip{3, 0, 1}, fn{25, i, i, 28},
    en{i, i, 27, i}, cn{}},  //
   {idx{27}, lvl{3}, pn{17}, cs{}, pip{3, 0, 2}, fn{i, 28, 25, i},
    en{i, 26, i, i}, cn{}},  //
   {idx{28}, lvl{3}, pn{17}, cs{}, pip{3, 0, 3}, fn{27, i, 26, i},
    en{25, i, i, i}, cn{}}  //
                            //
  };
};

auto tree_after_coarsen_sorted_map = [](node_idx n) -> node_idx {
  if (!n) { return n; }
  if (*n < 9) { return n; }
  if (*n >= 17 and *n < 21) { return node_idx{*n - 17 + 13}; }
  if (*n >= 21 and *n < 25) { return node_idx{*n - 21 + 9}; }
  if (*n >= 25 and *n < 29) { return node_idx{*n - 25 + 17}; }
  return node_idx{};
};
