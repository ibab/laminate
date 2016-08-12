
#ifndef LAMINATE_HDF_STREAM_H
#define LAMINATE_HDF_STREAM_H

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/coded_stream.h>
#include <H5Cpp.h>
#include <vector>
#include <sys/stat.h>

namespace laminate {

using google::protobuf::int64;

template <typename T>
class HDFOutputStream : public google::protobuf::io::ZeroCopyOutputStream {
public:
  struct Options;

private:
  int64 count_;
  Options options_;
  H5::H5File file_;
  H5::DataSet dataset_;
  std::vector<int> buffer_;

  bool exists(std::string path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0; 
  }

public:

  struct Options {
    Options() : chunksize(0) {}
    int chunksize;
  };

  HDFOutputStream(const std::string& fname, const std::string& dataset, const Options& options = Options())
    : count_(0), options_(options) {

    H5std_string filename = fname;
    H5std_string dset = dataset;
    if (exists(fname)) {
      file_ = H5::H5File(filename, H5F_ACC_RDWR);
    } else {
      file_ = H5::H5File(filename, H5F_ACC_TRUNC);
    }

    try {
      H5::Exception::dontPrint();
      dataset_ = file_.openDataSet(dset);
    } catch(H5::FileIException not_found_error) {
      H5::DSetCreatPropList plist;
      hsize_t chunkdims[1] = { 1 << 10 };
      plist.setChunk(1, chunkdims);
      plist.setDeflate(7);

      hsize_t dims[1];
      hsize_t maxdims[1] = { H5S_UNLIMITED };
      dims[0] = options_.chunksize;
      H5::DataSpace dataspace(1, dims, maxdims);
      H5::IntType dtype(H5::PredType::NATIVE_INT);
      dtype.setOrder(H5T_ORDER_LE);
      dataset_ = file_.createDataSet(dset, dtype, dataspace, plist);
    }

    buffer_ = std::vector<int>();
    buffer_.reserve(options_.chunksize);
  }

  ~HDFOutputStream() {
    file_.close();
  }

  bool Next(void** data, int* size) {
    count_ += *size / sizeof(T);
    *size = 0;
    return true;
  }

  void BackUp(int count) {}

  int64 ByteCount() const {
    return count_ * sizeof(T);
  }

};

class HDFInputStream : public google::protobuf::io::ZeroCopyInputStream {
public:
  HDFInputStream(const std::string& fname, const std::string& dataset);
  ~HDFInputStream();
  bool Next(const void** data, int* size);
  void BackUp(int count);
  bool Skip(int count);
  int64 ByteCount() const;
private:
};

}

#endif  // LAMINATE_HDF_STREAM_H
