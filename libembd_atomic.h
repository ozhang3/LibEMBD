#ifndef LIBEMBD_ATOMIC_H_
#define LIBEMBD_ATOMIC_H_

#include "libembd/libembd_platform_types.h"
#include "libembd/libembd_common.h"
#include "libembd/libembd_util.h"

#define __LIBEMBD_ATOMIC_RELAXED 0
#define __LIBEMBD_ATOMIC_CONSUME 1
#define __LIBEMBD_ATOMIC_ACQUIRE 2
#define __LIBEMBD_ATOMIC_RELEASE 3
#define __LIBEMBD_ATOMIC_ACQ_REL 4
#define __LIBEMBD_ATOMIC_SEQ_CST 5

// Memory order definitions (mimicking C11)
typedef enum {
    libembd_memory_order_relaxed = __LIBEMBD_ATOMIC_RELAXED,
    libembd_memory_order_consume = __LIBEMBD_ATOMIC_CONSUME,
    libembd_memory_order_acquire = __LIBEMBD_ATOMIC_ACQUIRE,
    libembd_memory_order_release = __LIBEMBD_ATOMIC_RELEASE,
    libembd_memory_order_acq_rel = __LIBEMBD_ATOMIC_ACQ_REL,
    libembd_memory_order_seq_cst = __LIBEMBD_ATOMIC_SEQ_CST
} libembd_memory_order;

// Define atomic types for various data types
#define LIBEMBD_ATOMIC_TYPE(type)    libembd_atomic_##type##_t
#define LIBEMBD_ATOMIC_TYPEDEF(type) typedef struct { volatile type value; } LIBEMBD_ATOMIC_TYPE(type)

LIBEMBD_ATOMIC_TYPEDEF(uint8);
LIBEMBD_ATOMIC_TYPEDEF(uint16);
LIBEMBD_ATOMIC_TYPEDEF(uint32);

typedef LIBEMBD_ATOMIC_TYPE(uint8) libembd_atomic_bool_t;

#define LIBEMBD_ATOMIC_VAR_INIT(VALUE)  ({VALUE})

// Compiler barrier to prevent compile-time reordering
#if defined(__ghs__)
    #include <ghs_barrier.h>
    #define LIBEMBD_COMPILER_BARRIER() __memory_changed()
#elif defined(__GNUC__) || defined(__clang__)
    #define LIBEMBD_COMPILER_BARRIER() __asm__ __volatile__("" ::: "memory")
#else
    #error Unsupported compiler!
#endif

#ifndef __ARM__
    #error Unsupported architecture!
#endif

#include <arm_ghs.h>

#define __LIBEMBD_DMB_INNER_SHARE()                         __DMB_OPT(__BARRIER_ISH)
#define __LIBEMBD_DMB_INNER_SHARE_STORE()                   __DMB_OPT(__BARRIER_ISHST)

#define __LIBEMBD_DMB_NON_INNER_SHARE()                     __DMB_OPT(__BARRIER_NSH)
#define __LIBEMBD_DMB_NON_INNER_SHARE_STORE()               __DMB_OPT(__BARRIER_NSHST)

#if LIBEMBD_IS_SINGLE_CORE_ONLY
    #define __LIBEMBD_DMB_SYSTEM()                          __LIBEMBD_DMB_INNER_SHARE()
    #define __LIBEMBD_DMB_STORE()                           __LIBEMBD_DMB_INNER_SHARE_STORE()

    #define LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_uint8(pU8)        (*(pU8))
    #define LIBEMBD_ATOMIC_STORE_EXCLUSIVE_uint8(u8,pU8)    (*(pU8) = u8)

    #define LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_uint16(pU16)      (*(pU16))
    #define LIBEMBD_ATOMIC_STORE_EXCLUSIVE_uint16(u16,pU16) (*(pU16) = u16)

    #define LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_uint32(pU32)      (*(pU32))
    #define LIBEMBD_ATOMIC_STORE_EXCLUSIVE_uint32(u32,pU32) (*(pU32) = u32)

