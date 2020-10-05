/* Copyright (C) 2009 Martijn Boekhorst
 * All rights reserved.
 *
 * $Id$
 */

#ifndef CHAIN_H
#define CHAIN_H

/* Chain "container" support. A chain is a cyclic linked list whereby a pointer to the last element is used to
 * reference it. This has a few benefits; insertion into head, appending to tail, retrieving the first element
 * finding the next element and retrieving the last element are all O(1) operations, and only a single pointer 
 * is used in the structure being chained. Removing an element in the middle of a chain and finding the previous
 * element of a chain, on the other hand, are very expensive O(n) operations.
 * To use this, declare a chain pointer (of arbitrary name) somewhere in the structure to be chained; this chain
 * pointer needs to be mentioned specifically for all operations (as a single structure may appear in multiple 
 * different chains.)
 * For efficiency, we rely on availability of an optimizing linker to inline the functions.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Given a structure and a member in that structure, returns the offset of the member in the structure; used
 * as a common argument to the chain functions. */
#define CHAIN_MEMBER(structtype, member)\
  ((size_t)(&((structtype*)0)->member))

/* Given a chain, returns the first element in the chain.
 * chain: the chain's tail pointer.
 * member: offset from the start of the structure to the chain member pointer.
 * returns: the first element in the chain, or NULL if the chain is empty.
 */
void *chain_first(void *chain, size_t member);

/* Given a chain, returns the last element in the chain. This is effectively a no-op but may be useful for 
 * clarity's sake.
 * chain: the chain's tail pointer.
 * member: offset from the start of the structure to the chain member pointer.
 */
void *chain_last(void *chain, size_t member);

/* Given a chain, removes and returns the first element in the chain, or NULL if there is no head.
 * pchain: a pointer to the chain's tail pointer. If needed, the pointer will be updated.
 * member: offset from the start of the structure to the chain member pointer.
 * returns: the element popped from the start of the chain, or NULL if the chain was empty.
 */
void *chain_pop(void **pchain, size_t member);

/* Given a chain and a pointer to a structure that is in the chain, finds the next structure in the chain,
 * or returns NULL if the structure passed is the last in the chain. As a convenience to start iteration
 * of the chain, NULL may be passed for prev, in which case the first element in the chain will be returned.
 * chain: the chain's tail pointer.
 * member: offset from the start of the structure to the chain member pointer.
 * prev: pointer to the structure for which we would like to find the next structure in sequence, or NULL
 *       to find the first structure in the sequence.
 *
 * Example: Iteration code:
 * struct my_list { struct my_list *chain; }
 * { struct my_list *tail = <.. some tail pointer to a chain we wish to iterate ..>;
 *   struct my_list *current = NULL; // initialized to NULL gets us the first on initial invocation.
 *   while (current = chain_next(tail, CHAIN_MEMBER(struct my_list, chain), current)) {
 *     // Do stuff w. current.
 *   }
 * }
 */
void *chain_next(void *chain, size_t member, void *prev);

/* Given two chains, appends them together; this operation is always O(1).
 * front: the front chain, its elements will appear first, retaining their sequence.
 * back: the back chain, its elements will appear after the front, retaining their sequence.
 * member: offset from the start of the structure to the chain member pointer.
 * returns: the new tail pointer to the chain.
 */
void *chain_append(void *front, void *back, size_t member);

/* Given a chain, reverses all elements. This is O(n).
 * pchain: a pointer to the chain's tail pointer. If needed (eg. len(chain)>1) the pointer will be
 *         updated.
 * member: offset from the start of the structure to the chain member pointer.
 * returns: no return value, but all elements in *pchain will be reversed in sequence.
 */
void chain_reverse(void **pchain, size_t member);

/* Given a chain, counts the number of elements. This is O(n).
 * chain: a pointer to the chain's tail pointer.
 * returns: the number of distinct elements in the chain. 
 */
size_t chain_length(void *chain, size_t member);

/* Sets the member pointer to the element itself, thus making a chain of one element.
 * element: element structure whose member pointer is uninitialized.
 * member: offset from the start of the structure to the element member pointer.
 * returns: no return value, but the element can now be used as a tail pointer.
 */
void chain_init(void *element, size_t member);

/* Macro versions, these perform casting around the core functions above, and determine the
 * member pointer offset for you.
 * The macro's are identical in use to the functions except where a size_t member appears
 * in the functions, the type of the element structure and its member field that contains the
 * chain pointer are now expected. */
#define CHAIN_FIRST(chain, struct_type, member)\
  ((struct_type *)chain_first((void *)chain, CHAIN_MEMBER(struct_type, member)))

#define CHAIN_LAST(chain, struct_type, member)\
  ((struct_type *)chain_last((void *)chain, CHAIN_MEMBER(struct_type, member)))

#define CHAIN_POP(pchain, struct_type, member)\
  ((struct_type *)chain_pop((void **)pchain, CHAIN_MEMBER(struct_type, member)))

#define CHAIN_NEXT(chain, struct_type, member, prev)\
  ((struct_type *)chain_next((void *)chain, CHAIN_MEMBER(struct_type, member), (void *)prev))

#define CHAIN_APPEND(front, back, struct_type, member)\
  ((struct_type *)chain_append((void *)front, (void *)back, CHAIN_MEMBER(struct_type, member)))

#define CHAIN_LENGTH(chain, struct_type, member)\
  chain_length((void *)chain, CHAIN_MEMBER(struct_type, member))

#define CHAIN_REVERSE(pchain, struct_type, member)\
  chain_reverse((void **)pchain, CHAIN_MEMBER(struct_type, member))

#define CHAIN_INIT(element, struct_type, member)\
  chain_init((void *)element, CHAIN_MEMBER(struct_type, member))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CHAIN_H */
