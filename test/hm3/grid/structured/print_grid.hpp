#pragma once
#include <iomanip>
#include <iostream>

void print_2d_grid() {
  for (int j = 0; j < 14; ++j) {
    for (int i = 0; i < 14; ++i) {
      std::cout << std::setw(3) << (j * 14 + i) << " ";
      if (i == 1 || i == 11) { std::cout << "  "; }
    }
    std::cout << std::endl;
    if (j == 1 or j == 11) { std::cout << std::endl; }
  }
}
