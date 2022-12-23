#ifndef _CUDA_OUTILS_H
#define _CUDA_OUTILS_H

#include "outils.h"

#ifndef CUDAMALLOC
#define CUDAMALLOC(CUDAPTR, SIZE, ERR)                   \
    do {                                                 \
        (ERR) = cudaMalloc((void **)&(CUDAPTR), (SIZE)); \
        if ((ERR) != cudaSuccess) {                      \
            dbgerr(                                      \
                "Failed to alloc memory in dev."         \
                "Error code: %s.\n",                     \
                cudaGetErrorString((ERR))                \
            );                                           \
            exit(EXIT_FAILURE);                          \
        }                                                \
    } while (0)
#endif

#ifndef CUDAFREE
#define CUDAFREE(CUDAPTR, ERR)                  \
    do {                                        \
        ERR = cudaFree(CUDAPTR);                \
                                                \
        if (ERR != cudaSuccess) {               \
            dbgerr(                             \
                "Failed to free Cuda pointer."  \
                " Error code %s.\n",            \
                cudaGetErrorString(ERR)         \
            );                                  \
            exit(EXIT_FAILURE);                 \
        } else {                                \
            CUDAPTR = NULL;                     \
        }                                       \
    } while (0)

#endif


#endif