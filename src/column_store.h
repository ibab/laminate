#ifndef LAMINATE_COLUMN_STORE_H
#define LAMINATE_COLUMN_STORE_H

#include <vector>
#include <string>
#include <google/protobuf/message.h>

#include "status.h"

namespace laminate {

class ColumnWriter {

public:
  // Constructs a ColumnWriter from a protocol buffer message
  static std::tuple<ColumnWriter, Status> Create(google::protobuf::Message& m);
private:
  ColumnWriter(
    std::vector<const google::protobuf::FieldDescriptor*> fds,
    std::vector<std::string> names
  );

  std::vector<const google::protobuf::FieldDescriptor*> fds_;
  std::vector<std::string> names_;

};

}  // namespace laminate

#endif  // LAMINATE_COLUMN_STORE_H
