#ifndef LIBEMBD_LOGGING_H_
#define LIBEMBD_LOGGING_H_

#include "libembd/libembd_platform_types.h"
#include "libembd/libembd_common.h"
#include "libembd/libembd_util.h"

// Log levels
#define LIBEMBD_LOG_LEVEL_NONE       0
#define LIBEMBD_LOG_LEVEL_ERROR      1
#define LIBEMBD_LOG_LEVEL_WARN       2
#define LIBEMBD_LOG_LEVEL_INFO       3
#define LIBEMBD_LOG_LEVEL_DEBUG      4
#define LIBEMBD_LOG_LEVEL_VERBOSE    5

//! please make sure the following macros are correctly configured!
/*--------------------------------------------------- Macro Configurations--------------------------------------------------------*/
//! please provide the necessary includes and define the loggger lib api prior to using this library
#ifndef LIBEMBD_LOGGER_LIB_API
    #define LIBEMBD_LOGGER_LIB_API 
#endif

//! please define a mapping from LIBEMBD log levels to native logging library log levels
typedef int LibEmbd_LoggerLib_LogLevel_t;
#define LIBEMBD_LOGGER_LIB_LOG_LEVEL_NONE      
#define LIBEMBD_LOGGER_LIB_LOG_LEVEL_ERROR       
#define LIBEMBD_LOGGER_LIB_LOG_LEVEL_WARN        
#define LIBEMBD_LOGGER_LIB_LOG_LEVEL_INFO        
#define LIBEMBD_LOGGER_LIB_LOG_LEVEL_DEBUG       
#define LIBEMBD_LOGGER_LIB_LOG_LEVEL_VERBOSE

//! Set compile-time log level
#ifndef LIBEMBD_LOG_LEVEL
    #define LIBEMBD_LOG_LEVEL LIBEMBD_LOG_LEVEL_DEBUG
#endif

//! please provide desired application id prior to including this file
#ifndef LIBEMBD_LOG_APPLICATION_ID
    #define LIBEMBD_LOG_APPLICATION_ID           "APPCORE"
#endif

//! please provide desired component id prior to including this file
#ifndef LIBEMBD_LOG_COMPONENT_ID
    #define LIBEMBD_LOG_COMPONENT_ID             "LOGGING"
#endif

typedef uint8 LibEmbd_LogAsHexDataLen_t;
//! please configure the following lines to change log to hex strategy
//! note that LIBEMBD_LOG_TO_HEX_MAX_BYTES should never be configured to be larger than the numerial limit of uint8
#ifndef LIBEMBD_LOG_TO_HEX_MAX_BYTES
    #define LIBEMBD_LOG_TO_HEX_MAX_BYTES ((LibEmbd_LogAsHexDataLen_t)64)
#endif
LIBEMBD_STATIC_ASSERT(LIBEMBD_LOG_TO_HEX_MAX_BYTES <= UINT8_MAX, "");

//! please configure the following line to choose the logger style (see above)
#define LIBEMBD_LOG_API LIBEMBD_LOG_WITH_CONTEXT
//#define LIBEMBD_LOG_API LIBEMBD_LOG_WITH_LOCATION
//#define LIBEMBD_LOG_API LIBEMBD_LOG_WITH_CONTEXT_AND_LOCATION

#ifndef LIBEMBD_LOG_TO_HEX_ENABLE_LINE_BREAK
    #define LIBEMBD_LOG_TO_HEX_ENABLE_LINE_BREAK STD_OFF
#endif

#ifndef LIBEMBD_LOG_TO_HEX_LINE_BREAK_THRESHOLD
    #define LIBEMBD_LOG_TO_HEX_LINE_BREAK_THRESHOLD 32
#endif
/*--------------------------------------------------- Macro Configurations--------------------------------------------------------*/

#define LIBEMBD_LOG_WITH_LEVEL LIBEMBD_LOGGER_LIB_API //!assumes logger lib api natively supports logging with level information

#define LIBEMBD_LOGGER_CONTEXT LIBEMBD_LOG_APPLICATION_ID, LIBEMBD_LOG_COMPONENT_ID
#define LIBEMBD_LOG_LOCATION __FILE__, __LINE__

