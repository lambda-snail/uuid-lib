#include <iostream>

#include "uuid.h"
#include "uuid_spec.h"

int main()
{
    using namespace LambdaSnail::Uuid;

    uuid uuid1 = uuid(spec::g_uuid_v4_spec);
    uuid uuid2 = uuid::nil;
    uuid uuid3 = uuid::max;

    std::cout << uuid1.as_string() << std::endl;
    std::cout << uuid2.as_string() << std::endl;
    std::cout << uuid3.as_string() << std::endl;
}
