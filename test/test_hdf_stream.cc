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

TEST_F(TestHDFStream, ConstructOutput) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  delete output;
}

// See whether we can open a stream that already exists in the file
TEST_F(TestHDFStream, ConstructTwice) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  delete output;
  output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  delete output;
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
  delete output;
}

TEST_F(TestHDFStream, TypedWriteTo) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  int data[] = {1, 2, 3, 4, 5};
  int* data_ = static_cast<int*>(data);
  int count = 5;
  while (count > 0) {
    output->NextTyped(&data_, &count);
  }
  ASSERT_EQ(count, 0);
  ASSERT_EQ(output->ByteCount(), sizeof(data));
  delete output;
}

TEST_F(TestHDFStream, ConstructOutputTwice) {
  auto* output1 = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  auto* output2 = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  delete output1;
  delete output2;
}

TEST_F(TestHDFStream, ConstructInput) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  delete output;
  auto* input = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  delete input;
}

TEST_F(TestHDFStream, TypedWriteRead) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  auto* input = new HDFInputStream<int>(tmppath_ + "/" + "test.h5", "test");

  // Write
  int data[] = {1, 2, 3, 4, 5};
  int* data_ = (int*)data;
  int count = 5;
  while (count > 0) {
    output->NextTyped(&data_, &count);
  }

  // Read
  const int* input_data;
  int size;
  input->NextTyped(&input_data, &size);
  ASSERT_EQ(size, 5);
  ASSERT_EQ(input_data[0], 1);
  ASSERT_EQ(input_data[1], 2);
  ASSERT_EQ(input_data[2], 3);
  ASSERT_EQ(input_data[3], 4);
  ASSERT_EQ(input_data[4], 5);

  // Write
  data_[0] = 6;
  data_[1] = 7;
  data_[2] = 8;
  data_[3] = 9;
  data_[4] = 10;
  count = 5;
  while (count > 0) {
    output->NextTyped(&data_, &count);
  }

  // Read
  input->NextTyped(&input_data, &size);
  ASSERT_EQ(size, 5);
  ASSERT_EQ(input_data[0], 6);
  ASSERT_EQ(input_data[1], 7);
  ASSERT_EQ(input_data[2], 8);
  ASSERT_EQ(input_data[3], 9);
  ASSERT_EQ(input_data[4], 10);
}
