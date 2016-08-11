
#include <iostream>
#include "column_store.h"
#include "examples/main.pb.h"

using namespace example;

int main() {

  Person person;
  auto meta = laminate::CreateColumnWriter(person);
  for (int i = 0; i < meta.names.size(); i++) {
    std::cout << meta.names[i] << std::endl;
  }
  return 0;
}
