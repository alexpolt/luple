/*
 * Author: Pavel I. Kryukov https://github.com/pavelkryukov
 * License: Public-domain software
 */

#include "luple.h"
#include "struct-reader.h"
#include "type-loophole.h"

#include <vector>

struct EmptyStruct {};

struct SimpleStructure
{
    int val;
    char key;
    short dum;
};

namespace struct_reader
{
    static_assert(std::is_same<as_type_list<EmptyStruct>, type_list<>>::value);
    static_assert(std::is_same<as_type_list<SimpleStructure>, type_list<int, char, short>>::value);
}

namespace loophole_ns
{
//    static_assert(std::is_same<as_type_list<EmptyStruct>, luple_ns::type_list<>>::value); Crashes VS
    static_assert(std::is_same<as_type_list<SimpleStructure>, luple_ns::type_list<int, char, short>>::value);
}

struct StructureWithVector
{
    int val;
    std::vector<int> storage;
};

namespace loophole_ns
{
    static_assert(std::is_same<as_type_list<StructureWithVector>, luple_ns::type_list<int, std::vector<int>>>::value);
}

int main()
{
    return 0;
}
