#include <gtest/gtest.h>

// #include "uuid.h"
// #include <uuid_factory.h>
//
// using namespace LambdaSnail::Uuid;
//
// TEST(UuidOperations, CopiedUuid_ShouldBeEqual)
// {
//     // Arrange
//     uuid a;
//     factory::create_uuid_v4(a);
//     uuid b = a;
//
//     // Assert
//     EXPECT_TRUE(a == b);
// }



int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}