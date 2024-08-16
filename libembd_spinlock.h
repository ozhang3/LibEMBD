#ifndef LIBEMBD_SPINLOCK_H_
#define LIBEMBD_SPINLOCK_H_

#include "libembd/libembd_common.h"

typedef struct LibEmbd_Spinlock_t LibEmbd_Spinlock_t;

/**
 * @brief Initialize a timed spinlock
 * 
 * @param spinlock pointer to uninitialized spinlock object
 * @param max_iterations maximum number of iterations to spin for
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timed_spinlock_init(LibEmbd_Spinlock_t * spinlock, LibEmbd_Size_t max_iterations);

/**
 * @brief Tries to acquire the spinlock
 * 
 * @param spinlock pointer to initialized spinlock object
 * @return TRUE: the lock is acquired. FALSE: failed to acquire lock due to timeout (maximum spin iterations has been reached)
 * 
 * @warning CAUTION This function is not safe for use in ISR context!
 */
LIBEMBD_HEADER_API_INLINE boolean LIBEMBD_ATTR_ALWAYS_INLINE libembd_timed_spinlock_try_acquire(LibEmbd_Spinlock_t * spinlock);

/**
 * @brief Releases the acquired spinlock
 * 
 * @param spinlock pointer to initialized spinlock object

 * @note The lock must have already been acquired before calling release else undefined behavior.
 */
LIBEMBD_HEADER_API_INLINE void LIBEMBD_ATTR_ALWAYS_INLINE libembd_timed_spinlock_release(LibEmbd_Spinlock_t * spinlock);

#include "libembd/internal/libembd_spinlock_impl.h"

#endif /* LIBEMBD_SPINLOCK_H_ */
