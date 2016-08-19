
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include "column_store.h"
#include "status.h"
#include "test/test.pb.h"

namespace fs = boost::filesystem;

class TestStore : public testing::Test {
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

class TestStoreVerbose : public testing::Test {
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

TEST_F(TestStoreVerbose, Run) {
  laminate::Store store(tmppath_ + "/test.h5", "w");
  test::TestMessage msg;
  msg.set_value1(1);
  msg.set_value2(2);
  test::TestMessage::InnerMessage* inner = msg.mutable_value3();
  inner->set_inner_value1(3);
  inner->set_inner_value2(4);

  laminate::Status ok;
  for (int i = 0; i < 10; i++) {
    ok.Update(store.Put(msg));
    if (!ok) {
      break;
    }
  }

  ASSERT_EQ(ok.Message(), "OK");
}
