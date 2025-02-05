/* Copyright 2020-2025 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>

#ifndef UNICODE_DATA_INCLUDED
#define UNICODE_DATA_INCLUDED
#include "unicode_data.h"
#endif

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifdef __cplusplus
#ifndef STRING_INCLUDED
#define STRING_INCLUDED
#include <string>
#endif /* STRING_INCLUDED */
#endif /* __cplusplus */

/* Bring in annotation for printf format strings to aid debugging */
#if _MSC_VER >= 1400
#  ifndef SAL_H_INCLUDED
#    define SAL_H_INCLUDED
#    include <sal.h>
#  endif
#  if _MSC_VER > 1400
#    define KLT_FORMAT_STRING(x) _Printf_format_string_ x
#  else
#    define KLT_FORMAT_STRING(x) __format_string x
#  endif
#else
#  define KLT_FORMAT_STRING(x) x
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct linebuf {
  char *line;
  size_t line_size;
  size_t line_size_allocated;
  void *current_buffer;
  size_t current_buffer_size;
  size_t current_pos;
  int is_eof;
};

struct reentrant_packer {
  void *buffer_start;
  void *buffer_pos;
  void *buffer_end;


  /* Current struct packing position measures in bytes from the
   * last reentry checkpoint */
  size_t num_bytes_to_start_of_struct;

  /* Position of the next byte to process (eg. at buffer_pos) as measured
   * in bytes from the last reentry checkpoint. If this is greater than
   * num_bytes_to_start_of_struct, then we still have to skip bytes to get
   * to the current position in the struct packing sequence. If it is 
   * equal to num_bytes_to_start_of_struct, then we are actively 
   * processing bytes. */
  size_t num_bytes_to_next_byte_in_struct;

  /* The maximum number of bytes into the stream for which rep_is_first_entry()
   * was called. This is used to ensure rep_is_first_entry() returns true only
   * once per byte of stream on which it is called. */
  size_t num_deepest_pass;
};


/* Clears the reentrant_packer structure for use, marks the buffer as empty and
 * the current struct processing position as the reentrant checkpoint. */
void rep_init(struct reentrant_packer *rep);

/* Sets the new buffer, discards the old buffer; note that this does not copy
 * the buffer, but retains a pointer copy to it. */
void rep_set_buffer(struct reentrant_packer *rep, void *buffer, size_t num_bytes);

/* Returns the number of bytes processed - for rep_pack() sequences this is the
 * number of bytes written in the current buffer. For rep_unpack() sequences this
 * is the number of bytes read from the current buffer. */
size_t rep_num_bytes_processed(const struct reentrant_packer *rep);

/* Returns the number of bytes remaining - for rep_pack() sequences this is the
 * amount of space available in the buffer for writing until it is full. For 
 * rep_unpack() sequences this is amount of bytes available to still be read
 * until the buffer is exhausted. */
size_t rep_num_bytes_remaining(const struct reentrant_packer *rep);

/* Should rep_pack or rep_unpack return 0 (buffer full or exhausted) at some point
 * down the line, the caller will have to supply a new buffer and re-enter the 
 * packing code. A call to rep_reentry_checkpoint indicates that this is the (new) 
 * point at which the (un)packing sequence is re-entered, any skipping or processing 
 * of input is done from this point forward. */
void rep_reentry_checkpoint(struct reentrant_packer *rep);

/* Returns non-zero if the next byte in (un)packing struct sequence is an actual
 * byte to be processed, rather than to be skipped for having been processed before.
 * In other words, are we skipping, or has the next byte not yet been processed?
 * Knowing this is useful for one-time initialization of variables prior to 
 * (un)packing; on reentrant calls, such initialization of variables might overwrite
 * results that are determined later in the struct sequence that have already been
 * peformed on an earlier entry. When performing such one-time initialization only at
 * the first entry, then we know that we will not overwrite it later.
 * Note that there is an edge case: if the buffer is exhausted then 
 * rep_is_first_entry() will happily return true if the next byte in a subsequent
 * buffer is to be processed rather than skipped. Any subsequent (un)pack call
 * will however return buffer exhaustion. Reentry with a new buffer will then
 * invoke rep_is_first_entry() again and, given that the next byte is still to 
 * be processed immediately, it will return true again. Any 
 * "if (rep_is_first_entry())" branch can therefore be invoked more than once,
 * care should be taken that this does not result in memory leaks or logic errors.
 * In other words, such a branch guards against overwriting any subsequently
 * unpacked variables, it does not guard for one-time invocation. Any subsequent
 * code can however rely on the branch having executed _at least_ once.
 */
