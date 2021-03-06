#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

#include "active.h"
#include <vector>

class util {
public:
  // Return, in seconds, x-y.
  static double timeval_subtract (struct timeval x, 
                                  struct timeval y);

    static void enable_bullet_recording();
    static void note_new_bullet(active::ptr& ptr);
    static size_t get_bullet_ptrs(std::vector<active::ptr>* dest);
};

inline double 
util::timeval_subtract (struct timeval x, 
			struct timeval y) {
  struct timeval result;
  /* Perform the carry for the later subtraction by updating y. */
  if (x.tv_usec < y.tv_usec) {
    int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
    y.tv_usec -= 1000000 * nsec;
    y.tv_sec += nsec;
  }
  if (x.tv_usec - y.tv_usec > 1000000) {
    int nsec = (x.tv_usec - y.tv_usec) / 1000000;
    y.tv_usec += 1000000 * nsec;
    y.tv_sec -= nsec;
  }
     
  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result.tv_sec = x.tv_sec - y.tv_sec;
  result.tv_usec = x.tv_usec - y.tv_usec;
     
  /* Return 1 if result is negative. */
  return result.tv_sec + (result.tv_usec / 1000000.0);
}

#endif
