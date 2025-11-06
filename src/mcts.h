#ifndef MCTS_H
#define MCTS_H

#include "common.h"
#include "node.h"
#include "random.h"
#include "state.h"

typedef struct {
  Node *current_node;

  RNG rng_state;
} MCTS;

extern void search(MCTS*, State*, u64, i32, NodeArena*);

#endif