int rep_is_first_entry(struct reentrant_packer *rep);

/* If this returns non-zero, the buffer in rep is either exhausted (unpack) or
 * full (pack). The buffer should be refreshed (newly read(unpack) or written
 * out (pack)) and reset using rep_set_buffer(). The *next* invocation of
 * rep_(un)pack must be after re-entry just after the last reentry checkpoint.
 * You cannot rely on rep_(un)pack to continue to return non-zero as it resets
 * part of the serialization logic to start skipping from the reentry checkpoint
 * onwards on subsequent invocations.
 */
int rep_pack(struct reentrant_packer *rep, const char *format, ...);
int rep_unpack(struct reentrant_packer *rep, const char *format, ...);
int rep_unpack_va(struct reentrant_packer *rep, const char *format, va_list args_);

/* Reads bytes_to_read bytes from rep and stores them into buffer. Returns non-zero
 * if the buffer is exhausted and needs to be refreshed. Returns zero if the bytes
 * have succesfully been read (either on this entry or a prior entry).
 * It is permissable to pass a NULL pointer to buffer, in which case rep_read()
 * will skip bytes instead of reading them. */
int rep_read(struct reentrant_packer *rep, void *buffer, size_t bytes_to_read);

/* Like rep_read() except the destination is another reentrant_packer rather than
 * a buffer. Returns non-zero if the buffer is exhausted and needs to be refreshed.
 * Returns zero if the bytes have succesfully been read (either on this entry or a
 * prior entry.)
 * NOTE: If the buffer is exhausted (and it returns non-zero), the dst_rep might
 * likely still contain data to be processed *prior* to a new buffer being provided.
 * Similarly, if the bytes have succesfully been passed, you won't know if this
 * occurred on this entry or on a prior entry.
 * Consequently, irrespective of the return value, you'll always have to check for
 * any data in the dst_rep reentrant_packer and process it if it is there. Only
 * then should you act on the return value of rep_nest().
 * NOTE: This function copies pointers to the buffers set in src_rep, consequently,
 *       both src_rep and dst_rep will use the same buffers. This means you must
 *       first completely use the data in dst_rep before refreshing the buffer in
 *       in src_rep. */
int rep_nest(struct reentrant_packer *dst_rep, struct reentrant_packer *src_rep, size_t bytes_to_nest);

/* Initializes the linebuf */
void linebuf_init(struct linebuf *lb);

/* Cleans / frees memory associated with the linebuf */
void linebuf_cleanup(struct linebuf *lb);

/* Seeks the input for the next line, a line is any
 * sequence of bytes ending with a newline, or the end
 * of the input; whichever comes first. Note that null
 * terminators ('\0') in the input are seen as neither
 * newline nor end of input, and will just be part of
 * the line found.
 * Returns 1 upon success (and the line can be accessed
 * with linebuf_current_line()).
 * Returns 0 when the input from the current buffer has
 * been exhausted and new input should be supplied using
 * linebuf_set_input().
 * Returns -1 if the linebuf fails to allocate memory. */
int linebuf_next_line(struct linebuf *lb);

/* Sets the current data buffer, replacing any prior buffer, sets
 * the current position to the start of the new databuffer.
 * NOTE: This does not *copy* the buffer, it should remain accessible
 * while reading from it. */
void linebuf_set_input(struct linebuf *lb, void *buf, size_t bufsize);

/* Sets the end of the input; the end of the current buffer will
 * be seen as the end of a line. */
