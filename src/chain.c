/* Copyright 2020 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http ://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h> /* size_t, NULL */

#include "chain.h"

/* Returns the 'chain' pointer from a member offset and element pointer. 
 * Because of the indirection, it is returned as an lvalue, eg. the 
 * following code is legal:
 * CHAIN_GET_PTR_LVALUE(elm,member) = NULL;
 */
#define CHAIN_GET_PTR_LVALUE(element, member)\
  (*(void**)(((char *)element)+member))

void *chain_first(void *chain, size_t member) {
  if (!chain) return NULL;
  return CHAIN_GET_PTR_LVALUE(chain, member);
}

void *chain_last(void *chain, size_t member) {
  member; /* C4100: 'member' : unreferenced formal parameter */
  return chain;
}

void *chain_pop(void **pchain, size_t member) {
  void *head;
  if (!*pchain) return NULL;
  head = CHAIN_GET_PTR_LVALUE(*pchain, member);
  if (head == *pchain) {
    /* first element is also the last, set to NULL, chain
     * is now empty. */
    *pchain = NULL;
    return head;
  }
  /* Stitch tail's successor to point to successor of previous head,
   * then make head a little chain of itself. */
  CHAIN_GET_PTR_LVALUE(*pchain, member) = CHAIN_GET_PTR_LVALUE(head, member);
  CHAIN_GET_PTR_LVALUE(head, member) = head;
  return head;
}

void *chain_next(void *chain, size_t member, void *prev) {
  if (!prev) return chain_first(chain, member);
  if (prev == chain) return NULL;
  return CHAIN_GET_PTR_LVALUE(prev, member);
}

void *chain_append(void *front, void *back, size_t member) {
  void *front_head, *front_tail, *back_head, *back_tail;
  if (!front) return back;
  if (!back) return front;
  front_head = chain_first(front, member);
  front_tail = chain_last(front, member);
  back_head = chain_first(back, member);
  back_tail = chain_last(back, member);
  CHAIN_GET_PTR_LVALUE(back_tail, member) = front_head;
  CHAIN_GET_PTR_LVALUE(front_tail, member) = back_head;
  return back_tail; /* next(back_tail) is front_head, eg. it is the proper tail ptr. */
}

size_t chain_length(void *chain, size_t member) {
  void *elm;
  size_t nr_elems = 0;
  for (elm = chain_first(chain, member);
       elm;
       elm = chain_next(chain, member, elm)) {
    nr_elems++;
  }
  return nr_elems;
}

void chain_reverse(void **pchain, size_t member) {
  void *new_chain = NULL;
  void *elm;
  /* Akward for() construct to avoid C4706: assignment within conditional expression */
  for (elm = chain_pop(pchain, member);
       elm;
       elm = chain_pop(pchain, member)) {
    new_chain = chain_append(elm, new_chain, member);
  }
  *pchain = new_chain;
}

void chain_init(void *element, size_t member) {
  CHAIN_GET_PTR_LVALUE(element, member) = element;
}
