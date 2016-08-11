
#ifndef LAMINATE_HDF_STREAM_H
#define LAMINATE_HDF_STREAM_H

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/coded_stream.h>


namespace laminate {

using google::protobuf::int64;

class HDFOutputStream : public google::protobuf::io::ZeroCopyOutputStream {
public:

  struct Options {
  };

  HDFOutputStream(google::protobuf::io::ZeroCopyOutputStream *output,
      const std::string& fname, const std::string& dataset, const Options& options);

  ~HDFOutputStream();
  bool Next(void** data, int* size);
  void BackUp(int count);
  int64 ByteCount() const;
private:
  google::protobuf::io::ZeroCopyOutputStream* output_;
};

class HDFInputStream : public google::protobuf::io::ZeroCopyInputStream {
public:

  HDFInputStream(google::protobuf::io::ZeroCopyInputStream* input);
  ~HDFInputStream();
  bool Next(const void** data, int* size);
  void BackUp(int count);
  bool Skip(int count);
  int64 ByteCount() const;
private:
  google::protobuf::io::ZeroCopyInputStream* input_;
};

}

#endif  // LAMINATE_HDF_STREAM_H
