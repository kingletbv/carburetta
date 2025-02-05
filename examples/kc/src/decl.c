/* Copyright 2023-2025 Kinglet B.V.
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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef NAME_SPACE_H_INCLUDED
#define NAME_SPACE_H_INCLUDED
#include "name_space.h"
#endif

#ifndef STMT_H_INCLUDED
#define STMT_H_INCLUDED
#include "stmt.h"
#endif

#ifndef SYMTAB_H_INCLUDED
#define SYMTAB_H_INCLUDED
#include "symtab.h"
#endif

#ifndef DECL_H_INCLUDED
#define DECL_H_INCLUDED
#include "decl.h"
#endif

#ifndef DYNAMIC_RUNTIME_LINKING_H_INCLUDED
#define DYNAMIC_RUNTIME_LINKING_H_INCLUDED
#include "dynamic_runtime_linking.h"
#endif

struct decl *decl_alloc_sized(struct name_space *ns, int *already_exists, const char *ident, struct situs *ident_loc, 
                              sc_storage_class_t sc, int function_specifiers, struct type_node *tn, struct decl_initializer *init,
                              size_t full_decl_size) {
  int is_new = -1;
  
  struct decl *d = (struct decl *)st_find_or_insert(&ns->ordinary_idents_, ident, &is_new, full_decl_size);
  if (!is_new) {
    *already_exists = 1;
    return d;
  }
  *already_exists = 0;
  d->sc_ = sc;
  d->fs_ = function_specifiers;
  d->chain_ = d;
  situs_init(&d->def_loc_);
  if (situs_clone(&d->def_loc_, ident_loc) < 0) {
    st_remove(&ns->ordinary_idents_, &d->sym_);
    free(d);
    return NULL;
  }
  d->global_forward_ = NULL;
  d->type_ = tn;
  d->init_ = init;
  d->dsp_ = NULL;
  d->external_ = NULL;
  d->is_external_ = 0;
  d->is_internal_ = 0;
  d->is_tentative_definition_ = 0;
  d->is_definition_ = 0;
  d->is_declaration_ = 0;
  d->is_typedef_ = 0;
  d->is_param_ = 0;
  d->is_hidden_global_ = 0;
  d->is_enum_ = 0;
  d->is_local_ = 0;
  d->is_passed_by_pointer_ = 0;
  d->param_offset_ = 0;
  d->local_offset_ = 0;
  d->enum_value_ = 0;
  d->disambiguation_ordinal_ = 0;
  d->num_refs_ = d->num_refs_allocated_ = 0;
  d->refs_ = NULL;
  return d;
}

struct decl *decl_alloc(struct name_space *ns, int *already_exists, const char *ident, struct situs *ident_loc, 
                        sc_storage_class_t sc, int function_specifiers, struct type_node *tn, struct decl_initializer *init) {
  return decl_alloc_sized(ns, already_exists, ident, ident_loc, sc, function_specifiers, tn, init, sizeof(struct decl));
}

void decl_free(struct symtab *st, struct decl *d) {
  if (st) {
    st_remove(st, &d->sym_);
  }
  decl_initializer_free(d->init_);
  situs_cleanup(&d->def_loc_);
  if (d->refs_) free(d->refs_);
}

struct decl_ref *decl_add_ref(struct decl *d, sc_relocation_type_t reloc_type, struct ds_portion *dsp, size_t offset) {
  if (d->num_refs_ >= d->num_refs_allocated_) {
    size_t new_num_refs_allocated = d->num_refs_allocated_ ? d->num_refs_allocated_ * 2 : 4;
    struct decl_ref *new_refs = (struct decl_ref *)realloc(d->refs_, new_num_refs_allocated * sizeof(struct decl_ref));
    if (!new_refs) return NULL;
    d->refs_ = new_refs;
    d->num_refs_allocated_ = new_num_refs_allocated;
  }
  struct decl_ref *dr = d->refs_ + d->num_refs_++;
  dr->rt_ = reloc_type;
  dr->dsp_ = dsp;
  dr->offset_ = offset;
  return dr;
}

struct decl_initializer *decl_initializer_alloc(void) {
  struct decl_initializer *di = (struct decl_initializer *)malloc(sizeof(struct decl_initializer));
  if (!di) return NULL;
  di->chain_ = di;
  di->offset_ = NULL;
  di->value_ = NULL;
  situs_init(&di->loc_);
  return di;
}

void decl_initializer_free(struct decl_initializer *in_chain) {
  struct decl_initializer *in = in_chain;
  if (in) {
    struct decl_initializer *next = in->chain_;
    do {
      in = next;
      next = in->chain_;

      expr_free(in->offset_);
      expr_free(in->value_);
      situs_cleanup(&in->loc_);
      free(in);

    } while (in != in_chain);
  }
}

struct decl_initializer *decl_initializer_join(struct decl_initializer *front, struct decl_initializer *back) {
  if (!front) return back;
  if (!back) return front;

  struct decl_initializer *front_head = front->chain_;
  struct decl_initializer *front_tail = front;
  struct decl_initializer *back_head = back->chain_;
  struct decl_initializer *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

struct decl *decl_join(struct decl *front, struct decl *back) {
  if (!front) return back;
  if (!back) return front;

  struct decl *front_head = front->chain_;
  struct decl *front_tail = front;
  struct decl *back_head = back->chain_;
  struct decl *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

void decl_print(FILE *fp, struct decl *d) {
  type_node_print_type_decl(fp, d->type_, d->sym_.ident_);
  fprintf(fp, ";\n");
}

int decl_realize_global(struct c_compiler *cc, struct decl *d, struct data_section *ds) {
  if ((d->sc_ == SC_TYPEDEF) || (d->sc_ == SC_REGISTER) || (d->sc_ == SC_AUTO)) {
    /* Silently ignore these non-globals */
    return 0;
  }

  struct ds_portion *dsp = ds_alloc_portion(ds);
  if (!dsp) {
    return -1;
  }
  char *p = NULL;
  uintptr_t d_sz = type_node_size(&cc->tb_, d->type_);

  if (ds_reserve(dsp, (size_t)d_sz, (void **)&p)) {
    return -1;
  }

  d->dsp_ = dsp;

  return 0;
}

