
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
  options.compressor = "blosclz";
  options.compression_level = 9;
  options.use_shuffling = true;
  options.typesize_bits = 32;
  BloscOutputStream* blosc = new BloscOutputStream(out, options);

  void* data;
  int size;

  for (int j = 0; j < 100; j++) {
    blosc->Next(&data, &size);
    int* arr = (int*)data;
    int numbers = size / 4;
    for (int i = 0; i < numbers; i++) {
      arr[i] = uniform(rand);
    }
    int rest = size - numbers * 4;
    blosc->BackUp(rest);
  }

  delete blosc;
  delete out;

  return 0;
}