#define LIBEMBD_LOG_WITH_CONTEXT(level, format, ...) \
    LIBEMBD_LOG_WITH_LEVEL(level, "[%s:%s] "format, LIBEMBD_LOGGER_CONTEXT, ##__VA_ARGS__)

#define LIBEMBD_LOG_WITH_LOCATION(level, format, ...) \
    LIBEMBD_LOG_WITH_LEVEL(level, "[%s:%d] "format, LIBEMBD_LOG_LOCATION, ##__VA_ARGS__)

#define LIBEMBD_LOG_WITH_CONTEXT_AND_LOCATION(level, format, ...) \
    LIBEMBD_LOG_WITH_LEVEL(level, "[%s:%s] [%s:%d] "format, LIBEMBD_LOGGER_CONTEXT, LIBEMBD_LOG_LOCATION, ##__VA_ARGS__)

// Compile-time log level macro
#if LIBEMBD_LOG_LEVEL >= LIBEMBD_LOG_LEVEL_ERROR
    #define LIBEMBD_LOG_ERROR(format, ...) LIBEMBD_LOG_API(LIBEMBD_LOGGER_LIB_LOG_LEVEL_ERROR, format, ##__VA_ARGS__)
    #define LIBEMBD_LOG_AS_HEX_ERROR(format, data, len, ...) appcore_log_as_hex_with_level(LIBEMBD_LOGGER_LIB_LOG_LEVEL_ERROR, format, data, len, ##__VA_ARGS__)
#else
    #define LIBEMBD_LOG_ERROR(format, ...) (void)0
    #define LIBEMBD_LOG_AS_HEX_ERROR(format, data, len, ...) (void)0
#endif

#if LIBEMBD_LOG_LEVEL >= LIBEMBD_LOG_LEVEL_WARN
    #define LIBEMBD_LOG_WARN(format, ...) LIBEMBD_LOG_API(LIBEMBD_LOGGER_LIB_LOG_LEVEL_WARN, format, ##__VA_ARGS__)
    #define LIBEMBD_LOG_AS_HEX_WARN(format, data, len, ...) appcore_log_as_hex_with_level(LIBEMBD_LOGGER_LIB_LOG_LEVEL_WARN, format, data, len, ##__VA_ARGS__)
#else
    #define LIBEMBD_LOG_WARN(format, ...) (void)0
    #define LIBEMBD_LOG_AS_HEX_WARN(format, data, len, ...) (void)0
#endif

#if LIBEMBD_LOG_LEVEL >= LIBEMBD_LOG_LEVEL_INFO
    #define LIBEMBD_LOG_INFO(format, ...) LIBEMBD_LOG_API(LIBEMBD_LOGGER_LIB_LOG_LEVEL_INFO, format, ##__VA_ARGS__)
    #define LIBEMBD_LOG_AS_HEX_INFO(format, data, len, ...) appcore_log_as_hex_with_level(LIBEMBD_LOGGER_LIB_LOG_LEVEL_INFO, format, data, len, ##__VA_ARGS__)
#else
    #define LIBEMBD_LOG_INFO(format, ...) (void)0
    #define LIBEMBD_LOG_AS_HEX_INFO(format, data, len, ...) (void)0
#endif

#if LIBEMBD_LOG_LEVEL >= LIBEMBD_LOG_LEVEL_DEBUG
    #define LIBEMBD_LOG_DEBUG(format, ...) LIBEMBD_LOG_API(LIBEMBD_LOGGER_LIB_LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
    #define LIBEMBD_LOG_AS_HEX_DEBUG(format, data, len, ...) appcore_log_as_hex_with_level(LIBEMBD_LOGGER_LIB_LOG_LEVEL_DEBUG, format, data, len, ##__VA_ARGS__)
#else
    #define LIBEMBD_LOG_DEBUG(format, ...) (void)0
    #define LIBEMBD_LOG_AS_HEX_DEBUG(format, data, len, ...) (void)0
#endif

#if LIBEMBD_LOG_LEVEL >= LIBEMBD_LOG_LEVEL_VERBOSE
    #define LIBEMBD_LOG_VERBOSE(format, ...) LIBEMBD_LOG_API(LIBEMBD_LOGGER_LIB_LOG_LEVEL_VERBOSE, format, ##__VA_ARGS__)
    #define LIBEMBD_LOG_AS_HEX_VERBOSE(format, data, len, ...) appcore_log_as_hex_with_level(LIBEMBD_LOGGER_LIB_LOG_LEVEL_VERBOSE, format, data, len, ##__VA_ARGS__)
#else
    #define LIBEMBD_LOG_VERBOSE(format, ...) (void)0
    #define LIBEMBD_LOG_AS_HEX_VERBOSE(format, data, len, ...) (void)0
#endif

#define LIBEMBD_LOG_DEVELOPMENT_ERROR(format, ...) LIBEMBD_LOG_WARN("[DEV ERR] "format, ##__VA_ARGS__)

LIBEMBD_HEADER_API void appcore_log_as_hex_with_level(LibEmbd_LoggerLib_LogLevel_t const level, const char* format, const void* data, uint16 length, ...) {
#if (LIBEMBD_LOGGER_LIB == LIBEMBD_LOGGER_LIB_NO_LIB)
    LIBEMBD_IGNORE_PARAM(level);
    LIBEMBD_IGNORE_PARAM(format);
    LIBEMBD_IGNORE_PARAM(data);
    LIBEMBD_IGNORE_PARAM(length);
#else
    // Precomputed lookup table for hex characters
    static const char hex_lookup[] = "0123456789abcdef";
    LibEmbd_LogAsHexDataLen_t const max_bytes = LIBEMBD_MIN(length, LIBEMBD_LOG_TO_HEX_MAX_BYTES);

#if LIBEMBD_LOG_TO_HEX_ENABLE_LINE_BREAK == STD_ON
    // 3 bytes per hex data + 1 '\n' per line + 1 for null termination 
    char hex_buffer[UINT8_MAX * 3 + (UINT8_MAX / LIBEMBD_LOG_AS_HEX_MAX_BYTES_PER_LINE) + 1]; 
    uint32 line_bytes = 0;
#else
    // 3 bytes per hex data + 1 for null termination 
    char hex_buffer[LIBEMBD_LOG_TO_HEX_MAX_BYTES * 3 + 1]; 
#endif

    LibEmbd_Size_t buffer_index = 0;
    const unsigned char* const bytes = (const unsigned char*)data;
    for (LibEmbd_Size_t i = 0; i < max_bytes; ++i) {
        hex_buffer[buffer_index++] = hex_lookup[bytes[i] >> 4]; // upper nibble
        hex_buffer[buffer_index++] = hex_lookup[bytes[i] & 0x0F]; // lower nibble
        hex_buffer[buffer_index++] = ' ';

#if LIBEMBD_LOG_TO_HEX_ENABLE_LINE_BREAK == STD_ON
        line_bytes++;
        if (line_bytes == LIBEMBD_LOG_TO_HEX_LINE_BREAK_THRESHOLD) {
            hex_buffer[buffer_index++] = '\n';
            line_bytes = 0;
        }
#endif    
    }

    hex_buffer[buffer_index] = '\0';

    va_list args;
    va_start(args, length);

    char log_message[512];
    vsnprintf(log_message, sizeof(log_message), format, args);
    va_end(args);

    LIBEMBD_LOG_API(level, "%s %s", log_message, hex_buffer);

#endif
}

LIBEMBD_HEADER_API void appcore_hex_to_str_no_line_break(const void* data, uint8 length, char* buffer)
{
#if (LIBEMBD_LOGGER_LIB == LIBEMBD_LOGGER_LIB_NO_LIB)
    LIBEMBD_IGNORE_PARAM(data);
    LIBEMBD_IGNORE_PARAM(length);
    LIBEMBD_IGNORE_PARAM(buffer);
#else
    // Precomputed lookup table for hex characters
    static const char hex_lookup[] = "0123456789abcdef";
    const unsigned char* bytes = (const unsigned char*)data;

    uint8 buffer_index = 0;
    for (uint8 i = 0; i < length; ++i) {
        buffer[buffer_index++] = hex_lookup[bytes[i] >> 4]; // upper nibble
        buffer[buffer_index++] = hex_lookup[bytes[i] & 0x0F]; // lower nibble
        buffer[buffer_index++] = ' ';
    }

    buffer[buffer_index] = '\0';
#endif
}

#endif /* LIBEMBD_LOGGING_H_ */
