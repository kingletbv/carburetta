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

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
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

#ifndef TEMPL_PARSER_H_INCLUDED
#define TEMPL_PARSER_H_INCLUDED
#include "templ_parser.h"
#endif

#ifndef C_PARSER_H_INCLUDED
#define C_PARSER_H_INCLUDED
#include "c_parser.h"
#endif

#ifndef PP_TOKENS_H_INCLUDED
#define PP_TOKENS_H_INCLUDED
#include "pp_tokens.h"
#endif

#ifndef PARTIAL_TYPE_SPECIFIERS_H_INCLUDED
#define PARTIAL_TYPE_SPECIFIERS_H_INCLUDED
#include "partial_type_specifiers.h"
#endif


void templ_context_init(struct templ_context *cx) {
  cx->is_typedefname_permitted_ = 1;
  cx->block_ = NULL;
  cx->current_switch_parent_ = NULL;
  cx->current_break_parent_ = NULL;
  cx->current_continue_parent_ = NULL;
  cx->current_func_ = NULL;
  cx->decl_spec_type_ = NULL;
  cx->decl_spec_fs_ = 0;
  cx->decl_spec_sc_ = 0;
}

void templ_context_cleanup(struct templ_context *cx) {
  /* no cleanup for context members as cx doesn't own them, the parser stack does */
}

void report_error(struct situs *loc, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "(%d): ", situs_line(loc));
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

static struct templ_declarator_scaffold *templ_ds_alloc(enum templ_declarator_scaffold_kind kind) {
  struct templ_declarator_scaffold *tds = (struct templ_declarator_scaffold *)malloc(sizeof(struct templ_declarator_scaffold));
  if (!tds) return NULL;
  tds->chain_ = tds;
  tds->fields_ = NULL;
  tds->kind_ = kind;
  tds->qualifiers_ = 0;
  tds->array_size_ = NULL;
  tds->is_static_array_ = 0;
  tds->is_variable_length_array_ = 0;
  tds->has_unknown_arguments_ = 0;
  return tds;
}

struct templ_declarator_scaffold *templ_ds_join(struct templ_declarator_scaffold *front, struct templ_declarator_scaffold *back) {
  if (!back) return front;
  if (!front) return back;
  struct templ_declarator_scaffold *front_head = front->chain_;
  struct templ_declarator_scaffold *front_tail = front;
  struct templ_declarator_scaffold *back_head = back->chain_;
  struct templ_declarator_scaffold *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

void templ_declarator_scaffold_free_chain(struct templ_declarator_scaffold *tdsf) {
  struct templ_declarator_scaffold *dsf = tdsf, *next;
  if (dsf) {
    next = dsf->chain_;
    do {
      dsf = next;
      next = dsf->chain_;

      if (dsf->fields_) {
        templ_dsf_free_chain(dsf->fields_);
      }

      expr_free(dsf->array_size_);

      free(dsf);
    } while (dsf != tdsf);
  }
}

struct templ_declarator_scaffold *templ_ds_array(struct templ_declarator_scaffold *super_type, struct expr *array_size_expr, int is_static_array, int is_variable_length_array) {
  struct templ_declarator_scaffold *tds = templ_ds_alloc(TDSK_ARRAY);
  if (!tds) return NULL;
  tds->array_size_ = array_size_expr;
  tds->is_static_array_ = is_static_array;
  tds->is_variable_length_array_ = is_variable_length_array;

  return templ_ds_join(tds, super_type);
}

struct templ_declarator_scaffold *templ_ds_qualifier(struct templ_declarator_scaffold *super_type, int qualifiers) {
  struct templ_declarator_scaffold *tds = templ_ds_alloc(TDSK_QUALIFIER);
  if (!tds) return NULL;
  tds->qualifiers_ = qualifiers;
  return templ_ds_join(tds, super_type);
}

struct templ_declarator_scaffold *templ_ds_function(struct templ_declarator_scaffold *super_type, struct templ_declarator_scaffold_field *params, int identifiers_only) {
  struct templ_declarator_scaffold *tds = templ_ds_alloc(TDSK_FUNCTION);
  if (!tds) return NULL;


  tds->has_unknown_arguments_ = !templ_dsf_are_arguments_known(params);
  tds->identifiers_only_ = identifiers_only;
  if (templ_dsf_is_isolated_void(params)) {
    /* Known and 0 arguments, remove the "void" argument */
    templ_dsf_free_chain(params);
  }
  else {
    tds->fields_ = params;
  }

  return templ_ds_join(tds, super_type);
}

struct templ_declarator_scaffold *templ_ds_pointer(struct templ_declarator_scaffold *super_type) {
  struct templ_declarator_scaffold *tds = templ_ds_alloc(TDSK_POINTER);
  if (!tds) return NULL;

