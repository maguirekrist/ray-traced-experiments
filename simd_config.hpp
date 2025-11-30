// simd_config.h
#pragma once

#if defined(__x86_64__) || defined(_M_X64) || \
    defined(__i386__)   || defined(_M_IX86)
    #define HAVE_X86_SIMD 1
    #include <immintrin.h>   // or <xmmintrin.h>, <emmintrin.h>, etc.
#else
    #define HAVE_X86_SIMD 0
#endif

