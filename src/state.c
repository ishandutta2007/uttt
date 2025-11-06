#include "state.h"

void State_init(State *state) {
  for (usize i = 0; i < len(state->boards); ++i) state->boards[i] = 0;
  state->last_move = -1;

  state->player = PLAYER_X;
  state->result = PLAYING;
}

void State_move(State *state, Move move) {
  if (state->last_move != -1 && state->last_move != move.grid) return;

  u32 board = state->boards[move.grid];
  u32 new_board = board | (1 << (move.cell << 1 | state->player));
  if (new_board == state->boards[move.grid]) return;

  State_update(state, move, new_board);
}

void State_update(State *state, Move move, u32 board) {
  state->boards[move.grid] = board;

  // Update global board
  Result sub_result = RESULT_TABLE[board];
  if (sub_result) {
    state->boards[9] |= sub_result << (move.grid << 1);
    state->result = RESULT_TABLE[state->boards[9]];
  }

  state->player = 1 - state->player;
  state->last_move = (state->boards[9] & (3 << (move.cell << 1))) ? -1 : move.cell;
}
