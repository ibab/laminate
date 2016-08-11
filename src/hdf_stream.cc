
#include "hdf_stream.h"

using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::ZeroCopyInputStream;

namespace laminate {

HDFOutputStream::HDFOutputStream(ZeroCopyOutputStream *output,
    const std::string& fname, const std::string& dataset, const Options& options)
  : output_(output) {
};

int64 HDFOutputStream::ByteCount() const {
  return 0;
}

void HDFOutputStream::BackUp(int count) {
}

bool HDFOutputStream::Next(void** data, int* size) {
  return false;
}

HDFOutputStream::~HDFOutputStream() {
}

HDFInputStream::HDFInputStream(ZeroCopyInputStream *input)
  : input_(input) {
}

HDFInputStream::~HDFInputStream() {
}

int64 HDFInputStream::ByteCount() const {
  return 0;
}

bool HDFInputStream::Skip(int count) {
  return false;
}

bool HDFInputStream::Next(const void** data, int* size) {
  return false;
}

void HDFInputStream::BackUp(int count) {
}

}
