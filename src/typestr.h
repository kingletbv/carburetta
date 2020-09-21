#ifndef TYPESTR_H
#define TYPESTR_H

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TYPESTR_TABLE_SIZE 127

struct typestr {
  struct typestr *hash_chain_;
  uint64_t hash_;

  /* Sequence of tokens that make up the type description, may
   * contain a single "$" TOK_SPECIAL_IDENT to denote the position
   * at which the identifier would appear. If no "$" placeholder
   * exists, one is implied to exist at the end of the snippet.
   * Note that a "$" placeholder may not explicitly exist at the
   * end of the snippet as this would remove the uniqueness of
   * a type definition. */
  struct snippet typestr_snippet_;
  
  int ordinal_;

  struct snippet constructor_snippet_;
  struct snippet destructor_snippet_;
};

struct typestr_table {
  struct typestr *hash_table_[TYPESTR_TABLE_SIZE];

  /* Array of pointers for all typestrs, index of array corresponds to typestr ordinal. */
  size_t num_typestrs_;
  size_t num_typestrs_allocated_;
  struct typestr **typestrs_; /* NOTE: Array of pointers, not objects, so we don't disturb the hash lists */
};

void typestr_table_init(struct typestr_table *tt);
void typestr_table_cleanup(struct typestr_table *tt);

struct typestr *typestr_find_or_add(struct typestr_table *tt, const struct snippet *typestr_snippet, int *is_new);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TYPESTR_H */
