#include <iostream>
#include <stack>
#include <vector>
#include <tuple>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <H5Cpp.h>

#include "include/column_store.h"
#include "include/hdf_stream.h"

namespace laminate {

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;

// Performs a depth-first traversal of the protobuf definition,
// storing the field descriptors and fully qualified names of
// all leaf (== primitive) entries.
ColumnWriter CreateColumnWriter(Message &m) {
  typedef std::tuple<const FieldDescriptor*, std::string> Info;
  const Descriptor* desc = m.GetDescriptor();
  std::stack<Info> stack;
  std::vector<const FieldDescriptor*> fds;
  std::vector<std::string> names;
  for (int i = 0; i < desc->field_count(); i++) {
    stack.push(Info(desc->field(i), desc->field(i)->name()));
  }

  while (!stack.empty()) {
    Info info = stack.top();
    stack.pop();
    const FieldDescriptor* fd = std::get<0>(info);
    const Descriptor* desc = fd->message_type();
    if (desc) {
      for (int i = 0; i < desc->field_count(); i++) {
        const FieldDescriptor* field = desc->field(i);
        stack.push(Info(field, std::get<1>(info) + "." + field->name()));
      }
    } else {
      fds.push_back(std::get<0>(info));
      names.push_back(std::get<1>(info));
    }
  }

  H5std_string fname = "file.h5";
  H5std_string dset = "myData";
  H5::H5File file(fname, H5F_ACC_TRUNC);
  hsize_t dimsf[2];              // dataset dimensions
  dimsf[0] = 10;
  dimsf[1] = 10;
  H5::DataSpace dataspace(2, dimsf);
  H5::IntType dtype(H5::PredType::NATIVE_INT);
  dtype.setOrder(H5T_ORDER_LE);
  H5::DataSet dataset = file.createDataSet(dset, dtype, dataspace);

  std::array<std::array<int, 10>, 10> data{};
  dataset.write(&data[0][0], H5::PredType::NATIVE_INT);

  return ColumnWriter{fds, names};
}

}  // namespace laminate