  return templ_ds_join(tds, super_type);
}

struct templ_declarator_scaffold_field *templ_dsf_alloc(struct templ_declarator_scaffold_field *siblings, char *ident, struct situs *ident_loc, struct type_node *tn) {
  struct templ_declarator_scaffold_field *tdsf = (struct templ_declarator_scaffold_field *)malloc(sizeof(struct templ_declarator_scaffold_field));
  if (!tdsf) return NULL;
  if (siblings) {
    tdsf->chain_ = siblings->chain_;
    siblings->chain_ = tdsf;
  }
  else {
    tdsf->chain_ = tdsf;
  }
  tdsf->ident_ = ident;
  situs_init(&tdsf->ident_loc_);
  if (situs_clone(&tdsf->ident_loc_, ident_loc)) {
    free(tdsf);
    return NULL;
  }
  tdsf->type_ = tn;
  tdsf->is_variadic_ = 0;
  return tdsf;
}


void templ_dsf_free_chain(struct templ_declarator_scaffold_field *dsf) {
  struct templ_declarator_scaffold_field *next, *field;
  field = dsf;
  if (field) {
    next = field->chain_;
    do {
      field = next;
      next = field->chain_;

      if (field->ident_) free(field->ident_);
      /* No need to release type_, owned by type_base */
      situs_cleanup(&field->ident_loc_);
      free(field);

    } while (field != dsf);
  }
}

struct templ_declarator_scaffold_field *templ_dsf_join(struct templ_declarator_scaffold_field *front, struct templ_declarator_scaffold_field *back) {
  if (!back) return front;
  if (!front) return back;
  struct templ_declarator_scaffold_field *front_head = front->chain_;
  struct templ_declarator_scaffold_field *front_tail = front;
  struct templ_declarator_scaffold_field *back_head = back->chain_;
  struct templ_declarator_scaffold_field *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

int templ_dsf_are_arguments_known(struct templ_declarator_scaffold_field *arguments) {
  if (!arguments) return 0;
  return 1;
}

int templ_dsf_is_isolated_void(struct templ_declarator_scaffold_field *arguments) {
  /* Must be 1 isolated argument */
  if (!arguments) return 0;
  if (arguments->chain_ != arguments) return 0;

  /* Argument must be void with no identifier */
  if (arguments->ident_) return 0;
  if (arguments->is_variadic_) return 0;
  if (!arguments->type_) return 0;

  /* Isolated, but isolated void ? */
  return (arguments->type_->kind_ == tk_void);
}


struct templ_declarator_entry *templ_de_alloc(struct templ_declarator_entry *siblings, struct templ_declarator_scaffold *tds, char *ident, struct situs *ident_loc, int bitfield_size) {
  struct templ_declarator_entry *tde = (struct templ_declarator_entry *)malloc(sizeof(struct templ_declarator_entry));
  if (siblings) {
    tde->chain_ = siblings->chain_;
    siblings->chain_ = tde;
  }
  else {
    tde->chain_ = tde;
  }
  tde->tds_ = tds;
  tde->ident_ = ident;
  situs_init(&tde->ident_loc_);
  if (situs_clone(&tde->ident_loc_, ident_loc)) {
    free(tde);
    return NULL;
  }
  tde->bitfield_size_ = bitfield_size;
  return tde;
}

void templ_de_free_chain(struct templ_declarator_entry *de) {
  struct templ_declarator_entry *tde = de, *next;
  if (tde) {
    next = tde->chain_;
    do {
      tde = next;
      next = tde->chain_;

      templ_declarator_scaffold_free_chain(tde->tds_);
      if (tde->ident_) free(tde->ident_);
      situs_cleanup(&tde->ident_loc_);
      free(tde);

    } while (tde != de);
  }
}

struct templ_declarator_entry *templ_de_join(struct templ_declarator_entry *front, struct templ_declarator_entry *back) {
  if (!back) return front;
  if (!front) return back;
  struct templ_declarator_entry *front_head = front->chain_;
  struct templ_declarator_entry *front_tail = front;
  struct templ_declarator_entry *back_head = back->chain_;
  struct templ_declarator_entry *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

static struct type_node *templ_build_function_type(struct c_compiler *cc, struct type_node *return_type, struct templ_declarator_scaffold *ftds) {
  struct type_node *ft = type_base_alloc_func(&cc->tb_, return_type);
  if (!ft) return NULL;

  struct templ_declarator_scaffold_field *dsf = ftds->fields_;
  if (dsf) {
    do {
      dsf = dsf->chain_;

      struct type_param *tp = type_param_alloc(dsf->ident_, &dsf->ident_loc_, dsf->type_, !!dsf->is_variadic_);
      if (!tp) {
        /* ft doesn't leak but is recorded in type_base for eventual cleanup */
        return NULL;
      }
      if (ft->params_) {
        ft->params_ = type_param_join(ft->params_, tp);
      }
      else {
        ft->params_ = tp;
      }

    } while (dsf != ftds->fields_);
  }

  ft->arguments_unknown_ = ftds->has_unknown_arguments_;
  ft->is_identifier_list_ = ftds->identifiers_only_;

