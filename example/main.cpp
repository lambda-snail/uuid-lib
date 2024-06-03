#include <iostream>

#include "uuid.h"
#include "uuid_spec.h"

/**
 * Example that demonstrates how to work with uuid-lib.
 */
int main()
{
    using namespace LambdaSnail::Uuid;

    // Create a uuid v4 with the built-in spec
    uuid uuid1 = uuid(spec::g_uuid_v4_spec);

    // uuid defines two constant UUIDs defined in the standard
    uuid empty = uuid::nil;
    uuid max   = uuid::max;

    // You can also define your own constant uuid if you want to
    uuid custom_constant = uuid(0x42);

    // uuid objects can be printed as std::strings
    std::cout << uuid1.as_string() << std::endl;
    std::cout << empty.as_string() << std::endl;
    std::cout << max.as_string() << std::endl;

    // We can also create uuid v7 with a built-in spec
    uuid v70 = uuid(spec::g_uuid_v7_spec);
    uuid v71 = uuid(spec::g_uuid_v7_spec);
    uuid v72 = uuid(spec::g_uuid_v7_spec);
    uuid v73 = uuid(spec::g_uuid_v7_spec);

    // These uuids should be consecutive
    std::cout << v70.as_string() << std::endl;
    std::cout << v71.as_string() << std::endl;
    std::cout << v72.as_string() << std::endl;
    std::cout << v73.as_string() << std::endl;
}
