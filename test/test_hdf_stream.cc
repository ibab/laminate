#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include "hdf_stream.h"

using namespace laminate;
namespace fs = boost::filesystem;

TEST(HDFOutputStream, Construct) {
  fs::path tmpdir = fs::temp_directory_path();
  fs::path path = fs::unique_path();
  fs::create_directory(tmpdir / path);

  HDFOutputStream* output = new HDFOutputStream(nullptr, (tmpdir / path).native() + "/" + "test.h5", "test");

  fs::remove_all(tmpdir / path);
}
