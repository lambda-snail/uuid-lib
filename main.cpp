#include <iostream>

#include "uuid.h"


int main()
{
    // using namespace std::chrono;
    // time_point<system_clock> now = std::chrono::system_clock::now();
    // uint64_t const time_stamp = now.time_since_epoch().count();

    //std::cout << now.time_since_epoch().count() << std::endl;

    // seed_state({ time_stamp, time_stamp });
    // long_jump();
    // next();

    using namespace LambdaSnail;

    // uuid g1 = uuid();
    auto g2 = LambdaSnail::guid();
    auto g3 = uuid<uuid_variant_v4<xoroshiro128pp>>();

    //guid e = guid::empty_guid;

    // auto a = reinterpret_cast<uint8_t*>(&g1);
    // auto b = reinterpret_cast<uint8_t*>(&g2);

    //std::cout << &g1 - &g2 << std::endl;

    // TODO: empty guid is not empty anymore ...
}
