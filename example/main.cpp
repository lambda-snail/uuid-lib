#include <iostream>

#include "uuid.h"
#include "uuid_factory.h"
#include "uuid_spec.h"

/**
 * Example that demonstrates how to work with uuid-lib.
 */
int main()
{
    using namespace LambdaSnail::Uuid;

    // Create a uuid v4 with the built-in spec
    uuid uuid1;
    factory::create_uuid_v4(uuid1);

    // uuid defines two constant UUIDs defined in the standard
    uuid empty = uuid::nil;
    uuid max   = uuid::max;

    // You can also define your own constant uuid if you want to
    uuid custom_constant = uuid(0x42);

    // uuid objects can be printed as std::strings
    std::cout << uuid1.as_string() << std::endl;
    std::cout << empty.as_string() << std::endl;
    std::cout << max.as_string() << std::endl;

    // The uuids can also be braced
    std::cout << uuid1.as_string_braced() << std::endl;

    // We can also create uuid v7 with a built-in spec
    uuid v7;
    factory::create_uuid_v7(v7);

    std::cout << v7.as_string() << std::endl;

    // However, if we create too many of the above uuids within a short period of time, we run into
    // problems if they get essentially the same time stamp.

    // To solve this problem, we can batch generate UUIDs with a fixed bit-length dedicated counter.
    // Using this method, up to 4096 uuids can be generated from the same timestamp.

    std::vector<uuid> uuids;
    factory::create_uuids_dedicated_counter(256, uuids);

    // Another way to solve the problem is to batch generate UUIDs with the monotonic random method.
    // This method sacrfices some of the randomness to allow up to 2^32 sequential uuids to be created
    // with the same time stamp.

    uuids.clear();
    factory::create_uuids_monotonic_random(10000, 4, uuids);

    // UUIDs can also be compared

    std::cout << "Equal:     " << (uuids[0] == uuids[1]) << std::endl;
    std::cout << "Less than: " << (uuids[0] < uuids[1]) << std::endl;

;}
