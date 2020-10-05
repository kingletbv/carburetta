#ifndef MUL_H
#define MUL_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int multiply_uint64(uint64_t a, uint64_t b, uint64_t *rhi, uint64_t *rlo);
int multiply_size_t(size_t a, size_t b, size_t *rhi, size_t *rlo);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MUL_H */
