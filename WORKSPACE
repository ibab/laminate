
git_repository(
  name = "protobuf",
  remote = "https://github.com/google/protobuf",
  commit = "ed87c1fe2c6e1633cadb62cf54b2723b2b25c280",
)

new_git_repository(
  name = "blosc",
  remote = "https://github.com/Blosc/c-blosc",
  commit = "f51ea6b7cd32d87aa234aa426d53283132498ef8",
  build_file = "blosc.BUILD",
)

new_http_archive(
  name = "zlib",
  url = "http://zlib.net/zlib-1.2.8.tar.gz",
  sha256 = "36658cb768a54c1d4dec43c3116c27ed893e88b02ecfcb44f2166f9c0b7f2a0d",
  build_file = "zlib.BUILD",
)

new_git_repository(
    name = "gtest",
    remote = "https://github.com/google/googletest.git",
    commit = "ec44c6c1675c25b9827aacd08c02433cccde7780",
    build_file = "gtest.BUILD",
)

new_http_archive(
    name = "hdf5",
    url = "http://www.hdfgroup.org/ftp/HDF5/current/src/hdf5-1.8.17.tar.gz",
    sha256 = "d9cda297ee76ade9881c4208987939250d397bae6252d0ccb66fa7d24d67e263",
    strip_prefix = "hdf5-1.8.17",
    build_file = "hdf5.BUILD",
)
