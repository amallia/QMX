#include <vector>
#include <array>

#include "QMX/qmx.hpp"


using namespace JASS;

void unittest_one(
    const std::vector<uint32_t> &sequence) {
  compress_integer_qmx_improved compressor;
  std::vector<uint32_t> compressed(sequence.size() * 2);
  std::vector<uint32_t> decompressed(sequence.size() + 256);

  auto size_once_compressed = compressor.encode(
      &compressed[0], compressed.size() * sizeof(compressed[0]), &sequence[0],
      sequence.size());
  compressor.decode(&decompressed[0], sequence.size(), &compressed[0],
                    size_once_compressed);
  decompressed.resize(sequence.size());
  assert(decompressed == sequence);
}

/*
    UNITTEST()
    -----------------------------------------
*/
void unittest(void) {
  /*
      Start with an example sequence of integers.
  */
  static const std::vector<uint32_t> sequence = {
      0x333,  0xC7,  0x21C, 0x78F,  0x66A,  0x787,  0xD0C, 0xEE,  0x416, 0x2F8,
      0x410,  0xFF3, 0x7A7, 0x35C,  0x5A8,  0x4ED,  0x3AD, 0x121, 0x3A7, 0x5EC,
      0x53,   0x50C, 0xFD6, 0x697,  0xF4,   0x894,  0xB5F, 0x381, 0x10C, 0xB1E,
      0x2E4,  0x32,  0x7EB, 0x1C6,  0x1DB,  0xE3,   0x27,  0x920, 0x262, 0x718,
      0x95,   0x7C0, 0x155, 0x8F,   0x83A,  0x1178, 0xCEF, 0x7DC, 0x3CB, 0x30E,
      0x2EA,  0x16F, 0x212, 0x4A,   0x9F0,  0x233,  0x7,   0x9F7, 0x1EE, 0x91,
      0x12FD, 0x7C,  0x291, 0x203,  0x2F8,  0x39B,  0x411, 0x61C, 0x3E2, 0x1DF,
      0xCD7,  0x5DA, 0xD35, 0x21,   0x1C8D, 0x25,   0x313, 0x314, 0xBBB, 0xFB,
      0x1E2,  0x60,  0x3F5, 0x513,  0x3AC,  0x769,  0x45E, 0x485, 0x1BA, 0x17B,
      0x2DC,  0x173, 0x151, 0x163E, 0x101,  0xE9D,  0xB67, 0x28B, 0x4CA, 0x955,
      0x6B3,  0x112, 0x225, 0x742,  0x432,  0x453,  0x3CF, 0x541, 0xCCE, 0xDB6,
      0x406,  0x58,  0x202, 0x647,  0x9F,   0x29,   0x153, 0x51E, 0x233, 0x7A3,
      0x731,  0x3A,  0xA0,  0xD23,  0x3C7,  0xD1,   0x5C,  0xB90, 0x22C, 0xE8,
      0x78B,  0x5E3};

  /*
      Allocate memory for the compressed version and the decompressed version
     (and initialise it)
  */
  static std::array<uint8_t, 100000 * sizeof(uint32_t)> compress_buffer_memory;
  static std::array<uint32_t, 100000> decompress_buffer_memory;
  compress_buffer_memory.fill(0);
  decompress_buffer_memory.fill(0);

  /*
      Allocatte a compresser
  */
  compress_integer_qmx_improved compressor;

  /*
      Make sure the place we're putting the compresed data is alligned to an odd
     address location, then compress.
  */
  uint8_t *compress_buffer = &compress_buffer_memory[1];
  size_t size_once_compressed =
      compressor.encode(compress_buffer, compress_buffer_memory.size() - 1,
                        &sequence[0], sequence.size());

  /*
      Shove a lode of 0's on the end of the buffer so that any overflow will
     result in failure.
  */
  compress_buffer[size_once_compressed] = 0;
  compress_buffer[size_once_compressed + 1] = 0;
  compress_buffer[size_once_compressed + 2] = 0;
  compress_buffer[size_once_compressed + 3] = 0;

  /*
      Make sure we're decompressing to an odd memory address, then decompress
     the compressed sequence
  */
  uint32_t *decompress_buffer =
      (uint32_t *)(((uint8_t *)&decompress_buffer_memory[0]) + 1);
  compressor.decode(decompress_buffer, sequence.size(), compress_buffer,
                    size_once_compressed);

  uint32_t pass;

  pass = true;
  for (uint32_t pos = 0; pos < sequence.size(); pos++)
    if (sequence[pos] != decompress_buffer[pos])
      pass = false; // LCOV_EXCL_LINE               // if this happens the the
                    // assert will fail.

  assert(pass);

  /*
      Test all valid instances that fit into one SIMD-word
  */
  std::vector<uint32_t> every_case;
  size_t instance;

  for (instance = 0; instance < 16 * 256; instance++)
    every_case.push_back(0);
  for (instance = 0; instance < 16 * 256; instance++)
    every_case.push_back(1);
  for (instance = 0; instance < 16 * 128; instance++)
    every_case.push_back(0x01);
  for (instance = 0; instance < 16 * 64; instance++)
    every_case.push_back(0x03);
  for (instance = 0; instance < 16 * 40; instance++)
    every_case.push_back(0x07);
  for (instance = 0; instance < 16 * 32; instance++)
    every_case.push_back(0x0F);
  for (instance = 0; instance < 16 * 24; instance++)
    every_case.push_back(0x1F);
  for (instance = 0; instance < 16 * 20; instance++)
    every_case.push_back(0x3F);
  for (instance = 0; instance < 16 * 16; instance++)
    every_case.push_back(0xFF);
  for (instance = 0; instance < 16 * 12; instance++)
    every_case.push_back(0x3FF);
  for (instance = 0; instance < 16 * 8; instance++)
    every_case.push_back(0xFFFF);
  for (instance = 0; instance < 16 * 4; instance++)
    every_case.push_back(0xFFFFFFFF);

  /*
      Test all valid instances that fit into two SIMD-words
  */
  for (instance = 0; instance < 16 * 36; instance++)
    every_case.push_back(0x7F);
  for (instance = 0; instance < 16 * 28; instance++)
    every_case.push_back(0x1FF);
  for (instance = 0; instance < 16 * 20; instance++)
    every_case.push_back(0xFFF);
  for (instance = 0; instance < 16 * 12; instance++)
    every_case.push_back(0x1FFFFF);

  std::vector<uint32_t> every_case_compressed(every_case.size());
  size_once_compressed = compressor.encode(&every_case_compressed[0],
                                           every_case_compressed.size() *
                                               sizeof(every_case_compressed[0]),
                                           &every_case[0], every_case.size());

  std::vector<uint32_t> every_case_decompressed(every_case.size());
  compressor.decode(&every_case_decompressed[0], every_case.size(),
                    &every_case_compressed[0], size_once_compressed);

  assert(every_case_decompressed == every_case);

  /*
      Check the end cases
  */
  /*
      15 * 8 bits
  */
  every_case.clear();
  for (instance = 0; instance < 15; instance++)
    every_case.push_back(0xFF);
  unittest_one(every_case);

  /*
      7 * 16 bits
  */
  every_case.clear();
  for (instance = 0; instance < 7; instance++)
    every_case.push_back(0xFFFF);
  unittest_one(every_case);

  /*
      7 * 8 bits
  */
  every_case.clear();
  for (instance = 0; instance < 7; instance++)
    every_case.push_back(0xFF);
  unittest_one(every_case);

  /*
      3 * 32 bits
  */
  every_case.clear();
  for (instance = 0; instance < 3; instance++)
    every_case.push_back(0xFFFFFFFF);
  unittest_one(every_case);

  /*
      3 * 16 bits
  */
  every_case.clear();
  for (instance = 0; instance < 3; instance++)
    every_case.push_back(0xFFFF);
  unittest_one(every_case);

  /*
      3 * 8 bits
  */
  every_case.clear();
  for (instance = 0; instance < 3; instance++)
    every_case.push_back(0xFF);
  unittest_one(every_case);

  /*
      Pathalogical case where everything must be promosted to the next block
     size
  */
  static const std::vector<uint32_t> pathalogical = {
      0X01,     0x01,     0x01,       0x01,       0x00,       0x00,
      0x00,     0x00,     0x03,       0x03,       0x03,       0x03,
      0x07,     0x07,     0x07,       0x07,       0x0F,       0x0F,
      0x0F,     0x0F,     0x1F,       0x1F,       0x1F,       0x1F,
      0x3F,     0x3F,     0x3F,       0x3F,       0x7F,       0x7F,
      0x7F,     0x7F,     0xFF,       0xFF,       0xFF,       0xFF,
      0x1FF,    0x1FF,    0x1FF,      0x1FF,      0x3FF,      0x3FF,
      0x3FF,    0x3FF,    0xFFF,      0xFFF,      0xFFF,      0xFFF,
      0xFFFF,   0xFFFF,   0xFFFF,     0xFFFF,     0x1FFFFF,   0x1FFFFF,
      0x1FFFFF, 0x1FFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
  unittest_one(pathalogical);

  /*
       Case to test the remaining lines of code
  */
  static const std::vector<uint32_t> remainder = {
      0x1f,   0x1f,   0x1f,   0x1f,   0x1f,   0x1f,   0x1f,   0x1f,
      0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFF};
  unittest_one(remainder);

  puts("PASSED");
}

int main(int argc, char const *argv[]) {
  unittest();
  return 0;
}
