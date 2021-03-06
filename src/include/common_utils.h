/*
 * vim:noexpandtab:shiftwidth=4:tabstop=8:
 */

/**
 * @file common_utils.h
 * @brief Common tools for printing, parsing, ....
 */

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "gsh_types.h"
#include "log.h"

/**
 * BUILD_BUG_ON - break compile if a condition is true.
 * @condition: the condition which the compiler should know is false.
 *
 * If you have some code which relies on certain constants being equal, or
 * other compile-time-evaluated condition, you should use BUILD_BUG_ON to
 * detect if someone changes it.
 *
 * The implementation uses gcc's reluctance to create a negative array, but
 * gcc (as of 4.4) only emits that error for obvious cases (eg. not arguments
 * to inline functions).  So as a fallback we use the optimizer; if it can't
 * prove the condition is false, it will cause a link error on the undefined
 * "__build_bug_on_failed".  This error message can be harder to track down
 * though, hence the two different methods.
 *
 * Blatantly stolen from kernel source, include/linux/kernel.h:651
 */
#ifndef __OPTIMIZE__
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#else
extern int __build_bug_on_failed;
#define BUILD_BUG_ON(condition)					\
	do {							\
		((void)sizeof(char[1 - 2*!!(condition)]));      \
		if (condition)					\
			__build_bug_on_failed = 1;		\
	} while (0)
#endif

/* Most machines scandir callback requires a const. But not all */
#define SCANDIR_CONST const

/* Most machines have mntent.h. */
#define HAVE_MNTENT_H 1

/* String parsing functions */

#ifndef HAVE_STRLCPY
extern size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_STRNLEN
#define strnlen(a, b) gsh_strnlen(a, b)
/* prefix with gsh_ to prevent library conflict -- will fix properly
   with new build system */
extern size_t gsh_strnlen(const char *s, size_t max);
#endif

#if defined(__APPLE__)
#define clock_gettime(a, ts) portable_clock_gettime(ts)
extern int portable_clock_gettime(struct timespec *ts);
#define pthread_yield() pthread_yield_np()
#undef SCANDIR_CONST
#define SCANDIR_CONST
#undef HAVE_MNTENT_H
#endif

#if defined(__FreeBSD__)
#undef SCANDIR_CONST
#define SCANDIR_CONST
#endif

/**
 * @brief Logging rwlock initialization
 *
 * @param[in,out] _lock The rwlock to initialize
 * @param[in,out] _attr The attributes used while initializing the lock
 */
#define PTHREAD_RWLOCK_init(_lock, _attr)				\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_init(_lock, _attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init rwlock %p (%s) at %s:%d",	\
				     _lock, #_lock,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Init rwlock %p (%s) "	\
				"at %s:%d", rc, _lock, #_lock,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging rwlock destroy
 *
 * @param[in,out] _lock The rwlock to destroy
 */
#define PTHREAD_RWLOCK_destroy(_lock)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_destroy(_lock);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy mutex %p (%s) at %s:%d",	\
				     _lock, #_lock,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Destroy mutex %p (%s) "	\
				"at %s:%d", rc, _lock, #_lock,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging write-lock
 *
 * @param[in,out] _lock Read-write lock
 */

#define PTHREAD_RWLOCK_wrlock(_lock)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_wrlock(_lock);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Got write lock on %p (%s) "	\
				     "at %s:%d", _lock, #_lock,		\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, write locking %p (%s) "	\
				"at %s:%d", rc, _lock, #_lock,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)							\

/**
 * @brief Logging read-lock
 *
 * @param[in,out] _lock Read-write lock
 */

#define PTHREAD_RWLOCK_rdlock(_lock)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_rdlock(_lock);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Got read lock on %p (%s) "	\
				     "at %s:%d", _lock, #_lock,		\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, read locking %p (%s) "	\
				"at %s:%d", rc, _lock, #_lock,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)							\

/**
 * @brief Logging read-write lock unlock
 *
 * @param[in,out] _lock Read-write lock
 */

#define PTHREAD_RWLOCK_unlock(_lock)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_unlock(_lock);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Unlocked %p (%s) at %s:%d",       \
				     _lock, #_lock,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, unlocking %p (%s) at %s:%d",	\
				rc, _lock, #_lock,			\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)							\

/**
 * @brief Logging mutex lock
 *
 * @param[in,out] _mtx The mutex to acquire
 */