  return ft;
}

struct type_node *templ_realize_type(struct c_compiler *cc, struct type_node *specifier, struct templ_declarator_scaffold *tds, int *fatality) {
  /* Starting with the specifier, build out greater types as we go along */
  struct type_node *tn = specifier;
  struct templ_declarator_scaffold *ds = tds;
  if (ds) {
    do {
      ds = ds->chain_;

      switch (ds->kind_) {
      case TDSK_ARRAY:
        if ((type_node_unqualified(tn)->kind_ == tk_array) && !(type_node_unqualified(tn)->array_size_expr_)) {
          fprintf(stderr, "Error: element type of array is incomplete\n");
          *fatality = 0;
          return NULL;
        }
        tn = type_base_array(cc, tn, ds->array_size_);
        if (!tn) {
          *fatality = 1;
          return NULL;
        }
        ds->array_size_ = NULL;
        tn->is_variable_length_array_ = ds->is_variable_length_array_;
        /* XXX: ds->is_static_array_ is skipped */
        break;
      case TDSK_FUNCTION:
        if ((type_node_unqualified(tn)->kind_ == tk_array) && !(type_node_unqualified(tn)->array_size_expr_)) {
          fprintf(stderr, "Error: function return type is incomplete\n");
          *fatality = 0;
          return NULL;
        }
        tn = templ_build_function_type(cc, tn, ds);
        break;
      case TDSK_POINTER:
        if ((type_node_unqualified(tn)->kind_ == tk_array) && !(type_node_unqualified(tn)->array_size_expr_)) {
          fprintf(stderr, "Error: type pointed to is incomplete\n");
          *fatality = 0;
          return NULL;
        }
        tn = type_base_pointer(&cc->tb_, tn);
        break;
      case TDSK_QUALIFIER:
        tn = type_base_qualifier(&cc->tb_, tn, ds->qualifiers_);
        break;
      }
      /* No memory */
      if (!tn) {
        *fatality = 1;
        return NULL;
      }

    } while (ds != tds);
  }

