#pragma once
#ifdef DEBUG
#define dlog(fmt, ...)                                                         \
  fprintf(stderr, "[LOG %s:%d]: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define PANIC(msg)                                                             \
  do                                                                           \
  {                                                                            \
    fprintf(stderr, "PANIC: %s\n", msg);                                       \
    abort();                                                                   \
  }                                                                            \
  while(0)
#else
#define dlog(fmt, ...)
#define PANIC(msg)
#endif
