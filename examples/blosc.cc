
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <random>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "blosc_stream.h"

using namespace google::protobuf;

int main() {
  int fd = open("out.blosc", O_CREAT | O_WRONLY | O_TRUNC, 0600);
  io::ZeroCopyOutputStream* out = new io::FileOutputStream(fd);

  std::random_device rd;
  std::mt19937 rand(rd());
  std::uniform_int_distribution<> uniform(0, 100);

  auto options = BloscOutputStream::DefaultOptions();
  options.compressor = "lz4";
  options.compression_level = 7;
  options.use_shuffling = true;
  options.typesize_bits = 32;
  options.chunk_size = 2 * 1024 * 1024;
  BloscOutputStream* blosc = new BloscOutputStream(out, options);

  void* data;
  int size;

  int total_number= 1000000;
  int written = 0;
  while (written < total_number) {
    blosc->Next(&data, &size);
    int* arr = (int*)data;
    int to_write = size / 4;

    for (int i = 0; i < to_write; i++) {
      arr[i] = uniform(rand);
    }
    int rest = size - to_write * 4;

    blosc->BackUp(rest);
    written += to_write;
  }

  delete blosc;
  delete out;

  return 0;
}
