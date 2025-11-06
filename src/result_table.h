#ifndef RESULT_H
#define RESULT_H

typedef enum {
  PLAYING = 0,
  X_WIN,
  O_WIN,
  TIE,
} Result;

#define TABLE_SIZE (1 << 18)
extern Result RESULT_TABLE[TABLE_SIZE];

extern void create_table(void);

#endif
