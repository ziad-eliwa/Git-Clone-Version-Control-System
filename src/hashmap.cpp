#include "hashmap.h"
#include <cstdint>
#include <cstring>
#include <string>

uint32_t Murmur3_32_scramble(uint32_t k) {
  const uint32_t c1 = 0xCC9E2D51;
  const uint32_t c2 = 0x1B873593;
  const uint32_t r1 = 15;
  k *= c1;
  k = (k << r1) | (k >> (32 - r1));
  k *= c2;
  return k;
}
uint32_t Murmur3_32(const uint8_t *data, int length, uint32_t seed) {
  const uint32_t r2 = 13;
  const uint32_t m = 5;
  const uint32_t n = 0xE6546B64;

  uint32_t h = seed;
  uint32_t k;
  for (int i = 0; i + 4 <= length; i += 4) {
    k = (data[i]) | (data[i + 1] << 8) | (data[i + 2] << 16) |
        (data[i + 3] << 24); // endianness-independent

    h ^= Murmur3_32_scramble(k);
    h = (h << r2) | (h >> (32 - r2));
    h = (h * m) + n;
  }

  k = 0; // remaining bytes
  for (int i = length - 1; i >= length / 4 * 4; i--)
    k <<= 8, k |= data[i]; // little-endian

  h ^= Murmur3_32_scramble(k);

  h ^= length;
  h ^= h >> 16;
  h *= 0x85EBCA6B;
  h ^= h >> 13;
  h *= 0xC2B2AE35;
  h ^= h >> 16;
  return h;
}
uint32_t Murmur3_32(const char *data, uint32_t seed) { // null-terminated
  return Murmur3_32(reinterpret_cast<const uint8_t *>(data), std::strlen(data),
                    seed);
}
uint32_t Murmur3_32(const std::string &data, uint32_t seed) {
  return Murmur3_32(reinterpret_cast<const uint8_t *>(data.data()),
                    data.length(), seed);
}
