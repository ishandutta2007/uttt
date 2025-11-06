#include "state.h"
#include "../node.h"
#include "../result_table.h"

static State state;

u32 state_size(void) { return sizeof(State); }
void *state_ptr(void) { return &state; }
void *boards(void) { return state.boards; }
i32 last_move(void) { return state.last_move; }
i32 player(void) { return state.player; }
i32 result(void) { return state.result; }

void init(void) { create_table(); State_init(&state); }
void reset(void) { State_init(&state); }
void move(i32 grid, i32 cell) { State_move(&state, (Move) {grid, cell}); }

static Node children[81 * 2];

void* children_ptr(void) { return children; }

void combine_children(u32 children_size) {
  usize count = children_size / sizeof(Node);
  Node *other = children + count;

  for (usize i = 0; i < count; ++i) {
    children[i].value += other[i].value;
    children[i].samples += other[i].samples;
  }
}

void select_child(u32 children_size) {
  Node *selected = children;

  usize count = children_size / sizeof(Node);
  f32 best_score = (f32)selected->value / (f32)selected->samples;

  for (usize i = 0; i < count; ++i) {
    Node *child = children + i;
    f32 score = (f32)child->value / (f32)child->samples;

    if (score > best_score) {
      best_score = score;
      selected = child;
    }
  }

  State_move(&state, selected->move);
}