  return tn;
}

struct type_node *templ_type_node_realize(struct c_compiler *cc, pts_type_specifier_t pts, struct type_node *specifier_type, struct situs *spec_loc, int type_qualifier, struct templ_declarator_scaffold *declarator, int *fatality) {
  /* Convert to type and identifier */
  struct type_node *tn = specifier_type;
  if (!tn) {
    enum type_kind tk = pts_type_kind(pts);
    if (tk == tk_invalid) {
      report_error(spec_loc, "Invalid type specifier");
    }
    else {
      tn = type_base_simple(&cc->tb_, tk);
      if (!tn) {
        report_error(spec_loc, "Internal error, failed to realize type specifier");
      }
    }
  }
  if (type_qualifier) {
    tn = type_base_qualifier(&cc->tb_, tn, type_qualifier);
    if (!tn) {
      return NULL;
    }
  }
  if (declarator) {
    tn = templ_realize_type(cc, tn, declarator, fatality);
    if (!tn) {
      return NULL;
    }
  }
  return tn;
  /* XXX: 6.7.5.3-8, 6.3.2.1  ? */
}

struct templ_declarator_scaffold_field *templ_dsf_realize(struct c_compiler *cc, pts_type_specifier_t pts, struct type_node *specifier, struct situs *spec_loc, int type_qualifier, struct templ_declarator_scaffold *declarator, char *ident, struct situs *ident_loc, int *fatality) {
  struct templ_declarator_scaffold_field *tdsf;
  struct type_node *tn = templ_type_node_realize(cc, pts, specifier, spec_loc, type_qualifier, declarator, fatality);
  if (!tn) {
    return NULL;
  }
  tdsf = templ_dsf_alloc(NULL, ident, ident_loc, tn);
  if (!tdsf) {
    *fatality = 1;
    return NULL;
  }
  return tdsf;
  /* XXX: 6.7.5.3-8, 6.3.2.1 */
}

struct type_field *templ_type_field_realize(struct c_compiler *cc, pts_type_specifier_t pts, struct type_node *specifier, struct situs *spec_loc, int type_qualifier, struct templ_declarator_scaffold *declarator, char *ident, struct situs *ident_loc, int bitfield_size, int *fatality) {
  struct type_node *tn = specifier;
  if (!tn) {
    enum type_kind tk = pts_type_kind(pts);
    if (tk == tk_invalid) {
      report_error(spec_loc, "Invalid type specifier");
    }
    else {
      tn = type_base_simple(&cc->tb_, tk);
      if (!tn) {
        *fatality = 1;
        report_error(spec_loc, "Internal error, failed to realize type specifier");
        return NULL;
      }
    }
  }
  if (type_qualifier) {
    tn = type_base_qualifier(&cc->tb_, tn, type_qualifier);
    if (!tn) {
      *fatality = 1;
      return NULL;
    }
  }
  tn = templ_realize_type(cc, tn, declarator, fatality);
  if (!tn) {
    return NULL;
  }
  struct type_field *tf;
  tf = type_field_alloc(ident, ident_loc, tn, bitfield_size);
  if (!tf) {
    *fatality = 1;
  }
  return tf;
  /* XXX: 6.7.5.3-8, 6.3.2.1 */
}

struct type_node *templ_specifier_realize(struct c_compiler *cc, pts_type_specifier_t pts, struct type_node *specifier, struct situs *spec_loc, int type_qualifier) {
  struct type_node *tn = specifier;
  if (!tn) {
    enum type_kind tk = pts_type_kind(pts);
    if (tk == tk_invalid) {
      report_error(spec_loc, "Invalid type specifier");
      /* XXX: How to distinguish this error from a fatal internal error or no memory condition? */
      return NULL;
    }
    else {
      tn = type_base_simple(&cc->tb_, tk);
      if (!tn) {
        report_error(spec_loc, "Internal error, failed to realize type specifier");
      }
    }
  }
  if (type_qualifier) {
    tn = type_base_qualifier(&cc->tb_, tn, type_qualifier);
    if (!tn) {
      return NULL;
    }
  }
  return tn;
}


struct templ_initializer *templ_initializer_literal(struct expr *x, struct situs *loc) {
  struct templ_initializer *tin = (struct templ_initializer *)malloc(sizeof(struct templ_initializer));
  if (!tin) return NULL;
  tin->kind_ = TINK_LITERAL;
  situs_init(&tin->loc_);
  if (situs_clone(&tin->loc_, loc)) {
    free(tin);
    return NULL;
  }
  tin->u_.literal_exp_ = x;
  return tin;
}

struct templ_initializer *templ_initializer_compound(struct templ_initializer_list_node *nodes, struct situs *loc) {
  struct templ_initializer *tin = (struct templ_initializer *)malloc(sizeof(struct templ_initializer));
  if (!tin) return NULL;
  tin->kind_ = TINK_COMPOUND;
  situs_init(&tin->loc_);
  if (situs_clone(&tin->loc_, loc)) {
    free(tin);
    return NULL;
  }
  tin->u_.compound_nodes_ = nodes;
  return tin;
}

void templ_initializer_free(struct templ_initializer *tin) {
  if (!tin) return;
  if (tin->kind_ == TINK_LITERAL) {
    expr_free(tin->u_.literal_exp_);
  }
  else if (tin->kind_ == TINK_COMPOUND) {
    templ_initializer_list_node_free(tin->u_.compound_nodes_);
  }
  free(tin);
}

struct templ_initializer_list_node *templ_initializer_list_node(struct templ_initializer_designation *designators, struct templ_initializer *initializer) {
  struct templ_initializer_list_node *tiln = (struct templ_initializer_list_node *)malloc(sizeof(struct templ_initializer_list_node));
  if (!tiln) return NULL;
  tiln->chain_ = tiln;
  tiln->designators_ = designators;
  tiln->initializer_ = initializer;
  return tiln;
}

struct templ_initializer_list_node *templ_initializer_list_node_join(struct templ_initializer_list_node *front, struct templ_initializer_list_node *back) {
  if (!back) return front;
  if (!front) return back;
  struct templ_initializer_list_node *front_head = front->chain_;
  struct templ_initializer_list_node *front_tail = front;
  struct templ_initializer_list_node *back_head = back->chain_;
  struct templ_initializer_list_node *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

void templ_initializer_list_node_free(struct templ_initializer_list_node *tinln) {
  struct templ_initializer_list_node *n = tinln;
  if (n) {
    struct templ_initializer_list_node *next = n->chain_;
    do {
      n = next;
      next = n->chain_;

      templ_initializer_designation_free(n->designators_);
      templ_initializer_free(n->initializer_);

      free(n);
    } while (n != tinln);
  }
}

struct templ_initializer_designation *templ_initializer_designation_index(struct expr *index, struct situs *loc) {
  struct templ_initializer_designation *d = (struct templ_initializer_designation *)malloc(sizeof(struct templ_initializer_designation));
  if (!d) return NULL;
  d->chain_ = d;
  d->kind_ = TINDK_INDEX;
  d->u_.index_exp_ = index;
  situs_init(&d->loc_);
  if (situs_clone(&d->loc_, loc)) {
    free(d);
    return NULL;
  }
  return d;
}

struct templ_initializer_designation *templ_initializer_designation_ident(const char *ident, struct situs *loc) {
  char *id = strdup(ident);
  if (!id) return NULL;
  struct templ_initializer_designation *d = (struct templ_initializer_designation *)malloc(sizeof(struct templ_initializer_designation));
  if (!d) {
    free(id);
    return NULL;
  }
  d->chain_ = d;
  d->kind_ = TINDK_IDENT;
  d->u_.ident_str_ = id;
  situs_init(&d->loc_);
  if (situs_clone(&d->loc_, loc)) {
    free(d);
    return NULL;
  }
  return d;
}


struct templ_initializer_designation *templ_initializer_designation_join(struct templ_initializer_designation *front, struct templ_initializer_designation *back) {
  if (!back) return front;
  if (!front) return back;
  struct templ_initializer_designation *front_head = front->chain_;
  struct templ_initializer_designation *front_tail = front;
  struct templ_initializer_designation *back_head = back->chain_;
  struct templ_initializer_designation *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

void templ_initializer_designation_free(struct templ_initializer_designation *tind) {
  struct templ_initializer_designation *d = tind;
  if (d) {
    struct templ_initializer_designation *next = d->chain_;
    do {
      d = next;
      next = d->chain_;

      if (d->kind_ == TINDK_INDEX) {
        expr_free(d->u_.index_exp_);
      }
      else if (d->kind_ == TINDK_IDENT) {
        if (d->u_.ident_str_) free(d->u_.ident_str_);
      }
      situs_cleanup(&d->loc_);
      free(d);

    } while (d != tind);
  }
}

int templ_initializer_iter(struct c_compiler *cc, struct type_node *tn, struct expr *base_offset, 
                           struct templ_initializer_designation *desig,
                           struct templ_initializer_list_node **pptiln, struct templ_initializer_list_node *last_tiln,
                           struct decl_initializer **ppini_chain,
                           uint64_t*top_array_observed_length);

int templ_initializer_iter_struct_union(struct c_compiler *cc, struct type_node *tn, struct expr *base_offset,
                                        struct templ_initializer_designation *desig,
                                        struct templ_initializer_list_node **pptiln, struct templ_initializer_list_node *last_tiln,
                                        struct decl_initializer **ppini_chain) {
  struct expr *current_offset = NULL;
  enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
  type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);

