#ifndef D327EBA1_30C2_4AC5_B88D_0962AD83CF8D
#define D327EBA1_30C2_4AC5_B88D_0962AD83CF8D

#include <stddef.h>
#include "libembd/libembd_platform_types.h"
#include "libembd/libembd_common.h"

#define LIBEMBD_CONCAT(a, b) a##b

#if defined(__ARM__) && defined(__ghs__)
    #include <arm_ghs.h>
    #define LIBEMBD_DISABLE_INTERRUPTS()    __DI()
    #define LIBEMBD_ENABLE_INTERRUPTS()     __EI()
    LIBEMBD_HEADER_API_INLINE void * libembd_return_address(void) { return __builtin_return_address(0); }
    LIBEMBD_HEADER_API_INLINE void * libembd_get_stack_pointer(void) { return __get_stack_pointer(); }
    LIBEMBD_HEADER_API_INLINE void libembd_preload(void * addr) { __PLD(addr); }
#endif

#define LIBEMBD_MIN(a, b)                           (((a) < (b))?(a):(b))
#define LIBEMBD_MAX(a, b)                           (((a) > (b))?(a):(b))

#define LIBEMBD_SET_BIT(num, position)              ((num) |= (1U << (position)))

#define LIBEMBD_CLEAR_BIT(num, position)            ((num) &= ~(1U << (position)))

#define LIBEMBD_FLIP_BIT(num, position)             ((num) ^= (1U << (position)))

#define LIBEMBD_ASSIGN_BIT(num, position, boolval)  ((num) = ((num) & ~(1U << (position))) | ((boolval) << (position)))

#define LIBEMBD_IS_BIT_SET(num, position)           (((num) & (1U << (position))) != 0)

/**
 * @brief Get number of elements in an array
 * 
 */
#define LIBEMBD_NUM_ELEM(arr) (sizeof((arr)) / sizeof(*(arr)))

/**
 * @brief Byte swap operations
 * 
 */
#ifdef __ghs__
    #include <arm_ghs.h>
    #define LIBEMBD_BSWAP16(u16) __REV16(u16)
    #define LIBEMBD_BSWAP32(u32) __REV(u32)
#else 
    #ifdef __GNUC__ // gnuc
        # if __GNUC_PREREQ (4, 3)
            #define LIBEMBD_BSWAP16(u16) __builtin_bswap16(u16)
            #define LIBEMBD_BSWAP32(u32) __builtin_bswap32(u32)
            #define LIBEMBD_BSWAP64(u32) __builtin_bswap64(u32)
        # elif __GNUC__ >= 2
            #define LIBEMBD_BSWAP16(u16) \
                ((u16 & 0xFF00u) >>  8u) | \
                ((u16 & 0x00FFu) <<  8u)  
            #define LIBEMBD_BSWAP32(u32) \
                ((u32 & 0xFF000000u) >> 24u) | \
                ((u32 & 0x00FF0000u) >>  8u) | \
                ((u32 & 0x0000FF00u) <<  8u) | \
                ((u32 & 0x000000FFu) << 24u) 
        # endif  // gcc version
    #else
        #error Unsupported compiler!
    #endif // __GNUC__   
#endif // __ghs__

#define LIBEMBD_EXECUTE_IF_NOT_NULL(func, ...) ( if(func != NULL) { return func(__VA_ARGS__); } )

#define LIBEMBD_IS_ODD(uint) (((uint) & 1))

#define LIBEMBD_IS_EVEN(uint) (!((uint) & 1))

#define LIBEMBD_IS_POWER_OF_TWO(uint) (!((uint) & ((uint) - 1))) 

#define LIBEMBD_FIND_INTERNAL(arr, len, val) \
  do { \
    for(LibEmbd_Size_t i = 0; i < len; i++){ \
      if(arr[i] == val) return &arr[i]; \
    } \
    return NULL; \
  } while (0)

/**
 * @brief Find val in an array of uint8s
 * 
 * @param arr array of uint8s
 * @param arr_len length of array
 * @param val value to find
 * @return uint8* position of value if found. NULL if not found.
 */
LIBEMBD_HEADER_API_INLINE uint8 const* LIBEMBD_ATTR_ALWAYS_INLINE libembd_find_u8(uint8 const * arr, LibEmbd_Size_t arr_len, uint8 val){
  LIBEMBD_FIND_INTERNAL(arr, arr_len, val);
}

