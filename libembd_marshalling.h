#ifndef LIBEMBD_MARSHALLING_H_
#define LIBEMBD_MARSHALLING_H_

#include <stddef.h>
#include "libembd/libembd_platform_types.h" //uint8 uint16 uint32 float32 typedefs
#include "libembd/libembd_common.h" //host endianness detection etc.
#include "libembd/libembd_util.h" //byte swap operations

/**
 * @file libEmbd_marshalling.h
 * @brief Serialization/deserialization library for primitive data types with support for endian conversions.
 *
 * This library provides functions to serialize and deserialize primitive data types
 * such as `uint32` and `float32` (see typedef in Platform_Types.h) with optional endianness conversion. It supports both direct
 * read/write operations without position adjustments and get/put operations with appropriate position adjustments. 
 * By design, all ser/des APIs provided by this library have "unsafe" included in their names. For performance and ease-of-use considerations,
 * these functions have no return value and it is the user's responsibility to ensure that no buffer overflow can occur by providing 
 * a large enough buffer in initialization. It is undefined behavior if buffer overflow does occur.
 *
 * Example usage:
 * @code
 * #include <stdio.h>
 * #include "libembd/libembd_marshalling.h"
 *
 * int main() {
 *     uint8_t buffer[128]; //! always make sure the buffer is large enough for subsequent operations
 *     LibEmbd_Serializer_t ser;
 *
 *     // Initialize serializer
 *     libembd_make_serializer(&ser, buffer, sizeof(buffer));
 *
 *     // Writing data and update write position
 *     libembd_put_uint32_to_network_unsafe(&ser, 42);
 *     libembd_put_float32_to_network_unsafe(&ser, 3.14f);
 *
 *     // Reading data with position adjustment
 *     uint32 int_value;
 *     float32 float_value;
 *     
 *     LibEmbd_Deserializer_t deser;
 *     libembd_make_deserializer(&deser, buffer, sizeof(buffer));
 * 
 *     libembd_get_uint32_from_network_unsafe(&ser, &int_value);
 *     libembd_get_float32_from_network_unsafe(&ser, &float_value);
 *
 *     printf("Read int32 from network byte order: %d\n", int_value);
 *     printf("Read float from network byte order: %f\n", float_value);
 * 
 *     return 0;
 * }
 * @endcode
 */

//! assumes big endian for network byte order
#if (LIBEMBD_HOST_ENDIANNESS == LIBEMBD_ENDIANNESS_LITTLE_ENDIAN)
    #define LIBEMBD_HTONS(x) LIBEMBD_BSWAP16(x)
    #define LIBEMBD_HTONL(x) LIBEMBD_BSWAP32(x)
    #define LIBEMBD_NTOHS(x) LIBEMBD_BSWAP16(x)
    #define LIBEMBD_NTOHL(x) LIBEMBD_BSWAP32(x)
#else
    #define LIBEMBD_HTONS(x) (x)
    #define LIBEMBD_HTONL(x) (x)
    #define LIBEMBD_NTOHS(x) (x)
    #define LIBEMBD_NTOHL(x) (x)
#endif

#define LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ptr) \
    LIBEMBD_ASSUME((ptr) != NULL)

#define LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, size) \
    LIBEMBD_ASSUME((ser)->position + (size) <= (ser)->capacity)

#define LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(ser, pos, size) \
    LIBEMBD_ASSUME((pos) + (size) <= (ser)->capacity)

/**
 * @brief Read u8/u16/u32/f32 from host order byte stream at given position
 * @param deser pointer to deserializer containing the byte stream
 * @param p2val pointer to variable to deserialize into
 * @param pos position in byte stream to perform the deserialization
 * 
 */
#define LIBEMBD_MARSHALLING_READ_FROM_HOST_TYPE_GENERIC(deser, pos, p2val) \
    do { \
        LIBEMBD_MEMCPY((p2val), &(deser)->buffer[(pos)], sizeof(*(p2val))); \
    } while (0) 

