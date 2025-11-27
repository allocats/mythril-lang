#pragma once
#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>

#define MEOW_ERROR(fmt, ...) \
    fprintf(stderr, "[ERROR] (%s:%d:%s): " fmt "\n", \
        __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define MEOW_LOG(fmt, ...) \
    fprintf(stderr, "[LOG] (%s:%d:%s): " fmt "\n", \
        __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define MEOW_WARN(fmt, ...) \
    fprintf(stderr, "[WARN] (%s:%d:%s): " fmt "\n", \
        __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define MEOW_ASSERT(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "[ASSERT] (%s:%d:%s): Assertion '%s' failed: " fmt "\n", \
                __FILE__, __LINE__, __func__, #expr, ##__VA_ARGS__); \
            abort(); \
        } \
    } while(0)

#define MEOW_PANIC(fmt, ...) \
    do { \
        fprintf(stderr, "[PANIC] (%s:%d:%s): " fmt "\n", \
            __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        abort(); \
    } while(0)
    
#define MEOW_UNREACHABLE(fmt, ...) \
    do { \
        fprintf(stderr, "[UNREACHABLE] (%s:%d:%s): " fmt "\n", \
            __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        abort(); \
    } while(0)
    
#define MEOW_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define MEOW_LIKELY(x)   __builtin_expect(!!(x), 1)

#endif // !MACROS_H
