#include <cstdlib>
#include "Log.hpp"

#define ASSERT(condition, message)                                    \
    do {                                                                   \
        if (!(condition)) {                                                \
            Log::Error("Assertion `" #condition "` failed in ", \
                                  __FILE__,                                \
                                  " line ",                                \
                                  __LINE__,                                \
                                  ": ",                                    \
                                  message);                                \
            std::exit(EXIT_FAILURE);                                       \
        }                                                                  \
    } while (false)

#define PANIC(message)                                                              \
    do {                                                                                 \
        Log::Error("Panic in ", __FILE__, " line ", __LINE__, ": ", message); \
        std::exit(EXIT_FAILURE);                                                         \
    } while (false)
