
#ifndef LAMINATE_HDF_STREAM_H
#define LAMINATE_HDF_STREAM_H

#include <H5Cpp.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>

namespace laminate {

using google::protobuf::int64;

template <typename T>
class HDFOutputStream : public google::protobuf::io::ZeroCopyOutputStream {
  public:
  struct Options {
    Options() : chunksize(0) {}
    int chunksize;
  };

  private:
  int64 count_;
  Options options_;
  H5::H5File file_;
  H5::DataSet dataset_;
  H5::DSetCreatPropList plist_;

  bool exists(std::string path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0;
  }

  public:
  HDFOutputStream(const std::string& fname, const std::string& dataset,
                  const Options& options = Options())
      : count_(0), options_(options) {
    H5std_string filename = fname;
    H5std_string dset = dataset;
    if (exists(fname)) {
      file_ = H5::H5File(filename, H5F_ACC_RDWR);
    } else {
      file_ = H5::H5File(filename, H5F_ACC_TRUNC);
    }

    if (H5Lexists(file_.getId(), dset.c_str(), H5P_DEFAULT) == true) {
      dataset_ = file_.openDataSet(dset);
    } else {
      hsize_t chunkdims[1] = {1 << 10};
      plist_.setChunk(1, chunkdims);
      plist_.setDeflate(7);

      hsize_t dims[1];
      hsize_t maxdims[1] = {H5S_UNLIMITED};
      // Start out with empty array;
      dims[0] = 0;
      H5::DataSpace dataspace(1, dims, maxdims);
      H5::IntType dtype(H5::PredType::NATIVE_INT);
      dtype.setOrder(H5T_ORDER_LE);
      dataset_ = file_.createDataSet(dset, dtype, dataspace, plist_);
    }
  }

  ~HDFOutputStream() {
    plist_.close();
    file_.close();
  }

  bool WriteChunk(T** data, int size) {
    H5::DataSpace filespace = dataset_.getSpace();
    hsize_t dims[1];
    filespace.getSimpleExtentDims(dims);
    hsize_t old_size[1];
    old_size[0] = dims[0];
    hsize_t offset[1];
    offset[0] = size;
    dims[0] += size;
    hsize_t start[1] = { 0 };
    dataset_.extend(dims);
    filespace = dataset_.getSpace();
    filespace.selectHyperslab(H5S_SELECT_SET, offset, old_size);
    H5::DataSpace memspace(1, offset);
    memspace.selectHyperslab(H5S_SELECT_SET, offset, start);
    dataset_.write(*data, H5::PredType::NATIVE_INT, memspace, filespace);
    return true;
  }

  bool Next(void** data, int* size) {
    int count = *size / sizeof(T);
    bool ok = NextTyped((T**)data, &count);
    *size = count * sizeof(T);
    return ok;
  }

  bool NextTyped(T** data, int* count) {
    count_ += *count;
    bool ok = WriteChunk(data, *count);
    *count = 0;
    return ok;
  }

  void BackUp(int count) {}

  int64 ByteCount() const {
    return count_ * sizeof(T);
  }
};

template <typename T>
class HDFInputStream : public google::protobuf::io::ZeroCopyInputStream {
  private:
  int count_;
  hsize_t dims_[1];
  H5::H5File file_;
  H5::DataSet dataset_;

  public:
  HDFInputStream(const std::string& fname, const std::string& dataset)
    : count_(0) {
    H5std_string filename = fname;
    H5std_string dset = dataset;
    hsize_t dims_[1];

    file_ = H5::H5File(filename, H5F_ACC_RDWR);
    dataset_ = file_.openDataSet(dset);
    H5::DataSpace filespace = dataset_.getSpace();
    // TODO error if rank != 1
    int rank = filespace.getSimpleExtentNdims();
  }
  ~HDFInputStream() {

  }

  bool ReadChunk(T** data, int* size) {
    H5::DataSpace filespace = dataset_.getSpace();
    filespace.getSimpleExtentDims(dims_);
    dims_[0] -= count_;
    hsize_t start[1] = { count_ };
    filespace.selectHyperslab(H5S_SELECT_SET, dims_, start);
    H5::DataSpace memspace(1, dims_);
    *data = new T[dims_[0]];
    *size = dims_[0];
    count_ = dims_[0] + count_;
    dataset_.read(*data, H5::PredType::NATIVE_INT, memspace, filespace);
    return true;
  }

  bool Next(const void** data, int* size) {
    bool ok = NextTyped((const T**)data, size);
    *size *= sizeof(T);
    return ok;
  }

  bool NextTyped(const T** data, int* count) {
    return ReadChunk(const_cast<T**>(data), count);
  }

  void BackUp(int count) {

  }
  bool Skip(int count) {

  }
  int64 ByteCount() const {
    return count_ * sizeof(T);
  }
};
}

#endif  // LAMINATE_HDF_STREAM_H
