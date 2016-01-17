#ifndef LAMINATE_BLOSC_STREAM_H
#define LAMINATE_BLOSC_STREAM_H

#include <google/protobuf/io/zero_copy_stream.h>
#include <blosc.h>

using google::protobuf::int64;

class BloscOutputStream : public google::protobuf::io::ZeroCopyOutputStream {
  public:
  BloscOutputStream(google::protobuf::io::ZeroCopyOutputStream* output,
                    // The size of the type to be stored in bits
                    // Allows blosc to do its blocking magic
                    int typesize);
  ~BloscOutputStream();
  bool Next(void** data, int* size);
  void BackUp(int count);
  int64 ByteCount() const;
  void Flush();

  private:
  void* buffer_;
  int buffer_size_;
  int buffer_filled_;
  int64 bytes_written_;
  google::protobuf::io::ZeroCopyOutputStream* output_;
  int typesize_;
};

#endif
