#include <iostream>

#include <libField/Field.hpp>

int main()
{
  Field<double, 1> f(10);
  std::cout << f << std::endl;
}
