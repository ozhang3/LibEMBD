#ifndef LIBEMBD_TIMER_IMPL_H_
#define LIBEMBD_TIMER_IMPL_H_

#include "libembd/libembd_util.h"
#include "libembd/libembd_timer.h"

#define LIBEMBD_TIMER_PERIOD_5_MS       ((libembd_timer_duration_ms)5u)
#define LIBEMBD_TIMER_PERIOD_10_MS      ((libembd_timer_duration_ms)10u)
#define LIBEMBD_TIMER_PERIOD_20_MS      ((libembd_timer_duration_ms)20u)
#define LIBEMBD_TIMER_PERIOD_100_MS     ((libembd_timer_duration_ms)100u)
#define LIBEMBD_TIMER_PERIOD_1000_MS    ((libembd_timer_duration_ms)1000u)


typedef uint8 LibEmbd_Timer_State_t;
#define TIMER_STATE_UNINIT              ((LibEmbd_Timer_State_t)0x00u)
#define TIMER_STATE_INIT                ((LibEmbd_Timer_State_t)0x01u)
#define TIMER_STATE_ACTIVE              ((LibEmbd_Timer_State_t)0x02u)
//#define TIMER_STATE_IN_CALLBACK         ((LibEmbd_Timer_State_t)0x04u)
//! note we deliberately leave out a "TIMER_STATE_IN_CALLBACK" state because we assume we are running in a strictly single-core environment
//! and that it is not possible for an observer to get an "in callback" state while the callback is being executed

#define LIBEMBD_IS_TIMER_UNITIALIZED(timer)         ((timer)->timer_state == TIMER_STATE_UNINIT)
#define LIBEMBD_IS_TIMER_STARTED(timer)             ((timer)->timer_state == TIMER_STATE_ACTIVE)
#define LIBEMBD_IS_TIMER_STOPPED(timer)             ((timer)->timer_state == TIMER_STATE_INIT)

#define LIBEMBD_SET_TIMER_STATE_STARTED(timer)      ((timer)->timer_state = TIMER_STATE_ACTIVE)
#define LIBEMBD_SET_TIMER_STATE_STOPPED(timer)      ((timer)->timer_state = TIMER_STATE_INIT)

struct LibEmbd_Timer_t {
    LibEmbd_Timer_State_t timer_state;
    LibEmbd_Timer_Type_t type;
    libembd_timer_duration_ms interval;
    libembd_timer_duration_ms time_elapsed;
    libembd_timer_task_t on_expiry;
};

// Helper macro for checking pointer validity
#if LIBEMBD_ENABLE_DEV_ERROR_CHECK
    #define LIBEMBD_TIMER_CHECK_POINTER_NOT_NULL(ptr) \
        do { \
            if((ptr) == NULL) { return E_NOT_OK; } \
        } while (0)
#else
    #define LIBEMBD_TIMER_CHECK_POINTER_NOT_NULL(ptr) (void)0
#endif

typedef void (*libembd_timer_post_expiry_handler_t)(LibEmbd_Timer_t * const timer);

LIBEMBD_LOCAL_INLINE void libembd_timer_post_expiry_oneshot(LibEmbd_Timer_t * const timer)
{
    LIBEMBD_SET_TIMER_STATE_STOPPED(timer);
}

LIBEMBD_LOCAL_INLINE void libembd_timer_post_expiry_recurring(LibEmbd_Timer_t * const timer)
{
    //check if user cancelled timer in expiry handler
    if(!LIBEMBD_IS_TIMER_STOPPED(timer)){
        timer->time_elapsed = 0u; //rearm timer
    }
}

LIBEMBD_STATIC_ASSERT(TIMER_TYPE_ONE_SHOT == 0x00u &&\
                      TIMER_TYPE_PERIODIC == 0x01u &&\
                      TIMER_TYPE_LAST_DO_NOT_USE == 0x02u, "");
                      
LIBEMBD_LOCAL const libembd_timer_post_expiry_handler_t g_libembd_post_expiry_handler_map[TIMER_TYPE_LAST_DO_NOT_USE] =
{
    libembd_timer_post_expiry_oneshot, libembd_timer_post_expiry_recurring
};

#define LIBEMBD_TIMER_HANDLE_POST_EXPIRY(timer, timer_type)     (g_libembd_post_expiry_handler_map[timer_type](timer))