int decl_static_initializer_exec(struct c_compiler *cc, struct ds_portion *dsp, size_t offset, struct decl_initializer *di, void *local_base, void *param_base, void *return_value_ptr) {
  struct decl_initializer *ini = di;
  if (ini) {
    do {
      ini = ini->chain_;

      int r;
      struct expr_temp target_result;
      expr_prepare(ini->offset_);
      expr_prepare(ini->value_);
      r = expr_eval(cc, &target_result, ini->offset_, 1, NULL, NULL, NULL, NULL);
      if (r) {
        return r;
      }
      size_t target_offset = offset + target_result.v_.u64_;
      uint8_t *target = ((uint8_t *)dsp->data_) + target_offset;

      struct type_node *value_type = expr_type(cc, ini->value_);
      if (!value_type) {
        return -1;
      }

      struct decl *reloc_decl = NULL;

      int is_valid = 0;

      switch (expr_find_decl_addressed(cc, ini->value_, &reloc_decl)) {
      case EXPR_FIND_DECL_NONE_FOUND:
        is_valid = 1;
        break;
      case EXPR_FIND_DECL_ONE_FOUND: {
        /* Record relocation to decl symbol */
        size_t sz = type_node_size(&cc->tb_, value_type);
        if (sz == 4) {
          if (!decl_add_ref(reloc_decl, SCRT_ABSOLUTE_32, dsp, target_offset)) {
            cc_no_memory(cc);
            return -1;
          }
        }
        else if (sz == 8) {
          if (!decl_add_ref(reloc_decl, SCRT_ABSOLUTE_64, dsp, target_offset)) {
            cc_no_memory(cc);
            return -1;
          }
        }
        else {
          cc_error_loc(cc, &ini->loc_, "Error: relocatable initializer expression is not a 32-bit or 64-bit value\n");
          is_valid = 0;
          break;
        }
        is_valid = 1;
        break;
      }
      case EXPR_FIND_DECL_MULTIPLE_FOUND:
        cc_error_loc(cc, &ini->loc_, "Error: initializer expression contains multiple declaration references\n");
        is_valid = 0;
        break;
      case EXPR_FIND_DECL_ERROR:
        cc_error_loc(cc, &ini->loc_, "Error: initializer expression is not constant\n");
        is_valid = 0;
        break;
      }

      struct expr_temp value_result;
      r = expr_eval(cc, &value_result, ini->value_, 1, reloc_decl, local_base, param_base, return_value_ptr);
      if (r) {
        return r;
      }
      if (type_node_is_scalar_type(value_type)) {
        enum type_kind tk = type_node_unqualified(value_type)->kind_;

        switch (tk) {
          case tk_char:
          case tk_signed_char:
          case tk_short_int:
          case tk_int:
          case tk_long_int:
          case tk_long_long_int:
          case tk_bool:
          case tk_unsigned_char:
          case tk_unsigned_short_int:
          case tk_unsigned_int:
          case tk_unsigned_long_int:
          case tk_unsigned_long_long_int: {
            /* One of the integer types */
            switch (type_node_size(&cc->tb_, value_type)) {
              case 1:
                *target = (uint8_t)value_result.v_.u64_;
                break;
              case 2:
                *(uint16_t *)target = (uint16_t)value_result.v_.u64_;
                break;
              case 4:
                *(uint32_t *)target = (uint32_t)value_result.v_.u64_;
                break;
              case 8:
                *(uint64_t *)target = (uint64_t)value_result.v_.u64_;
                break;
              default:
                assert(0 && "unexpected integer type size");
            }
          }
          case tk_float:
            *(float *)target = value_result.v_.f_;
            break;
          case tk_double:
          case tk_long_double:
            *(double *)target = value_result.v_.d_;
            break;
          case tk_float_complex:
            ((float *)target)[0] = value_result.v_.fc_.f_;
            ((float *)target)[1] = value_result.v_.fc_.fi_;
            break;
          case tk_double_complex:
          case tk_long_double_complex:
            ((double *)target)[0] = value_result.v_.dc_.d_;
            ((double *)target)[1] = value_result.v_.dc_.di_;
            break;
          case tk_float_imaginary:
            *(float *)target = value_result.v_.fi_;
            break;
          case tk_double_imaginary:
          case tk_long_double_imaginary:
            *(double *)target = value_result.v_.di_;
            break;
          case tk_pointer:
            /* XXX: Probably want to end up recording some kind of relocation here. */
            *(uintptr_t *)target = value_result.v_.u64_;
            break;
          default:
            assert(0 && "Unknown scalar type");
            break;
        }
      }
      else {
        /* Type must be tk_void or tk_function */
        cc_printf(cc, "Error: invalid type for initializer\n");
        return -1;
      }

    } while (ini != di);
  }

  return 0;
}