void linebuf_finish(struct linebuf *lb);

/* True if the current buffer has been depleted and the end of input
 * was set. */
int linebuf_is_finished(const struct linebuf *lb);

/* Returns a pointer into the current buffer at the current input
 * offset, and increments the current offset with size_requested bytes,
 * or to the end of the buffer (whichever comes first.) */
size_t linebuf_read_bytes(struct linebuf *lb, void **buf, size_t size_requested);

/* Returns a pointer to the linebuffer which, if linebuf_next_line() returned
 * 1, will contain a synthetically added null-terminated string of the last line found. */
char *linebuf_current_line(struct linebuf *lb);

/* Returns the size of the current line excluding the synthetically added
 * null terminator. */
size_t linebuf_current_line_size(const struct linebuf *lb);

/* Array version of realloc, allocates count * size, will fail allocation by
 * returning NULL if the underlying malloc fails, or if count * size causes
 * an overflow for the size_t type. Does not zero the memory.
 * Free the memory by using clib free() or by arealloc'ing with 0 count*size. */
void *arealloc(void *mem, size_t count, size_t size);

/* Win32 TCHAR version of memory allocated sprintf; takes the format string and
 * returns a freshly allocated TCHAR string sprintf'ed from the format string and
 * the arguments. Free the memory using clib free(). */
#ifdef _TCHAR_DEFINED
TCHAR *tmasprintf(KLT_FORMAT_STRING(const TCHAR *format), ...);
#endif

/* Ordinary single byte char version of a memory allocated sprintf; takes the
 * format string and a variable number of arguments, and freshly allocates a
 * sufficiently large string and fills it with the formatted result. Free the
 * string using clib free(). */
char *masprintf(KLT_FORMAT_STRING(const char *format), ...);

/* Ordinary single byte char version of a memory allocated vsprintf; takes the
 * format string and a va_list of arguments, and freshly allocates a sufficiently
 * large string and fills it with the formatted result. Free the string using
 * clib free(). */
char *vmasprintf(const char *format, va_list args);

/* Debug dumps the binary data at pointer data of size datasize using a readable
 * binary dump format; formats it as if data starts at offset baseoffset. */
char *bindumpf(const void *data, size_t datasize, size_t baseoffset);

/* Debug dumps the binary data at pointer data of size datasize using a C-source
 * code format such that it may be used as an array initializer. */
char *bindumpc(const void *data, size_t datasize, size_t baseoffset);

/* Performs a 64 bit times 64 bit unsigned multiplication, resulting in a 128
 * bit result, split out over a high 64 bit value and a low 64 bit value.
 * If rhi is not NULL, it will receive the upper 64 bits, if rlo is not NULL,
 * it will receive the lower 64 bits.
 * Returns non-zero if the upper 64 bits are non-zero, and zero otherwise,
 * consequently you can use the return value to see if the result would fit
 * in 64 bits and only pass a pointer to rlo to get at the result. */
int mul64(uint64_t a, uint64_t b, uint64_t *rhi, uint64_t *rlo);

/* Similar to mul64 but for size_t, which may vary to 32 bits or 64 bits depending on platform */
int mul_size_t(size_t a, size_t b, size_t *rhi, size_t *rlo);

/* Performs an unsigned division of a 128 bit numerator by a 64 bit denominator, returning 
 * a 128 bit number, split into a high and a low 64 bit number.
 * Return value == -1 for division by zero.
 *                  1 if the result requires more than 64 bits
 *                  0 if the result requires less than 64 bits.
 * *rhi is optional, you can check the return value for 1 to see if the result fits into *rlo.
 */
int div128by64(uint64_t numhi, uint64_t numlo, uint64_t den, uint64_t *rhi, uint64_t *rlo);