/**
 * @brief Read u8/u16/u32/f32 from host order byte stream and increment the read pointer accordingly
 * @param deser pointer to deserializer containing the byte stream
 * @param p2val pointer to variable to deserialize into
 * 
 */
#define LIBEMBD_MARSHALLING_GET_FROM_HOST_TYPE_GENERIC(deser, p2val) \
    do { \
        LIBEMBD_MEMCPY((p2val), &(deser)->buffer[(deser)->position], sizeof(*(p2val))); \
        (deser)->position += sizeof(*(p2val)); \
    } while (0) 

/**
 * @brief Write u8/u16/u32/f32 to host order byte stream and increment the write pointer accordingly
 * @param deser pointer to serializer containing the byte stream
 * @param val value to serialize
 * @param pos position in byte stream to perform the serialization
 * 
 */
#define LIBEMBD_MARSHALLING_WRITE_TO_HOST_TYPE_GENERIC(ser, pos, val) \
    do { \
        LIBEMBD_MEMCPY(&(ser)->buffer[(pos)], &(val), sizeof((val))); \
    } while (0) 

/**
 * @brief Write u8/u16/u32/f32 to host order byte stream at given position
 * @param ser pointer to deserializer containing the byte stream
 * @param val value to serialize
 * 
 */
#define LIBEMBD_MARSHALLING_PUT_TO_HOST_TYPE_GENERIC(ser, val) \
    do { \
        LIBEMBD_MEMCPY(&(ser)->buffer[(ser)->position], &val, sizeof((val))); \
        (ser)->position += sizeof((val)); \
    } while (0) 

// Serializer context
typedef struct {
    uint8 *buffer;
    uint32 capacity;
    uint32 position; //write position
} LibEmbd_Serializer_t;

// Serializer context
typedef struct {
    uint8 const *buffer;
    uint32 capacity;
    uint32 position; //read position
} LibEmbd_Deserializer_t;

