#ifndef LIBEMBD_TIMER_H_
#define LIBEMBD_TIMER_H_

#include "libembd/libembd_common.h"

typedef uint8 LibEmbd_Timer_Type_t;
#define TIMER_TYPE_ONE_SHOT                 ((LibEmbd_Timer_Type_t)0x00u)
#define TIMER_TYPE_PERIODIC                 ((LibEmbd_Timer_Type_t)0x01u)

#define TIMER_TYPE_LAST_DO_NOT_USE          ((LibEmbd_Timer_Type_t)0x02u)

/**
 * @brief The user callback to be called when timer expires.
 * @warning //!Do not call timer tick functions inside the callback, which leads to undefined behavior.
 *          Other timer apis are safe to call in callback context. However, you may still receive errors for incorrect use of the apis (see below). 
 */
typedef void (*libembd_timer_task_t)(void);

typedef uint32 libembd_timer_duration_ms; //!ms precision!
#define LIBEMBD_TIMER_DURATION_INF          ((libembd_timer_duration_ms)UINT32_MAX) //infinite timeout
LIBEMBD_STATIC_ASSERT(sizeof(libembd_timer_duration_ms) == sizeof(uint32), "");

typedef struct LibEmbd_Timer_t LibEmbd_Timer_t;

/**
 * @brief Construct a timer
 * 
 * @param timer pointer to an unitialized timer object
 * @param timer_type one-shot/periodic
 * @param timer_task user callback to be executed when timer expires. Must have signature void().
 * @return Std_ReturnType E_OK on success, E_NOT_OK otherwise
 * 
 * @warning It is undefined behavior to intialize an already initialized timer.
 */
LIBEMBD_HEADER_API_INLINE Std_ReturnType LIBEMBD_ATTR_ALWAYS_INLINE libembd_make_timer(LibEmbd_Timer_t* timer, LibEmbd_Timer_Type_t timer_type, libembd_timer_task_t timer_task);

/**
 * @brief Arm the timer
 * 
 * @param timer pointer to initialized timer
 * @param duration the "expires after" time or the period of the timer depending on timer type
 * @return Std_ReturnType E_OK on success, E_NOT_OK otherwise
 * 
 * @note It's ok to start an already started timer. The semantics is equivalent to arming the timer with the new timeout value while leaving the timer state unchanged.
 */
LIBEMBD_HEADER_API_INLINE Std_ReturnType LIBEMBD_ATTR_ALWAYS_INLINE libembd_start_timer(LibEmbd_Timer_t* timer, libembd_timer_duration_ms duration);

/**
 * @brief Disarm the timer
 * 
 * @param timer pointer to initialized timer
 * @note Stopping an unitialized or already-stopped timer has no effect
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_stop_timer(LibEmbd_Timer_t* timer);

/**
 * @brief Reset timer elapsed time
 * 
 * @param timer pointer to initialized timer
 * @note Rewinding a timer does not stop it. Rewinding a stopped timer has no effect.
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_rewind_timer(LibEmbd_Timer_t* timer);

/**
 * @brief The timer tick function called every 5ms
 * 
 * @param timer pointer to initialized timer
 * @note This function needs to be unconditionally invoked exactly every 5ms to ensure correct behavior
 * @note User callbacks will be invoked inside the tick function on timer expiry. 
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timer_tick_5ms(LibEmbd_Timer_t* timer);

/**
 * @brief The timer tick function called every 10ms
 * 
 * @param timer pointer to initialized timer
 * @note This function needs to be unconditionally invoked exactly every 10ms to ensure correct behavior
 * @note User callbacks will be invoked inside the tick function on timer expiry. 
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timer_tick_10ms(LibEmbd_Timer_t* timer);

/**
 * @brief The timer tick function called every 20ms
 * 
 * @param timer pointer to initialized timer
 * @note This function needs to be unconditionally invoked exactly every 20ms to ensure correct behavior
 * @note User callbacks will be invoked inside the tick function on timer expiry. 
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timer_tick_20ms(LibEmbd_Timer_t* timer);

/**
 * @brief The timer tick function called every 100ms
 * 
 * @param timer pointer to initialized timer
 * @note This function needs to be unconditionally invoked exactly every 100ms to ensure correct behavior
 * @note User callbacks will be invoked inside the tick function on timer expiry. 
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timer_tick_100ms(LibEmbd_Timer_t* timer);

/**
 * @brief The timer tick function called every 1s
 * 
 * @param timer pointer to initialized timer
 * @note This function needs to be unconditionally invoked exactly every 1s to ensure correct behavior
 * @note User callbacks will be invoked inside the tick function on timer expiry. 
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timer_tick_1s(LibEmbd_Timer_t* timer);

/**
 * @brief Utility function to check if the timer is stopped 
 * 
 * @param timer pointer to initialized timer object
 * @return TRUE if timer is stopeed FALSE otherwise
 */
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_timer_is_timer_stopped(LibEmbd_Timer_t const * timer);

/**
 * @brief Utility function to check if the timer is started 
 * 
 * @param timer pointer to initialized timer object
 * @return TRUE if timer is started FALSE otherwise
 */
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_timer_is_timer_started(LibEmbd_Timer_t const * timer);

#include "libembd/internal/libemd_timer_impl.h"

#endif /* LIBEMBD_TIMER_H_ */

