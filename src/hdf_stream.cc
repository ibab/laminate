#include <vector>
#include <sys/stat.h>

#include "hdf_stream.h"

using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::ZeroCopyInputStream;

namespace laminate {

bool exists(std::string path) {
    struct stat info;   
    return stat(path.c_str(), &info) == 0; 
}

HDFOutputStream::HDFOutputStream(ZeroCopyOutputStream *output,
    const std::string& fname, const std::string& dataset, const Options& options)
  : output_(output),
    count_(0) {

  H5std_string filename = fname;
  H5std_string dset = dataset;
  if (exists(fname)) {
    file_ = H5::H5File(filename, H5F_ACC_RDWR);
  } else {
    file_ = H5::H5File(filename, H5F_ACC_TRUNC);
  }

  H5::DSetCreatPropList plist;
  hsize_t chunkdims[1] = { 100 };
  plist.setChunk(1, chunkdims);
  plist.setDeflate(7);

  hsize_t dims[1];
  hsize_t maxdims[1] = { H5S_UNLIMITED };
  dims[0] = options_.chunksize;
  H5::DataSpace dataspace(1, dims, maxdims);
  H5::IntType dtype(H5::PredType::NATIVE_INT);
  dtype.setOrder(H5T_ORDER_LE);
  dataset_ = file_.createDataSet(dset, dtype, dataspace, plist);

  buffer_ = std::vector<int>();
  buffer_.reserve(options_.chunksize);
};

HDFOutputStream::~HDFOutputStream() {
  file_.close();
}

int64 HDFOutputStream::ByteCount() const {
  return count_;
}

void HDFOutputStream::BackUp(int count) {
}

bool HDFOutputStream::Next(void** data, int* size) {
  return false;
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
