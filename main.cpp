#include <cmath>
#include <iostream>

using namespace std;

int main() {

  char operation;
  int a;
  int b;

  cout << "Enter a Number: ";
  cin >> a;

  cout << "Enter an Operation (+, -, *, /): ";
  cin >> operation;

  std::cout << "Enter a Number: ";
  std::cin >> b;

  switch (operation) {
  case '+':
    std::cout << a << " + " << b << " = " << a + b << std::endl;
    break;
  case '-':
    std::cout << a << " - " << b << " = " << a - b << std::endl;
    break;
  case '*':
    std::cout << a << " * " << b << " = " << a * b << std::endl;
    break;
  case '/':
    b == 0 ? std::cout << a << " / " << b << " = " << a / b
                       << " 🐛       <---  uh oh a bug" << std::endl
           : std::cout << a << " / " << b << " = " << a / b << std::endl;
    break;

  default:
    std::cout << std::endl << "not a valid operation" << std::endl;
    break;
  }

  return 0;
}
