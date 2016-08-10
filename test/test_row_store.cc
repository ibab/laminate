
#include <gtest/gtest.h>

#include "row_store.h"

TEST(RowStoreTest, Creation) {
  laminate::RowStore rs("test.rs", "w");
}
