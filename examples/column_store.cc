
#include "column_store.h"
#include <iostream>
#include "examples/example.pb.h"

using namespace example;

int main() {
  Person person;
  auto meta = laminate::ColumnWriter::Create(person);
  return 0;
}
