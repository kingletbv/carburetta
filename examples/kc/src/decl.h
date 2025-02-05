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

#ifndef DECL_H
#define DECL_H

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef STMT_H_INCLUDED
#define STMT_H_INCLUDED
#include "stmt.h"
#endif

#ifndef NAME_SPACE_H_INCLUDED
#define NAME_SPACE_H_INCLUDED
#include "name_space.h"
#endif

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifndef DATA_SECTION_H_INCLUDED
#define DATA_SECTION_H_INCLUDED
#include "data_section.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct c_compiler;

#define DECL_FUNCTION_SPECIFIER_INLINE 1

typedef enum sc_storage_class {
  SC_NONE,
  SC_TYPEDEF,
  SC_EXTERN,
  SC_STATIC,
  SC_AUTO,
  SC_REGISTER
} sc_storage_class_t;

typedef enum sc_relocation_type {
  /* Absolute 64 bit pointer */
  SCRT_ABSOLUTE_64,

  /* Absolute 32 bit pointer */
  SCRT_ABSOLUTE_32,

  /* Instruction pointer relative 64 bit pointer
   * (Relative to the final location of the destination) */
  SCRT_RIP_REL_64,

  /* Instruction pointer relative 32 bit pointer
   * (Relative to the final location of the destination) */
  SCRT_RIP_REL_32,
} sc_relocation_type_t;

/* Reference to a realized variable corresponding to a declaration. See decl::refs_ */ 
struct decl_ref {
  /* Location of reference */
  struct ds_portion *dsp_;
  
  /* Offset of reference */
  size_t offset_;

  /* Relocation type of reference */
  sc_relocation_type_t rt_;

  /* Non-zero if the destination has been realized (e.g. the location was performed.)
   * If so, then the pointer to the decl's storage (e.g. decl::ds_ or decl::external_)
   * has been added to whatever value was there previously. */
  int realized_:1;
};

struct decl {
  /* Symbol housing both identifier (sym_.ident_) of decl and linkage into a symbol table */
  struct sym sym_;

  /* Storage class of the declaration */
  sc_storage_class_t sc_;

  /* Function specifiers, combination of DECL_FUNCTION_SPECIFIER_XXX flags */
  int fs_;

  /* Chain of sibling declarations */
  struct decl *chain_;

  /* Location of definition */
  struct situs def_loc_;

  /* Global forward; see comments at hidden_global_, this is a local scope declaration that
   * forwards to the actual declaration at global scope. That actual declaration may or may
   * not be hidden from the global scope using the hidden_global_ flag. */
  struct decl *global_forward_;

  /* C type of the declaration */
  struct type_node *type_;

  /* Initializer of the declaration, if any. */
  struct decl_initializer *init_;

  /* Data section portion storage, if any. */
  struct ds_portion *dsp_;

  /* External pointer value to data, if any. */
  void *external_;

  /* If non-zero, this is an external (has external linkage.) If external_ is NULL, then it is an
   * unresolved external. */
  int is_external_:1;

  /* If non-zero, this is an internal (has internal linkage.) */
  int is_internal_:1;

  /* Nonzero if this is a parameter */
  int is_param_ : 1;


  /* If non-zero, this is a tentative definition */
  int is_tentative_definition_:1;

  /* If non-zero, this is a definition */
  int is_definition_:1;

  /* If non-zero, this is a declaration (and needs later linkage to resolve.) */
  int is_declaration_:1;

  /* If non-zero, this is a typedef */
  int is_typedef_:1;

  /* If non-zero, this is a hidden global (static or extern); happens for instance:
   *   void f(void) {
   *     extern int x;
   *   }
   *   void g(void) {
   *     extern int x;
   *   }
   *   void h(void) {
   *     x++; // Cannot find identifier 'x'
   *   }
   *   int x = 3;
   * In this case, the first "extern int x" declares a hidden global, the second 
   * "extern int x" should find this global and link to it, and the third "int x = 3"
   * converts the hidden global from declaration to definition. Note however that
   * the function h() will fail to compile because 'x' is not visible in h(), even 
   * though it appears in the global namespace, the is_hidden_global_ flag prevents
   * it from being seen until defined by "int x = 3"
   */
  int is_hidden_global_:1;

