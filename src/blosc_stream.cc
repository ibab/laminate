#include <iostream>
#include "blosc_stream.h"

#define LAMINATE_BLOSC_CHUNKSIZE 1024 * 1024

using google::protobuf::io::ZeroCopyOutputStream;

BloscOutputStream::BloscOutputStream(ZeroCopyOutputStream* output, int typesize)
    : buffer_size_(LAMINATE_BLOSC_CHUNKSIZE),
      buffer_filled_(0),
      output_(output),
      bytes_written_(0),
      typesize_(typesize) {
  buffer_ = operator new(buffer_size_);
}

BloscOutputStream::~BloscOutputStream() {
  if (buffer_filled_ > 0) {
    Flush();
  }
  operator delete(buffer_);
}

void BloscOutputStream::Flush() {
  int blosc_max_buffer_size = buffer_filled_ + BLOSC_MAX_OVERHEAD;
  void* blosc_buffer = operator new(blosc_max_buffer_size);

  int blosc_buffer_size = blosc_compress_ctx(7,               // clevel
                                             1,               // doshuffle
                                             typesize_,       // typesize
                                             buffer_filled_,  // nbytes
                                             buffer_,         // src
                                             blosc_buffer,    // dest
                                             blosc_max_buffer_size,  // destsize
                                             "blosclz",  // compressor
                                             0,          // blocksize
                                             8           // numinternalthreads
                                             );

  void* out_buffer;
  int out_size;

  int written = 0;

  while (written < blosc_buffer_size) {
    output_->Next(&out_buffer, &out_size);
    if (written + out_size > blosc_buffer_size) {
      // This is the last chunk
      memcpy(out_buffer, (char*)blosc_buffer + written,
             blosc_buffer_size - written);
      // Let's back up the bit we don't want to write
      output_->BackUp(out_size - (blosc_buffer_size - written));
      written = blosc_buffer_size;
    } else {
      // We're not finished yet
      memcpy(out_buffer, (char*)blosc_buffer + written, out_size);
      written += out_size;
    }
  }

  bytes_written_ += buffer_filled_;

  operator delete(blosc_buffer);
}

bool BloscOutputStream::Next(void** data, int* size) {
  // We try to fill up a buffer of LAMINATE_BLOSC_CHUNKSIZE bytes
  // before compressing and passing the data downstream.
  // If we get destroyed, we are forced to flush the incomplete buffer.

  if (buffer_filled_ < buffer_size_) {
    // Internal buffer is not full, hand out the remaining chunk
    *data = (char*)buffer_ + buffer_filled_;
    *size = buffer_size_ - buffer_filled_;
  } else {
    // Internal buffer is full, compress and push downstream
    Flush();
    // We reuse the existing buffer
    *data = buffer_;
    *size = buffer_size_;
  }

  // Assume that user has filled out buffer completely
  buffer_filled_ = buffer_size_;

  return true;
}

void BloscOutputStream::BackUp(int count) {
  // FIXME Don't allow to back up more than has been written in
  // last call to Next()
  buffer_filled_ -= count;
}

int64 BloscOutputStream::ByteCount() const { return bytes_written_; }
