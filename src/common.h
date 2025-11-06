#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define export __attribute__((visibility("default")))
#define len(x) (sizeof(x) / sizeof(x[0]))

#define popcnt __builtin_popcount
#define ctz    __builtin_ctz
#define clz    __builtin_clz
#define sqrtf  __builtin_sqrtf

// This is actually floor(log2(x)) / log2(e) which is not exactly ln(x) but
// good enough for our use case.
#define fastln(x) ((f32)(31 - clz(x)) * 0.6931471805599453f)

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef size_t usize;

typedef float  f32;
typedef double f64;

#ifdef __FILE_NAME__
  #define ASSERT_FILE __FILE_NAME__
#else
  #define ASSERT_FILE __FILE__
#endif

#define ASSERT_STR_EXPAND(x) ASSERT_STR(x)
#define ASSERT_STR(x) #x
#define ASSERT_LINE ASSERT_STR_EXPAND(__LINE__)

#define ASSERT_LOC ASSERT_FILE ":" ASSERT_LINE ": "

#define assert(x, msg)                                                         \
  do {                                                                         \
    if ((x)) {                                                                 \
    } else {                                                                   \
      const char str[] = ASSERT_LOC "Assertion `" #x "` failed\n" msg "\n";    \
      log_error(str, sizeof(str) - 1);                                         \
      __builtin_trap();                                                        \
    }                                                                          \
  } while (0)

extern void log_error(const char *ptr, u32 size);
extern void dump(f32);

#endif
