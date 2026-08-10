## Measurement Table

| Metric | Value |
| --- | --- |
| Functions changed (the headline number) | 7 |
| Names of those functions | `Snake::Snake`, `Game::Game`, `draw()`, `input()`, `update()`, `reset()`, `run()` |
| Files changed | 4 (`main.cpp`, `scores.txt`, `LLM-LOG.md`, `a.exe`) |
| Lines added / deleted | 99 insertions(+) / 21 deletions(-) |
| % of codebase touched | ~14.5% (103 lines changed in `main.cpp` ÷ 708 total lines) |
| Hours your group spent | ~1.5 hours |
| LLM model + tool | Gemini 3.1 Pro (High) via VS Code Agent |
| LLM prompts to working code | 1 |
| What code you gave the LLM | Whole file (`main.cpp`) |
| Pasted this assignment doc into the LLM? | Yes |

---

## Analysis

**Which functions did you have to change that are not about snakes at all? Why did a second snake reach them?**
The functions not strictly about snakes are `draw()`, `input()`, `run()`, and `reset()`. `draw()` was reached because the rendering logic and scoreboard needed to display both snakes and their respective scores. `input()` was reached because it needed to capture the second player's distinct WASD keystrokes. `run()` was modified because the game's loop speed decrement is tied to the maximum score of the active players. `reset()` had to be updated to ensure the second snake and its score were reinitialized upon restarting.

**Was there a single place where "there is exactly one snake" was written down — or was that assumption spread across the code?**
The assumption was scattered throughout the entire `Game` class. It manifested as a single `Snake snake;` instance, a single `score` variable, hardcoded arrow-key mappings in `input()`, hardcoded rendering logic in `draw()`, and singular collision checks in `update()`. There was no generalized array or entity system managing the game's actors.

**In Lecture 3 your group predicted which functions would change. Compare your prediction to what actually happened. What did you miss, and why was it not visible from reading?**
We predicted changes to `Snake::move()`, `Food::generate()`, `Game::input()`, `Game::update()`, `Game::draw()`, `Game::reset()`, `Game::Game`, and `showInitialInstructions()`.

What actually happened was slightly different. We correctly predicted changes to `input()`, `update()`, `draw()`, `reset()`, and `Game::Game`. However, we **over-predicted** some functions: `Snake::move()`, `Food::generate()`, and `showInitialInstructions()` did not need to be changed at all. For `Food::generate()`, instead of modifying the function, we just combined the snake bodies in `update()` and passed the merged array. 

We also **under-predicted** two functions: `Snake::Snake` (which needed an offset to prevent both snakes spawning on each other) and `Game::run()`. We completely missed `run()` because the game loop's speed decrement was implicitly tied to the single player's `score` variable at the very bottom of the loop. This secondary dependency was not visible from reading because it looked like isolated loop management rather than core game logic.

**What one design change to this codebase would have made this a twenty-minute job?**
Refactoring the game to use a `std::vector<Player>` where each `Player` struct bundles a `Snake` object, a `score`, and a control scheme mapping. If the core game loop simply iterated over this list in `update()`, `draw()`, and `input()`, adding a new player would simply require initializing a second `Player` object and appending it to the list, requiring almost zero changes to the core game logic.
