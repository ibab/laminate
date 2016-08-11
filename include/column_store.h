#ifndef LAMINATE_COLUMN_STORE_H
#define LAMINATE_COLUMN_STORE_H

#include <google/protobuf/message.h>

namespace laminate {

class ColumnWriter {
  std::vector<const google::protobuf::FieldDescriptor*> fds;
  std::vector<std::string> names;
};

ColumnWriter CreateFieldMeta(google::protobuf::Message &m);

}  // namespace laminate

#endif  // LAMINATE_COLUMN_STORE_H
