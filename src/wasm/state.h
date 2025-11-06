#ifndef WASM_STATE_H
#define WASM_STATE_H

#include "../state.h"

export u32 state_size(void);
export void *state_ptr(void);
export void *boards(void);
export i32 last_move(void);
export i32 player(void);
export i32 result(void);

export void init(void);
export void move(i32, i32);
export void reset(void);

export void* children_ptr(void);
export void combine_children(u32);
export void select_child(u32);

#endif