/* Packs arguments together in a destination buffer with no padding in little-endian
 * format.
 * buffer_size contains size of the buffer allocated, the function will not write
 * beyond this size.
 * buffer is the buffer where encoded arguments will be written.
 * format is a format string which is a list of letters corresponding to arguments to be
 * encoded. Each argument is specified by encoding type:
 * 'b' : an 8-bit byte.
 * 'w' : a 16-bit word.
 * 'd' : a 32-bit double-word.
 * 'q' : a 64-bit quad-word.
 * format is followed by the arguments to be encoded, in order of the format string.
 * Return value is the number of bytes needed to write the format string, if this value
 * is less than or equal to buffer_size, those bytes were written. It is therefore possible
 * to use the return value to determine the size of the buffer required, as passing in 0
 * for buffer_size ensures the buffer pointer will remain unaffected (and can be NULL.)
 */
size_t pack_le(size_t buffer_size, char *buffer, const char *format, ...);

/* Unpacks arguments from a binary input buffer from little-endian format to whatever
 * the platform's argument format is.
 * buffer_size contains the size of the input buffer, the function will not read beyond
 * this size. If data beyond this size is needed, 0 is returned.
 * buffer is the buffer from which bytes are decoded into arguments.
 * format is a format string which is a list of letters corresponding to arguments to be
 * decoded. Each argument is specified by encoding type:
 * 'b' : a pointer to an 8-bit byte.
 * 'w' : a pointer to a 16-bit word.
 * 'd' : a pointer to a 32-bit double-word.
 * 'q' : a pointer to a 64-bit quad-word.
 * format is followed by a series of pointers where the decoded values will be stored.
 * Return value is the number of bytes read (eg. the decoded size of the format string,)
 * or 0 if the input buffer was insufficiently large to read the entire format string.
 * If 0 is returned, the state of the argument values to which pointers were passed is
 * undefined.
 */
size_t unpack_le(size_t buffer_size, const char *buffer, const char *format, ...);

/* population_count64(x) - counts the number of bits that are set in the 64 bit value x */
#if defined(_MSC_VER) && defined(_WIN64)
#define population_count64(x) ((int)__popcnt64(x))
#elif defined(_MSC_VER) && !defined(_WINX64)
#define population_count64(x) ((int)__popcnt((unsigned int)x) + __popcnt((unsigned int)(x >> 32)))
#elif defined(__GNUC__)
#define population_count64(x) __builtin_popcntll(x)
#else
static int population_count64(uint64_t v) {
  v = ((v >> 1) & 0x5555555555555555ULL) + (v & 0x5555555555555555ULL);
  v = ((v >> 2) & 0x3333333333333333ULL) + (v & 0x3333333333333333ULL);
  v = ((v >> 4) & 0x0F0F0F0F0F0F0F0FULL) + (v & 0x0F0F0F0F0F0F0F0FULL);
  v = ((v >> 8) & 0x00FF00FF00FF00FFULL) + (v & 0x00FF00FF00FF00FFULL);
  v = ((v >> 16) & 0x0000FFFF0000FFFFULL) + (v & 0x0000FFFF0000FFFFULL);
  v = ((v >> 32) & 0x00000000FFFFFFFFULL) + (v & 0x00000000FFFFFFFFULL);
  return (int)v;
}
#endif

/* leading_zero_count64 - counts the number of bits, from bit 63 on down, that are zero
 * until a bit is found that is set. If bit 63 is set, this returns 0, if the entire quad
 * is zero, this returns 64. */
static int leading_zero_count64(uint64_t v) {
  int lz = 0;
  if (!(v & 0xFFFFFFFF00000000ULL)) {
    lz += 32;
    v <<= 32;
  }
  if (!(v & 0xFFFF000000000000ULL)) {
    lz += 16;
    v <<= 16;
  }
  if (!(v & 0xFF00000000000000ULL)) {
    lz += 8;
    v <<= 8;
  }
  if (!(v & 0xF000000000000000ULL)) {
    lz += 4;
    v <<= 4;
  }
  if (!(v & 0xC000000000000000ULL)) {
    lz += 2;
    v <<= 2;
  }
  if (!(v & 0x8000000000000000ULL)) {
    lz += 1;
    v <<= 1;
  }
  if (!v) {
    lz++;
  }
  return lz;
}