  tn = type_node_unqualified(tn);
  assert((tn->kind_ == tk_structure) || (tn->kind_ == tk_union));

  struct type_field *field = NULL;

  if (desig) {
    if (desig->kind_ != TINDK_IDENT) {
      report_error(&desig->loc_, "only identifier designators expected for struct or union initalization");
      return -1;
    }

    
    struct type_field *tf = tn->fields_;
    if (tf) {
      do {
        tf = tf->chain_;

        if (!strcmp(tf->ident_, desig->u_.ident_str_)) {
          field = tf;
          break;
        }

      } while (tf != tn->fields_);
    }

    if (!field) {
      report_error(&desig->loc_, "field \"%s\" not found in struct or union type", desig->u_.ident_str_);
      return -1;
    }
  }
  else {
    /* First field .. */
    field = tn->fields_->chain_;
  }

  if ((tn->kind_ == tk_union) ||
      (field == tn->fields_->chain_ /* field is the first */)) {
    if (base_offset) {
      base_offset->refs_++;
      current_offset = base_offset;
    }
    else {
      current_offset = NULL;
    }
  }
  else {
    struct expr *field_offset;
    field_offset = type_node_struct_offset_expr(cc, tn, field);
    if (!field_offset) {
      /* Error occurred, which has been reported */
      return -1;
    }

    if (base_offset) {
      current_offset = expr_alloc(et_add);
      if (!current_offset) {
        expr_free(field_offset);
        cc_no_memory(cc);
        return -1;
      }

      current_offset->children_[0] = base_offset;
      base_offset->refs_++;
      current_offset->children_[1] = field_offset;
    }
    else {
      current_offset = field_offset;
    }
  }

  if (desig && (desig != (*pptiln)->designators_)) {
    /* Designator has not completed navigation, recurse until we do. When we come out, we continue at the next array offset */
    desig = desig->chain_;
  }
  else {
    desig = NULL;
  }

  for (;;) {
    if (templ_initializer_iter(cc, field->type_, current_offset, desig, pptiln, last_tiln, ppini_chain, NULL)) {
      expr_free(current_offset);
      return -1;
    }
    
    if (!*pptiln) {
      /* Reached end of initializer nodes in list */
      expr_free(current_offset);
      return 0;
    }

    desig = NULL;

    if (tn->kind_ == tk_union) {
      /* Exit here, a union is only a single field. */
      return 0;
    }

    do {
      if (field == tn->fields_) {
        /* Last field, exit here. */
        return 0;
      }
      field = field->chain_;
    } while (!field->ident_); /* skip unnamed fields. */

    struct expr *field_offset;
    field_offset = type_node_struct_offset_expr(cc, tn, field);
    if (!field_offset) {
      /* Error occurred, which has been reported */
      return -1;
    }

    if (base_offset) {
      current_offset = expr_alloc(et_add);
      if (!current_offset) {
        expr_free(field_offset);
        cc_no_memory(cc);
        return -1;
      }

      current_offset->children_[0] = base_offset;
      base_offset->refs_++;
      current_offset->children_[1] = field_offset;
    }
    else {
      current_offset = field_offset;
    }
  }

  
}

int templ_initializer_iter_array(struct c_compiler *cc, struct type_node *tn, struct expr *base_offset, 
                                 struct templ_initializer_designation *desig,
                                 struct templ_initializer_list_node **pptiln, struct templ_initializer_list_node *last_tiln,
                                 struct decl_initializer **ppini_chain,
                                 uint64_t *top_array_observed_length) {
  struct expr *current_offset = NULL;
  uint64_t counted_index;
  enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
  type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);
  tn = type_node_unqualified(tn);

  struct expr *size_expr = type_node_size_expr(cc, tn->derived_from_);
  if (!size_expr) {
    return -1; /* error already reported */
  }

