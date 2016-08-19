#include <H5Cpp.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>
#include <iostream>
#include <stack>
#include <tuple>
#include <vector>
#include <unordered_set>

#include "column_store.h"
#include "hdf_stream.h"

namespace laminate {

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::io::ZeroCopyOutputStream;

enum FieldType {
  REQUIRED,
  OPTIONAL,
  REPEATED
};

ColumnWriter::ColumnWriter(
    std::vector<std::vector<const google::protobuf::FieldDescriptor*>> fds,
    std::vector<std::string> names)
  : fds_(fds), names_(names) {}

// Performs a depth-first traversal of the protobuf definition,
// storing the field descriptors and fully qualified names of
// all leaf (== primitive) entries.
std::tuple<ColumnWriter, Status> ColumnWriter::Create(Message& m) {
  const Descriptor* desc = m.GetDescriptor();
  std::stack<std::vector<const FieldDescriptor*>> stack;
  // Field descriptors of leaf fields
  std::vector<std::vector<const FieldDescriptor*>> fds;
  // Used to check whether there are recursive messages
  std::unordered_set<const Descriptor*> visited;
  // Used to store the fully qualified name of each field:
  std::vector<std::string> names;
  for (int i = 0; i < desc->field_count(); i++) {
    stack.push(std::vector<const FieldDescriptor*>{desc->field(i)});
  }

  while (!stack.empty()) {
    std::vector<const FieldDescriptor*> fd_seq = stack.top();
    stack.pop();
    const FieldDescriptor* fd = fd_seq.back();
    const Descriptor* desc = fd->message_type();
    if (visited.count(desc)) {
      // Type is not a simple tree
      return std::make_tuple(
          ColumnWriter(),
          Status::Error("Recursive messages are not supported"));
    }
    if (desc) {
      for (int i = 0; i < desc->field_count(); i++) {
        const FieldDescriptor* field = desc->field(i);
        std::vector<const FieldDescriptor*> tmp = fd_seq;
        tmp.push_back(field);
        stack.push(tmp);
      }
    } else {
      fds.push_back(fd_seq);
      names.push_back(fd->full_name());
    }
  }

  return std::make_tuple(ColumnWriter(fds, names), Status::OK());
}

ColumnWriter::ColumnWriter() {}

ColumnWriter::ColumnWriter(const ColumnWriter& w) {
  names_ = w.names_;
  fds_ = w.fds_;
  streams_ = w.streams_;
}

ColumnWriter::~ColumnWriter() {
  for (int i = 0; i < streams_.size(); i++) {
    if(streams_[i]) {
      delete streams_[i];
    }
  }
}

std::vector<std::string> ColumnWriter::Names() {
  return names_;
}

std::vector<std::vector<const google::protobuf::FieldDescriptor*>> ColumnWriter::Descriptors() {
  return fds_;
}

std::vector<google::protobuf::io::ZeroCopyOutputStream*> ColumnWriter::Streams() {
  return streams_;
}

Status ColumnWriter::SetStreams(std::vector<google::protobuf::io::ZeroCopyOutputStream*>& streams) {
  if (names_.size() != streams.size()) {
      return Status::Error("Number of streams doesn't match number of fields");
  }
  streams_ = streams;
  return Status::OK();
}

// Splits a message into several columns using the Dremel column shredding
// algorithm and writes the data into the output streams of this ColumnWriter
Status ColumnWriter::ShredRecord(Message& m) {
  const Reflection* ref = m.GetReflection();
  for (int i = 0; i < names_.size(); i++) {
    Message* tmp = &m;
    Message* sub = &m;
    const Reflection* subref = m.GetReflection();
    std::vector<const FieldDescriptor*> fds = fds_[i];
    for (int j = 0; fds[j]->message_type() != nullptr; j++) {
      sub = ref->MutableMessage(tmp, fds[j]);
      tmp = sub;
      subref = tmp->GetReflection();
    }
    CodedOutputStream coded(streams_[i]);
    FieldDescriptor::CppType type = fds.back()->cpp_type();

#define CASE_TYPE(A, B, C)                    \
  case FieldDescriptor::CPPTYPE_##A: {        \
    B val = subref->Get##C(*sub, fds.back()); \
    coded.WriteRaw(&val, sizeof(B));          \
    break;                                    \
  }
    switch(type) {
      CASE_TYPE(INT32, int, Int32);
      CASE_TYPE(INT64, long, Int64);
      CASE_TYPE(FLOAT, float, Float);
      CASE_TYPE(DOUBLE, double, Double);
      default:
        return Status::Error("Unknown field type encountered");
    }
  }
#undef CASE_TYPE
  return Status::OK();
}

Store::Store(std::string path, std::string mode) {
  // TODO check mode
  path_ = path;
  initialized_ = false;
}

Status Store::Put(Message& m) {
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
