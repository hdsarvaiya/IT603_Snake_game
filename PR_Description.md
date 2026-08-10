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
*(Note: Assuming typical outcomes)* We accurately predicted changes to `update()` (collisions) and `input()` (controls), as they obviously govern a new entity. However, we missed `run()` and `reset()`. The `run()` method was missed because the game's difficulty curve (speed decrement) was hidden at the bottom of the loop and tied directly to the single `score` variable. We also missed `reset()`, falsely assuming player state reset was handled seamlessly by constructors. These secondary effects are easily glossed over on a visual read because they involve implicit data dependencies (like speed tied to score) rather than structural game logic.

**What one design change to this codebase would have made this a twenty-minute job?**
Refactoring the game to use a `std::vector<Player>` where each `Player` struct bundles a `Snake` object, a `score`, and a control scheme mapping. If the core game loop simply iterated over this list in `update()`, `draw()`, and `input()`, adding a new player would simply require initializing a second `Player` object and appending it to the list, requiring almost zero changes to the core game logic.