  if (desig) {
    if (desig->kind_ != TINDK_INDEX) {
      report_error(&desig->loc_, "only index designators expected for array initialization");
      expr_free(size_expr);
      return -1;
    }

    struct expr *index = desig->u_.index_exp_;

    if (!index) {
      /* Some error occurred deep in parsing, this was already reported, fail transparently. */
      expr_free(size_expr);
      return -1;
    }
    index->refs_++; /* make sure desig can be freed without losing the index expr */

    struct expr *index_expr = expr_convert_arithmetic_type(cc, type_base_simple(&cc->tb_, cc->tb_.uintptr_equivalent_), index);
    if (!index_expr) {
      report_error(&desig->loc_, "unable to convert index type to pointer sized integer");
      expr_free(index); /* refs-- */
      expr_free(size_expr);
      return -1;
    }

    if (!expr_is_constant(index_expr)) {
      /* Expression not constant, this is an error (see 6.7.8 Initialization in particular the grammar.) */
      expr_free(index);
      expr_free(size_expr);
      return -1;
    }

    struct expr_temp index_result;
    int r = expr_eval(cc, &index_result, index_expr, 1, NULL, NULL, NULL, NULL);
    if (r) {
      expr_free(index);
      expr_free(size_expr);
      return -1;
    }

    counted_index = index_result.v_.u64_;
    struct type_node *index_type = type_node_unqualified(expr_type(cc, index));
    if (index_type && type_kind_is_signed(&cc->tb_, index_type->kind_)) {
      if (0 > (int64_t)counted_index) {
        report_error(&desig->loc_, "designator out of bounds");
        expr_free(index);
        expr_free(size_expr);
        return -1;
      }
    }


    if (tn->array_size_ && (counted_index >= tn->array_size_)) {
      report_error(&desig->loc_, "designator exceeds array size");
      expr_free(index);
      expr_free(size_expr);
      return -1;
    }
    
    struct expr *offset_expr = expr_alloc(et_mul);
    if (!offset_expr) {
      expr_free(size_expr);
      expr_free(index_expr);
      expr_free(current_offset);
      cc_no_memory(cc);
      return -1;
    }
    offset_expr->children_[0] = size_expr;
    offset_expr->children_[1] = index_expr;

    if (!base_offset) {
      current_offset = offset_expr;
    }
    else {
      current_offset = expr_alloc(et_add);
      if (!current_offset) {
        expr_free(offset_expr);
        expr_free(current_offset);
        cc_no_memory(cc);
        return -1;
      }

      current_offset->children_[0] = base_offset;
      base_offset->refs_++;
      current_offset->children_[1] = offset_expr;
    }

    if (desig != (*pptiln)->designators_) {
      /* Designator has not completed navigation, recurse until we do. When we come out, we continue at the next array offset */
      desig = desig->chain_;
      if (templ_initializer_iter(cc, type_node_unqualified(tn)->derived_from_, current_offset, desig, pptiln, last_tiln, ppini_chain, NULL)) {
        expr_free(current_offset);
        return -1;
      }
      struct expr *nxt = expr_alloc(et_add);
      if (!nxt) {
        cc_no_memory(cc);
        expr_free(current_offset);
        return -1;
      }
      current_offset->refs_++;
      size_expr->refs_++;
      nxt->children_[0] = current_offset;
      nxt->children_[1] = size_expr;
      current_offset = nxt;
    }
  }
  else {
    counted_index = 0;
    current_offset = base_offset;
    if (base_offset) {
      base_offset->refs_++;
    }
  }

  while (*pptiln) {
    if (templ_initializer_iter(cc, tn->derived_from_, current_offset, NULL, pptiln, last_tiln, ppini_chain, NULL)) {
      expr_free(size_expr);
      expr_free(current_offset);
      return -1;
    }

    if (top_array_observed_length) {
      if (*top_array_observed_length < (counted_index+1)) {
        *top_array_observed_length = counted_index + 1;
      }
    }

    if ((!*pptiln) || (*pptiln)->designators_) {
      /* Either we reached the end or we pop out to the top context where this designator originates from; not here.*/
      expr_free(size_expr);
      expr_free(current_offset);
      return 0;
    }

    counted_index++;
    if (tn->array_size_ && (counted_index >= tn->array_size_)) {
      /* Exceeding size of array; don't handle this literal here, but let parent handle it.
       * This is not an error (though it may become one if no parent caller can handle this.) */
      break;
    }

    /* Step to next element in array; we might be at offset 0 if current_offset is NULL */
    if (!current_offset) {
      size_expr->refs_++;
      current_offset = size_expr;
    }
    else {
      struct expr *nxt = expr_alloc(et_add);
      if (!nxt) {
        cc_no_memory(cc);
        expr_free(size_expr);
        expr_free(current_offset);
        return -1;
      }
      current_offset->refs_++;
      size_expr->refs_++;
      nxt->children_[0] = current_offset;
      nxt->children_[1] = size_expr;
      current_offset = nxt;
    }
  }

  /* ran out of literal initializer list nodes or exceeded array size; no longer need the /next/ offset. */
  expr_free(size_expr);
  expr_free(current_offset);

