#ifndef VERSION_H
#define VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define CARBURETTA_MAJOR_VER 0
#define CARBURETTA_MINOR_VER 8
#define CARBURETTA_PATCH_VER 11

#define CARBURETTA_STRINGIFY_i(x) #x
#define CARBURETTA_STRINGIFY(x) CARBURETTA_STRINGIFY_i(x)

#if CARBURETTA_PATCH_VER & 1
// Odd patch version means development version
#define CARBURETTA_VERSION_STR CARBURETTA_STRINGIFY(CARBURETTA_MAJOR_VER) "." CARBURETTA_STRINGIFY(CARBURETTA_MINOR_VER) "." CARBURETTA_STRINGIFY(CARBURETTA_PATCH_VER) " (development)"
#else
#define CARBURETTA_VERSION_STR CARBURETTA_STRINGIFY(CARBURETTA_MAJOR_VER) "." CARBURETTA_STRINGIFY(CARBURETTA_MINOR_VER) "." CARBURETTA_STRINGIFY(CARBURETTA_PATCH_VER)
#endif

#define CARBURETTA_COPYRIGHT_STR "(C) 2020-2023 Kinglet B.V."

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VERSION_H */