/* Convert UTF-16 to UTF-8, illegal unicode characters will be converted to the replacement character FFFD.
 * src_size indicates the number of uint16_t's in the src array, if it is ~(size_t)0 then src is assumed
 * to be null-terminated.
 * The resulting UTF-8 string is stored in buffer; if the number of UTF-8 bytes exceeds buffer_size, the
 * result will be truncated (buffer is never accessed at index buffer_size and beyond.)
 * The function returns the size, in bytes, of the UTF-8 string. If the result in buffer was truncated,
 * the function will still return the full size of the buffer needed. It is therefore valid to pass in
 * 0 for buffer_size and NULL for buffer to measure the size of the buffer needed.
 * If src_size was ~(size_t)0 then the output buffer will be null-terminated (buffer size permitting.) */
size_t convert_utf16_to_utf8(size_t buffer_size, char *buffer, const uint16_t *src, size_t src_size);
size_t convert_utf8_to_utf16(size_t buffer_size, uint16_t *buffer, const char *src, size_t src_size);

/* Convert between Win32 TCHAR and UTF-8 char strings; acts as
 * a strdup'ping no-op if TCHAR is defined as char. The strings
 * returned should be freed by caller using free(). */
#ifdef _TCHAR_DEFINED
char *tstr2acstr(const TCHAR *tsrc);
TCHAR *cstr2atstr(const char *csrc);
TCHAR *wstr2atstr(const wchar_t *wsrc);
#endif
wchar_t *cstr2awstr(const char *csrc);
char *wstr2acstr(const wchar_t *wsrc);

/* Returns a string to describe the error code; the string should be freed using free() */
#ifdef _HRESULT_DEFINED
char *hr2cstr(HRESULT hr);
#endif

/* Iterates all files in the directory specified by directory_path, note that
 * it skips any nested directories.
 * Call dir_enum_begin to start; it should always return a non-NULL void* to be
 * used as handle (if not, NULL is returned as there is a memory allocation failure 
 * on a small amount of memory.)
 * To discover if dir_enum_begin() succeeded, call dir_enum_get_error(), if this 
 * returns a non-NULL descriptive error string; an error has occurred.
 * Repeated invocations to dir_enum_next() with the earlier returned handle will
 * return full absolute paths to the files found (i.e. including the directory
 * it is in, and absolute in the sense that it is not relative to the current 
 * directory.)
 * After the last file has been returned, dir_enum_next() will return NULL.
 * To discover if this is an error or just the end of the files in the directory,
 * call dir_enum_get_error() (where a NULL return indicates the end of the files
 * in the directory..)
 * The void* handle returned by dir_enum_begin() should always be free'd by calling
 * dir_enum_end() ; irrespective of any failures.
 * dir_enum_begin2() permits passing a "want_directories" flag which, if set, also
 * causes directories in the directory_path to be returned. */
void *dir_enum_begin(const char *directory_path);
void *dir_enum_begin2(const char *directory_path, int want_directories);
const char *dir_enum_next(void *dir_enum_handle);
const char *dir_enum_next2(void *dir_enum_handle, int *is_directory);
void dir_enum_end(void *dir_enum_handle);

/* Returns the full (absolute) path of the directory being enumerated */
const char *dir_enum_full_path(void *dir_enum_handle);

const char *dir_enum_get_error(void *dir_enum_handle);

int dir_is_dir(const char *possible_directory_path);

/* Create directory and any necessary parent directories. 
 * Returns zero upon success, non-zero upon failure. Currently failure is logged
 * to stderr (which is bad, obv.) */
int make_dirp(const char *directory_path);

/* Returns non-zero if the unicode codepoint is an uppercase letter
 * (unicode category "Lu") */
#define is_lu(cp) (((cp) >= 0) && (cp < 0x110000) && ((unicode_precise_category_map_lookup[(cp)/256] && ((unicode_precise_category_map_lookup[(cp)/256])[(cp) & 0xFF] == UCCG_LU)) || (unicode_granular_category_map[(cp)/256] == UCCG_LU)))

