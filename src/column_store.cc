
#include <iostream>
#include <stack>
#include <tuple>
#include <vector>
#include <unordered_set>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

#include "column_store.h"
#include "hdf_stream.h"

namespace laminate {

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::io::ZeroCopyOutputStream;

ColumnStore::ColumnStore(std::string path, std::string mode) {
  // TODO check mode
  path_ = path;
  initialized_ = false;
}

Status ColumnStore::Put(Message& m) {
  if (!initialized_) {
    // We are seeing a message for the first time, and need to create the
    // ColumnWriter
    Status ok;
    std::tie(writer_, ok) = ColumnWriter::Create(m);
    if (!ok) {
      return ok;
    }
    std::vector<ZeroCopyOutputStream*> streams;
    for(int i = 0; i < writer_.Names().size(); i++) {
      FieldDescriptor::CppType type = writer_.Descriptors()[i].back()->cpp_type();

#define CASE_TYPE(A, B)                                                   \
  case FieldDescriptor::CPPTYPE_##A:                                      \
    streams.push_back(new HDFOutputStream<B>(path_, writer_.Names()[i])); \
    break;

      switch(type) {
        CASE_TYPE(INT32, int);
        CASE_TYPE(INT64, long);
        CASE_TYPE(FLOAT, float);
        CASE_TYPE(DOUBLE, double);
        default:
          return Status::Error("Unknown field type encountered");
      }
#undef CASE_TYPE
    }
    writer_.SetStreams(streams);
    initialized_ = true;
  }

  return writer_.ShredRecord(m);
}

}  // namespace laminate
