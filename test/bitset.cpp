#include <gtest/gtest.h>

extern "C" {
    #include "ctools/bitset.h"
}

TEST(bitset, all_bits_work) {
    const unsigned int num_bits_to_test = 1 << 9;

    struct bitset bs;
    bitset_create(&bs, num_bits_to_test);

    for (unsigned int i = 0; i < num_bits_to_test; i++) {
        EXPECT_EQ(bitset_assign(&bs, i, 1), 0);
        EXPECT_NE(bitset_get(&bs, i), 0);
        EXPECT_EQ(bitset_assign(&bs, i, 0), 0);
        EXPECT_EQ(bitset_get(&bs, i), 0);
    }

    bitset_destroy(&bs);
}
