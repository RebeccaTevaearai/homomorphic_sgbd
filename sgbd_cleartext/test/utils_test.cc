#include "transpiler/examples/sgbd/utils.h"

#include "transpiler/data/tfhe_data.h"
#include "gtest/gtest.h"

TEST(TableTest, getNbRow_empty_table) {
    Table t = {
        "test",
        {},
    };

    int size = t.getNbRow();
    EXPECT_EQ(size, 0);
}

/*
TEST(TableTest, getColumnIndex_with_no_value) {
    Table t = {
        "test",
        {},
    };

    try {
        int index = t.getColumnIndex("column");
    } catch (std::exception &e) {

    }
    EXPECT_EQ(size, 0);
}
*/