#include "node.h"

Node *NodeArena_head(NodeArena arena) {
  return arena.nodes + arena.size;
}

void NodeArena_init(NodeArena *arena) {
  arena->size = 0;
}

bool NodeArena_push(NodeArena *arena, Move move, Node *parent) {
  if (arena->size < arena->capacity) {
    u32 relative_parent = (arena->nodes + arena->size) - parent;
    arena->nodes[arena->size++] = (Node) {
      .move = move,
      .children_count = 0,
      .children = 0,
      .parent = relative_parent,
      .samples = 0,
      .value = 0,
    };
    return true;
  }

  return false;
}
