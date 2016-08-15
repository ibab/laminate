#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <random>
#include <functional>
#include <google/protobuf/io/coded_stream.h>

#include "hdf_stream.h"

using namespace laminate;
namespace fs = boost::filesystem;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::io::CodedInputStream;

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

class TestHDFStreamVerbose : public testing::Test {
  public:
  void SetUp() override {
    fs::path tmpdir = fs::temp_directory_path();
    fs::path path = fs::unique_path();
    fs::create_directory(tmpdir / path);
    tmppath_ = (tmpdir / path).native();
    std::cerr << "Output at " << tmppath_ << std::endl;
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

TEST_F(TestHDFStream, LargeWriteRead) {
  std::random_device rnd_device;
  std::mt19937 mersenne_engine(rnd_device());
  std::uniform_int_distribution<int> dist(-100, 100);

  auto rand = std::bind(dist, mersenne_engine);
  std::vector<int> random_data(300000);
  std::generate(random_data.begin(), random_data.end(), rand);

  auto* output = new HDFOutputStream<int>(tmppath_ + "/" + "test.h5", "test");
  auto* coded = new CodedOutputStream(output);
  for (int x: random_data) {
    coded->WriteLittleEndian32((unsigned int)x);
  }
  delete coded;
  delete output;

  auto* input = new HDFInputStream<int>(tmppath_ + "/" + "test.h5", "test");
  auto* coded_in = new CodedInputStream(input);
  unsigned int num;

  for(int i = 0; i < random_data.size(); i++) {
    coded_in->ReadLittleEndian32(&num);
    ASSERT_EQ((int)num, random_data[i]);
  }
  delete coded_in;
  delete input;
}