#else
    #define __LIBEMBD_DMB_SYSTEM()                          __DMB_OPT(__BARRIER_SY)
    #define __LIBEMBD_DMB_STORE()                           __DMB_OPT(__BARRIER_ST) 

    #define LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_uint8(pU8)        __LDREXB(pU8)
    #define LIBEMBD_ATOMIC_STORE_EXCLUSIVE_uint8(pU8)       __STREXB(pU8)

    #define LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_uint16(pU16)      __LDREXH(pU16)
    #define LIBEMBD_ATOMIC_STORE_EXCLUSIVE_uint16(u16,pU16) __STREXH(u16, pU16)

    #define LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_uint32(pU32)      __LDREX(pU32)
    #define LIBEMBD_ATOMIC_STORE_EXCLUSIVE_uint32(u32,pU32) __STREX(u32, pU32)

#endif

#define LIBEMBD_ATOMIC_CLEAR_EXCLUSIVE_MONITOR()            __CLREX()

#define __LIBEMBD_DMB_DEFAULT()                         __LIBEMBD_DMB_SYSTEM()
#define __LIBEMBD_DMB_NONE()

LIBEMBD_HEADER_API_INLINE void __libembd_memory_barrier_default_internal(void)
{
    __LIBEMBD_DMB_DEFAULT();
    LIBEMBD_COMPILER_BARRIER();
}

LIBEMBD_HEADER_API_INLINE void __libembd_memory_barrier_internal(int const mo)
{
    switch(mo){
        case __LIBEMBD_ATOMIC_RELAXED : 
            __LIBEMBD_DMB_NONE();
            LIBEMBD_COMPILER_BARRIER();
            return;
        case __LIBEMBD_ATOMIC_CONSUME : 
            __LIBEMBD_DMB_SYSTEM();
            LIBEMBD_COMPILER_BARRIER();
            return;
        case __LIBEMBD_ATOMIC_ACQUIRE : 
            __LIBEMBD_DMB_SYSTEM();
            LIBEMBD_COMPILER_BARRIER();
            return;
        case __LIBEMBD_ATOMIC_RELEASE : 
            __LIBEMBD_DMB_STORE();
            LIBEMBD_COMPILER_BARRIER();
            return;
        case __LIBEMBD_ATOMIC_ACQ_REL : 
            __LIBEMBD_DMB_SYSTEM();
            LIBEMBD_COMPILER_BARRIER();
            return;
        case __LIBEMBD_ATOMIC_SEQ_CST : 
            __LIBEMBD_DMB_SYSTEM();
            LIBEMBD_COMPILER_BARRIER();
            return;
    }
}

LIBEMBD_HEADER_API_INLINE uint8 LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_load_uint8(libembd_atomic_uint8_t const * ptr);
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_store_uint8(libembd_atomic_uint8_t * ptr, uint8 const val);

LIBEMBD_HEADER_API_INLINE uint16 LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_load_uint16(libembd_atomic_uint16_t const * ptr);
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_store_uint16(libembd_atomic_uint16_t * ptr, uint16 const val);

LIBEMBD_HEADER_API_INLINE uint32 LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_load_uint32(libembd_atomic_uint32_t const * ptr);
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_store_uint32(libembd_atomic_uint32_t * ptr, uint32 const val);

LIBEMBD_HEADER_API_INLINE uint8 LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_load_explicit_uint8(libembd_atomic_uint8_t const * ptr, libembd_memory_order order);
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_store_explicit_uint8(libembd_atomic_uint8_t * ptr, uint8 const val, libembd_memory_order order);

LIBEMBD_HEADER_API_INLINE uint16 LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_load_explicit_uint16(libembd_atomic_uint16_t const * ptr, libembd_memory_order order);
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_store_explicit_uint16(libembd_atomic_uint16_t * ptr, uint16 const val, libembd_memory_order order);

LIBEMBD_HEADER_API_INLINE uint32 LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_load_explicit_uint32(libembd_atomic_uint32_t const * ptr, libembd_memory_order order);
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_store_explicit_uint32(libembd_atomic_uint32_t * ptr, uint32 const val, libembd_memory_order order);

LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_compare_exchange_weak_uint8(libembd_atomic_uint8_t * ptr, uint8* pExepcted, uint8 desired);
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_compare_exchange_strong_uint8(libembd_atomic_uint8_t * ptr, uint8* pExepcted, uint8 desired);

LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_compare_exchange_weak_uint16(libembd_atomic_uint16_t * ptr, uint16* pExepcted, uint16 desired);
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_compare_exchange_strong_uint16(libembd_atomic_uint16_t * ptr, uint16* pExepcted, uint16 desired);

LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_compare_exchange_weak_uint32(libembd_atomic_uint32_t * ptr, uint32* pExepcted, uint32 desired);
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_atomic_compare_exchange_strong_uint32(libembd_atomic_uint32_t * ptr, uint32* pExepcted, uint32 desired);

#define LIBEMBD_ATOMIC_OP_IMPLEMENTATION(type) \
    LIBEMBD_HEADER_API_INLINE type libembd_atomic_load_##type(LIBEMBD_ATOMIC_TYPE(type) const * ptr) { \
        __libembd_memory_barrier_default_internal();\
        type const val = LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_##type(&ptr->value);\
        return val; \
    } \
    LIBEMBD_HEADER_API_INLINE void libembd_atomic_store_##type(LIBEMBD_ATOMIC_TYPE(type) * ptr, type const val) { \
        LIBEMBD_ATOMIC_STORE_EXCLUSIVE_##type(val, &ptr->value);\
        __libembd_memory_barrier_default_internal();\
    } \
    LIBEMBD_HEADER_API_INLINE boolean libembd_atomic_compare_exchange_weak_##type(LIBEMBD_ATOMIC_TYPE(type) * ptr, type* expected, type const desired) { \
        __libembd_memory_barrier_internal(__LIBEMBD_ATOMIC_ACQUIRE); \
        type const old_val = LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_##type(&ptr->value);\
        if(old_val != *expected) {\
            *expected = old_val;\
            return FALSE;\
        }\
        boolean const success = LIBEMBD_ATOMIC_STORE_EXCLUSIVE_##type(desired, &ptr->value) == 0;\
        if(success){\
            __libembd_memory_barrier_internal(__LIBEMBD_ATOMIC_RELEASE); \
        } else {\
            __libembd_memory_barrier_internal(__LIBEMBD_ATOMIC_RELAXED); \
        }\
        return success;\
    } \
    LIBEMBD_HEADER_API_INLINE boolean libembd_atomic_compare_exchange_strong_##type(LIBEMBD_ATOMIC_TYPE(type) * ptr, type* expected, type const desired) { \
        __libembd_memory_barrier_internal(__LIBEMBD_ATOMIC_ACQUIRE); \
        type const old_val = LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_##type(&ptr->value);\
        boolean success;\
        do{\
            if(old_val != *expected) {\
                *expected = old_val;\
                LIBEMBD_ATOMIC_CLEAR_EXCLUSIVE_MONITOR();\
                return FALSE;\
            }\
            success = LIBEMBD_ATOMIC_STORE_EXCLUSIVE_##type(desired, &ptr->value) == 0;\
            if(success){\
                __libembd_memory_barrier_internal(__LIBEMBD_ATOMIC_RELEASE); \
            } else {\
                __libembd_memory_barrier_internal(__LIBEMBD_ATOMIC_RELAXED); \
            }\
        } while (!success); \
        return TRUE;\
    } \

#define LIBEMBD_ATOMIC_OP_EXPLICIT_IMPLEMENTATION(type) \
    LIBEMBD_HEADER_API_INLINE type libembd_atomic_load_explicit_##type(LIBEMBD_ATOMIC_TYPE(type) const * ptr, libembd_memory_order mo) { \
        __libembd_memory_barrier_internal(mo); \
        type const val = LIBEMBD_ATOMIC_LOAD_EXCLUSIVE_##type(&ptr->value);\
        return val; \
    } \
    LIBEMBD_HEADER_API_INLINE void libembd_atomic_store_explicit_##type(LIBEMBD_ATOMIC_TYPE(type) * ptr, type const val, libembd_memory_order mo) { \
        LIBEMBD_ATOMIC_STORE_EXCLUSIVE_##type(val, &ptr->value);\
        __libembd_memory_barrier_internal(mo); \
    } \

LIBEMBD_ATOMIC_OP_IMPLEMENTATION(uint8)
LIBEMBD_ATOMIC_OP_IMPLEMENTATION(uint16)
LIBEMBD_ATOMIC_OP_IMPLEMENTATION(uint32)

LIBEMBD_ATOMIC_OP_EXPLICIT_IMPLEMENTATION(uint8)
LIBEMBD_ATOMIC_OP_EXPLICIT_IMPLEMENTATION(uint16)
LIBEMBD_ATOMIC_OP_EXPLICIT_IMPLEMENTATION(uint32)

// Atomic flag type
typedef struct {
    volatile uint8 flag;
} AppCore_AtomicFlag_t;

#endif /* LIBEMBD_ATOMIC_H_ */
