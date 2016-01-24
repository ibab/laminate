#ifndef LAMINATE_BLOSC_STREAM_H
#define LAMINATE_BLOSC_STREAM_H

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/coded_stream.h>
#include <blosc.h>

using google::protobuf::int64;

class BloscOutputStream : public google::protobuf::io::ZeroCopyOutputStream {
  public:
  /* 
   * Options for the blosc compressor and the output stream.
   * Many of these are identical to the arguments of the
   * `blosc_decompress` function in `blosc.h`.
   */
  class Options {
      public:
          // Default: 5
          int compression_level;
          // As we can't know about the size of the
          // data type a priori, we disable bit shuffling.
          // Set this to `true` if your input data is homogeneous
          // and you know the length of each item in bits.
          // Default: false
          bool use_shuffling;
          // Default: 32
          int typesize_bits;
          // Default: "blosclz"
          std::string compressor;
          // Blosc will automatically pick a blocksize
          // Default: 0
          int blocksize;
          // Default: 4
          int numinternalthread;
          // Length of the internal buffer that the output stream
          // will try to fill and compress in one piece.
          // As blosc introduces a small header for each chunk,
          // settings this to a larger number could make sense if
          // your data is highly compressed.
          // Default: 1024 * 1024 bytes
          int chunk_size;
  };

  /*
   * Constructor assuming default compression options
   */
  BloscOutputStream(google::protobuf::io::ZeroCopyOutputStream* output);
  /*
   * Constructor that allows you to set compression options
   */
  BloscOutputStream(google::protobuf::io::ZeroCopyOutputStream* output,
                    const Options& options);
  ~BloscOutputStream();
  bool Next(void** data, int* size);
  void BackUp(int count);
  int64 ByteCount() const;
  /*
   * Compresses the internal buffer and writes it downstream.
   * Calling this function can result in compressed chunks smaller
   * than the default one and thus smaller compression ratios.
   */
  void Flush();

  static Options DefaultOptions();

  private:
  const Options options_;
  int buffer_size_;
  int buffer_filled_;
  void* buffer_;
  void* blosc_buffer_;
  int64 bytes_written_;
  google::protobuf::io::ZeroCopyOutputStream* output_;
};

class BloscInputStream : public google::protobuf::io::ZeroCopyInputStream {
  public:
  BloscInputStream(google::protobuf::io::ZeroCopyInputStream* input);
  ~BloscInputStream();
  bool Next(const void** data, int* size);
  void BackUp(int count);
  bool Skip(int count);
  int64 ByteCount() const;
  private:
  google::protobuf::io::ZeroCopyInputStream* input_;
  void* uncompressed_data_;
  uint32_t uncompressed_size_;
  uint32_t read_;
  uint32_t served_;
  uint32_t bytecount_;
  bool readChunk();
};

#endif
