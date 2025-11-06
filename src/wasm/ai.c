#include "ai.h"

#define NODES_CAP (1 << 24)
static Node nodes[NODES_CAP];

static State state;
static MCTS searcher;
static Move result;

static NodeArena arena = {
  .nodes = nodes,
  .capacity = NODES_CAP,
};

void init(void) { create_table(); }
void *state_ptr(void) { return &state; }
void *result_ptr(void) { return &result; }

void ai_search(u64 seed, i32 steps) {
  search(&searcher, &state, seed, steps * (1 << 20), &arena);

  Node *node = searcher.current_node;
  export_children(node + node->children, node->children_count * sizeof(Node));
}
