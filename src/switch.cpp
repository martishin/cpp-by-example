#include <iostream>

int main() {
  int const x = 2;
  switch (x) {
    case 1:
      std::cout << "One" << std::endl;
      break;
    case 2:
      std::cout << "Two" << std::endl;
      break;
    default:
      break;
  }
  return 0;
}