#define PTHREAD_MUTEX_lock(_mtx)					\
	do {								\
		int rc;							\
									\
		rc = pthread_mutex_lock(_mtx);				\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Acquired mutex %p (%s) at %s:%d",	\
				     _mtx, #_mtx,			\
				     __FILE__, __LINE__);		\
		} else{							\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, acquiring mutex %p (%s) "	\
				"at %s:%d", rc, _mtx, #_mtx,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging mutex unlock
 *
 * @param[in,out] _mtx The mutex to relinquish
 */

#define PTHREAD_MUTEX_unlock(_mtx)					\
	do {								\
		int rc;							\
									\
		rc = pthread_mutex_unlock(_mtx);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Released mutex %p (%s) at %s:%d",	\
				     _mtx, #_mtx,			\
				     __FILE__, __LINE__);		\
		} else{							\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, releasing mutex %p (%s) "	\
				"at %s:%d", rc, _mtx, #_mtx,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging mutex initialization
 *
 * @param[in,out] _mtx The mutex to initialize
 * @param[in,out] _attr The attributes used while initializing the mutex
 */
#define PTHREAD_MUTEX_init(_mtx, _attr)					\
	do {								\
		int rc;							\
									\
		rc = pthread_mutex_init(_mtx, _attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init mutex %p (%s) at %s:%d",	\
				     _mtx, #_mtx,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Init mutex %p (%s) "		\
				"at %s:%d", rc, _mtx, #_mtx,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging mutex destroy
 *
 * @param[in,out] _mtx The mutex to destroy
 */

#define PTHREAD_MUTEX_destroy(_mtx)					\
	do {								\
		int rc;							\
									\
		rc = pthread_mutex_destroy(_mtx);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy mutex %p (%s) at %s:%d",	\
				     _mtx, #_mtx,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Destroy mutex %p (%s) "	\
				"at %s:%d", rc, _mtx, #_mtx,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging condition variable initialization
 *
 * @param[in,out] _cond The condition variable to initialize
 * @param[in,out] _attr The attributes used while initializing the
 *			condition variable
 */
#define PTHREAD_COND_init(_cond, _attr)					\
	do {								\
		int rc;							\
									\
		rc = pthread_cond_init(_cond, _attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init cond %p (%s) at %s:%d",	\
				     _cond, #_cond,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Init cond %p (%s) "		\
				"at %s:%d", rc, _cond, #_cond,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging condtion variable destroy
 *
 * @param[in,out] _cond The condition variable to destroy
 */

#define PTHREAD_COND_destroy(_cond)					\
	do {								\
		int rc;							\
									\
		rc = pthread_cond_destroy(_cond);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy cond %p (%s) at %s:%d",	\
				     _cond, #_cond,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Destroy cond %p (%s) "	\
				"at %s:%d", rc, _cond, #_cond,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Inline functions for timespec math
 *
 * This is for timespec math.  If you want to do
 * do the same kinds of math on timeval values,
 * See timeradd(3) in GLIBC.
 *
 * The primary purpose of nsecs_elapsed_t is for a compact
 * and quick way to handle time issues relative to server
 * start and server EPOCH (which is not quite the same thing
 * but too complicated to explain here).
 */

/**
 * @brief Get the abs difference between two timespecs in nsecs
 *
 * useful for cheap time calculation. Works with Dr. Who...
 *
 * @param[in] start timespec of before end
 * @param[in] end   timespec after start time
 *
 * @return Elapsed time in nsecs
 */

static inline nsecs_elapsed_t
timespec_diff(const struct timespec *start,
	      const struct timespec *end)
{
	if ((end->tv_sec > start->tv_sec)
	    || (end->tv_sec == start->tv_sec
		&& end->tv_nsec >= start->tv_nsec)) {
		return (end->tv_sec - start->tv_sec) * NS_PER_SEC +
		    (end->tv_nsec - start->tv_nsec);
	} else {
		return (start->tv_sec - end->tv_sec) * NS_PER_SEC +
		    (start->tv_nsec - end->tv_nsec);
	}
}

/**
 * @brief Convert a timespec to an elapsed time interval
 *
 * This will work for wallclock time until 2554.
 */

static
inline nsecs_elapsed_t timespec_to_nsecs(struct timespec *timespec)
{
	return timespec->tv_sec * NS_PER_SEC + timespec->tv_nsec;
}

/**
 * @brief Convert an elapsed time interval to a timespec
 */

static
inline void nsecs_to_timespec(nsecs_elapsed_t interval,
			      struct timespec *timespec)
{
	timespec->tv_sec = interval / NS_PER_SEC;
	timespec->tv_nsec = interval % NS_PER_SEC;
}

/**
 * @brief Add an interval to a timespec
 *
 * @param[in]     interval Nanoseconds to add
 * @param[in,out] timespec Time
 */

static inline void
timespec_add_nsecs(nsecs_elapsed_t interval,
		   struct timespec *timespec)
{
	timespec->tv_sec += (interval / NS_PER_SEC);
	timespec->tv_nsec += (interval % NS_PER_SEC);
	if (timespec->tv_nsec > NS_PER_SEC) {
		timespec->tv_sec += (timespec->tv_nsec / NS_PER_SEC);
		timespec->tv_nsec = timespec->tv_nsec % NS_PER_SEC;
	}
}

/**
 * @brief Subtract an interval from a timespec
 *
 * @param[in]     interval Nanoseconds to subtract
 * @param[in,out] timespec Time
 */

static inline void
timespec_sub_nsecs(nsecs_elapsed_t interval, struct timespec *t)
{
	struct timespec ts;

	nsecs_to_timespec(interval, &ts);

	if (ts.tv_nsec > t->tv_nsec) {
		t->tv_sec -= (ts.tv_sec + 1);
		t->tv_nsec = ts.tv_nsec - t->tv_nsec;
	} else {
		t->tv_sec -= ts.tv_sec;
		t->tv_nsec -= ts.tv_nsec;
	}
}

/**
 * @brief Compare two times
 *
 * Determine if @c t1 is less-than, equal-to, or greater-than @c t2.
 *
 * @param[in] t1 First time
 * @param[in] t2 Second time
 *
 * @retval -1 @c t1 is less-than @c t2
 * @retval 0 @c t1 is equal-to @c t2
 * @retval 1 @c t1 is greater-than @c t2
 */

static inline int gsh_time_cmp(const struct timespec *t1,
			       const struct timespec *t2)
{
	if (t1->tv_sec < t2->tv_sec) {
		return -1;
	} else if (t1->tv_sec > t2->tv_sec) {
		return 1;
	} else {
		if (t1->tv_nsec < t2->tv_nsec)
			return -1;
		else if (t1->tv_nsec > t2->tv_nsec)
			return 1;
	}

	return 0;
}

/**
 * @brief Get the time right now as a timespec
 *
 * @param[out] ts Timespec struct
 */

static inline void now(struct timespec *ts)
{
	int rc;

	rc = clock_gettime(CLOCK_REALTIME, ts);
	if (rc != 0) {
		LogCrit(COMPONENT_MAIN, "Failed to get timestamp");
		assert(0);	/* if this is broken, we are toast so die */
	}
}

/**
 * @brief Copy a string into a buffer safely
 *
 * This function doesn't overflow and and and makes sure the buffer is
 * null terminated.
 *
 * @param[out] dest      Destination buffer
 * @param[in]  src       Source string
 * @param[in]  dest_size Total size of dest
 *
 * @retval 0 on success.
 * @retval -1 if the buffer would overflow (the buffer is not modified)
 */

static inline int strmaxcpy(char *dest, const char *src, size_t dest_size)
{
	int len = strlen(src);
	if (len >= dest_size)
		return -1;

	memcpy(dest, src, len + 1);
	return 0;
}

/**
 * @brief Append a string to buffer safely
 *
 * This function doesn't overflow the buffer, and makes sure the
 * buffer is null terminated.
 *
 * @param[in,out] dest      Destination buffer
 * @param[in]     src       Source string
 * @param[in]     dest_size Total size of dest
 *
 * @retval 0 on success.
 * @retval -1 if the buffer would overflow (the buffer is not modified).
 */

static inline int strmaxcat(char *dest, const char *src, size_t dest_size)
{
	int destlen = strlen(dest);
	int remain = dest_size - destlen;
	int srclen = strlen(src);
	if (remain <= srclen)
		return -1;

	memcpy(dest + destlen, src, srclen + 1);
	return 0;
}

#endif				/* !COMMON_UTILS_H */
