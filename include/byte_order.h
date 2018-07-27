#ifndef ADD_H
#define ADD_H

#include <cstdint>
#include <sys/param.h>

constexpr bool is_little_endian()
{
    return __BYTE_ORDER == __LITTLE_ENDIAN;
}

template<bool little_endian = is_little_endian()>
class ByteOrderTemplate {
public:
    static uint32_t to_little_endian(uint32_t value)
    {
        return value;
    }
    static uint32_t from_little_endian(uint32_t value)
    {
        return value;
    }
};

template<>
class ByteOrderTemplate<false> {
public:
    static uint32_t to_little_endian(uint32_t value)
    {
        return reverse(value);
    }
    static uint32_t from_little_endian(uint32_t value)
    {
        return reverse(value);
    }
private:
    static uint32_t reverse(uint32_t value)
    {
        uint32_t result = 0;
        for(int i = 0; i < 4; ++i)
        {
            uint8_t byte = (value >> 8 * i) & UINT8_MAX;
            result |= byte << (24 - 8 * i);
        }
        return result;
    }
};

using ByteOrder = ByteOrderTemplate<>;

#endif // ADD_H