/**
 * @brief Find val in an array of uint16s
 * 
 * @param arr array of uint16s
 * @param arr_len length of array
 * @param val value to find
 * @return uint16* position of value if found. NULL if not found.
 */
LIBEMBD_HEADER_API_INLINE uint16 const* LIBEMBD_ATTR_ALWAYS_INLINE libembd_find_u16(uint16 const * arr, LibEmbd_Size_t arr_len, uint16 val){
  LIBEMBD_FIND_INTERNAL(arr, arr_len, val);
}

/**
 * @brief Find val in an array of uint32s
 * 
 * @param arr array of uint32s
 * @param arr_len length of array
 * @param val value to find
 * @return uint32* position of value if found. NULL if not found.
 */
LIBEMBD_HEADER_API_INLINE uint32 const* LIBEMBD_ATTR_ALWAYS_INLINE libembd_find_u32(uint32 const * arr, LibEmbd_Size_t arr_len, uint32 val){
  LIBEMBD_FIND_INTERNAL(arr, arr_len, val);
}

/**
 * @brief Swap two elements in an array
 * 
 * @param array array of generic elements
 * @param pos1 index of element 1
 * @param pos2 index of element 2
 */
#define LIBEMBD_SWAP_GENERIC(arr, pos1, pos2) \
    do { \
        LibEmbd_Size_t const elem_size = sizeof(*(arr)); \
        uint8 temp[elem_size]; \
        LIBEMBD_MEMCPY(temp, (uint8 *)(arr) + (pos1) * elem_size, elem_size); \
        LIBEMBD_MEMCPY((uint8 *)(arr) + (pos1) * elem_size, (uint8 *)(arr) + (pos2) * elem_size, elem_size); \
        LIBEMBD_MEMCPY((uint8 *)(arr) + (pos2) * elem_size, temp, elem_size); \
    } while (0)

/**
 * @brief Swap two elements in an integer array
 * 
 * @param array array of integer elements
 * @param pos1 index of integer 1
 * @param pos2 index of integer 2
 */
#define LIBEMBD_SWAP_INT_INTERNAL(arr, pos1, pos2) \
    do { \
        LibEmbd_Size_t temp; \
        temp = arr[pos1]; \
        arr[pos1] = arr[pos2]; \
        arr[pos2] = temp; \
    } while (0)

