#include "hdf_stream.h"

using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::ZeroCopyInputStream;

namespace laminate {

HDFInputStream::HDFInputStream(const std::string& fname,
                               const std::string& dataset) {}

HDFInputStream::~HDFInputStream() {}

int64 HDFInputStream::ByteCount() const {
  return 0;
}

bool HDFInputStream::Skip(int count) {
  return false;
}

bool HDFInputStream::Next(const void** data, int* size) {
  return false;
}

void HDFInputStream::BackUp(int count) {}

template <>
class HDFOutputStream<int>;
}
