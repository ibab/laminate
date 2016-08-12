#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include "hdf_stream.h"

using namespace laminate;
namespace fs = boost::filesystem;

class TestHDFStream : public testing::Test {
  public:
  void SetUp() override {
    fs::path tmpdir = fs::temp_directory_path();
    fs::path path = fs::unique_path();
    fs::create_directory(tmpdir / path);
    tmppath_ = (tmpdir / path).native();
  }

  void TearDown() override {
    fs::remove_all(tmppath_);
  }

  std::string tmppath_;
};

TEST_F(TestHDFStream, Construct) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
}

// See whether we can open a stream that already exists in the file
TEST_F(TestHDFStream, ConstructTwice) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  delete output;
  output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
}

TEST_F(TestHDFStream, WriteTo) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  int data[] = {1, 2, 3, 4, 5};
  int count = sizeof(data);
  void* arg = (void*)data;
  while (count > 0) {
    output->Next(&arg, &count);
  }
  ASSERT_EQ(count, 0);
  ASSERT_EQ(output->ByteCount(), sizeof(data));
}