  /* Nonzero if this is an enum constant; actual int value in enum_value_ */
  int is_enum_:1;

  /* Non-zero if this is a local (automatic) variable */
  int is_local_:1;

  /* Nonzero if this value is passed by pointer rather than directly represented
   * (for instance, struct as a parameter value may be allocated by the caller and
   * passed as a pointer, depending on the ABI.) */
  int is_passed_by_pointer_:1;

  /* Offset from the stackpointers entry value if this is a parameter */
  uint64_t param_offset_;

  /* Offset on the stackframe if this declaration is a local variable inside a function */
  uint64_t local_offset_;

  /* Constant value if is_enum_==1 (type is "int", not int64 but depends on type_base::bits_per_int_) */
  int64_t enum_value_;

  /* Number that starts at 0 and increments for each global declaration that needs to be dis-ambiguated
   * from the first declaration that matched.
   * For instance:
   * void f(void) {
   *   static int x; // first x
   *   {
   *      static int x; // second x
   *   }
   * }
   * Here we declare two static variables with the same name, but they are in different scopes, so they
   * are different variables. At the global level, these both will be named "f$x" - which is incorrect as
   * they are not the same variable. Consequently, the disambiguation_ordinal on the decl for "f$x" is
   * used to number the next variable "f$x$1" and so on. */
  int disambiguation_ordinal_;

  /* Final object references to this declaration */
  size_t num_refs_;
  size_t num_refs_allocated_;
  struct decl_ref *refs_;
};

struct decl_initializer {
  /* Cyclic chain of values to initialize */
  struct decl_initializer *chain_;

  /* Offset to the value from the base of the variable */
  struct expr *offset_;

  /* Value to initialize with */
  struct expr *value_;

  /* Location of the initializer, for reporting errors if any */
  struct situs loc_;
};

struct decl *decl_alloc(struct name_space *ns, int *already_exists, const char *ident, struct situs *ident_loc, sc_storage_class_t sc, int function_specifiers, struct type_node *tn, struct decl_initializer *init);

struct decl *decl_alloc_sized(struct name_space *ns, int *already_exists, const char *ident, struct situs *ident_loc, 
                              sc_storage_class_t sc, int function_specifiers, struct type_node *tn, struct decl_initializer *init,
                              size_t full_decl_size);

void decl_free(struct symtab *st, struct decl *d);

struct decl_ref *decl_add_ref(struct decl *d, sc_relocation_type_t reloc_type, struct ds_portion *dsp, size_t offset);

struct decl_initializer *decl_initializer_alloc(void);
void decl_initializer_free(struct decl_initializer *in_chain);
struct decl_initializer *decl_initializer_join(struct decl_initializer *front, struct decl_initializer *back);
int decl_static_initializer_exec(struct c_compiler *cc, struct ds_portion *dsp, size_t offset, struct decl_initializer *di, void *local_base, void *param_base, void *return_value_ptr);
int decl_initializer_as_expr(struct c_compiler *cc, struct expr *target_ptr, struct expr **ppexpr, struct decl_initializer *di);

struct decl *decl_join(struct decl *front, struct decl *back);

void decl_print(FILE *fp, struct decl *d);

int decl_realize_global(struct c_compiler *cc, struct decl *d, struct data_section *ds);

/* if ns != global_ns, then this is not in the global namespace, if !is_global, then this is a local variable.
 * If REPL or template mode, then is_global need not imply ns == global_ns as we might be in an inner block stmt
 * at the file level (e.g. a for loop or whatnot at the file level - obviously not permitted for C99 but we permit
 * it for REPL and template modes.) */
struct decl *decl_create_update(struct c_compiler *cc, struct type_node *tn, struct name_space *ns, struct name_space *global_ns, 
                                const char *ident, struct situs *ident_loc,
                                sc_storage_class_t sc, int function_specifiers, int has_initializer, int is_global);

int decl_resolve_all_global_decl_relocs(struct c_compiler *cc);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DECL_H */
