#pragma once
/// \file
///
/// Tree 1D tests
#include "tree.hpp"

namespace hm3 {
namespace test {

struct uniform_tree {
  std::vector<node> nodes{
   {idx{0}, lvl{0}, pn{i}, cs{1, 2}, pip{}, fn{i, i}, en{}, cn{}, an{},
    nc{0.5}},                                                                //
   {idx{1}, lvl{1}, pn{0}, cs{3, 4}, pip{0}, fn{i, 2}, an{5}, nc{0.25}},     //
   {idx{2}, lvl{1}, pn{0}, cs{5, 6}, pip{1}, fn{1, i}, an{4}, nc{0.75}},     //
   {idx{3}, lvl{2}, pn{1}, cs{7, 8}, pip{0, 0}, fn{i, 4}, an{9}, nc{.125}},  //
   {idx{4}, lvl{2}, pn{1}, cs{9, 10}, pip{0, 1}, fn{3, 5}, an{8, 11},
    nc{.375}},  //
   {idx{5}, lvl{2}, pn{2}, cs{11, 12}, pip{1, 0}, fn{4, 6}, an{10, 13},
    nc{0.625}},  //
   {idx{6}, lvl{2}, pn{2}, cs{13, 14}, pip{1, 1}, fn{5, i}, an{12},
    nc{0.875}},                                                           //
   {idx{7}, lvl{3}, pn{3}, cs{}, pip{0, 0, 0}, fn{i, 8}, an{8}},          //
   {idx{8}, lvl{3}, pn{3}, cs{}, pip{0, 0, 1}, fn{7, 9}, an{7, 9}},       //
   {idx{9}, lvl{3}, pn{4}, cs{}, pip{0, 1, 0}, fn{8, 10}, an{8, 10}},     //
   {idx{10}, lvl{3}, pn{4}, cs{}, pip{0, 1, 1}, fn{9, 11}, an{9, 11}},    //
   {idx{11}, lvl{3}, pn{5}, cs{}, pip{1, 0, 0}, fn{10, 12}, an{10, 12}},  //
   {idx{12}, lvl{3}, pn{5}, cs{}, pip{1, 0, 1}, fn{11, 13}, an{11, 13}},  //
   {idx{13}, lvl{3}, pn{6}, cs{}, pip{1, 1, 0}, fn{12, 14}, an{12, 14}},  //
   {idx{14}, lvl{3}, pn{6}, cs{}, pip{1, 1, 1}, fn{13, i}, an{13}}        //
                                                                          //
  };
};

struct tree_after_refine {
  std::vector<node> nodes{
   {idx{0}, lvl{0}, pn{i}, cs{1, 2}, pip{}, fn{i, i}, en{}, cn{}, an{},
    nc{0.5}},                                                                //
   {idx{1}, lvl{1}, pn{0}, cs{3, 4}, pip{0}, fn{i, 2}, an{5}, nc{0.25}},     //
   {idx{2}, lvl{1}, pn{0}, cs{5, 6}, pip{1}, fn{1, i}, an{4}, nc{0.75}},     //
   {idx{3}, lvl{2}, pn{1}, cs{7, 8}, pip{0, 0}, fn{i, 4}, an{9}, nc{.125}},  //
   {idx{4}, lvl{2}, pn{1}, cs{9, 10}, pip{0, 1}, fn{3, 5}, an{8, 11},
    nc{.375}},  //
   {idx{5}, lvl{2}, pn{2}, cs{11, 12}, pip{1, 0}, fn{4, 6}, an{10, 13},
    nc{0.625}},  //
   {idx{6}, lvl{2}, pn{2}, cs{13, 14}, pip{1, 1}, fn{5, i}, an{12}, nc{0.875}},
   {idx{7}, lvl{3}, pn{3}, cs{}, pip{0, 0, 0}, fn{i, 8}, an{17}},             //
   {idx{8}, lvl{3}, pn{3}, cs{17, 18}, pip{0, 0, 1}, fn{7, 9}, an{7, 19}},    //
   {idx{9}, lvl{3}, pn{4}, cs{19, 20}, pip{0, 1, 0}, fn{8, 10}, an{10, 18}},  //
   {idx{10}, lvl{3}, pn{4}, cs{}, pip{0, 1, 1}, fn{9, 11}, an{15, 20}},       //
   {idx{11}, lvl{3}, pn{5}, cs{15, 16}, pip{1, 0, 0}, fn{10, 12},
    an{10, 12}},                                                             //
   {idx{12}, lvl{3}, pn{5}, cs{}, pip{1, 0, 1}, fn{11, 13}, an{13, 16}},     //
   {idx{13}, lvl{3}, pn{6}, cs{}, pip{1, 1, 0}, fn{12, 14}, an{12, 14}},     //
   {idx{14}, lvl{3}, pn{6}, cs{}, pip{1, 1, 1}, fn{13, i}, an{13}},          //
   {idx{15}, lvl{4}, pn{11}, cs{}, pip{1, 0, 0, 0}, fn{i, 16}, an{10, 16}},  //
   {idx{16}, lvl{4}, pn{11}, cs{}, pip{1, 0, 0, 1}, fn{15, i}, an{12, 15}},  //
   {idx{17}, lvl{4}, pn{8}, cs{}, pip{0, 0, 1, 0}, fn{i, 18}, an{7, 18}},    //
   {idx{18}, lvl{4}, pn{8}, cs{}, pip{0, 0, 1, 1}, fn{17, 19}, an{17, 19}},  //
   {idx{19}, lvl{4}, pn{9}, cs{}, pip{0, 1, 0, 0}, fn{18, 20}, an{18, 20}},  //
   {idx{20}, lvl{4}, pn{9}, cs{}, pip{0, 1, 0, 1}, fn{19, i}, an{10, 19}}    //
                                                                             //
  };
};

struct tree_after_coarsen {
  std::vector<node> nodes{
   {idx{0}, lvl{0}, pn{i}, cs{1, 2}, fn{i, i}, an{}},           //
   {idx{1}, lvl{1}, pn{0}, cs{3, 4}, fn{i, 2}, an{5}},          //
   {idx{2}, lvl{1}, pn{0}, cs{5, 6}, fn{1, i}, an{4}},          //
   {idx{3}, lvl{2}, pn{1}, cs{7, 8}, fn{i, 4}, an{9}},          //
   {idx{4}, lvl{2}, pn{1}, cs{9, 10}, fn{3, 5}, an{5, 8}},      //
   {idx{5}, lvl{2}, pn{2}, cs{}, fn{4, 6}, an{10, 13}},         //
   {idx{6}, lvl{2}, pn{2}, cs{13, 14}, fn{5, i}, an{5}},        //
   {idx{7}, lvl{3}, pn{3}, cs{}, fn{i, 8}, an{17}},             //
   {idx{8}, lvl{3}, pn{3}, cs{17, 18}, fn{7, 9}, an{7, 19}},    //
   {idx{9}, lvl{3}, pn{4}, cs{19, 20}, fn{8, 10}, an{10, 18}},  //
   {idx{10}, lvl{3}, pn{4}, cs{}, fn{9, i}, an{5, 20}},         //
   {idx{13}, lvl{3}, pn{6}, cs{}, fn{i, 14}, an{5, 14}},        //
   {idx{14}, lvl{3}, pn{6}, cs{}, fn{13, i}, an{13}},           //
   {idx{17}, lvl{4}, pn{8}, cs{}, fn{i, 18}, an{7, 18}},        //
   {idx{18}, lvl{4}, pn{8}, cs{}, fn{17, 19}, an{17, 19}},      //
   {idx{19}, lvl{4}, pn{9}, cs{}, fn{18, 20}, an{18, 20}},      //
   {idx{20}, lvl{4}, pn{9}, cs{}, fn{19, i}, an{10, 19}}        //

   //
  };
};

auto tree_after_coarsen_sorted_map = [](node_idx n) -> node_idx {
  if (!n || *n < 5) { return n; }
  switch (*n) {
    case 5: {
      return node_idx{13};
    }
    case 6: {
      return node_idx{14};
    }
    case 7: {
      return node_idx{5};
    }
    case 8: {
      return node_idx{6};
    }
    case 13: {
      return node_idx{15};
    }
    case 14: {
      return node_idx{16};
    }
    case 17: {
      return node_idx{7};
    }
    case 18: {
      return node_idx{8};
    }
    case 19: {
      return node_idx{11};
    }
    case 20: {
      return node_idx{12};
    }
  }
  return n;
};

}  // namespace test
}  // namespace hm3
