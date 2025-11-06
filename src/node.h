#ifndef NODE_H
#define NODE_H

#include "common.h"
#include "state.h"

typedef struct {
  Move move;

  u32 children_count;
  u32 children;
  u32 parent;

  i32 value;
  u32 samples;
} Node;

typedef struct {
  Node *nodes;
  u32 capacity;
  u32 size;
} NodeArena;

extern Node *NodeArena_head(NodeArena);
extern void NodeArena_init(NodeArena*);
extern bool NodeArena_push(NodeArena*, Move, Node*);

#endif