/**
 * @brief Swap two elements in an array
 * 
 * @param array array of generic elements
 * @param element_size byte size of elements
 * @param pos1 index of element 1
 * @param pos2 index of element 2
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_swap_generic(void *array, uint8 element_size, LibEmbd_Size_t pos1, LibEmbd_Size_t pos2) {
    uint8 temp[UINT8_MAX];
    uint8 * const arr = (uint8 *)array;

    // Copy the element at pos1 to temp
    LIBEMBD_MEMCPY(temp, (uint8 *)(arr) + (pos1) * element_size, element_size);

    // Copy the element at pos2 to pos1
    LIBEMBD_MEMCPY((uint8 *)(arr) + (pos1) * element_size, (uint8 *)(arr) + (pos2) * element_size, element_size);

    // Copy the temp element to index pos2
    LIBEMBD_MEMCPY((uint8 *)(arr) + (pos2) * element_size, temp, element_size);
}

LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_swap_u8(uint8 *array, LibEmbd_Size_t pos1, LibEmbd_Size_t pos2) {
    LIBEMBD_SWAP_INT_INTERNAL(array, pos1, pos2);
}

LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_swap_u16(uint16 *array, LibEmbd_Size_t pos1, LibEmbd_Size_t pos2) {
    LIBEMBD_SWAP_INT_INTERNAL(array, pos1, pos2);
}

LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_swap_u32(uint32 *array, LibEmbd_Size_t pos1, LibEmbd_Size_t pos2) {
    LIBEMBD_SWAP_INT_INTERNAL(array, pos1, pos2);
}

#define LIBEMBD_INTERNAL_EMPLACE_BACK_IMPLEMENTATION(TYPE, SUFFIX) \
    LIBEMBD_HEADER_API_INLINE boolean libembd_contains_##SUFFIX(TYPE* arr, LibEmbd_Size_t size, TYPE value) { \
        for (LibEmbd_Size_t i = 0; i < size; i++) { \
            if (arr[i] == value) return TRUE; \
        } \
        return FALSE; \
    } \
    LIBEMBD_HEADER_API_INLINE TYPE* libembd_try_emplace_back_##SUFFIX(TYPE* arr, LibEmbd_Size_t size, TYPE value) { \
        if (libembd_contains_##SUFFIX(arr, size, value)) return NULL; \
        arr[size] = value; \
        return &arr[size]; \
    }

// Function declarations for int, float, and double
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_contains_u8(uint8* arr, LibEmbd_Size_t size, uint8 value);
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_contains_u16(uint16* arr, LibEmbd_Size_t size, uint16 value);
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_contains_u32(uint32* arr, LibEmbd_Size_t size, uint32 value);
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_contains_generic(void* arr, LibEmbd_Size_t arr_size, const void* value, LibEmbd_Size_t elem_size);

LIBEMBD_HEADER_API_INLINE uint8* LIBEMBD_ATTR_ALWAYS_INLINE libembd_try_emplace_back_u8(uint8* arr, LibEmbd_Size_t size, uint8 value);
LIBEMBD_HEADER_API_INLINE uint16* LIBEMBD_ATTR_ALWAYS_INLINE libembd_try_emplace_back_u16(uint16* arr, LibEmbd_Size_t size, uint16 value);
LIBEMBD_HEADER_API_INLINE uint32* LIBEMBD_ATTR_ALWAYS_INLINE libembd_try_emplace_back_u32(uint32* arr, LibEmbd_Size_t size, uint32 value);
LIBEMBD_HEADER_API_INLINE void* LIBEMBD_ATTR_ALWAYS_INLINE libembd_try_emplace_back_generic(void* arr, LibEmbd_Size_t arr_size, const void* value, LibEmbd_Size_t elem_size);

// Function definitions for int, float, and double
LIBEMBD_INTERNAL_EMPLACE_BACK_IMPLEMENTATION(uint8, u8)
LIBEMBD_INTERNAL_EMPLACE_BACK_IMPLEMENTATION(uint16, u16)
LIBEMBD_INTERNAL_EMPLACE_BACK_IMPLEMENTATION(uint32, u32)

LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_contains_generic(void* arr, LibEmbd_Size_t arr_size, const void* value, LibEmbd_Size_t elem_size)
{
    for(LibEmbd_Size_t i = 0; i < arr_size; i++){
        if(LIBEMBD_MEMCMP((uint8*)arr + i * elem_size, value, elem_size) == 0)
            return TRUE;
    }
    return FALSE;
}

LIBEMBD_HEADER_API_INLINE void* LIBEMBD_ATTR_ALWAYS_INLINE libembd_try_emplace_back_generic(void* arr, LibEmbd_Size_t arr_size, const void* value, LibEmbd_Size_t elem_size)
{
    if(libembd_contains_generic(arr, arr_size, value, elem_size))
        return NULL;

    void* const emplace_location = (void*)((uint8*)arr + arr_size * elem_size);
    LIBEMBD_MEMCPY(emplace_location, value, elem_size);
    return emplace_location;
}

//! assumes single threaded use
#define LIBEMBD_CALL_ONCE(func) \
    do {\
        static boolean has_been_called = FALSE; \
        if(!has_been_called) { \
            func(); \
            has_been_called = TRUE; \
        } \
    } while (0)

/**
 * @brief Convert BCD digits to string (eg: 59 -> '59')
 * @note output_str must be at least 2 * bcd_byte_length + 1 long
 * 
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_bcd_to_string(uint8 const * bcd_bytes, LibEmbd_Size_t const bcd_byte_length, char* output_str)
{
    for(LibEmbd_Size_t pos = 0; pos < bcd_byte_length; pos++){
        output_str[2 * pos] = (bcd_bytes[pos] >> 4) + '0';
        output_str[2 * pos + 1] = (bcd_bytes[pos] & 0x0Fu) + '0';
    }
    output_str[2 * bcd_byte_length] = '\0';
}

#endif /* D327EBA1_30C2_4AC5_B88D_0962AD83CF8D */