int decl_initializer_as_expr(struct c_compiler *cc, struct expr *target_ptr, struct expr **ppexpr, struct decl_initializer *di) {
  struct expr *seq = NULL;
  struct decl_initializer *ini = di;

  enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
  type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);

  struct type_node *aggr_type = expr_type(cc, target_ptr);
  if (!aggr_type) {
    cc_printf(cc, "Error: could not determine aggregate type\n");
    return -1;
  }
  if (!type_node_is_pointer_type(aggr_type)) {
    cc_printf(cc, "Error: initializer to non-pointer type\n");
    return -1;
  }

  uint64_t aggr_size = type_node_size(&cc->tb_, type_node_unqualified(aggr_type)->derived_from_);
  seq = expr_alloc(ET_CLEAR);
  if (!seq) {
    cc_no_memory(cc);
    return -1;
  }

  seq->children_[0] = target_ptr;
  seq->children_[1] = expr_alloc(et_c);
  if (!seq->children_[1]) {
    cc_no_memory(cc);
    expr_free(seq);
    return -1;
  }
  seq->children_[1]->v_.u64_ = aggr_size;

  if (ini) {
    do {
      ini = ini->chain_;

      struct type_node *value_type = expr_type(cc, ini->value_);
      if (!value_type) {
        cc_no_memory(cc);
        expr_free(seq);
        return -1;
      }

      struct expr *t = expr_alloc(ET_ADD_PTR_UINTPTR);
      if (!t) {
        cc_no_memory(cc);
        expr_free(seq);
        return -1;
      }
      t->children_[0] = target_ptr;
      target_ptr->refs_++;
      t->children_[1] = ini->offset_;
      ini->offset_->refs_++;


      if (ini->value_->et_ == ET_INDIRECTION_PTR) {
        struct expr *x = expr_alloc(ET_COPY);
        if (!x) {
          cc_no_memory(cc);
          expr_free(t);
          expr_free(seq);
          return -1;
        }
        x->children_[0] = t;
        x->children_[1] = ini->value_->children_[0];
        ini->value_->children_[0]->refs_++;
        uint64_t copy_size = type_node_size(&cc->tb_, value_type);
        x->children_[2] = expr_alloc(et_c);
        if (!x->children_[2]) {
          cc_no_memory(cc);
          expr_free(x);
          expr_free(seq);
          return -1;
        }
        x->children_[2]->v_.u64_ = copy_size;

        struct expr *next_seq = expr_alloc(ET_SEQ);
        if (!next_seq) {
          cc_no_memory(cc);
          expr_free(x);
          expr_free(seq);
          return -1;
        }
        next_seq->children_[0] = seq;
        next_seq->children_[1] = x;
        seq = next_seq;
      }
      else {
        /* Assignment of scalar, grab opcode based on type */
        enum type_kind tk = type_node_unqualified(value_type)->kind_;
        enum expr_type opcode = ET_INVALID;

        switch (tk) {
          case tk_long_double_complex:
            opcode = ET_STORE_LDC;
            break;
          case tk_long_double_imaginary:
            opcode = ET_STORE_LDI;
            break;
          case tk_double_complex:
            opcode = ET_STORE_DC;
            break;
          case tk_double_imaginary:
            opcode = ET_STORE_DI;
            break;
          case tk_float_complex:
            opcode = ET_STORE_FC;
            break;
          case tk_float_imaginary:
            opcode = ET_STORE_FI;
            break;
          case tk_long_double:
            opcode = ET_STORE_LD;
            break;
          case tk_double:
            opcode = ET_STORE_D;
            break;
          case tk_float:
            opcode = ET_STORE_F;
            break;
          case tk_long_long_int:
            opcode = ET_STORE_SLLI;
            break;
          case tk_long_int:
            opcode = ET_STORE_SLI;
            break;
          case tk_int:
            opcode = ET_STORE_SI;
            break;
          case tk_short_int:
            opcode = ET_STORE_SS;
            break;
          case tk_signed_char:
            opcode = ET_STORE_SC;
            break;
          case tk_unsigned_long_long_int:
            opcode = ET_STORE_ULLI;
            break;
          case tk_unsigned_long_int:
            opcode = ET_STORE_ULI;
            break;
          case tk_unsigned_int:
            opcode = ET_STORE_UI;
            break;
          case tk_unsigned_short_int:
            opcode = ET_STORE_US;
            break;
          case tk_unsigned_char:
            opcode = ET_STORE_UC;
            break;
          case tk_char:
            opcode = ET_STORE_C;
            break;
          case tk_pointer:
            opcode = ET_STORE_PTR;
            break;
        }
        if (opcode == ET_INVALID) {
          cc_printf(cc, "Internal error: no matching opcode for initializer type\n");
          return -1;
        }

        struct expr *x = expr_alloc(opcode);
        if (!x) {
          cc_no_memory(cc);
          expr_free(t);
          expr_free(seq);
          return -1;
        }
        x->children_[0] = t;
        x->children_[1] = ini->value_;
        ini->value_->refs_++;

        struct expr *next_seq = expr_alloc(ET_SEQ);
        if (!next_seq) {
          cc_no_memory(cc);
          expr_free(x);
          expr_free(seq);
          return -1;
        }
        next_seq->children_[0] = seq;
        next_seq->children_[1] = x;
        seq = next_seq;
      }

    } while (ini != di);
  }

  *ppexpr = seq;

  return 0;
}

