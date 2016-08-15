
#ifndef LAMINATE_HDF_STREAM_H
#define LAMINATE_HDF_STREAM_H

#include <H5Cpp.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <sys/stat.h>
#include <vector>
#include <array>
#include <iostream>

#include "src/types.h"

namespace laminate {

using google::protobuf::int64;

template <typename T>
class HDFOutputStream : public google::protobuf::io::ZeroCopyOutputStream {
  public:
  struct Options {
    Options() : chunksize(1 << 18) {}
    int chunksize;
  };

  private:
  int64 count_;
  Options options_;
  H5::H5File file_;
  H5::DataSet dataset_;
  H5::DSetCreatPropList plist_;
  int filled_;
  std::vector<T> buffer_;

  bool exists(std::string path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0;
  }

  public:
  HDFOutputStream(const std::string& fname, const std::string& dataset,
                  const Options& options = Options())
      : count_(0), options_(options), filled_(0), buffer_(options.chunksize / sizeof(T), 0) {
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
      plist_.setDeflate(9);

      hsize_t dims[1];
      hsize_t maxdims[1] = {H5S_UNLIMITED};
      // Start out with empty array;
      dims[0] = 0;
      H5::DataSpace dataspace(1, dims, maxdims);
      H5::IntType dtype(AsHdfType<T>::value);
      dtype.setOrder(H5T_ORDER_LE);
      dataset_ = file_.createDataSet(dset, dtype, dataspace, plist_);
    }
  }

  ~HDFOutputStream() {
    // We need to flush the buffered data to HDF5
    WriteBuffer();
    plist_.close();
    file_.close();
  }

  bool WriteBuffer() {
    H5::DataSpace filespace = dataset_.getSpace();

    hsize_t dims[1];
    filespace.getSimpleExtentDims(dims);
    hsize_t old_size[1];
    old_size[0] = dims[0];
    dims[0] += filled_;
    dataset_.extend(dims);

    hsize_t offset[1] = {filled_};

    filespace = dataset_.getSpace();
    filespace.selectHyperslab(H5S_SELECT_SET, offset, old_size);
    H5::DataSpace memspace(1, offset);
    dataset_.write(&buffer_[0], AsHdfType<T>::value, memspace, filespace);
    filled_ = 0;
    return true;
  }

  bool Next(void** data, int* size) {
    int count;
    bool ok = Next((T**) data, &count);
    *size = count * sizeof(T);
    return ok;
  }

  bool Next(T** data, int* count) {
    bool ok = true;
    if (filled_ == buffer_.size()) {
      ok = WriteBuffer();
      if (ok) {
        count_ += buffer_.size();
        filled_ = 0;
      } else {
        return false;
      }
    }

    *data = &buffer_[filled_];
    *count = buffer_.size() - filled_;
    count_ += buffer_.size() - filled_;
    filled_ = buffer_.size();
    return ok;
  }

  void BackUp(int size) {
    filled_ -= size / sizeof(T);
    count_ -= size / sizeof(T);
  }

  void BackUpTyped(int count) {
    filled_ -= count;
    count_ -= count;
  }

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
  int skip_;
  int backup_;
  std::vector<T> buffer_;

  public:
  HDFInputStream(const std::string& fname, const std::string& dataset)
      : count_(0), skip_(0), backup_(0), buffer_(1 << 18 / sizeof(T), 0) {
    H5std_string filename = fname;
    H5std_string dset = dataset;
    hsize_t dims_[1];

    file_ = H5::H5File(filename, H5F_ACC_RDWR);
    dataset_ = file_.openDataSet(dset);
    H5::DataSpace filespace = dataset_.getSpace();
    // TODO error if rank != 1
    int rank = filespace.getSimpleExtentNdims();
  }
  ~HDFInputStream() {}

  bool ReadChunk(T** data, int* size) {
    H5::DataSpace filespace = dataset_.getSpace();
    filespace.getSimpleExtentDims(dims_);
    // TODO check for start < 0 or start > length
    hsize_t chunk[1] = {1 << 18 / sizeof(T)};
    if (count_ >= dims_[0]) {
      return false;
    } else if (count_ + chunk[0] > dims_[0]) {
      chunk[0] = dims_[0] - count_;
    }
    hsize_t start[1] = {count_};
    backup_ = 0;
    skip_ = 0;
    filespace.selectHyperslab(H5S_SELECT_SET, chunk, start);
    H5::DataSpace memspace(1, chunk);
    count_ += chunk[0];
    dataset_.read(*data, H5::PredType::NATIVE_INT, memspace, filespace);
    *size = chunk[0];
    return true;
  }

  bool Next(const void** data, int* size) {
    bool ok = Next((const T**)data, size);
    *size *= sizeof(T);
    return ok;
  }

  bool Next(const T** data, int* count) {
    *data = &buffer_[0];
    return ReadChunk(const_cast<T**>(data), count);
  }

  void BackUp(int count) {
    count_ -= count / sizeof(T);
  }
  void BackUpTyped(int count) {
    count_ -= count;
  }
  bool Skip(int count) {
    count_ += count / sizeof(T);
  }
  bool SkipTyped(int count) {
    count_ += count;
  }
  int64 ByteCount() const {
    return count_ * sizeof(T);
  }
};
}

#endif  // LAMINATE_HDF_STREAM_H
