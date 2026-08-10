# LLM Log

## 1. Setup

| | |
|---|---|
| Model(s) | Gemini 3.1 Pro (High) |
| Tool / harness | Antigravity |
| IDE / editor | Antigravity IDE |
| Did you paste this assignment document into it? | Yes — Parts A through E |
| Repository | `hdsarvaiya/IT603_Snake_game` |
| Main file involved | `main.cpp` |

---

## 2. Session

### Prompt 1 — Implement Two-Player Multiplayer

You are modifying an existing C++ Snake game repository.

The objective is to add local two-player multiplayer while keeping the existing game behavior and structure intact.

The required changes were:

- Add a second snake.
- Keep Player 1's existing controls unchanged.
- Use `W/A/S/D` controls for Player 2.
- Make both snakes move during the same game loop.
- Make both players share the same regular food.
- Maintain an independent score for each player.
- Detect wall collisions for both players.
- Detect self-collisions for both players.
- Detect collisions between the two snakes.
- Display both scores.
- Announce which player lost when the game ends.
- Keep the implementation within the assignment scope.
- Avoid unrelated refactoring or feature additions.

The assignment document containing Parts A through E was provided as context, and the repository was available to the agent for inspection.

### Relevant reply

The assistant inspected the existing code and implemented the second-player functionality in `main.cpp`.

The implementation added a second `Snake` object and a second score. It also modified the constructor so the two snakes could start at different positions.

The following areas were updated to support multiplayer:

- `Snake::Snake`
- `Game::Game`
- `Game::draw()`
- `Game::input()`
- `Game::update()`
- `Game::reset()`
- `Game::run()`

The implementation also added collision checks between the two snakes and changed food generation so that food would not be placed on either snake.

---

### Prompt 2 — Review the Implementation and Git Diff

After the multiplayer implementation was generated, I asked the assistant to review the repository before submission.

The main purpose of this prompt was to distinguish the actual assignment changes from unrelated Git changes.

The assistant was asked to:

- Compare the feature branch against `main`.
- Identify all changed files.
- Check whether the changes were actually related to multiplayer.
- Identify whitespace or line-ending noise.
- Check runtime-generated changes such as `scores.txt`.
- Check the accidental `a.exe` change.
- Determine the exact functions modified by the multiplayer implementation.
- Verify that no unnecessary features or refactoring had been introduced.

### Relevant reply

The review identified that the multiplayer implementation itself was concentrated in `main.cpp`.

It identified seven affected functions:

1. `Snake::Snake`
2. `Game::Game`
3. `Game::draw()`
4. `Game::input()`
5. `Game::update()`
6. `Game::reset()`
7. `Game::run()`

The review also identified unrelated repository changes, including:

- An accidental `a.exe` change.
- Runtime changes to `scores.txt`.
- Line-ending differences in `main.cpp`.

These changes were treated as repository noise rather than part of the multiplayer feature.

---

### Prompt 3 — Prepare Assignment Documentation

I then asked the assistant to prepare the required `PR_Description.md` and `LLM-LOG.md` documentation.

The documentation needed to cover:

- The measurement table.
- The functions affected by the change.
- Why functions outside the direct snake movement logic were reached.
- The original single-snake assumption.
- The predicted functions versus the functions actually changed.
- A design change that would make adding additional players easier.
- The use of the LLM during the implementation.

### Relevant reply

The assistant prepared the documentation based on the actual implementation and repository diff.

The analysis identified that the original game was designed around one fixed `Snake` object and one score variable. Because there was no general player abstraction, adding a second player required changes across input, rendering, updating, resetting, initialization, and game-speed logic.

The analysis also identified that some functions originally predicted to change were not actually modified, while two functions were missed in the original prediction:

- `Snake::Snake`
- `Game::run()`

The suggested architectural improvement was to represent players using a reusable player abstraction, such as a `std::vector<Player>`, where each player contains a snake, score, and controls.

---

### Prompt 4 — Build and Verification

After the implementation and documentation were prepared, I asked the assistant to help verify the project locally.

The project was compiled using:

```powershell
g++ main.cpp -o snake_game.exe