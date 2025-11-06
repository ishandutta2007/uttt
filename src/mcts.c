// Monte Carlo tree search algorithm
// See: <https://en.wikipedia.org/wiki/Monte_Carlo_tree_search>
#include "mcts.h"

static void mcts_init(MCTS *searcher, u64 seed, NodeArena *arena) {
  NodeArena_init(arena);

  searcher->current_node = NodeArena_head(*arena);
  searcher->rng_state = seed;
  NodeArena_push(arena, (Move) {-1, -1}, searcher->current_node);
}

// Upper Confidence bounds applied to Trees (UCT)
static f32 uct(Node *node, f32 t) {
  const f32 c = 1.4142135623730951f; // sqrt2
  f32 n = node->samples;
  f32 w = node->value;

  f32 uct = (w / n) + c * sqrtf(t / n);

  return uct;
}

static Node *select_child(Node *node) {
  Node *best_child = node + node->children;
  if (!best_child->samples) return best_child;

  f32 t = fastln(node->samples);
  f32 best_uct = uct(best_child, t);

  for (usize i = 1; i < node->children_count; ++i) {
    Node *child = node + node->children + i;
    if (!child->samples) return child;

    f32 child_uct = uct(child, t);
    if (child_uct > best_uct) {
      best_uct = child_uct;
      best_child = child;
    }
  }

  return best_child;
}

// Iterate over a board and add all possible moves to the tree
static bool expand_board(Node *node, u32 grid_idx, u32 board, NodeArena *arena) {
  u32 mask = (~(board | (board >> 1))) & 0x15555u;

  while (mask) {
    u32 cell = mask & -mask;
    mask ^= cell;

    u32 cell_idx = ctz(cell) >> 1;

    node->children_count++;
    if (!NodeArena_push(arena, (Move) {grid_idx, cell_idx}, node)) {
      node->children_count = 0;
      return false;
    }
  }

  return true;
}

static bool expand_node(Node *node, State *state, NodeArena *arena) {
  node->children = NodeArena_head(*arena) - node;
  i8 grid_idx = state->last_move;

  // Also iterate over all local boards when the player can move anywhere
  if (grid_idx == -1) {
    u32 global_board = state->boards[9];
    u32 global_mask = (~(global_board | (global_board >> 1))) & 0x15555u;

    while (global_mask) {
      u32 grid = global_mask & -global_mask;
      global_mask ^= grid;

      grid_idx = ctz(grid) >> 1;
      if (!expand_board(node, grid_idx, state->boards[grid_idx], arena)) {
        return false;
      }
    }

    return true;
  } else {
    return expand_board(node, grid_idx, state->boards[grid_idx], arena);
  }
}

// Picking a random empty cell
static u32 random_move_mask(u32 board, RNG *rng) {
  // Create a bitmask representing all empty cells
  u32 mask = (~(board | (board >> 1))) & 0x15555u;

  // Remove a random amount of bits from the left
  u32 idx = RNG_u32(rng, popcnt(mask));
  for (usize i = 0; i < idx; ++i) mask &= (mask - 1);

  // Select the leftmost bit after removal
  return mask & -mask;
}

static Result playout(State *state, RNG *rng, i32 *steps) {
  while (!state->result) {
    i8 grid = state->last_move;
    
    // Select a random local board when the player can move anywhere
    if (grid == -1) {
      u32 mask = random_move_mask(state->boards[9], rng);

      grid = ctz(mask) >> 1;
    }

    u32 board = state->boards[grid];
    u32 mask = random_move_mask(board, rng);

    board |= (mask << state->player);
    State_update(state, (Move) {grid, ctz(mask) >> 1}, board);

    --*steps;
  }

  return state->result;
}

static void mcts_search(MCTS *searcher, State state, i32 *steps, NodeArena *arena) {
  // Selection
  while (searcher->current_node->children_count && !state.result) {
    Node *child = select_child(searcher->current_node);

    searcher->current_node = child;
    State_move(&state, child->move);
    *steps = *steps - 2;
  }

  // Expansion
  if (searcher->current_node->samples && !state.result) {
    if (expand_node(searcher->current_node, &state, arena)) {
      Node *child = searcher->current_node + searcher->current_node->children;
      searcher->current_node = child;
      State_move(&state, child->move);
      *steps = *steps - 2;
    }
  }

  // Simulation
  Player current_player = 1 - state.player;
  Result result = playout(&state, &searcher->rng_state, steps);

  // Back-propagation
  for (;;) {
    searcher->current_node->samples++;

    if (result != TIE) {
      if (current_player == result - 1) searcher->current_node->value++;
      else searcher->current_node->value--;
    }

    current_player = 1 - current_player;
    u32 parent = searcher->current_node->parent;
    if (!searcher->current_node->parent) break;
    searcher->current_node -= parent;
  }
}

void search(MCTS *mcts, State *state, u64 seed, i32 steps, NodeArena *arena) {
  mcts_init(mcts, seed, arena);

  // Dynamic time allocation based on tree-traversal and simulation steps
  while (steps > 0) {
    mcts_search(mcts, *state, &steps, arena);
  }
}

// NOTE: Currently the AI is not very random. It may be because the bias
// produced by choosing the first child with the best UCT value is propagated
// over a large number of simulations. This must be addressed to reduce the
// predictability of the AI.
