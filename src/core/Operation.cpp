#include <core/Entry.hpp>
#include <core/Operation.hpp>
#include <cstddef>
#include <vector>

using buddy::core::Entry;
using buddy::core::BUDDY_IDENTIFIER_MASK;

auto buddy::core::startsWithBUDDYIdentifier(const std::vector<std::byte>& data)
    -> bool
{
    if(data.size() < 3) {
        return false;
    }

    return std::equal(std::begin(BUDDY_IDENTIFIER_MASK),
                      std::end(BUDDY_IDENTIFIER_MASK),
                      std::begin(data),
                      std::end(data) + 3);
}