/**
 * @brief serializer constructor
 * 
 * @param ser pointer to uninitialized serializer object
 * @param buffer pointer to buffer into which data will be serialized
 * @param capacity length of buffer
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_make_serializer(LibEmbd_Serializer_t *ser, uint8 *buffer, uint32 capacity);

/**
 * @brief resets serializer for reuse
 * 
 * @param ser pointer to initialized serializer object
 * @note The serrializer object passed to this method must be a valid object returned by libembd_make_serializer.
 *       After reset, the serializer retains the buffer and capacity information, but any other state information 
 *       such as position markers will be re-initialized.
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_reset_serializer(LibEmbd_Serializer_t *ser);

/**
 * @brief deserializer constructor
 * 
 * @param deser pointer to uninitialized deserializer object
 * @param buffer pointer to buffer from which data will be extracted
 * @param capacity length of buffer
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_make_deserializer(LibEmbd_Deserializer_t *deser, uint8 const *buffer, uint32 capacity);

/**
 * @brief resets deserializer for reuse
 * 
 * @param deser pointer to initialized deserializer object
 * @note The deserrializer object passed to this method must be a valid object returned by libembd_make_deserializer.
 *       After reset, the deserializer retains the buffer and capacity information, but any other state information 
 *       such as position markers will be re-initialized.
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_reset_deserializer(LibEmbd_Deserializer_t *deser);

/**
 * @brief move serializer position ahead by num_bytes
 * 
 * @param ser pointer to initialized serializer object
 * @param num_bytes number of bytes to skip ahead
 * @note serializer position will not exceed buffer capacity
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_serializer_skip(LibEmbd_Serializer_t * ser, uint32 num_bytes);

/**
 * @brief move serializer position to position
 * 
 * @param ser pointer to initialized serializer object
 * @param position position to move to
 * @note serializer position will not exceed buffer capacity
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_serializer_seek(LibEmbd_Serializer_t * ser, uint32 position);

/**
 * @brief move deserializer position ahead by num_bytes
 * 
 * @param deser pointer to initialized deserializer object
 * @param num_bytes number of bytes to skip ahead
 * @note serializer position will not exceed buffer capacity
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_deserializer_skip(LibEmbd_Deserializer_t * deser, uint32 num_bytes);

/**
 * @brief move deserializer position to position
 * 
 * @param deser pointer to initialized deserializer object
 * @param position position to move to
 * @note serializer position will not exceed buffer capacity
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_deserializer_seek(LibEmbd_Deserializer_t * deser, uint32 position);

/**
 * @brief Writes uint8 value to underlying buffer and updates write position
 * 
 * @param ser pointer to initialized serializer object
 * @param host_value uint8 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_put_uint8_unsafe(LibEmbd_Serializer_t *ser, uint8 host_value);

/**
 * @brief Writes uint16 value in network byte order to underlying buffer and updates write position
 * 
 * @param ser pointer to initialized serializer object
 * @param host_value uint16 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_put_uint16_to_network_unsafe(LibEmbd_Serializer_t *ser, uint16 host_value);

/**
 * @brief Writes uint16 value in host byte order to underlying buffer and updates write position
 * 
 * @param ser pointer to initialized serializer object
 * @param host_value uint16 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_put_uint16_to_host_unsafe(LibEmbd_Serializer_t *ser, uint16 host_value);

/**
 * @brief Writes uint32 value in network byte order to underlying buffer and updates write position
 * 
 * @param ser pointer to initialized serializer object
 * @param host_value uint32 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_put_uint32_to_network_unsafe(LibEmbd_Serializer_t *ser, uint32 host_value);

/**
 * @brief Writes uint32 value in host byte order to underlying buffer and updates write position
 * 
 * @param ser pointer to initialized serializer object
 * @param host_value uint32 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_put_uint32_to_host_unsafe(LibEmbd_Serializer_t *ser, uint32 host_value);

/**
 * @brief Writes float32 value in network byte order to underlying buffer and updates write position
 * 
 * @param ser pointer to initialized serializer object
 * @param host_value float32 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_put_float32_to_network_unsafe(LibEmbd_Serializer_t *ser, float32 host_value);

/**
 * @brief Writes float32 value in host byte order to underlying buffer and updates write position
 * 
 * @param ser pointer to initialized serializer object
 * @param host_value float32 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_put_float32_to_host_unsafe(LibEmbd_Serializer_t *ser, float32 host_value);

/**
 * @brief Writes raw bytes to underlying buffer and updates write position
 * 
 * @param ser pointer to initialized serializer object
 * @param buffer pointer to input buffer
 * @param buffer_length input buffer length
 * @warning assumes the underlying buffer is large enough to perform the write 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_put_buffer_unsafe(LibEmbd_Serializer_t *ser, const void* buffer, uint32 buffer_length);

/**
 * @brief Writes uint8 value to underlying buffer without updating write position
 * 
 * @param ser pointer to initialized serializer object
 * @param position the position at which to perform the serialization
 * @param host_value uint8 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_write_uint8_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint8 host_value);

/**
 * @brief Writes uint16 value in network byte order to underlying buffer without updating write position
 * 
 * @param ser pointer to initialized serializer object
 * @param position the position at which to perform the serialization
 * @param host_value uint16 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_write_uint16_to_network_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint16 host_value);

/**
 * @brief Writes uint16 value in host byte order to underlying buffer without updating write position
 * 
 * @param ser pointer to initialized serializer object
 * @param position the position at which to perform the serialization
 * @param host_value uint16 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_write_uint16_to_host_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint16 host_value);

/**
 * @brief Writes uint32 value in network byte order to underlying buffer without updating write position
 * 
 * @param ser pointer to initialized serializer object
 * @param position the position at which to perform the serialization
 * @param host_value uint32 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_write_uint32_to_network_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint32 host_value);

/**
 * @brief Writes uint32 value in host byte order to underlying buffer without updating write position
 * 
 * @param ser pointer to initialized serializer object
 * @param position the position at which to perform the serialization
 * @param host_value uint32 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_write_uint32_to_host_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint32 host_value);

/**
 * @brief Writes float32 value in network byte order to underlying buffer without updating write position
 * 
 * @param ser pointer to initialized serializer object
 * @param position the position at which to perform the serialization
 * @param host_value float32 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_write_float32_to_network_unsafe(LibEmbd_Serializer_t *ser, uint32 position, float32 host_value);

/**
 * @brief Writes float32 value in host byte order to underlying buffer without updating write position
 * 
 * @param ser pointer to initialized serializer object
 * @param position the position at which to perform the serialization
 * @param host_value float32 value to serialize
 * @warning assumes the underlying buffer is large enough to perform the serialization 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_write_float32_to_host_unsafe(LibEmbd_Serializer_t *ser, uint32 position, float32 host_value);

/**
 * @brief Writes raw bytes to underlying buffer without updating write position
 * 
 * @param ser pointer to initialized serializer object
 * @param position the position at which to perform the write
 * @param buffer pointer to input buffer
 * @param buffer_length input buffer length
 * @warning assumes the underlying buffer is large enough to perform the write 
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_write_buffer_unsafe(LibEmbd_Serializer_t *ser, uint32 position, const void* buffer, uint32 buffer_length);

/**
 * @brief Reads uint8 value from underlying buffer and updates read position
 *
 * @param deser pointer to initialized deserializer object
 * @param value pointer to uint8 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_get_uint8_unsafe(LibEmbd_Deserializer_t *deser, uint8 *value);

/**
 * @brief Reads uint16 value in network byte order from underlying buffer and updates read position
 *
 * @param deser pointer to initialized deserializer object
 * @param value pointer to uint16 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_get_uint16_from_network_unsafe(LibEmbd_Deserializer_t *deser, uint16 *value);

/**
 * @brief Reads uint16 value in host byte order from underlying buffer and updates read position
 *
 * @param deser pointer to initialized deserializer object
 * @param value pointer to uint16 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_get_uint16_from_host_unsafe(LibEmbd_Deserializer_t *deser, uint16 *value);

/**
 * @brief Reads uint32 value in network byte order from underlying buffer and updates read position
 *
 * @param deser pointer to initialized deserializer object
 * @param value pointer to uint32 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_get_uint32_from_network_unsafe(LibEmbd_Deserializer_t *deser, uint32 *value);

/**
 * @brief Reads uint32 value in host byte order from underlying buffer and updates read position
 *
 * @param deser pointer to initialized deserializer object
 * @param value pointer to uint32 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_get_uint32_from_host_unsafe(LibEmbd_Deserializer_t *deser, uint32 *value);

/**
 * @brief Reads float32 value in network byte order from underlying buffer and updates read position
 *
 * @param deser pointer to initialized deserializer object
 * @param value pointer to float32 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_get_float32_from_network_unsafe(LibEmbd_Deserializer_t *deser, float32 *value);

/**
 * @brief Reads float32 value in host byte order from underlying buffer and updates read position
 *
 * @param deser pointer to initialized deserializer object
 * @param value pointer to float32 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_get_float32_from_host_unsafe(LibEmbd_Deserializer_t *deser, float32 *value);

/**
 * @brief Reads raw bytes from underlying buffer and updates read position
 *
 * @param deser pointer to initialized deserializer object
 * @param buffer pointer to output buffer
 * @param pLength pointer to variable holding length of buffer to be read, updated with actual bytes read
 * @warning Assumes the underlying buffer is large enough to perform the read
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_get_buffer_unsafe(LibEmbd_Deserializer_t *deser, void *buffer, uint32 *pLength);

/**
 * @brief Reads uint8 value from underlying buffer without updating read position
 *
 * @param deser pointer to initialized deserializer object
 * @param position the position at which to perform the deserialization
 * @param value pointer to uint8 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_read_uint8_from_host_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint8 *value);

/**
 * @brief Reads uint16 value in network byte order from underlying buffer without updating read position
 *
 * @param deser pointer to initialized deserializer object
 * @param position the position at which to perform the deserialization
 * @param value pointer to uint16 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_read_uint16_from_network_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint16 *value);

/**
 * @brief Reads uint16 value in host byte order from underlying buffer without updating read position
 *
 * @param deser pointer to initialized deserializer object
 * @param position the position at which to perform the deserialization
 * @param value pointer to uint16 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_read_uint16_from_host_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint16 *value);

/**
 * @brief Reads uint32 value in network byte order from underlying buffer without updating read position
 *
 * @param deser pointer to initialized deserializer object
 * @param position the position at which to perform the deserialization
 * @param value pointer to uint32 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_read_uint32_from_network_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint32 *value);

/**
 * @brief Reads uint32 value in host byte order from underlying buffer without updating read position
 *
 * @param deser pointer to initialized deserializer object
 * @param position the position at which to perform the deserialization
 * @param value pointer to uint32 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_read_uint32_from_host_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint32 *value);

/**
 * @brief Reads float32 value in network byte order from underlying buffer without updating read position
 *
 * @param deser pointer to initialized deserializer object
 * @param position the position at which to perform the deserialization
 * @param value pointer to float32 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_read_float32_from_network_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, float32 *value);

/**
 * @brief Reads float32 value in host byte order from underlying buffer without updating read position
 *
 * @param deser pointer to initialized deserializer object
 * @param position the position at which to perform the deserialization
 * @param value pointer to float32 variable to deserialize into
 * @warning Assumes the underlying buffer is large enough to perform the deserialization
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_read_float32_from_host_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, float32 *value);

/**
 * @brief Reads raw bytes from underlying buffer without updating read position
 *
 * @param deser pointer to initialized deserializer object
 * @param position the position at which to perform the read
 * @param buffer pointer to output buffer
 * @param pLength pointer to variable holding length of buffer to be read, updated with actual bytes read
 * @warning Assumes the underlying buffer is large enough to perform the read
 */
