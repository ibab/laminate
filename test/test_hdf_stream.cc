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
  int* data;
  int count;
  output->Next(&data, &count);
  for (int i = 0; i < count; i++) {
    data[i] = i;
  }
  output->BackUpTyped(3);

  ASSERT_EQ(output->ByteCount(), (count - 3) * sizeof(int));
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
  int* data;
  int count;
  output->Next(&data, &count);
  for (int i = 0; i < 5; i++) {
    data[i] = i;
  }
  output->BackUpTyped(count - 5);
  delete output;

  // Read
  const int* input_data;
  input->Next(&input_data, &count);
  for (int i = 0; i < 5; i++) {
    ASSERT_EQ(input_data[i], i);
  }
  input->BackUpTyped(count - 5);
  delete input;
}

TEST_F(TestHDFStream, InputSkip) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");

  // Write
  int* data;
  int count;
  output->Next(&data, &count);
  for (int i = 0; i < 5; i++) {
    data[i] = i;
  }
  output->BackUpTyped(count - 5);
  delete output;

  // Read
  auto* input = new HDFInputStream<int>(tmppath_ + "/" + "test.h5", "test");
  input->SkipTyped(2);
  const int* input_data;
  input->Next(&input_data, &count);
  ASSERT_EQ(count, 3);
  ASSERT_EQ(input_data[0], 2);
  ASSERT_EQ(input_data[1], 3);
  ASSERT_EQ(input_data[2], 4);
  delete input;
}

TEST_F(TestHDFStream, InputBackUp) {
  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  // Write
  int* data;
  int count;
  output->Next(&data, &count);
  for (int i = 0; i < 10; i++) {
    data[i] = i;
  }
  output->BackUpTyped(count - 10);
  delete output;

  // Read
  auto* input = new HDFInputStream<int>(tmppath_ + "/" + "test.h5", "test");
  const int* input_data;
  input->SkipTyped(8);
  input->BackUpTyped(3);
  input->Next(&input_data, &count);
  ASSERT_EQ(count, 5);
  ASSERT_EQ(input_data[0], 5);
  ASSERT_EQ(input_data[1], 6);
  ASSERT_EQ(input_data[2], 7);
  ASSERT_EQ(input_data[3], 8);
  ASSERT_EQ(input_data[4], 9);
}