static struct decl *decl_create_typedef(struct c_compiler *cc, struct type_node *tn, struct name_space *ns, struct name_space *global_ns,
                                        const char *ident, struct situs *ident_loc,
                                        sc_storage_class_t sc, int function_specifiers, int has_initializer) {
  /* If local, the typedef should be unique (not found in namespace)
   * if at filescope, the typedef can be refined with the composite of the previous and current type. */
  int is_file_scope = ns == global_ns;

  int already_exists = 0;
  struct decl *d;
  d = decl_alloc(ns, &already_exists, ident, ident_loc, sc, function_specifiers, tn, NULL);
  if (!d) {
    cc_no_memory(cc);
    return NULL;
  }

  if (!is_file_scope && already_exists) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return NULL;
  }

  if (!d->is_typedef_ && already_exists) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined as non-typedef (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return NULL;
  }

  if (has_initializer) {
    cc_error_loc(cc, ident_loc, "Typedef \"%s\" cannot have initializer", ident);
    return NULL;
  }

  if (!already_exists) {
    d->is_typedef_ = 1;
  }
    
  if (is_file_scope && already_exists) {
    /* Create composite type; augment the pre-existing variable, can be internal (static) or external, we don't care. */
    struct type_node *old_tn = d->type_;
    if (!type_node_is_compatible(old_tn, tn)) {
      cc_error_loc(cc, ident_loc, "Identifier \"%s\" type disagreement (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
      return NULL;
    }
    struct type_node *new_tn = type_node_composite(cc, old_tn, tn);
    if (!new_tn) {
      cc_no_memory(cc);
      return NULL;
    }
    d->type_ = new_tn;
  }
  return d;
}

