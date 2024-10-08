#include <gtest/gtest.h>

#include "uuid.h"
#include <uuid_factory.h>

using namespace LambdaSnail::Uuid;

TEST(UuidOperations, CopiedUuid_ShouldBeEqual)
{
    // Arrange
    uuid a;
    factory::create_uuid_v4(a);
    uuid b = a;

    // Assert
    EXPECT_TRUE(a == b);
}

TEST(UuidOperations, NilAndMax_ShouldBeDifferent)
{
    EXPECT_FALSE(uuid::nil == uuid::max);
}


bool uuid_lt_reference(uuid const &a, uuid const &b)
{
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (a.octets[i] < b.octets[i]) return true;
    }

    return false;
}

TEST(UuidOperations, Nil_ShouldBeLessThanMax)
{
    EXPECT_TRUE(uuid::nil < uuid::max);
}

TEST(UuidOperations, Nil_ShouldBeLessThanOrEqualMax)
{
    EXPECT_TRUE(uuid::nil < uuid::max);
}

TEST(UuidOperations, AnyV4_ShouldBeLessThanMax)
{
    uuid v4;
    factory::create_uuid_v4(v4);

    EXPECT_TRUE(v4 < uuid::max);
}

TEST(UuidOperations, AnyV7_ShouldBeLessThanMax)
{
    uuid v7;
    factory::create_uuid_v7(v7);

    EXPECT_TRUE(v7 < uuid::max);
}

TEST(UuidOperations, TwoConsecutiveV7_FirstShouldBeLessThanLast)
{
    std::vector<uuid> uuids;
    factory::create_uuids_dedicated_counter(2, uuids);

    EXPECT_TRUE(uuids[0] < uuids[1]);
}