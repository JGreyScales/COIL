#pragma once
#include <cstdint>

namespace BitUtils {
    /**
     * Reverses the bits in an 8-bit byte.
     * Transforms 00001010 (10) into 01010000 (80)
     */
    inline uint8_t ReverseByte(uint8_t b) {
        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
        return b;
    }
}