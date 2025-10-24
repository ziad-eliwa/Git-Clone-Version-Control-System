#include "vector.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(VectorTest, Constructors) {
  Vector<int> v1(10);
  EXPECT_GE(v1.capacity(), 10);
  EXPECT_EQ(v1.size(), 10);

  Vector<int> v2;
  EXPECT_EQ(v2.size(), 0);

  Vector<int> v3({10, 20});
  EXPECT_EQ(v3.size(), 2);
  EXPECT_EQ(v3[0], 10);
  EXPECT_EQ(v3[1], 20);
}

TEST(VectorTest, BracketOperator) {
  Vector<int> v(100);
  for (int i = 0; i < 10; i++)
    v[i] = i;

  for (int i = 0; i < 10; i++)
    EXPECT_EQ(v[i], i);
}

TEST(VectorTest, Push) {
  Vector<int> v;
  for (int i = 0; i < 100; i++)
    v.push_back(i);

  for (int i = 0; i < 100; i++)
    EXPECT_EQ(v[i], i);
}

TEST(VectorTest, Pop) {
  Vector<int> v(10);
  for (int i = 0; i < 10; i++)
    v[i] = i;
  for (int i = 0; i < 10; i++)
    EXPECT_EQ(v.pop_back(), 9 - i);
}

TEST(VectorTest, OStreamOperator) {
  Vector<int> v1({10, 20, 30});
  std::stringstream ss1;
  ss1 << v1;
  EXPECT_EQ(ss1.str(), "[10, 20, 30]");

  Vector<int> v2;
  std::stringstream ss2;
  ss2 << v2;
  EXPECT_EQ(ss2.str(), "[]");

  Vector<int> v3(1);
  std::stringstream ss3;
  ss3 << v3;
  EXPECT_EQ(ss3.str(), "[0]");
}
