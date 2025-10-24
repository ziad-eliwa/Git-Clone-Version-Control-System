#include "hashmap.h"
#include <gtest/gtest.h>

TEST(HashMapTest, Hash) {
  EXPECT_EQ(Murmur3_32("", 0x00000000), 0x00000000);
  EXPECT_EQ(Murmur3_32("", 0x00000001), 0x514E28B7);
  EXPECT_EQ(Murmur3_32("", 0xFFFFFFFF), 0x81F16F39);
  EXPECT_EQ(Murmur3_32("test"), 0xBA6BD213);
  EXPECT_EQ(Murmur3_32("test", 0x00000000), 0xBA6BD213);
  EXPECT_EQ(Murmur3_32("test", 0x9747B28C), 0x704B81DC);
  EXPECT_EQ(Murmur3_32("Hello, world!", 0x00000000), 0xC0363E43);
  EXPECT_EQ(Murmur3_32("Hello, world!", 0x9747B28C), 0x24884CBA);
  EXPECT_EQ(
      Murmur3_32("The quick brown fox jumps over the lazy dog", 0x00000000),
      0x2E4FF723);
  EXPECT_EQ(
      Murmur3_32("The quick brown fox jumps over the lazy dog", 0x9747B28C),
      0x2fa826cd);
}

TEST(HashMapTest, HashMap) {
  HashMap<int, int> hm;
  for (int i = 100; i <= 1000; i += 100) {
    hm[i] = i;
    hm[i] = i;
  }
  EXPECT_EQ(hm.size(), 10);

  for (int i = 100; i <= 1000; i += 100)
    EXPECT_EQ(hm[i], i);
}

TEST(HashMapTest, LoadFactor) {
  HashMap<int, int> hm;
  for (int i = 0; i < 10000; i++) {
    hm[i] = i;
    EXPECT_LE(hm.load_factor(), hm.MAX_LOAD_FACTOR);
  }
  for (int i = 0; i < 10000; i++)
    EXPECT_EQ(hm[i], i);
}
