#ifndef LIBEMBD_COMMON_H_
#define LIBEMBD_COMMON_H_

#include <string.h> //for memcpy
#include <stdio.h> //for sprintf
#include <Std_Types.h>
#include "Platform_Types.h"

//! please make sure the following macros are correctly configured!
/*--------------------------------------------------- Macro Configurations--------------------------------------------------------*/
// Compile-time processor singularity check
#define LIBEMBD_IS_SINGLE_CORE_ONLY 1

#define LIBEMBD_CACHE_LINE_SIZE     64
/*--------------------------------------------------- Macro Configurations--------------------------------------------------------*/

// Compile-time endianness check
#define LIBEMBD_ENDIANNESS_BIG_ENDIAN 0
#define LIBEMBD_ENDIANNESS_LITTLE_ENDIAN 1

#ifdef __LITTLE_ENDIAN__
    #define LIBEMBD_HOST_ENDIANNESS  LIBEMBD_ENDIANNESS_LITTLE_ENDIAN 
#elif defined __BIG_ENDIAN__
    #define LIBEMBD_HOST_ENDIANNESS  LIBEMBD_ENDIANNESS_BIG_ENDIAN 
#else
    #error Unable to detect host endianness!
#endif

#define LIBEMBD_API extern
#define LIBEMBD_INTERNAL_GLOBAL extern
#define LIBEMBD_INTERNAL_API extern
#define LIBEMBD_CALLBACK extern
#define LIBEMBD_LOCAL static
#define LIBEMBD_LOCAL_INLINE static inline
#define LIBEMBD_HEADER_API static
#define LIBEMBD_HEADER_API_INLINE static inline
#define LIBEMBD_INTERNAL
#define LIBEMBD_ATTR_NO_INLINE      __attribute__((noinline))
#define LIBEMBD_ATTR_ALWAYS_INLINE  __attribute__((always_inline))
#define LIBEMBD_ATTR_PACKED         __attribute__((packed))
#define LIBEMBD_ALIGNAS(alignment)  __attribute__((aligned(alignment)))

#if defined(__GNUC__) || defined(__clang__)
    #define LIBEMBD_DISABLE_WARNING_PUSH _Pragma("GCC diagnostic push")
    #define LIBEMBD_DISABLE_WARNING_POP _Pragma("GCC diagnostic pop")
    #define LIBEMBD_DISABLE_WARNING(warning_name) _Pragma(#warning_name)
#elif defined(_MSC_VER)
    #define LIBEMBD_DISABLE_WARNING_PUSH __pragma(warning(push))
    #define LIBEMBD_DISABLE_WARNING_POP __pragma(warning(pop))
    #define LIBEMBD_DISABLE_WARNING(warning_number) __pragma(warning(disable : warning_number))
#elif defined(__ghs__)
    #define LIBEMBD_DISABLE_WARNING_PUSH _Pragma("GHS diagnostic push")
    #define LIBEMBD_DISABLE_WARNING_POP _Pragma("GHS diagnostic pop")
    #define LIBEMBD_DISABLE_WARNING(warning_number) _Pragma("GHS diagnostic nowarning " #warning_number)
#else
    #define LIBEMBD_DISABLE_WARNING_PUSH
    #define LIBEMBD_DISABLE_WARNING_POP
    #define LIBEMBD_DISABLE_WARNING(warning_name)
#endif

#if defined(__GNUC__) && __GNUC__ >= 7
    #define LIBEMBD_ATTR_FALLTHROUGH __attribute__((fallthrough))
#elif defined(__clang__) && __clang_major__ >= 3
    #define LIBEMBD_ATTR_FALLTHROUGH [[clang::fallthrough]]
#else
    #define LIBEMBD_ATTR_FALLTHROUGH ((void)0)
#endif

#define LIBEMBD_IGNORE_PARAM(param) ((void)param)
#define LIBEMBD_MEMCPY(dest, src, size) memcpy(dest, src, size)
#define LIBEMBD_MEMCMP(buf1, buf2, size) memcmp(buf1, buf2, size)
#define LIBEMBD_MEMSET(dest, val, size) memset(dest, val, size)
#define LIBEMBD_MEMMOVE(dest, src, len) memmove(dest, src, len)
#define LIBEMBD_SPRINTF sprintf

#if (__STDC_VERSION__ >= 201112L)
    #include <assert.h>
    #define LIBEMBD_STATIC_ASSERT _Static_assert
#else
    #define LIBEMBD_STATIC_ASSERT(expr, msg) typedef char appcore_static_assert_##__FILE__##__LINE__[(expr) ? 1 : -1]
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define LIBEMBD_UNREACHABLE() __builtin_unreachable()
#else
    #define LIBEMBD_UNREACHABLE() (void)0
#endif

#define LIBEMBD_ASSERT(expr) assert(expr)

/// Optimizer allowed to assume that EXPR evaluates to true
#define LIBEMBD_ASSUME(expr) ((void)((expr) ? (void)0 : LIBEMBD_UNREACHABLE()))

/// Expect asserts the condition in debug builds and assumes the condition to be
/// true in release builds.
#ifdef NDEBUG
    #define LIBEMBD_EXPECT(EXPR) LIBEMBD_ASSUME(EXPR)
#else
    #define LIBEMBD_EXPECT(EXPR) LIBEMBD_ASSERT(EXPR)
#endif

#define LIBEMBD_ENABLE_DEV_ERROR_CHECK          STD_ON

typedef uint32 LibEmbd_Size_t;

typedef struct {
    const uint8* data;
    uint16 length;
} LibEmbd_ConstBufferView_t;

typedef struct {
    uint8* data;
    uint16 length;
} LibEmbd_MutableBufferView_t;

#endif /* LIBEMBD_COMMON_H_ */