static struct decl *decl_create_variable(struct c_compiler *cc, struct type_node *tn, struct name_space *ns, struct name_space *global_ns,
                                         const char *ident, struct situs *ident_loc,
                                         sc_storage_class_t sc, int function_specifiers, int has_initializer) {
  int is_file_scope = ns == global_ns;
  // Create variable, if is_file_scope and sc == SC_STATIC, then give it internal linkage, 
  // if not is_file_scope then local (automatic) linkage, otherwise external linkage.
  // Keep in mind that if is_file_scope, then there may already be a declaration that overlaps (e.g. tentative definition.)

  int already_exists = 0;
  struct decl *d;
  d = decl_alloc(ns, &already_exists, ident, ident_loc, sc, function_specifiers, tn, NULL);
  if (!d) {
    cc_no_memory(cc);
    return NULL;
  }

  if (already_exists && d->is_typedef_) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined as a typedef (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return NULL;
  }

  if (!is_file_scope && already_exists) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return NULL;
  }

  if (!is_file_scope && !already_exists) {
    /* Allocate local (automatic lifetime)
     * A local is always a definition. */
    d->is_definition_ = 1;
    d->is_local_ = 1;
    return d;
  }

  /* Always is_file_scope from here on... */

  int this_is_definition = 0;
  if (has_initializer) {
    /* This is a definition if it has an initializer (C99 6.9.2 paragraph 2) */
    this_is_definition = 1;
  }
  else {
    /* Not a definition but a tentative definition */
    this_is_definition = 0;
  }

  /* May not have two sites of defintion */
  if (already_exists && d->is_definition_ && this_is_definition) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return d;
  }

  if (already_exists && (sc == SC_STATIC) && d->is_external_ && !d->is_declaration_) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" linkage disagreement, static here, but external before (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return d;
  }

  if (already_exists && (sc == SC_STATIC) && d->is_external_ && d->is_declaration_) {
    /* This is a static definition of a previously declared external; so for example: 
      *   extern int x;
      *   static int x;
      * is valid code, the first "extern int x" is a declaration for x (which is external)
      * and the second "static int x" is a definition for the same variable, but defines it
      * as internal. That definition overrules the "external-ness" of the prior declaration.
      * Note that the following:
      *   extern int x = 1;
      *   static int x;
      * is illegal as "extern int x = 1", because of the presence of the initializer, is not
      * a declaration but a definition of an externally visibile variable (and so the later 
      * "static int x" is in conflict.) */
    d->is_declaration_ = 0;
    d->is_definition_ = 1;
    d->is_external_ = 0;
    d->is_internal_ = 1;
    d->is_local_ = 0;
    situs_clone(&d->def_loc_, ident_loc);
  }

  if (!already_exists) {
    if (sc == SC_STATIC) {
      d->is_internal_ = 1;
    }
    else if (is_file_scope) {
      d->is_external_ = 1;
    }
    else /* sc != SC_STATIC and !is_file_scope */ {
      /* Note that it can't be SC_EXTERN because then it'd be going through decl_create_extern_variable() */
      d->is_local_ = 1;
    }
  }

  if (already_exists) {
    /* Create composite type */
    struct type_node *old_tn = d->type_;
    if (!type_node_is_compatible(old_tn, tn)) {
      cc_error_loc(cc, ident_loc, "Identifier \"%s\" type disagreement (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
      return NULL;
    }
    struct type_node *new_tn = type_node_composite(cc, old_tn, tn);
    if (!new_tn) {
      cc_no_memory(cc);
      return NULL;
    }
    d->type_ = new_tn;
  }

  if (this_is_definition) {
    d->is_definition_ = 1;
  }
  else {
    d->is_tentative_definition_ = 1;
  }
    
  return d;
}

