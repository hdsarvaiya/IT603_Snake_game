
# 🐍 Snake Game (Windows Console Edition)

## 📘 Overview

The **Snake Game** is a modern recreation of the classic arcade experience, designed to run smoothly within the Windows Command Prompt.
It offers both nostalgic charm and enhanced gameplay with interactive visuals, colorful designs, sound effects, and multiple play modes.

Players control a growing snake that moves around the screen, eating food items to increase its length and score. The game ends if the snake collides with itself or, depending on the selected mode, with the walls.

---

## 🎮 Features

* **Two Gameplay Modes:**

  * **Classic Mode (Wall Mode):** Hitting a wall ends the game.
  * **Portal Mode:** The snake reappears from the opposite side of the screen for a continuous experience.
* **Colorful Console Graphics:** Smooth character-based visuals with color-coded snake, food, and bonus items.
* **Bonus Food:** Occasional special food appears, offering extra points for quick players.
* **Leaderboard System:** Automatically records and displays top scores.
* **Pause and Resume:** Players can pause the game at any time.
* **Sound Effects:** Interactive tones for movements, eating food, and game over.
* **Custom Themes:** Choose from multiple color styles to personalize your gameplay.

---

## 🕹️ Controls

| Key       | Action                    |
| --------- | ------------------------- |
| **W / ↑** | Move Up                   |
| **S / ↓** | Move Down                 |
| **A / ←** | Move Left                 |
| **D / →** | Move Right                |
| **P**     | Pause / Resume            |
| **R**     | Restart (after Game Over) |
| **Q**     | Quit (after Game Over)    |

---

## 🌈 Game Flow

1. **Welcome Screen:** The game opens with an animated title and a friendly introduction.
2. **Theme Selection:** Players can choose their preferred visual theme — Classic, Neon, or Dark.
3. **Mode Selection:** Choose between *Classic (Wall Mode)* or *Portal Mode*.
4. **Gameplay:** Control the snake to collect food (`@`) and bonus food (`*`). Each item increases your score and snake length.
5. **Game Over:** When the snake collides (depending on mode), the game ends and your score is saved in the leaderboard.
6. **Replay Option:** Players can restart or exit gracefully.

---
## 🧠 Class Structure

Below is the detailed breakdown of all the classes implemented in the Snake Game project:

---

### 🧩 1. **Point**
**Purpose:**  
Represents a coordinate (**x**, **y**) position on the game grid.

**Key Members:**
- `int x, y` — stores the coordinate values.  
- `operator==` — compares two `Point` objects for equality.

---

### 🐍 2. **Snake**
**Purpose:**  
Manages the snake’s body structure, direction, and movement logic.

**Key Members:**
- `deque<Point> body` — stores all body segments of the snake.  
- `Direction dir` — tracks the current direction of movement.  
- `move()` — updates the snake’s position, checks for collisions, and handles food consumption.  
- `changeDir()` — changes the movement direction while preventing direct reversal.

---

### 🍎 3. **Food**
**Purpose:**  
Handles the creation and positioning of regular food items on the game board.

**Key Members:**
- `Point position` — holds the food’s location.  
- `bool active` — indicates whether the food is currently visible.  
- `generate()` — randomly generates new food on the board.  
- `deactivate()` — marks food as consumed.

---

### 💫 4. **BonusFood** *(inherits from `Food`)*
**Purpose:**  
Represents a special type of food that offers extra points and appears for a limited time.

**Key Members:**
- `int timer` — countdown timer before the bonus food disappears.  
- `activateBonus()` — spawns a bonus food item on the board.  
- `isExpired()` — checks if the bonus food’s timer has run out.  

---

### 🎮 5. **Game**
**Purpose:**  
Serves as the central class managing the entire gameplay — rendering, input, scoring, and state management.

**Key Members:**
- `Snake snake` — main player object.  
- `Food food` — standard food item.  
- `BonusFood bonusFood` — special timed food item.  
- `int score, highScore` — tracks current and best scores.  
- `bool gameOver, paused, wallMode` — stores gameplay states.  
- **Main Methods:**  
  - `draw()` — renders the game board.  
  - `input()` — processes player input.  
  - `update()` — updates the game state per frame.  
  - `reset()` — restarts the game.  
  - `run()` — controls the main game loop.  

**Additional Functionality:**  
Handles leaderboard storage, sound effects, color themes, and game mode (Wall / Portal).

---
---


## 🏆 Scoring

* Regular Food (`@`): **+1 Point**
* Bonus Food (`*`): **+5 Points**
* The game speed slightly increases as your score rises, testing your reflexes and precision.

---

## 💾 Leaderboard

Your highest scores are saved automatically and displayed in the top-five leaderboard.
The leaderboard file (`scores.txt`) is created automatically in the same directory as the game.

---

## ⚙️ System Requirements

* **Operating System:** Windows 10 or later
* **Environment:** Windows Command Prompt (UTF-8 enabled)
* **Compiler:** Any C++17 compatible compiler (e.g., MinGW or Visual Studio)

---

## 🧩 How to Run

1. Compile the program using a C++ compiler:

   ```
   g++ snake_game.cpp -o snake_game
   ```
2. Run the executable:

   ```
   snake_game.exe
   ```
3. Follow the on-screen instructions to begin playing.

---

## 💡 Developer Notes

This project was created as a fun and interactive console application blending classic gameplay with modern enhancements.
It demonstrates the use of:

* Console-based animations and color formatting
* Simple sound design
* Object-oriented programming for modular design
* File-based leaderboard management

---



---

Would you like me to make this version **more creative** (e.g., for GitHub, with emojis and formatted sections) or keep it **strictly formal** (for academic submission or portfolio)?

