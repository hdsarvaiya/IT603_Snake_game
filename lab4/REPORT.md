# Lab 4 — Harshvardhansinh Sarvaiya

| | |
|---|---|
| Repository | https://github.com/hdsarvaiya/IT603_Snake_game (my own snake game) |
| Base tag | `lab4-base` at commit `c3d0a51` |
| Pull request | this PR, `lab4` → `main` |

---

## 1. Five rules — [5]

Written before opening the source. Behaviour, with an observable outcome.

| # | Rule |
|---|---|
| 1 | Eating a regular fruit increases the player's score by 10, increases the snake's tail length by 1, and spawns a new fruit at an unoccupied coordinate. |
| 2 | When Border Wrap is disabled (OFF), moving the snake's head into or beyond any boundary wall causes game over (`gameOver = true`). |
| 3 | Moving the snake's head into any segment of its own tail causes game over (`gameOver = true`). |
| 4 | When Border Wrap is enabled (ON), moving the snake's head past a border boundary wraps it to the opposite boundary without causing game over. |
| 5 | When the game ends with a score strictly higher than the current high score, the game prompts the player to input their name and saves the new score and name to `highscore.txt`. |

The rules are left exactly as I wrote them. Two of them turned out to be wrong about my own game once I opened the source:

- **Rule 1:** a regular fruit scores **+1**, not +10 (`main.cpp:199`, `score++`; the README also says +1). The tests assert +1 and name the discrepancy in their messages.
- **Rule 5:** there is **no name prompt and no `highscore.txt`**. On every game over, `saveScore` (`main.cpp:245-255`) appends the bare score to a top-5 list in `scores.txt`. It does not check whether the score beats the high score.

I remembered the game as it looked to play, not as it was written. That mismatch is what a spec-first test would have exposed.

---

## 2. What you could test, and what stopped you — [10]

No source changes in this part. Every `file:line` below is a line in `lab4-base`. Tests are in `lab4/tests.cpp`, which `#include`s `main.cpp` with `main` renamed. That is a test-file-only trick: the game has no header.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | +10 score, +1 tail, new fruit on a free cell | **Partial, not counted.** `rule1_eating_fruit_grows_tail_by_one_and_changes_score` drives `Snake::move` directly and checks tail +1 and score +1. The respawn half cannot be reached. | `main.cpp:481-482`: the respawn happens inside `Game::update` on `food` (`main.cpp:224`), which is private with no accessor, and so is `score` (`main.cpp:226`). The cell is picked by `rand()` at `main.cpp:90`, re-seeded from the clock by `srand(time(nullptr))` at `main.cpp:260` in `Game`'s constructor, so a test cannot choose or predict it. Also `score++` at `main.cpp:199` is +1, so the rule as written is false. |
| 2 | Walls ON: head into wall → game over | **Yes**, at the `Snake::move` boundary. `rule2_walls_on_head_into_wall_is_a_collision` | None for the decision (`main.cpp:184-185`, `move` returns `false`). The `gameOver` flag the rule names (`main.cpp:227`) is private with no getter. It is set only at `main.cpp:468`, and only when `move` returns false, so the test checks the input to that line, not the flag itself. |
| 3 | Head into own body → game over | **Yes**, at the `Snake::move` boundary. `rule3_head_into_own_body_is_a_collision` | Same as rule 2: `gameOver` (`main.cpp:227`) is unobservable. To grow the snake long enough to bite itself, the test places fruit through the public `Food::generate`, passing every cell but one as "occupied" so `rand()` at `main.cpp:90` has one choice. Each bite plays `Beep` (`main.cpp:202-203`), which is audible but doesn't block. |
| 4 | Wrap ON: head past border → opposite side, no game over | **Yes.** `rule4_portal_head_past_right_edge_wraps_to_left_edge` | None for the right edge (`main.cpp:178-179`). |
| 5 | New high score → name prompt → saved to `highscore.txt` | **No** | The behaviour does not exist: `saveScore` (`main.cpp:245-255`) writes only integers, to a hard-coded `"scores.txt"` (`main.cpp:251`, read at `main.cpp:236`), with no prompt. Even the part that does exist can't be reached. To finish with `score > highScore` (`main.cpp:469`), the snake has to eat, and the only way to steer it is `_kbhit()`/`_getch()` at `main.cpp:398-400` inside `Game::input`, reading the real console. The fruit cell comes from `rand()` (`main.cpp:90`), clock-seeded at `main.cpp:260`. The file path is fixed, so a test would overwrite the player's real leaderboard. The game-over `Beep`s (`main.cpp:474-476`) block for about 0.7 s. |

> **Rules testable without modifying the source: 3 / 5** (rules 2, 3, 4)

All three are tested one level below the rule: at `Snake::move`'s return value, not at `Game`'s `gameOver`. That is honest only because `main.cpp:465-468` sets `gameOver = true` whenever `move` returns false, with nothing in between. Nothing reaches `Game::run` (`main.cpp:512-537`). It is an infinite loop with `Sleep(speed)` at `main.cpp:525`, and every test avoids it.

---

