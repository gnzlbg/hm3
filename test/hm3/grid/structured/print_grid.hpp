#pragma once
#include <iomanip>
#include <iostream>

inline void print_2d_grid() {
  for (int j = 13; j >= 0; --j) {
    std::cout << std::setw(3) << j << " | ";
    for (int i = 0; i < 14; ++i) {
      std::cout << std::setw(3) << (j * 14 + i) << " ";
      if (i == 1 || i == 11) { std::cout << "  "; }
    }
    std::cout << std::endl;
    if (j == 2 or j == 12) { std::cout << std::endl; }
  }
  std::cout << std::endl;
  std::cout << std::setw(3) << "i:"
            << " | ";
  for (int i = 0; i < 14; ++i) {
    std::cout << std::setw(3) << i << " ";
    if (i == 1 || i == 11) { std::cout << "  "; }
  }
  std::cout << std::endl;
}
