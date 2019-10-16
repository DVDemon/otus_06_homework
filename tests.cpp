#include "matrix.h"
#include <gtest/gtest.h>


TEST(test_empty, basic_test_set)
{
   homework::Matrix<int,-1> matrix;
   ASSERT_TRUE(matrix.size()==0);
}


TEST(test_get_value, basic_test_set)
{
   homework::Matrix<int,-1> matrix;
   ASSERT_TRUE(matrix[100][100]==-1);
}


TEST(test_set_value, basic_test_set)
{
   homework::Matrix<int,-1> matrix;
   matrix[100][100]=555;
   ASSERT_TRUE(matrix[100][100]==555);
}


TEST(test_size_after_set, basic_test_set)
{
   homework::Matrix<int,-1> matrix;
   matrix[100][100]=555;
   ASSERT_TRUE(matrix.size()==1);
}

TEST(test_tuple, basic_test_set)
{
   homework::Matrix<int,-1> matrix;
   matrix[100][100]=555;

   int a,b,c;
   std::tie(a,b,c) = matrix[100][100];

   ASSERT_TRUE((a==100)&&(b==100)&&(c==555));
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}