static struct decl *decl_create_extern_variable(struct c_compiler *cc, struct type_node *tn, struct name_space *ns, struct name_space *global_ns,
                                                const char *ident, struct situs *ident_loc,
                                                sc_storage_class_t sc, int function_specifiers, int has_initializer) {
  int is_file_scope = ns == global_ns;
  /* If is_file_scope and not has_initializer then this is a declaration; any prior declarations have their type composited.
   * If is_file_scope and has_initializer then this is a definition (and we should not have any prior definitions.)
   * If not is_file_scope then we need to create a global forward to a declaration at file scope; it would be an error
   * for such a local 'extern' to have an initializer. */

  int already_exists = 0;
  struct decl *d;
  d = decl_alloc(ns, &already_exists, ident, ident_loc, sc, function_specifiers, tn, NULL);
  if (!d) {
    cc_no_memory(cc);
    return NULL;
  }

  /* If this is local, then return the local decl and not the global decl we'll be working on. */
  struct decl *d_to_return = d;

  if (already_exists && d->is_typedef_) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined as a typedef (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return NULL;
  }

  if (!is_file_scope && already_exists) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return NULL;
  }

  if (!is_file_scope && !already_exists) {
    /* If we are on local scope and processing SC_EXTERN, make sure to find or define the global variable in global_ns and
     * then create a local variable in ns that points to the global variable (using global_forward_ ). */
    struct decl *gd;
    int global_already_exists;
    gd = decl_alloc(global_ns, &global_already_exists, ident, ident_loc, sc, function_specifiers, tn, NULL);
    if (!gd) {
      cc_no_memory(cc);
      return NULL;
    }
    if (!global_already_exists) {
      gd->is_hidden_global_ = 1;
      d->global_forward_ = gd;
    }
    else if (gd->is_typedef_) {
      cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined as a typedef (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
      return NULL;
    }
    /* Continue processing with the global variable, not the local one as it only forwards. */
    d = gd;
  }

  /* From this point forward, treat it as at file scope. */
  if (has_initializer) {
    if (tn->kind_ == tk_function) {
      cc_error_loc(cc, ident_loc, "Identifier \"%s\" cannot be initialized as it is a function", ident);
      return NULL;
    }

    /* Definition. */
    if (d->is_definition_) {
      /* The global is already defined, so cannot define it again (the initializer is what makes it a definition.) */
      cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
      return NULL;
    }

    if (!already_exists) {
      d->is_external_ = 1;
    }

    if (already_exists) {
      /* Create composite type */
      struct type_node *old_tn = d->type_;
      if (!type_node_is_compatible(old_tn, tn)) {
        cc_error_loc(cc, ident_loc, "Identifier \"%s\" type disagreement (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
        return NULL;
      }
      struct type_node *new_tn = type_node_composite(cc, old_tn, tn);
      if (!new_tn) {
        cc_no_memory(cc);
        return NULL;
      }
      d->type_ = new_tn;
    }

    d->is_definition_ = 1;

    return d_to_return;
  }

  /* No initializer, just declaration, possibly refinement of declaration type */
  if (already_exists) {
    /* Create composite type */
    struct type_node *old_tn = d->type_;
    if (!type_node_is_compatible(old_tn, tn)) {
      cc_error_loc(cc, ident_loc, "Identifier \"%s\" type disagreement (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
      return NULL;
    }
    struct type_node *new_tn = type_node_composite(cc, old_tn, tn);
    if (!new_tn) {
      cc_no_memory(cc);
      return NULL;
    }
    d->type_ = new_tn;
  }
  else {
    d->is_declaration_ = 1;
    if (sc == SC_STATIC) {
      /* Edge-case: when a function prototype is declared, it is deemed "extern", 
       * and so ends up here, even if it is declared static. (Note that only a function ends up here, as per decl_create_update(). */
      d->is_internal_ = 1;
    }
    else {
      d->is_external_ = 1;
    }
  }

  return d_to_return;
}

static struct decl *decl_create_static_local_variable(struct c_compiler *cc, struct type_node *tn, struct name_space *ns, struct name_space *global_ns,
                                                      const char *ident, struct situs *ident_loc,
                                                      sc_storage_class_t sc, int function_specifiers, int has_initializer) {
  /* Create local forwarder, it should not exist. */
  int already_exists = 0;
  struct decl *d;
  d = decl_alloc(ns, &already_exists, ident, ident_loc, sc, function_specifiers, tn, NULL);
  if (!d) {
    cc_no_memory(cc);
    return NULL;
  }
  if (already_exists) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" already defined (see line %d in file %s)", ident, situs_line(&d->def_loc_), situs_filename(&d->def_loc_));
    return NULL;
  }
  
  const char *function_ident = cc->ctx_.current_func_ ? cc->ctx_.current_func_->d_.sym_.ident_ : NULL;
  if (!function_ident) {
    // Make up a function name for disambiguation, we'll use "$IMPLICIT_GLOBAL"
    function_ident = "$IMPLICIT_GLOBAL";
  }

  /* make global identifier (link name relying on cc->asm_decoration_char_ to disambiguate / make unique. */
  size_t global_name_len = strlen(ident) + 1 + strlen(function_ident) + 1;
  char *global_name = (char *)malloc(global_name_len);
  if (!global_name) {
    cc_no_memory(cc);
    return NULL;
  }
  global_name[0] = '\0';
  strcat(global_name, function_ident);
  char asm_decor[2] = { cc->asm_decoration_char_, '\0' };
  strcat(global_name, asm_decor);
  strcat(global_name, ident);

  /* Create global variable. */
  int global_already_exists;
  struct decl *gd;
  gd = decl_alloc(global_ns, &global_already_exists, global_name, ident_loc, sc, function_specifiers, tn, NULL);
  if (!gd) {
    cc_no_memory(cc);
    free(global_name);
    return NULL;
  }
  if (global_already_exists) {
    /* Dis-ambiguate global ; see comment at decl::disambiguation_ordinal_ */
    char *new_global_name = (char *)malloc(global_name_len + 1 + 32 + 1);
    if (!new_global_name) {
      cc_no_memory(cc);
      free(global_name);
      return NULL;
    }
    new_global_name[0] = '\0';
    strcat(new_global_name, global_name);
    char disambiguation[33];
    sprintf(disambiguation, "%c%d", cc->asm_decoration_char_, gd->disambiguation_ordinal_++);
    strcat(new_global_name, disambiguation);
    free(global_name);
    global_name = new_global_name;
    gd = decl_alloc(global_ns, &global_already_exists, global_name, ident_loc, sc, function_specifiers, tn, NULL);
    if (!gd) {
      cc_no_memory(cc);
      free(global_name);
      return NULL;
    }
    if (global_already_exists) {
      cc_fatal_loc(cc, ident_loc, "Internal error: Disambiguation of global name \"%s\" failed.", global_name);
      free(global_name);
      return NULL;
    }
  }
  gd->is_internal_ = 1;
  gd->is_definition_ = 1;
  free(global_name);
  d->global_forward_ = gd;
  return d;
}