## 3. Coverage, and what it missed — [6]

Measured on the Part B suite, against `lab4-base`:

| | |
|---|---|
| Line coverage | **23.40 %** of 312 lines of `main.cpp` |
| Branch coverage | **18.86 %** of 440 branches executed, **13.86 %** taken at least once |
| Command used | `mkdir build && cd build && g++ -std=c++11 --coverage -O0 -g ../lab4/tests.cpp -o tests && ./tests && gcov -b tests.cpp` (`tests.cpp` `#include`s `main.cpp`, so it is one translation unit; `gcov` writes `main.cpp.gcov`) |

After Part D's two tests: 34.89 % lines, 29.19 % branches executed, 19.00 % taken.

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | 1, the "new fruit at an unoccupied coordinate" part |
| Line that runs | `main.cpp:84` `if (!onSnake)`, the filter that skips snake cells in `Food::generate`, executed 972 times (with `main.cpp:76-80` over 150,000 times). It runs because the test helper `placeFoodAt` calls `Food::generate` to set fruit up for rules 1 and 3. |
| The assertion that is missing | After a fruit is generated against the real snake body: `CHECK(!contains(snake.getBody(), food.getPos()), "new fruit is not on the snake")`. No Part B test checks where the fruit landed relative to the snake. The filter only ever ran against a fake "body" the helper built, and was used as a tool, not checked as behaviour. It is added at `lab4/tests.cpp:180` in Part D. |

A mutation run found a second gap of the same kind. Deleting `body.pop_back()` at `main.cpp:207` makes the snake grow on every move, not just when eating. All 18 Part B checks still pass. Every non-eating move runs that line, but no test asserts that the length stays at 3. The rule-1 test only checks growth on the one move that eats.

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | **Rule 1:** eating a fruit adds one point, adds one segment, and respawns the fruit on a free cell, observed through `Game::update` |
| Commit 1 (seam) | `Add RandomSource seam to Food and Game`. Only `main.cpp` changes. |
| Commit 2 (test) | `Add Lab 4 tests and report`. Only `lab4/tests.cpp` and `lab4/REPORT.md`. |
| Seam kind | **object**: a virtual `RandomSource::next(int bound)` |
| Enabling point | `Game`'s constructor, `Game(int w, int h, RandomSource &rng = stdRandom)`. Production `main()` still writes `Game game(80, 25)` and gets `StdRandom`, which calls `rand() % bound` exactly as before. The test passes a `ScriptedRandom`. `Food::generate` and `BonusFood::activateBonus` take the same parameter with the same default. |
| What production code gave up | (1) **Encapsulation.** `Game` now has four public getters (`getScore`, `isGameOver`, `getSnake`, `getFood`). Its score, state, snake and fruit are part of its public surface, and any future caller can depend on them. (2) **A global.** `static StdRandom stdRandom` is file-scope state, and the seam hides `rand()` rather than removing it: `srand(time(nullptr))` still runs in the constructor. (3) **Value semantics.** `Game` holds a `RandomSource&`, so it can't be copy-assigned and must not outlive the source it was given. Nothing assigns a `Game` today. (4) One virtual call per random draw, at most three a tick, which is negligible next to `Sleep`. |

**No behaviour change in commit 1:** each `rand()` call is replaced one-for-one by `rng.next(...)`, in the same order, with the same modulus. I checked this directly. I built `lab4-base` and commit 1 side by side, seeded both with `srand(42)`, ran 3000 `update()` ticks in portal mode, and hashed every `draw()` frame. The hashes are identical (`1119502163`), bonus-food spawns included. The Part B tests also pass unchanged on commit 1.

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | **stub** (`ScriptedRandom`, `lab4/tests.cpp:137-149`) |
| The method under test | `Game::update()` (the eat → score → respawn path) |

`Game` asks the collaborator a question ("which of these N free cells?", "spawn a bonus this tick?") and uses the `int` it gets back. It never tells it to do anything. Because `next()` returns a value, its effect shows up in `Game`'s own state (score, snake length, fruit position), so the test asserts on that state and never on whether or how `next()` was called. That makes it a stub, not a spy or mock. It is not a fake either: it has no working logic of its own, only canned answers.

---

## 6. Two smells in your own tests — [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | **Assertion roulette** | `lab4/tests.cpp:89-90` | The `CHECK` is inside a loop with the same message on every step, so a failure can't say which move died. Put the column in the message, or assert once on the final head position. |
| 2 | **Mystery guest / resource optimism** | `lab4/tests.cpp:162` and `:174` | `Game`'s constructor silently reads `scores.txt` from whatever directory the test runs in (`main.cpp:236`) and changes the console mode. If a test ever reaches game over, `saveScore` overwrites the player's real leaderboard. Inject the leaderboard stream or path the same way the random source is injected, or run the suite in a temp directory it creates. |

Also found (not fixed): **eager test** at `lab4/tests.cpp:66-80`. It checks collision, growth, score and fruit deactivation after one move, so a failure mixes four behaviours. The fix is to keep the shared arrangement and split it into one test per check. The Part D tests (`:159-182`) are already split this way.
