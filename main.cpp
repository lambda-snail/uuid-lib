#include <iostream>

#include "uuid.h"
#include "uuid_spec.h"

int main()
{
    using namespace LambdaSnail::Uuid;

    uuid uuid1 = uuid(spec::g_uuid_v4_spec);
    // uuid uuid2 = uuid::nil;
    // uuid uuid3 = uuid::max;
    //
    // std::cout << uuid1.as_string() << std::endl;
    // std::cout << uuid2.as_string() << std::endl;
    // std::cout << uuid3.as_string() << std::endl;

    uuid v70 = uuid(spec::g_uuid_v7_spec);
    uuid v71 = uuid(spec::g_uuid_v7_spec);
    uuid v72 = uuid(spec::g_uuid_v7_spec);
    uuid v73 = uuid(spec::g_uuid_v7_spec);

    std::cout << v70.as_string() << std::endl;
    std::cout << v71.as_string() << std::endl;
    std::cout << v72.as_string() << std::endl;
    std::cout << v73.as_string() << std::endl;
}
