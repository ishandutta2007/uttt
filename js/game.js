import { registerAI } from './ai.js'
import { state } from './state.js'

/**
 * @param {string} msg
 * @returns {never}
 */
function assertNonNull(msg = "Non null assertion failed") {
  throw new Error(msg)
}

/** @type {HTMLDivElement} */
const gameRoot = document.querySelector('#container') ?? assertNonNull()

const mainGrid = document.querySelector('#main-grid') ?? assertNonNull()
/** @type {HTMLDivElement[]} */
let mainCells = new Array(9)
/** @type {HTMLDivElement[]} */
let subGrids = new Array(9)
/** @type {HTMLDivElement[]} */
let subCells = new Array(9 * 9)

for (let i = 0; i < 9; ++i) {
  const mainCell = document.createElement('div')
  mainCell.classList.add('main-cell')

  const subGrid = document.createElement('div')
  subGrid.classList.add('sub-grid')

  for (let j = 0; j < 9; ++j) {
    const subCell = document.createElement('div')
    subCell.classList.add('sub-cell')
    subCell.addEventListener('click', () => {
      if (!playerTurn[state.currentPlayer]) return

      state.move(i, j)
      updateHTML()
    })

    subGrid.appendChild(subCell)
    subCells[9 * i + j] = subCell
  }

  mainCell.appendChild(subGrid)
  mainGrid.appendChild(mainCell)

  subGrids[i] = subGrid
  mainCells[i] = mainCell
}

const settingsButton = gameRoot.querySelector('#settings') ?? assertNonNull()
const restartButton = gameRoot.querySelector('#restart') ?? assertNonNull()
const endMessage = gameRoot.querySelector('#end-message') ?? assertNonNull()

let playerTurn = [true, false]
let AIStrength = 8

const { invokeAI, stopAI } = await registerAI(updateHTML)

function updateHTML() {
  for (const elem of mainCells) elem.className = 'main-cell'
  for (const elem of subCells) elem.className = 'sub-cell'
  for (const elem of subGrids) elem.className = 'sub-grid'

  endMessage.className = ['', 'x', 'o', 'tie'][state.result]

  let globalBoard = state.boards[9]
  for (let i = 0; i < 9; ++i) {
    const cell = globalBoard & 3
    globalBoard >>= 2

    switch (cell) {
      case 0: {
        if (state.result) break
        if (state.lastMove != -1 && state.lastMove != i) break
        subGrids[i].classList.add('active')
      } break
      case 1: mainCells[i].classList.add('x'); continue
      case 2: mainCells[i].classList.add('o'); continue
      default: break
    }

    let board = state.boards[i]
    for (let j = 0; j < 9; ++j) {
      const tile = ['', 'x', 'o', 'error'][board & 3]
      board >>= 2

      if (tile) {
        if (tile == 'error') throw new Error(`Local board (${i}, ${j}) tied`)
        subCells[9 * i + j].classList.add(tile)
      }
    }
  }

  if (!(state.result || playerTurn[state.currentPlayer])) {
    invokeAI(AIStrength)
  }
}

async function reset() {
  await stopAI()

  state.reset()
  updateHTML()
}

restartButton.addEventListener('click', reset)

/** @type {HTMLDivElement} */
const settingsRoot = document.querySelector('#settings-container') ?? assertNonNull()
/** @type {HTMLInputElement} */
const settingsPlayerX = settingsRoot.querySelector('#settings-player-x') ?? assertNonNull()
/** @type {HTMLInputElement} */
const settingsPlayerO = settingsRoot.querySelector('#settings-player-o') ?? assertNonNull()
/** @type {HTMLInputElement} */
const settingsAI = settingsRoot.querySelector('#settings-ai') ?? assertNonNull()

const settingsAIDisplay = settingsRoot.querySelector('#settings-ai-display') ?? assertNonNull()
const settingsSubmit = settingsRoot.querySelector('#settings-submit') ?? assertNonNull()
const settingsReturn = settingsRoot.querySelector('#settings-return') ?? assertNonNull()

settingsButton.addEventListener('click', () => {
  settingsPlayerX.value = playerTurn[0] ? 'human' : 'ai'
  settingsPlayerO.value = playerTurn[1] ? 'human' : 'ai'

  settingsAI.value = AIStrength.toString()
  settingsAIDisplay.textContent = AIStrength.toString()

  settingsRoot.hidden = false
})

settingsSubmit.addEventListener('click', () => {
  playerTurn = [
    settingsPlayerX.value == 'human',
    settingsPlayerO.value == 'human'
  ]

  AIStrength = parseInt(settingsAI.value)
  settingsRoot.hidden = true

  reset()
})

settingsAI.addEventListener('input', (e) => {
  settingsAIDisplay.textContent = (/** @type {HTMLInputElement} */ (e.target ?? assertNonNull())).value
})

settingsReturn.addEventListener('click', () => settingsRoot.hidden = true)

updateHTML()
gameRoot.hidden = false