struct decl *decl_create_update(struct c_compiler *cc, struct type_node *tn, struct name_space *ns, struct name_space *global_ns, 
                                const char *ident, struct situs *ident_loc,
                                sc_storage_class_t sc, int function_specifiers, int has_initializer, int is_global) {
  int is_file_scope = ns == global_ns;
  int is_function = type_node_is_function(tn);

  if ((function_specifiers & DECL_FUNCTION_SPECIFIER_INLINE) && !is_function) {
    cc_error_loc(cc, ident_loc, "Identifier \"%s\" invalid 'inline' function specifier", ident);
    return NULL;
  }

  if (is_function) {
    /* Functions are always implicitly "extern" */
    return decl_create_extern_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
  }


  if (is_file_scope) {
    switch (sc) {
      case SC_NONE:
        return decl_create_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
      case SC_TYPEDEF:
        return decl_create_typedef(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
      case SC_EXTERN:
        return decl_create_extern_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
      case SC_STATIC:
        return decl_create_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
      case SC_AUTO:
        cc_error_loc(cc, ident_loc, "Identifier \"%s\" invalid 'auto' storage class at file scope", ident);
        return decl_create_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
      case SC_REGISTER:
        cc_error_loc(cc, ident_loc, "Identifier \"%s\" invalid 'register' storage class at file scope", ident);
        return decl_create_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
    }
  }
  else /* local scope */ {
    switch (sc) {
      case SC_NONE:
      case SC_AUTO:
      case SC_REGISTER:
        if (!is_global) {
          /* This is the common case for regular C, if we're inside a scope, then it implies we're inside a function. */
          return decl_create_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
        }
        else {
          /* For template or REPL modes, if we're inside a scope then we might still be at file level - this occurs when
           * we execute compound statements of whatever kind at the file level - normal C does not allow this but we need
           * to handle it for template and REPL modes. */
          return decl_create_static_local_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
        }
        break;
      case SC_TYPEDEF:
        return decl_create_typedef(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
      case SC_EXTERN:
        return decl_create_extern_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
      case SC_STATIC:
        return decl_create_static_local_variable(cc, tn, ns, global_ns, ident, ident_loc, sc, function_specifiers, has_initializer);
    }
  }

  assert(0 && "Unknown storage class");
  return NULL;
}

int decl_resolve_global_decl(struct c_compiler *cc, struct decl *d) {
  size_t n;
  uint64_t reloc_value = 0;
  /* Resolve all references to this global. */
  if (d->is_external_ || d->is_internal_) {
    if (d->is_tentative_definition_) {
      /* Process tentative definition: make it a final definition. */
      if (decl_realize_global(cc, d, &cc->ds_)) {
        cc_fatal(cc, "failed to realize tentative definition of \"%s\"\n", d->sym_.ident_);
        return -1;
      }
      d->is_tentative_definition_ = 0;
      d->is_definition_ = 1;
    }
    if (d->is_definition_) {
      if (!d->dsp_) {
        cc_fatal(cc, "internal error: no storage allocated for defined external \"%s\".\n", d->sym_.ident_);
        return -1;
      }
      reloc_value = (uint64_t)d->dsp_->data_;
    }
    else if (d->is_declaration_) {
      /* Late binding external.. */
      if (!d->external_) {
        void *p;
        int r;
        r = drl_resolve_sym(cc, d, &p);

        if (r) {
          cc_fatal_loc(cc, &d->def_loc_, "Failed to dynamically resolve linkage for \"%s\" (%d)\n", d->sym_.ident_, r);
          return -1;
        }
        else {
          d->external_ = p;
        }
      }
      reloc_value = (uint64_t)d->external_;
    }
  }
  else if (d->dsp_) {
    /* Global variable. Should really have been caught by the other cases (is_external_ or is_internal_) ... */
    reloc_value = (uint64_t)d->dsp_->data_;
  }
  else {
    cc_fatal(cc, "internal error: cannot relocate non-static \"%s\".\n", d->sym_.ident_);
    return -1; /* can't do it; wrong type - should never have had a decl_ref. */
  }

  for (n = 0; n < d->num_refs_; ++n) {
    struct decl_ref *dr = d->refs_ + n;
    if (!dr->realized_) {
      uint8_t *dst = ((uint8_t *)dr->dsp_->data_) + dr->offset_;
      switch (dr->rt_) {
        case SCRT_ABSOLUTE_64:
          *(uint64_t *)dst += reloc_value;
          break;
        case SCRT_ABSOLUTE_32:
          *(uint32_t *)dst += (uint32_t)reloc_value;
          break;
        case SCRT_RIP_REL_64:
          *(uint64_t *)dst += reloc_value - (uint64_t)dst;
          break;
        case SCRT_RIP_REL_32:
          *(uint32_t *)dst += ((uint32_t)reloc_value) - (uint32_t)(uintptr_t)dst;
          break;
        default:
          assert(0);
          break;
      }
      dr->realized_ = 1;
    }
  }
  return 0;
}

int decl_resolve_all_global_decl_relocs(struct c_compiler *cc) {
  struct decl *d;
  d = (struct decl *)cc->global_ns_.ordinary_idents_.seq_;
  if (d) {
    do {
      size_t n;
      for (n = 0; n < d->num_refs_; ++n) {
        struct decl_ref *dr = d->refs_ + n;
        if (!dr->realized_) {
          if (decl_resolve_global_decl(cc, d)) {
            return -1;
          }
          break;
        }
      }
      d = (struct decl *)d->sym_.next_;
    } while (d != (struct decl *)cc->global_ns_.ordinary_idents_.seq_);
  }
  return 0;
}