  return 0;
}


int templ_initializer_iter(struct c_compiler *cc, struct type_node *tn, struct expr *base_offset, 
                           struct templ_initializer_designation *desig,
                           struct templ_initializer_list_node **pptiln, struct templ_initializer_list_node *last_tiln,
                           struct decl_initializer **ppini_chain,
                           uint64_t *top_array_observed_length) {
  int r;
  tn = type_node_unqualified(tn);

  if (!desig) {
    /* Not designated, note that for any templ_initializer_iter invocation this can only happen once at the start,
     * as all other non-designated values are handled inside the struct_union or array iter functions (and any
     * others remaining do not fit.) */
    if ((tn->kind_ == tk_structure) || (tn->kind_ == tk_union)) {
      if ((*pptiln)->initializer_->kind_ == TINK_LITERAL) {
        /* Entire struct be single expression if the expression's type is compatible */
        struct type_node *et = expr_type(cc, (*pptiln)->initializer_->u_.literal_exp_);
        if (!et) {
          /* XXX: Distinguish between structural errors and memory conditions */
          report_error(&(*pptiln)->initializer_->loc_, "failed to determine type of expression");
          return -1;
        }
        et = type_node_unqualified(et);
        if (type_node_is_compatible(tn, et)) {
          /* Assignment of literal value */
          struct decl_initializer *ini = decl_initializer_alloc();
          if (!ini) {
            cc_no_memory(cc);
            return -1;
          }
          situs_swap(&ini->loc_, &(*pptiln)->initializer_->loc_);
          if (base_offset) {
            base_offset->refs_++;
            ini->offset_ = base_offset;
          }
          else {
            enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
            type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);
            ini->offset_ = expr_alloc(et_c);
            ini->offset_->v_.u64_ = 0;
          }
          ini->value_ = (*pptiln)->initializer_->u_.literal_exp_;
          if (ini->value_) {
            ini->value_->refs_++;
          }
          *ppini_chain = decl_initializer_join(*ppini_chain, ini);

          /* Step to next.. */
          if ((*pptiln) != last_tiln) {
            *pptiln = (*pptiln)->chain_;
          }
          else {
            *pptiln = NULL;
          }
          return 0;
        }
        else {
          /* Type node not compatible, there might be a deeper match */
          return templ_initializer_iter_struct_union(cc, tn, base_offset, NULL, pptiln, last_tiln, ppini_chain);
        }
      }
      else /* kind_ == TINK_COMPOUND */ {
        struct templ_initializer_list_node *compound_tail = (*pptiln)->initializer_->u_.compound_nodes_;
        struct templ_initializer_list_node *pp_compound_child = compound_tail->chain_;
        do {
          r = templ_initializer_iter_struct_union(cc, tn, base_offset, NULL, &pp_compound_child, compound_tail, ppini_chain);
          if (r) { 
            return r;
          }
        } while (pp_compound_child && pp_compound_child->designators_);
        if (pp_compound_child) {
          /* Literals should have been completely distributed out into the struct and any of its children,
           * if we end up with a literal here, then the compound contained too many elements. */
          report_error(&pp_compound_child->initializer_->loc_, "too many literals for struct-union initialization");
          return -1;
        }
        /* Step to next.. */
        if ((*pptiln) != last_tiln) {
          *pptiln = (*pptiln)->chain_;
        }
        else {
          *pptiln = NULL;
        }
        return 0;
      }
    }
    else if (tn->kind_ == tk_array) {
      if ((*pptiln)->initializer_->kind_ == TINK_LITERAL) {
        struct expr *lit = (*pptiln)->initializer_->u_.literal_exp_;
        struct type_node *string_lit_type = expr_type(cc, lit);
        if (!string_lit_type) {
          cc_no_memory(cc);
          return -1;
        }
        struct type_node *elm_type = type_node_unqualified(tn->derived_from_);
        /* regular char string literal or wide string literal? */
        if ((((elm_type->kind_ == tk_char) ||      (elm_type->kind_ == tk_unsigned_char) || (elm_type->kind_ == tk_signed_char)) && ((lit->et_ == ET_INDIRECTION_PTR) && lit->children_[0] && lit->children_[0]->et_ == ET_C_STRING_LIT)) || 
            (((elm_type->kind_ == tk_short_int) || (elm_type->kind_ == tk_unsigned_short_int))                                   && ((lit->et_ == ET_INDIRECTION_PTR) && lit->children_[0] && lit->children_[0]->et_ == ET_C_WIDE_STRING_LIT))) {
          struct type_node *string_lit_type = expr_type(cc, lit);
          uint64_t new_size;
          struct expr *new_array_lit = NULL;
          if (!tn->array_size_ || (tn->array_size_ == string_lit_type->array_size_)) {
            /* This literal matches the size of the type (either because the array type is incomplete
              * and will be patched up to the size actually used later, or because the length of the
              * array type matches the length of the string literal including null terminator.) */
            new_size = string_lit_type->array_size_;
          }
          else if (tn->array_size_ == (string_lit_type->array_size_ - 1)) {
            /* The length of the array type matches the length of the string literal *minus one*,
             * that is to say, minus the null terminator. This is permitted. */
            new_size = tn->array_size_;
          }
          else {
            /* Incompatible size */
            report_error(&(*pptiln)->initializer_->loc_, "Incompatible size of string literal and character array type");
            return -1;
          }

          if (top_array_observed_length && (new_size > *top_array_observed_length)) {
            *top_array_observed_length = new_size;
          }
          struct expr *value = NULL;
          if (new_size == string_lit_type->array_size_) {
            lit->refs_++;
            value = lit;
          }
          else {
            /* Re-cast the string literal to an array with length -1 (excl. null terminator) */
            struct type_node *desired_ptr_to_array_type;
            struct expr *array_size_type_conversion;
            desired_ptr_to_array_type = type_base_pointer(&cc->tb_, type_base_array_fixed_length(cc, elm_type, new_size));
            if (!desired_ptr_to_array_type) {
              /* Error already reported */
              return -1;
            }

            array_size_type_conversion = expr_alloc(ET_CVPTR2PTR);
            if (!array_size_type_conversion) {
              cc_no_memory(cc);
              return -1;
            }

            lit->children_[0]->refs_++;
            array_size_type_conversion->children_[0] = lit->children_[0] /* ET_C_STRING_LIT or ET_C_WIDE_STRING_LIT */;
            array_size_type_conversion->type_arg_ = desired_ptr_to_array_type;

            value = expr_alloc(ET_INDIRECTION_PTR);
            if (!value) {
              cc_no_memory(cc);
              expr_free(array_size_type_conversion);
              return -1;
            }
            value->children_[0] = array_size_type_conversion;
          }
          /* Assignment of literal value */
          struct decl_initializer *ini = decl_initializer_alloc();
          if (!ini) {
            cc_no_memory(cc);
            expr_free(value);
            return -1;
          }
          situs_swap(&ini->loc_, &(*pptiln)->initializer_->loc_);
          if (base_offset) {
            base_offset->refs_++;
            ini->offset_ = base_offset;
          }
          else {
            enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
            type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);
            ini->offset_ = expr_alloc(et_c);
            ini->offset_->v_.u64_ = 0;
          }
          ini->value_ = value;
          *ppini_chain = decl_initializer_join(*ppini_chain, ini);

          /* Step to next.. */
          if ((*pptiln) != last_tiln) {
            *pptiln = (*pptiln)->chain_;
          }
          else {
            *pptiln = NULL;
          }
          return 0;
        }
        return templ_initializer_iter_array(cc, tn, base_offset, NULL, pptiln, last_tiln, ppini_chain, top_array_observed_length);
      }
      else /* tink_ == TINK_COMPOUND */ {
        struct templ_initializer_list_node *compound_tail = (*pptiln)->initializer_->u_.compound_nodes_;
        struct templ_initializer_list_node *pp_compound_child = compound_tail->chain_;
        do {
          if (pp_compound_child->designators_) {
            r = templ_initializer_iter_array(cc, tn, base_offset, pp_compound_child->designators_->chain_, &pp_compound_child, compound_tail, ppini_chain, top_array_observed_length);
          }
          else {
            r = templ_initializer_iter_array(cc, tn, base_offset, NULL, &pp_compound_child, compound_tail, ppini_chain, top_array_observed_length);
          }
          
          if (r) {
            return r;
          }
        } while (pp_compound_child && pp_compound_child->designators_);
        if (pp_compound_child) {
          /* Literals should have been completely distributed out into the struct and any of its children,
           * if we end up with a literal here, then the compound contained too many elements. */
          report_error(&pp_compound_child->initializer_->loc_, "too many literals for array initialization");
          return -1;
        }
        /* Step to next.. */
        if ((*pptiln) != last_tiln) {
          *pptiln = (*pptiln)->chain_;
        }
        else {
          *pptiln = NULL;
        }
        return 0;
      }
      
    }
    else {
      /* scalar type */

      if ((*pptiln)->initializer_->kind_ == TINK_COMPOUND) {
        /* Scalar type with a compound initializer.. This *is* permitted, but requires the
         * compound to consist of only a single value; or another compound with the same
         * constraint. */
        struct templ_initializer_list_node *compound_tail = (*pptiln)->initializer_->u_.compound_nodes_;
        struct templ_initializer_list_node *pp_compound_child = compound_tail->chain_;
        r = templ_initializer_iter(cc, tn, base_offset, NULL, &pp_compound_child, compound_tail, ppini_chain, NULL);
        if (r) {
          return -1;
        }
        if (pp_compound_child) {
          report_error(&pp_compound_child->initializer_->loc_, "too many literals for scalar value");
          return -1;
        }
      }
      else /* kind_ == TINK_LITERAL */ {

        /* Assignment of literal value */
        struct decl_initializer *ini = decl_initializer_alloc();
        if (!ini) {
          cc_no_memory(cc);
          return -1;
        }
        situs_swap(&ini->loc_, &(*pptiln)->initializer_->loc_);
        if (base_offset) {
          base_offset->refs_++;
          ini->offset_ = base_offset;
        }
        else {
          enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
          type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);
          ini->offset_ = expr_alloc(et_c);
          ini->offset_->v_.u64_ = 0;
        }

        /* Ensure arrays are decayed to pointers, note that arrays initializers
         * will not get here (tn->kind_ would be tk_array). */
        if (expr_pointer_decay(cc, &(*pptiln)->initializer_->u_.literal_exp_)) {
          return -1;
        }

        ini->value_ = (*pptiln)->initializer_->u_.literal_exp_;
        if (ini->value_) {
          ini->value_->refs_++;
        }
        *ppini_chain = decl_initializer_join(*ppini_chain, ini);
      }
      /* Step to next.. */
      if ((*pptiln) != last_tiln) {
        *pptiln = (*pptiln)->chain_;
      }
      else {
        *pptiln = NULL;
      }
      return 0;
    }
  }
  else /* more designators to process */ {
    if ((tn->kind_ == tk_structure) || (tn->kind_ == tk_union)) {
      return templ_initializer_iter_struct_union(cc, tn, base_offset, desig, pptiln, last_tiln, ppini_chain);
    }
    else if (tn->kind_ == tk_array) {
      return templ_initializer_iter_array(cc, tn, base_offset, desig, pptiln, last_tiln, ppini_chain, top_array_observed_length);
    }
    else {
      report_error(&desig->loc_, "designator cannot be applied to scalar type");
      return -1;
    }
  }
}

struct decl_initializer *templ_initializer_realize(struct c_compiler *cc, struct templ_initializer *tin, struct type_node *tn, uint64_t *top_array_observed_length) {
  struct decl_initializer *di = NULL;
  int r;
  struct templ_initializer_list_node dummy;
  dummy.chain_ = &dummy;
  dummy.designators_ = NULL;
  dummy.initializer_ = tin;
  struct templ_initializer_list_node *pp_tiln = &dummy;
  struct templ_initializer_list_node *pp_tail = &dummy;

  if (top_array_observed_length) *top_array_observed_length = 0;

  r = templ_initializer_iter(cc, tn, NULL, NULL, &pp_tiln, pp_tail, &di, top_array_observed_length);
  if (r) {
    decl_initializer_free(di);
    return NULL;
  }

  return di;
}
