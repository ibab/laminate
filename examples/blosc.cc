
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <random>
#include <iostream>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include "blosc_stream.h"


const uint32_t total_number= 30000000;

using namespace google::protobuf;

void writeValues() {

  int fd = open("out.blosc", O_CREAT | O_WRONLY | O_TRUNC, 0600);
  io::ZeroCopyOutputStream* out = new io::FileOutputStream(fd);

  std::mt19937 rand(0);
  std::uniform_int_distribution<> uniform(0, 100);

  auto options = BloscOutputStream::DefaultOptions();
  options.compressor = "lz4";
  options.compression_level = 9;
  options.use_shuffling = true;
  options.typesize_bits = 32;
  options.chunk_size = 1024 * 1024;
  BloscOutputStream* blosc = new BloscOutputStream(out, options);

  io::CodedOutputStream* coded = new io::CodedOutputStream(blosc);

  void* data;
  int size;

  for (uint32_t i = 0; i < total_number; i++) {
      coded->WriteLittleEndian32(i);
  }

  delete coded;
  delete blosc;
  delete out;
}

void readValues() {
    int fd = open("out.blosc", O_RDONLY, 0600);
    io::ZeroCopyInputStream* in = new io::FileInputStream(fd);
    io::ZeroCopyInputStream* blosc = new BloscInputStream(in);
    io::CodedInputStream* coded = new io::CodedInputStream(blosc);
    coded->SetTotalBytesLimit(500000000, -1);

    std::mt19937 rand(0);
    std::uniform_int_distribution<> uniform(0, 100);

    for (int i = 0; i < total_number; i++) {
        uint32_t out;
        int expected = i;
        coded->ReadLittleEndian32(&out);
        if (out != expected) {
            std::cout << "Expected " << expected << " but got " << out << " at " << i << std::endl;
            exit(1);
        }
    }
}

int main() {
  writeValues();
  readValues();

  return 0;
}
