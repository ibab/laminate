#include <iostream>
#include "blosc_stream.h"

using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::io::CodedInputStream;

BloscOutputStream::BloscOutputStream(ZeroCopyOutputStream* output)
    : BloscOutputStream(output, DefaultOptions()) {}

BloscOutputStream::BloscOutputStream(ZeroCopyOutputStream* output,
                                     const Options& options)
    : buffer_size_(options.chunk_size),
      buffer_filled_(0),
      bytes_written_(0),
      output_(output),
      options_(options) {
  buffer_ = operator new(buffer_size_);
  blosc_buffer_ = operator new(options.chunk_size + BLOSC_MAX_OVERHEAD);
}

BloscOutputStream::~BloscOutputStream() {
  if (buffer_filled_ > 0) {
    Flush();
  }
  operator delete(buffer_);
}

// XXX handle errors on write
void BloscOutputStream::Flush() {
  int blosc_max_buffer_size = buffer_filled_ + BLOSC_MAX_OVERHEAD;

  int blosc_buffer_size = blosc_compress_ctx(
      options_.compression_level, options_.use_shuffling,
      options_.typesize_bits, buffer_filled_, buffer_, blosc_buffer_,
      blosc_max_buffer_size, options_.compressor.c_str(), options_.blocksize,
      options_.numinternalthread);

  // We create a new CodedOutputStream for each compressed
  // chunk we write. Don't worry, this is fast!
  google::protobuf::io::CodedOutputStream coded(output_);
  
  // First, we write a header consisting of 2 varints:
  //  - compressed size of chunk
  //  - uncompressed size of chunk
  coded.WriteVarint32(blosc_buffer_size);
  coded.WriteVarint32(buffer_filled_);

  // Then, we write the compressed chunk itself
  // XXX Can we avoid the malloc and copy in WriteRaw()?
  // Maybe through WriteRawMaybeAliased() or GetDirectBufferPointer()?
  coded.WriteRaw(blosc_buffer_, blosc_buffer_size);

  bytes_written_ += coded.ByteCount();
}

bool BloscOutputStream::Next(void** data, int* size) {
  // We try to fill up a buffer of `options.chunk_size` bytes
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
  // XXX Don't allow to back up more than has been written in
  // last call to Next()
  buffer_filled_ -= count;
}

BloscOutputStream::Options BloscOutputStream::DefaultOptions() {
  Options options;
  options.compression_level = 5;
  options.typesize_bits = 32;
  options.use_shuffling = false;
  options.numinternalthread = 4;
  options.compressor = "blosclz";
  options.chunk_size = 1024 * 1024;

  return options;
}

int64 BloscOutputStream::ByteCount() const { return bytes_written_; }

BloscInputStream::BloscInputStream(google::protobuf::io::ZeroCopyInputStream* input)
    : input_(input)
{
    uncompressed_data_ = nullptr;
    read_ = 0;
    uncompressed_size_ = 0;
    served_ = uncompressed_size_;
    bytecount_ = 0;
}

BloscInputStream::~BloscInputStream() {
}

bool BloscInputStream::readChunk() {
    CodedInputStream coded(input_);

    uint32_t compressed_size;

    bool ok = true;

    ok &= coded.ReadVarint32(&compressed_size);
    if (!ok) { return false; }
    ok &= coded.ReadVarint32(&uncompressed_size_);
    if (!ok) { return false; }

    google::protobuf::io::CodedInputStream::Limit limit = coded.PushLimit(compressed_size);
    void* compressed_data = operator new(compressed_size);
    operator delete(uncompressed_data_);
    uncompressed_data_ = operator new(uncompressed_size_);

    ok &= coded.ReadRaw(compressed_data, compressed_size);
    coded.PopLimit(limit);

    int out;
    if (ok) {
        out = blosc_decompress_ctx(
                compressed_data,
                uncompressed_data_,
                uncompressed_size_,
                4);
    }

    operator delete(compressed_data);
    served_ = 0;
    return ok;
}

bool BloscInputStream::Next(const void** data, int* size) {
    if (served_ == uncompressed_size_) {
        bool ok = readChunk();
        if (!ok) { return false; }
    }

    *data = (char*) uncompressed_data_ + served_;
    *size = uncompressed_size_ - served_;
    served_ = uncompressed_size_;
    bytecount_ += *size;
    return true;
}

void BloscInputStream::BackUp(int count) {
    //std::cout << "Back up " << count << std::endl;
    served_ -= count;
    bytecount_ -= count;
}

bool BloscInputStream::Skip(int count) {
    std::cout << "Skip " << count << std::endl;
    served_ += count;
    bytecount_ += count;
    return true;
}

int64 BloscInputStream::ByteCount() const {
    return bytecount_;
}
