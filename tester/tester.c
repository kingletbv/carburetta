#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define enum_tests \
xx(t1, "Calculator test") \
xx(t2, "Matching braces") \
xx(t3, "Dot Anychar Latin-1") \
xx(t4, "Dot Anychar UTF-8") \
xx(t5, "Overlapping unicode UTF-8 ranges") \
xx(t6, "Character set unicode category \\p{}") \
xx(t7, "Character set unicode category inversion \\P{}")

#define xx(id, desc) int id(void);
enum_tests
#undef xx

int main(int argc, char **argv) {
  int r;
  int failed = 0;
  int num_tests_passed = 0;
  int total_num_tests = 0;
#define xx(id, d) total_num_tests++;
  enum_tests
#undef xx
#define xx(id, desc) \
  r = id(); \
  if (r) { \
    fprintf(stderr, "%s %s: failed test (%d)\n", #id, desc, r); \
    failed = 1; \
  } \
  else { \
    num_tests_passed++; \
  }
  enum_tests
#undef xx

  if (failed) return EXIT_FAILURE;

  if (num_tests_passed == total_num_tests) {
    printf("%d/%d tests passed\n", num_tests_passed, total_num_tests);
  }
  else {
    fprintf(failed ? stderr : stdout, "%s%d/%d tests passed\n", failed ? "FAILED: " : "", num_tests_passed, total_num_tests);
  }

  return EXIT_SUCCESS;
}

int codepoint_as_utf8(char *dst, uint32_t codepoint) {
  uint8_t *dst_utf8 = (uint8_t *)dst;
  if (codepoint <= 0x7F) {
    *dst_utf8++ = (uint8_t)codepoint;
  }
  else if (codepoint <= 0x7FF) {
    *dst_utf8++ = (uint8_t)((codepoint & 0x7C0) >> 6) | 0xC0;
    *dst_utf8++ = (uint8_t)(codepoint & 0x3F) | 0x80;
  }
  else if (codepoint <= 0xFFFF) {
    *dst_utf8++ = (uint8_t)((codepoint & 0xF000) >> 12) | 0xE0;
    *dst_utf8++ = (uint8_t)((codepoint & 0x0FC0) >> 6) | 0x80;
    *dst_utf8++ = (uint8_t)(codepoint & 0x003F) | 0x80;
  }
  else if (codepoint <= 0x10FFFF) {
    *dst_utf8++ = (uint8_t)((codepoint & 0x1C0000) >> 18) | 0xF0;
    *dst_utf8++ = (uint8_t)((codepoint & 0x03F000) >> 12) | 0x80;
    *dst_utf8++ = (uint8_t)((codepoint & 0x000FC0) >> 6) | 0x80;
    *dst_utf8++ = (uint8_t)(codepoint & 0x00003F) | 0x80;
  }
  else {
    // Invalid codepoint
  }
  return (int)(((char *)dst_utf8) - dst);
}

int get_next_utf8_codepoint(const char **pputf8) {
  uint8_t *utf8 = (uint8_t *)*pputf8;
  if ((*utf8) < 0x80) {
    *pputf8 += 1;
    return utf8[0];
  }
  if ((*utf8) < 0xC0) {
    /* 0x80 - 0xBF - in the middle of a sequence, not at its start.. */
    *pputf8 += 1;
    return 0xFFFD /* REPLACEMENT CHAR */;
  }
  if ((*utf8) < 0xE0) {
    if ((utf8[1] & 0xC0) == 0x80) {
      int codepoint = (((int)utf8[0] & 0x1F) << 6) | ((int)(utf8[1]) & 0x3F);
      *pputf8 += 2;
      return codepoint;
    }
    else {
      /* Second byte of sequence is not 10xxxxxx binary */
      *pputf8 += 1;
      return 0xFFFD /* REPLACEMENT CHAR */;
    }
  }
  if ((*utf8) < 0xF0) {
    if (((utf8[1] & 0xC0) == 0x80) &&
        ((utf8[2] & 0xC0) == 0x80)) {
      int codepoint = (((int)utf8[0] & 0x0F) << 12) | (((int)utf8[1] & 0x3F) << 6) | ((int)(utf8[2]) & 0x3F);
      *pputf8 += 3;
      return codepoint;
    }
    else {
      /* Either second or third byte of sequence is not 10xxxxxx binary */
      *pputf8 += 1;
      return 0xFFFD /* REPLACEMENT CHAR */;
    }
  }
  if ((*utf8) < 0xF8) {
    if (((utf8[1] & 0xC0) == 0x80) &&
        ((utf8[2] & 0xC0) == 0x80) &&
        ((utf8[3] & 0xC0) == 0x80)) {
      int codepoint = (((int)utf8[0] & 0x07) << 18) | (((int)utf8[1] & 0x3F) << 12) | (((int)utf8[2] & 0x3F) << 6) | ((int)(utf8[3]) & 0x3F);
      *pputf8 += 4;
      return codepoint;
    }
    else {
      /* Either second or third byte of sequence is not 10xxxxxx binary */
      *pputf8 += 1;
      return 0xFFFD /* REPLACEMENT CHAR */;
    }
  }
  /* Incorrect byte for leading a sequence */
  *pputf8 += 1;
  return 0xFFFD /* REPLACEMENT CHAR */;
}
