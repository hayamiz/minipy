#ifndef THREAD_HPP
#define THREAD_HPP

#define LOG_UNLOCKED(...) {\
    const int thread_id = static_cast<int>(pthread_self());             \
    fprintf(stderr, "%08x %s:%d: ", thread_id, __FILE__, __LINE__);     \
    fprintf(stderr, __VA_ARGS__);                                       \
    fprintf(stderr, "\n");                                              \
  }

#define PTH_ASSERT(expression)  {                                       \
    const int status = (expression);                                    \
    if (status != 0) {                                                  \
      LOG_UNLOCKED("%s: %s",#expression, strerror(status));             \
      abort();                                                          \
    }                                                                   \
  }


#endif
