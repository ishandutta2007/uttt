/** @type {WebAssembly.Memory} */
let memory
const decoder = new TextDecoder()

const env = {
  /**
   * @param {number} ptr
   * @param {number} size
   */
  log_error(ptr, size) {
    const memArr = new Uint8Array(memory.buffer, ptr, size)
    console.error(decoder.decode(memArr))
  },
  /**
   * @param {number} x
   */
  dump(x) { console.log(x) },
}

/**
 * @typedef StateExport
 * @prop {WebAssembly.Memory} memory
 * @prop {() => number} boards
 * @prop {() => number} state_ptr
 * @prop {() => number} state_size
 * @prop {() => number} last_move
 * @prop {() => number} player
 * @prop {() => number} result
 * @prop {(grid: number, cell: number) => void} move
 * @prop {() => number} children_ptr
 * @prop {(size: number) => void} combine_children
 * @prop {(size: number) => void} select_child
 * @prop {() => void} reset
 * @prop {() => void} init
 */

const wasm = await WebAssembly.instantiateStreaming(fetch('./wasm/state.wasm'), { env })
const exports = /** @type {StateExport} */ (wasm.instance.exports)

memory = exports.memory

const boards = new Uint32Array(memory.buffer, exports.boards(), 10)
const stateBuffer = new Uint8Array(memory.buffer, exports.state_ptr(), exports.state_size())

const state = {
  boards,
  get lastMove() { return exports.last_move() },
  get currentPlayer() { return exports.player() },
  get result() { return exports.result() },

  /**
   * @param {number} grid
   * @param {number} cell
   */
  move(grid, cell) { exports.move(grid, cell) },
  /**
   * @param {ArrayBuffer} buffer
   */
  setChildren(buffer) {
    const arr = new Uint8Array(buffer)
    const slice = new Uint8Array(memory.buffer, exports.children_ptr(), arr.length)
    slice.set(arr)
  },
  /**
   * @param {ArrayBuffer} buffer
   */
  addChildren(buffer) {
    const arr = new Uint8Array(buffer)
    const slice = new Uint8Array(memory.buffer, exports.children_ptr() + arr.length, arr.length)
    slice.set(arr)

    exports.combine_children(arr.length)
  },
  /**
   * @param {number} length
   */
  bestMove(length) { exports.select_child(length) },
  reset() { exports.reset() }
}

exports.init()

export { state, stateBuffer }
