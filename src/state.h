#ifndef STATE_H
#define STATE_H

#include "common.h"
#include "result_table.h"

typedef enum {
  PLAYER_X = 0,
  PLAYER_O,
} Player;

typedef struct {
  i8 grid;
  i8 cell;
} Move;

typedef struct {
  // The last board is the global board, to make retrieval from JS easier
  u32 boards[10];
  i8 last_move;

  Player player;
  Result result;
} State;

extern void State_init(State *);

// Execute a move, create a new board then assign it to the state
extern void State_move(State *, Move);

// Assign the moved board in the state with the newly created board
extern void State_update(State *, Move, u32);

#endif
