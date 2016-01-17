
#include <iostream>

#include <google/protobuf/text_format.h>

#include "row_store.h"
#include "main.pb.h"

// Example for how to write messages
void write_messages(const std::string& filename, int number) {
  laminate::RowStore store(filename, "w");

  example::Person person;

  for (int i = 0; i < number; i++) {
    person.set_name("A. Person");
    person.set_email("person@example.com");
    for (int j = 0; j < 3; j++) {
      auto number = person.add_phones();
      number->set_number("999999999");
      number->set_type(example::Person::MOBILE);
    }
    store.Put(person);
    // Make sure to clear the message after each write!
    person.Clear();
  }
}

// Example for how to read messages
void print_messages(const std::string& filename, int number) {
  laminate::RowStore store(filename, "r");
  example::Person person;

  for (int i = 0; i < number; i++) {
    store.ReadNext(person);
    std::string out;
    google::protobuf::TextFormat::PrintToString(person, &out);
    std::cout << out;
  }
}

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "blosc_stream.h"
using google::protobuf::io::FileOutputStream;
using google::protobuf::io::ZeroCopyOutputStream;

void test_blosc() {
  ZeroCopyOutputStream *file = new FileOutputStream(0);
  ZeroCopyOutputStream *output = new BloscOutputStream(file);

}

int main() {
  write_messages("data.rows", 1000);
  print_messages("data.rows", 1);
  test_blosc();
}
