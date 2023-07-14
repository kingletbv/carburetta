#ifndef SITUS_H
#define SITUS_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h>
#endif

struct situs_span {
  /* Filename of source in which the span appears */
  const char *filename_;

  /* Start and end byte offsets in the original source in which the span appears */
  size_t start_, end_;

  /* Start line and column, both 1 based, of the span of text */
  int start_line_, start_col_;

  /* End line and column, both 1 based, of the span of text */
  int end_line_, end_col_;

  /* If non-zero, this is a substitution, num_bytes_ indicates the size of text the
   * original span of text has been substituted by. If false, num_bytes_ is always
   * equal to end_ - start_. */
  int is_substitution_ : 1;

  /* For a substitution, the range being substituted may consist of multiple spans
   * that are not adjacent. When that is the case, the first has a non-zero num_bytes_,
   * and the successive spans have num_bytes_ to 0 with is_aux_ non-zero. */
  int is_aux_ : 1;

  /* Number of bytes of text that this situs_span describes. */
  size_t num_bytes_;

  bool operator==(const situs_span &other) const noexcept {
    return (filename_ == other.filename_) &&
           (start_ == other.start_) &&
           (end_ == other.end_) &&
           (start_line_ == other.start_line_) &&
           (start_col_ == other.start_col_) &&
           (end_line_ == other.end_line_) &&
           (end_col_ == other.end_col_) &&
           (is_substitution_ == other.is_substitution_) &&
           (is_aux_ == other.is_aux_) &&
           (num_bytes_ == other.num_bytes_);
  }
};

struct situs_dynamic {
  size_t num_spans_allocated_;
  struct situs_span *spans_;
};

class Situs {
public:
  ~Situs() {
    if (num_spans_ > 1) {
      delete u_.many_.spans_;
    }
  }
  size_t num_spans_ = 0;
  union {
    struct situs_dynamic many_; /* dynamic array if num_spans_ > 1 for mutliple situs spans */
    struct situs_span one_;     /* static situs_span if num_spans_ == 1 (which is the likely case for normal sourcecode) */
  } u_;

  Situs &operator=(const Situs &other);
  Situs &operator=(Situs &&other) noexcept;

  void reset();

  int move_range(Situs *from, size_t byte_length, const char *text);
  void clone(const Situs *src);
  void clone_as_single_span_all_gaps_filled(const Situs *src);
  void concat(const Situs *tail);
  void skip(size_t byte_length, const char *text);
  size_t len();
  void convert_to_substitution(size_t num_bytes);

  /* Returns true if the situs is not empty, *start will contain the offset to the
   * first byte, *end will contain the offset after the last byte. This assumes that
   * the situs is in a contiguous range (otherwise individual spans should be examined.)
   * Returns false if the situs is empty; in which case *start and *end will both be
   * set to 0. */
  bool get_byte_range(size_t *start, size_t *end) const;

  /* Initializes situs to tbe at the point after src_after; the span contains no bytes,
   * (so the span's start_ and end_ are identical) but it may represent sourcecode text
   * (and hence num_bytes is non-zero). Useful for edge-case situations where code is
   * inserted by the compiler.
   * Note that the resulting span is always, by definition, a substitution. */
  void init_from_after(const Situs *src_after, size_t num_bytes);

  situs_span *first_span() { return num_spans_ > 1 ? u_.many_.spans_ : &u_.one_; }
  const situs_span *first_span() const { return num_spans_ > 1 ? u_.many_.spans_ : &u_.one_; }

  situs_span *last_span() { return num_spans_ > 1 ? &u_.many_.spans_[num_spans_ - 1] : &u_.one_; }
  const situs_span *last_span() const { return num_spans_ > 1 ? &u_.many_.spans_[num_spans_ - 1] : &u_.one_; }

  int line();
  int col();
  const char *filename();

protected:
  void chop(Situs *from, size_t byte_length, const char *text, size_t *chop_length);
  void glue(Situs *chop);
  static void find_offset(const char *bytes, size_t byte_size, int *num_lines, int *num_cols);
};


#endif /* SITUS_H */
