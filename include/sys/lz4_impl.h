#include <sys/zfs_context.h>

/**************************************
*  CPU Feature Detection
**************************************/
/*
 * LZ4_FORCE_SW_BITCOUNT
 * Define this parameter if your target system or compiler does not support hardware bit count
 */
/*
 * Illumos : we can't use GCC's __builtin_ctz family of builtins in the
 * kernel
 * Linux : we can use GCC's __builtin_ctz family of builtins in the
 * kernel
 */
#undef	LZ4_FORCE_SW_BITCOUNT
#if defined(__sparc)
#define	LZ4_FORCE_SW_BITCOUNT
#endif


/**************************************
*  Compiler Options
**************************************/
#define LZ4_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

#if (LZ4_GCC_VERSION >= 302) || (__INTEL_COMPILER >= 800) || defined(__clang__)
#  define expect(expr,value)    (__builtin_expect ((expr),(value)) )
#else
#  define expect(expr,value)    (expr)
#endif

#ifndef likely
#  define likely(expr)     expect((expr) != 0, 1)
#endif
#ifndef unlikely
#  define unlikely(expr)   expect((expr) != 0, 0)
#endif


/**************************************
*  Basic Types
**************************************/
typedef  uint8_t BYTE;
typedef uint16_t U16;
typedef uint32_t U32;
typedef  int32_t S32;
typedef uint64_t U64;


/**************************************
*  Common Constants
**************************************/
#define MINMATCH 4

#define COPYLENGTH 8
#define LASTLITERALS 5
#define MFLIMIT (COPYLENGTH+MINMATCH)

#define MAXD_LOG 16
#define MAX_DISTANCE ((1 << MAXD_LOG) - 1)

#define ML_BITS  4
#define ML_MASK  ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)
