#ifndef LIBEMBD_PLATFORM_TYPES_H_
#define LIBEMBD_PLATFORM_TYPES_H_

/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 *********************************************************************************************************************/
#define CPU_TYPE_8       (8u)
#define CPU_TYPE_16      (16u)
#define CPU_TYPE_32      (32u)
#define CPU_TYPE_64      (64u)

#ifdef __PRQA__
# ifndef TRUE
#  define TRUE          (_Bool)1
# endif

# ifndef FALSE
#  define FALSE         (_Bool)0
# endif
#else
# ifndef TRUE
#  define TRUE (1u)
# endif

# ifndef FALSE
#  define FALSE (0u)
# endif
#endif /* PRQA */

#if defined(_M_IX86)
  /* Visual C++ compiler targeting 32 bit code for x86 */
# define CPU_TYPE         CPU_TYPE_32
#elif defined(_M_AMD64)
  /* Visual C++ compiler targeting 64 bit code for x64 */
# define CPU_TYPE         CPU_TYPE_64
#elif defined(__x86_64__)
  /* gcc or clang compiler targeting 64 bit code for x64 */
# define CPU_TYPE         CPU_TYPE_64
#elif defined(__i386__)
  /* gcc or clang compiler targeting 32 bit code for x86 */
# define CPU_TYPE         CPU_TYPE_32
#else
 #error "Unknown processor architecture"
#endif

#define CPU_BIT_ORDER    LSB_FIRST       /*little endian bit ordering*/

#define CPU_BYTE_ORDER   LOW_BYTE_FIRST  /*little endian byte ordering*/

/* data type limitation defines */
#define UINT8_MIN        0u
#if !defined UINT8_MAX
# define UINT8_MAX        255u
#endif /* !defined UINT8_MAX */
#define UINT16_MIN       0u
#if !defined UINT16_MAX
#define UINT16_MAX       65535u
#endif /* !defined UINT16_MAX */
#define UINT32_MIN       0ul
#if !defined UINT32_MAX
#define UINT32_MAX       4294967295ul
#endif /* !defined UINT32_MAX */
#define UINT64_MIN       0ull
#if !defined UINT64_MAX
#define UINT64_MAX       18446744073709551615ull
#endif /* !defined UINT64_MAX */

#define SINT8_MIN        -128
#define SINT8_MAX        127
#define SINT16_MIN       -32768
#define SINT16_MAX       32767
#define SINT32_MIN       -2147483648
#define SINT32_MAX       2147483647
#define SINT64_MIN       -9223372036854775808
#define SINT64_MAX       9223372036854775807

#define FLOAT32_MIN      1.17549435e-38f
#define FLOAT32_MAX      3.402823466e+38f
#define FLOAT32_EPSILON  1.19209290e-07f

#define FLOAT64_MIN       2.2250738585072014e-308l
#define FLOAT64_MAX       1.7976931348623157e+308l
#define FLOAT64_EPSILON   2.2204460492503131e-16l

/**********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/
#ifdef __PRQA__
typedef _Bool boolean;
#else
typedef unsigned char       boolean;        /* for use with TRUE/FALSE        */
#endif  /* __PRQA__ */

typedef signed char           sint8;         /*        -127 .. +127            */
typedef unsigned char         uint8;         /*           0 .. 255             */
typedef signed short          sint16;        /*      -32767 .. +32767          */
typedef unsigned short        uint16;        /*           0 .. 65535           */
typedef signed int            sint32;        /* -2147483647 .. +2147483647     */
typedef unsigned int          uint32;        /*           0 .. 4294967295      */

typedef signed char           sint8_least;   /* At least 7 bit + 1 bit sign    */
typedef unsigned char         uint8_least;   /* At least 8 bit                 */
typedef signed short          sint16_least;  /* At least 15 bit + 1 bit sign   */
typedef unsigned short        uint16_least;  /* At least 16 bit                */
typedef signed int            sint32_least;  /* At least 31 bit + 1 bit sign   */
typedef unsigned int          uint32_least;  /* At least 32 bit                */

typedef float                 float32;
typedef double                float64;

#define PLATFORM_SUPPORT_SINT64_UINT64
#if defined(_WIN32) && (defined(_MSC_VER) || defined (RC_INVOKED) || defined(__MINGW32__) || defined(__MINGW64__))
  /* Visual Studio C++ compiler, Visual Studio resource compiler or Mingw-w64 on Windows platform */
  typedef signed long long    sint64;
  typedef unsigned long long  uint64;
#elif defined(__linux__) && (defined(__clang__) || defined(__GNUC__))
  /* clang or gcc compiler on Linux platform */
  typedef signed long         sint64;
  typedef unsigned long       uint64;
#else
  #error "Unknown compiler"
#endif

typedef void* VoidPtr;
typedef const void* ConstVoidPtr;

/**********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

#endif /* LIBEMBD_PLATFORM_TYPES_H_ */

/**********************************************************************************************************************
 *  END OF FILE: Platform_Types.h
 *********************************************************************************************************************/