/* Decodes the UTF8 codepoint sequence pointed to by the pointer stored at pputf8 and
 * increments the pointer stored at pputf8 to the start of the next codepoint sequence.
 * Returns the codepoint. */
int get_next_utf8_codepoint(const char **pputf8);

/* Encodes the codepoint as UTF8 writing it to dst onwwards; returns the number of bytes
 * used to encode the codepoint */
int codepoint_as_utf8(char *dst, uint32_t codepoint);

/* Converts from the number of seconds since january 1st 0000 to a normal calendar
 * representation: years since 0000, month (1..12), day (1..31), hour (0..23), 
 * minutes (0..59) and seconds (0..59). */
uint64_t seconds_since_0000_from_yyyymmddhhmmss(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second);

/* Converts from the normal calendar representation (years since 0000, month (1..12), 
 * day (1..31), hour (0..23), minutes (0..59) and seconds (0..59)) to the number of
 * seconds since 0000. */
void yyyymmddhhmmss_from_seconds_since_0000(uint32_t *year, uint32_t *month, uint32_t *day, uint32_t *hour, uint32_t *minute, uint32_t *second, uint64_t seconds_since_0000);

/* Reads a line of password from stdin, ensuring that the terminal does not echo back
 * the characters. Password ends on end of file or newline. The password string returned
 * can be of arbitrary length and should be freed using free().
 * If a failure occurs, NULL is returned. */
char *read_password_line(void);

/* The binary data pointed to by bin_bytes of length num_bin_bytes is converted to
 * a base64 encoded version which is written to str. To discover how large the
 * buffer at str should be, use base64_len_as_b64() */
void base64_bin_to_b64(size_t num_bin_bytes, const uint8_t *bin_bytes, char *str);

/* The base64 data pointed to by base64_str of length base64_str_length is converted
 * to binary data which is written to the buffer at bin_bytes. To discover how large
 * the buffer at bin_bytes should be, use base64_len_as_bin() */
int base64_b64_to_bin(size_t base64_str_length, const char *base64_str, uint8_t *bin_bytes);

/* Determines the size of the binary buffer needed to contain the binary data encoded
 * by the base64 data at base64_str of length base64_str_length. The actual data is 
 * needed because trailing padding '=' digits in the base64 stream may influence the 
 * actual size of the data.
 * The size excludes any consideration of null terminators. */
size_t base64_len_as_bin(size_t base64_str_length, const char *base64_str);

/* Determines the size of the base64 encoded representation of num_bin_bytes bytes of
 * binary data. This size accommodates '=' padding to the next multiple of 4. */
size_t base64_len_as_b64(size_t num_bin_bytes);

/* Retrieves strong pseudo-random bytes and stores them in buf. Returns zero upon
 * success, non-zero upon failure. */
int get_random_bytes(void *buf, size_t num_bytes);

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifdef __cplusplus
  /* C++ version of masprintf, returns an std::string; we're inlining this so
  * we don't introduce a .cpp file in helpers */
std::string mstrprintf(KLT_FORMAT_STRING(const char *format), ...);
#ifdef _TCHAR_DEFINED
std::string tstr2str(const TCHAR *tsrc);
TCHAR *str2tstr(const std::string &str);
struct freeing_tstr {
  freeing_tstr() {}
  freeing_tstr(freeing_tstr &&rv) {
    s = rv.s;
    rv.s = nullptr;
  }
  ~freeing_tstr() {
    if (s) free(s);
  }
  operator TCHAR *() {
    return s;
  }
  operator const TCHAR *() {
    return s;
  }
  TCHAR *s = nullptr;
};
// Auto-free on out of scope, especially useful in expressions as
// the value will clean up after itself. Also dangerous if the pointer
// survives the expression.
// From spec:
//   Temporary objects are destroyed as the last step in evaluating the 
//   full-expression (1.9) that (lexically) contains the point where they 
//   were created. [12.2/3]
freeing_tstr str2ftstr(const std::string &ftstr);
#endif /* _TCHAR_DEFINED */
#endif /* __cplusplus */


#endif /* HELPERS_H */