LIBEMBD_LOCAL_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_read_buffer_unsafe(LibEmbd_Deserializer_t *deser, uint32 position, void *buffer, uint32 *pLength);


/*-----------------------------------------------------------------Internal functions Begin----------------------------------------------------------------------------*/
LIBEMBD_LOCAL_INLINE void libembd_write_to_network_short_internal(LibEmbd_Serializer_t* ser, uint32 const pos, uint16 const val)
{
    uint16 const network_value = (uint16)LIBEMBD_HTONS(val);
    LIBEMBD_MEMCPY(&ser->buffer[ser->position], &network_value, sizeof(network_value));
}

LIBEMBD_LOCAL_INLINE void libembd_write_to_network_long_internal(LibEmbd_Serializer_t* ser, uint32 const pos, uint32 const val)
{
    uint32 const network_value = (uint32)LIBEMBD_HTONL(val);
    LIBEMBD_MEMCPY(&ser->buffer[ser->position], &network_value, sizeof(network_value));
}

// Write float32 to network byte order
LIBEMBD_LOCAL_INLINE void libembd_write_to_network_float32_internal(LibEmbd_Serializer_t *ser, uint32 const pos, float32 const host_value) {
    LIBEMBD_STATIC_ASSERT(sizeof(float32) == sizeof(uint32), "Unexpected type size!");

    uint32 int_value;
    LIBEMBD_MEMCPY(&int_value, &host_value, sizeof(host_value));
    int_value = (uint32)LIBEMBD_HTONL(int_value);
    LIBEMBD_MEMCPY(&ser->buffer[pos], &int_value, sizeof(int_value));
}

