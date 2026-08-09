#include <iostream>

int main() {
  // Declaration and initialization
  int matrix[3][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};

  // Access and print elements using nested loops
  for (int i = 0; i < 3; i++) {   // Rows
    for (int j = 0; j < 4; j++) { // Columns
      std::cout << matrix[i][j] << " ";
    }
    std::cout << "\n";
  }

  // Updating an element
  matrix[0][0] = 99;

  return 0;
}