LIBEMBD_HEADER_API_INLINE LibEmbd_Std_ReturnType libembd_make_timer(LibEmbd_Timer_t* timer, LibEmbd_Timer_Type_t timer_type, libembd_timer_task_t timer_task)
{
    LIBEMBD_TIMER_CHECK_POINTER_NOT_NULL(timer);
    LIBEMBD_TIMER_CHECK_POINTER_NOT_NULL(timer_task);

    if((timer_type != TIMER_TYPE_ONE_SHOT) && (timer_type != TIMER_TYPE_PERIODIC))
        return E_NOT_OK;

    timer->type = timer_type;
    timer->on_expiry = timer_task;

    LIBEMBD_SET_TIMER_STATE_STOPPED(timer);

    return E_OK;
}

LIBEMBD_HEADER_API_INLINE LibEmbd_Std_ReturnType libembd_start_timer(LibEmbd_Timer_t* timer, libembd_timer_duration_ms duration)
{
    if(LIBEMBD_IS_TIMER_UNITIALIZED(timer)){
        return E_NOT_OK; //cannot start an unitiialized timer (starting an already started timer is ok)
    }

    timer->interval = duration; 
    timer->time_elapsed = 0u; //make sure to (re)initialize the elapsed time to zero as this may be a restart

    LIBEMBD_SET_TIMER_STATE_STARTED(timer);

    return E_OK;
}

LIBEMBD_HEADER_API_INLINE void libembd_stop_timer(LibEmbd_Timer_t* timer)
{
    if(LIBEMBD_IS_TIMER_UNITIALIZED(timer)){
        return; //cannot stop an unitialized timer
    }

    LIBEMBD_SET_TIMER_STATE_STOPPED(timer); //reset timer state
    //no need to reset other state information here as they will be assigned again when the timer re-starts
}

LIBEMBD_HEADER_API_INLINE void libembd_rewind_timer(LibEmbd_Timer_t* timer)
{
    if(!LIBEMBD_IS_TIMER_STARTED(timer)){
        return; //rewind only works on started timers
    }
    
    timer->time_elapsed = 0u;
}

LIBEMBD_LOCAL_INLINE void libembd_timer_tick_internal(LibEmbd_Timer_t* const timer, libembd_timer_duration_ms const period)
{
    //only active timers that are not executing timer tasks are processed in a timer tick
    if(!LIBEMBD_IS_TIMER_STARTED(timer)){
        return;
    }

    // increment time elapsed by the timer period 
    timer->time_elapsed += period;

    if(timer->time_elapsed >= timer->interval){
        
        timer->on_expiry(); //invoke user callback

        LIBEMBD_TIMER_HANDLE_POST_EXPIRY(timer, timer->type); //disarm/rearm timer
    }
}

LIBEMBD_HEADER_API_INLINE void libembd_timer_tick_5ms(LibEmbd_Timer_t* timer)
{
    libembd_timer_tick_internal(timer, LIBEMBD_TIMER_PERIOD_5_MS);
}

LIBEMBD_HEADER_API_INLINE void libembd_timer_tick_10ms(LibEmbd_Timer_t* timer)
{
    libembd_timer_tick_internal(timer, LIBEMBD_TIMER_PERIOD_10_MS);
}

LIBEMBD_HEADER_API_INLINE void libembd_timer_tick_20ms(LibEmbd_Timer_t* timer)
{
    libembd_timer_tick_internal(timer, LIBEMBD_TIMER_PERIOD_20_MS);
}

LIBEMBD_HEADER_API_INLINE void libembd_timer_tick_100ms(LibEmbd_Timer_t* timer)
{
    libembd_timer_tick_internal(timer, LIBEMBD_TIMER_PERIOD_100_MS);
}

LIBEMBD_HEADER_API_INLINE void libembd_timer_tick_1s(LibEmbd_Timer_t* timer)
{
    libembd_timer_tick_internal(timer, LIBEMBD_TIMER_PERIOD_1000_MS);    
}

LIBEMBD_HEADER_API_INLINE boolean libembd_timer_is_timer_stopped(LibEmbd_Timer_t const * timer)
{
    return timer->timer_state == TIMER_STATE_INIT;
}

LIBEMBD_HEADER_API_INLINE boolean libembd_timer_is_timer_started(LibEmbd_Timer_t const * timer)
{
    return timer->timer_state == TIMER_STATE_ACTIVE;
}

#endif /* LIBEMBD_TIMER_IMPL_H_ */