// Write float32 to network byte order
LIBEMBD_LOCAL_INLINE void libembd_put_to_network_float32_internal(LibEmbd_Serializer_t *ser, float32 const host_value) {
    libembd_write_to_network_float32_internal(ser, ser->position, host_value);
    ser->position += sizeof(host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_put_to_network_short_internal(LibEmbd_Serializer_t* ser, uint16 val)
{
    libembd_write_to_network_short_internal(ser, ser->position, val);
    ser->position += sizeof(uint16);
}

LIBEMBD_LOCAL_INLINE void libembd_put_to_network_long_internal(LibEmbd_Serializer_t* ser, uint32 val)
{
    libembd_write_to_network_long_internal(ser, ser->position, val);
    ser->position += sizeof(uint32);
}

LIBEMBD_LOCAL_INLINE void libembd_read_u8_internal(LibEmbd_Deserializer_t const* deser, uint32 const pos, uint8* p2val)
{
    *p2val = deser->buffer[pos];
}

LIBEMBD_LOCAL_INLINE void libembd_get_u8_internal(LibEmbd_Deserializer_t* deser, uint8* p2val)
{
    libembd_read_u8_internal(deser, deser->position, p2val);
    deser->position += sizeof(*p2val);
}

LIBEMBD_LOCAL_INLINE void libembd_read_from_network_short_internal(LibEmbd_Deserializer_t const* deser, uint32 const pos, uint16* p2val)
{
    uint16 network_value;
    LIBEMBD_MEMCPY(&network_value, &deser->buffer[pos], sizeof(network_value));
    *p2val = (uint16)LIBEMBD_NTOHS(network_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_from_network_short_internal(LibEmbd_Deserializer_t* deser, uint16* p2val)
{
    libembd_read_from_network_short_internal(deser, deser->position, p2val);
    deser->position += sizeof(*p2val);
}


LIBEMBD_LOCAL_INLINE void libembd_read_from_network_long_internal(LibEmbd_Deserializer_t const* deser, uint32 const pos, uint32* p2val)
{
    uint32 network_value;
    LIBEMBD_MEMCPY(&network_value, &deser->buffer[pos], sizeof(network_value));
    *p2val = (uint32)LIBEMBD_NTOHL(network_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_from_network_long_internal(LibEmbd_Deserializer_t* deser, uint32* p2val)
{
    libembd_read_from_network_long_internal(deser, deser->position, p2val);
    deser->position += sizeof(*p2val);
}

LIBEMBD_LOCAL_INLINE void libembd_read_from_network_float32_internal(LibEmbd_Deserializer_t const* deser, uint32 const pos, float32* p2val)
{
    LIBEMBD_STATIC_ASSERT(sizeof(float32) == sizeof(uint32), "Unexpected type size!");
    uint32 int_value;
    LIBEMBD_MEMCPY(&int_value, &deser->buffer[deser->position], sizeof(int_value));
    int_value = (uint32)LIBEMBD_NTOHL(int_value);
    LIBEMBD_MEMCPY(p2val, &int_value, sizeof(*p2val));
   
}

LIBEMBD_LOCAL_INLINE void libembd_get_from_network_float32_internal(LibEmbd_Deserializer_t* deser, float32* p2val)
{   
    libembd_read_from_network_float32_internal(deser, deser->position, p2val);
    deser->position += sizeof(*p2val);
}
/*-----------------------------------------------------------------Internal Functions End----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------API Implementaton Begin----------------------------------------------------------------------------*/
LIBEMBD_LOCAL_INLINE void libembd_make_serializer(LibEmbd_Serializer_t* ser, uint8* buffer, uint32 const capacity)
{
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(buffer);

    ser->buffer = buffer;
    ser->capacity = capacity;
    ser->position = 0;
}

LIBEMBD_LOCAL_INLINE void libembd_reset_serializer(LibEmbd_Serializer_t *ser)
{
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);

    ser->position = 0;
}

LIBEMBD_LOCAL_INLINE void libembd_make_deserializer(LibEmbd_Deserializer_t* deser, uint8 const* buffer, uint32 const capacity)
{
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(buffer);

    deser->buffer = buffer;
    deser->capacity = capacity;
    deser->position = 0;
}

LIBEMBD_LOCAL_INLINE void libembd_reset_deserializer(LibEmbd_Deserializer_t *deser)
{
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);

    deser->position = 0;
}

LIBEMBD_LOCAL_INLINE void libembd_serializer_skip(LibEmbd_Serializer_t * ser, uint32 num_bytes)
{
    uint32 const new_position = LIBEMBD_MIN(ser->position + num_bytes, ser->capacity);
    ser->position = new_position;
}

LIBEMBD_LOCAL_INLINE void libembd_serializer_seek(LibEmbd_Serializer_t * ser, uint32 position)
{
    uint32 const new_position = LIBEMBD_MIN(position, ser->capacity);
    ser->position = new_position;
}

LIBEMBD_LOCAL_INLINE void libembd_deserializer_skip(LibEmbd_Deserializer_t * deser, uint32 num_bytes)
{
    uint32 const new_position = LIBEMBD_MIN(deser->position + num_bytes, deser->capacity);
    deser->position = new_position;
}

LIBEMBD_LOCAL_INLINE void libembd_deserializer_seek(LibEmbd_Deserializer_t * deser, uint32 position)
{
    uint32 const new_position = LIBEMBD_MIN(position, deser->capacity);
    deser->position = new_position;
}

LIBEMBD_LOCAL_INLINE void libembd_write_uint8_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint8 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(ser, position , sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_WRITE_TO_HOST_TYPE_GENERIC(ser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_put_uint8_unsafe(LibEmbd_Serializer_t *ser, uint8 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_PUT_TO_HOST_TYPE_GENERIC(ser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_write_uint16_to_network_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint16 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(ser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_write_to_network_short_internal(ser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_put_uint16_to_network_unsafe(LibEmbd_Serializer_t *ser, uint16 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_put_to_network_short_internal(ser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_write_uint16_to_host_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint16 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_WRITE_TO_HOST_TYPE_GENERIC(ser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_put_uint16_to_host_unsafe(LibEmbd_Serializer_t *ser, uint16 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_PUT_TO_HOST_TYPE_GENERIC(ser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_write_uint32_to_network_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint32 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(ser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_write_to_network_long_internal(ser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_put_uint32_to_network_unsafe(LibEmbd_Serializer_t *ser, uint32 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_put_to_network_long_internal(ser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_write_uint32_to_host_unsafe(LibEmbd_Serializer_t *ser, uint32 position, uint32 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(ser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_WRITE_TO_HOST_TYPE_GENERIC(ser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_put_uint32_to_host_unsafe(LibEmbd_Serializer_t *ser, uint32 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_PUT_TO_HOST_TYPE_GENERIC(ser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_write_float32_to_network_unsafe(LibEmbd_Serializer_t *ser, uint32 position, float32 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(ser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_write_to_network_float32_internal(ser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_put_float32_to_network_unsafe(LibEmbd_Serializer_t *ser, float32 const host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_put_to_network_float32_internal(ser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_write_float32_to_host_unsafe(LibEmbd_Serializer_t *ser, uint32 position, float32 host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(ser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_WRITE_TO_HOST_TYPE_GENERIC(ser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_put_float32_to_host_unsafe(LibEmbd_Serializer_t *ser, float32 const host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_PUT_TO_HOST_TYPE_GENERIC(ser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_write_buffer_unsafe(LibEmbd_Serializer_t *ser, uint32 position, const void* buffer, uint32 buffer_length)
{
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(ser, position, buffer_length);

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MEMCPY(&(ser->buffer[position]), buffer, buffer_length);
}

LIBEMBD_LOCAL_INLINE void libembd_put_buffer_unsafe(LibEmbd_Serializer_t *ser, const void* buffer, uint32 buffer_length)
{
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(ser);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(ser, buffer_length);

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MEMCPY(&(ser->buffer[ser->position]), buffer, buffer_length);
}

LIBEMBD_LOCAL_INLINE void libembd_read_uint8_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint8 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(deser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_read_u8_internal(deser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_uint8_unsafe(LibEmbd_Deserializer_t *deser, uint8 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(deser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_get_u8_internal(deser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_read_uint16_from_network_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint16 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(deser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_read_from_network_short_internal(deser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_uint16_from_network_unsafe(LibEmbd_Deserializer_t *deser, uint16 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(deser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_get_from_network_short_internal(deser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_read_uint16_from_host_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint16 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(deser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_READ_FROM_HOST_TYPE_GENERIC(deser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_uint16_from_host_unsafe(LibEmbd_Deserializer_t *deser, uint16 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(deser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_GET_FROM_HOST_TYPE_GENERIC(deser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_read_uint32_from_network_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint32 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(deser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_read_from_network_long_internal(deser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_uint32_from_network_unsafe(LibEmbd_Deserializer_t *deser, uint32 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(deser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_get_from_network_long_internal(deser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_read_uint32_from_host_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, uint32 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(deser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_READ_FROM_HOST_TYPE_GENERIC(deser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_uint32_from_host_unsafe(LibEmbd_Deserializer_t *deser, uint32 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(deser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_GET_FROM_HOST_TYPE_GENERIC(deser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_read_float32_from_network_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, float32 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(deser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_read_from_network_float32_internal(deser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_float32_from_network_unsafe(LibEmbd_Deserializer_t *deser, float32 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(deser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    libembd_get_from_network_float32_internal(deser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_read_float32_from_host_unsafe(LibEmbd_Deserializer_t const *deser, uint32 position, float32 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(deser, position, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_READ_FROM_HOST_TYPE_GENERIC(deser, position, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_get_float32_from_host_unsafe(LibEmbd_Deserializer_t *deser, float32 *host_value) {
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(host_value);
    LIBEMBD_MARSHALLING_ASSERT_NO_OVERFLOW(deser, sizeof(host_value));

    /*-----------------------implementation-------------------------*/
    LIBEMBD_MARSHALLING_GET_FROM_HOST_TYPE_GENERIC(deser, host_value);
}

LIBEMBD_LOCAL_INLINE void libembd_read_buffer_unsafe(LibEmbd_Deserializer_t *deser, uint32 position, void* buffer, uint32* pLength)
{
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(buffer);
    LIBEMBD_MARSHALLING_ASSERT_HAS_SPACE_FOR(deser, position, 0); //check position valid

    /*-----------------------implementation-------------------------*/
    uint32 const bytes_copyable = deser->capacity - position;
    uint32 const bytes_copied = LIBEMBD_MIN(*pLength, bytes_copyable);
    LIBEMBD_MEMCPY(buffer, &(deser->buffer[position]), bytes_copied);
    *pLength = bytes_copied;
}

LIBEMBD_LOCAL_INLINE void libembd_get_buffer_unsafe(LibEmbd_Deserializer_t *deser, void* buffer, uint32* pLength)
{
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(deser);
    LIBEMBD_MARSHALLING_ASSERT_POINTER_NOT_NULL(buffer);

    /*-----------------------implementation-------------------------*/
    uint32 const bytes_in_buffer = deser->capacity - deser->position;
    uint32 const bytes_copied = LIBEMBD_MIN(*pLength, bytes_in_buffer);
    LIBEMBD_MEMCPY(buffer, &(deser->buffer[deser->position]), bytes_copied);
    *pLength = bytes_copied;
}
/*-----------------------------------------------------------------API Implementaton End----------------------------------------------------------------------------*/

#endif /* LIBEMBD_MARSHALLING_H_ */
