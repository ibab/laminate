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
  ColumnWriter(const ColumnWriter& w);
  ColumnWriter();
  ~ColumnWriter();
  static std::tuple<ColumnWriter, Status> Create(google::protobuf::Message& m);
  std::vector<std::string> Names();
  std::vector<std::vector<const google::protobuf::FieldDescriptor*>> Descriptors();
  std::vector<google::protobuf::io::ZeroCopyOutputStream*> Streams();
  Status SetStreams(std::vector<google::protobuf::io::ZeroCopyOutputStream*>& streams);
  Status ShredRecord(google::protobuf::Message& m);

  private:
  ColumnWriter(
    std::vector<std::vector<const google::protobuf::FieldDescriptor*>> fds,
    std::vector<std::string> names
  );

  std::vector<std::vector<const google::protobuf::FieldDescriptor*>> fds_;
  std::vector<std::string> names_;
  std::vector<google::protobuf::io::ZeroCopyOutputStream*> streams_;
};

class Store {
  public:
  Store(std::string path, std::string mode);
  Status Put(google::protobuf::Message &m);

  private:
  ColumnWriter writer_;
  std::string path_;
  bool initialized_;
};

}  // namespace laminate

#endif  // LAMINATE_COLUMN_STORE_H
