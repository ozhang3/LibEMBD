#ifndef LIBEMBD_SPINLOCK_IMPL_H_
#define LIBEMBD_SPINLOCK_IMPL_H_

#include "libembd_platform_types.h"
#include "libembd/libembd_common.h"
#include "libembd/libembd_spinlock.h"
#include "libembd/libembd_atomic.h"

struct LibEmbd_Spinlock_t {
    LibEmbd_Size_t max_iterations;
    libembd_atomic_bool_t atomic_flag;
};

LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timed_spinlock_init(LibEmbd_Spinlock_t * spinlock, LibEmbd_Size_t max_iterations)
{
    libembd_atomic_store_uint8(&spinlock->atomic_flag, 0);
    spinlock->max_iterations = max_iterations;
}

LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_timed_spinlock_acquire(LibEmbd_Spinlock_t * spinlock)
{
    uint8 expected = 0;
    LibEmbd_Size_t iteration = 0;
    for(;;){
        if(libembd_atomic_compare_exchange_weak_uint8(&spinlock->atomic_flag, &expected, 1)){
            return TRUE;
        }else{
            if(iteration++ == spinlock->max_iterations){
                return FALSE;
            }
        }
    }
}

LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timed_spinlock_release(LibEmbd_Spinlock_t * spinlock)
{
    libembd_atomic_store_explicit_uint8(&spinlock->atomic_flag, 0, libembd_memory_order_release);
}

#endif /* LIBEMBD_SPINLOCK_IMPL_H_ */
