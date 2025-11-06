#ifndef WASM_AI_H
#define WASM_AI_H

#include "../mcts.h"

export void init(void);
export void* state_ptr(void);
export void ai_search(u64, i32);

extern void export_children(void*, u32);

#endif
