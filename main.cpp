// ============================== SNAKE GAME (Windows CMD) ==============================
// Console-based Snake Game with Wall / Portal modes, color, bonus, sound, pause.
// ======================================================================================

#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <conio.h>   // _kbhit(), _getch()
#include <windows.h> // console functions, Beep, Sleep
#include <algorithm> // max
#include <fstream>
#include <chrono>
#include <string>
#include <climits>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

using namespace std;

// ----------------------------- Console color escape codes -----------------------------
#define RESET "\033[0m"
static string GREEN = "\033[32m";
static string RED = "\033[31m";
#define WHITE "\033[37m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"

// ----------------------------- Single-width border glyphs -----------------------------
static const char *TL = u8"┌";
static const char *TR = u8"┐";
static const char *BL = u8"└";
static const char *BR = u8"┘";
static const char *H = u8"─";
static const char *V = u8"│";

// ------------------------------------ Point type -------------------------------------
class Point
{
public:
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point &other) const { return x == other.x && y == other.y; }
};

// --------------------------------- Movement direction --------------------------------
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// -------------------------------------- Food -----------------------------------------
class Food
{
private:
    Point position;
    bool active;

public:
    Food() : active(false) {}
    void generate(const deque<Point> &snakeBody, int width, int height)
    {
        vector<Point> valid;
        for (int i = 1; i < height - 1; ++i)
        {
            for (int j = 1; j < width - 1; ++j)
            {
                Point candidate(i, j);
                bool onSnake = false;
                for (auto &seg : snakeBody)
                {
                    if (seg == candidate)
                    {
                        onSnake = true;
                        break;
                    }
                }
                if (!onSnake)
                    valid.push_back(candidate);
            }
        }
        if (!valid.empty())
        {
            position = valid[rand() % valid.size()];
            active = true;
        }
    }
    Point getPos() const { return position; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }
};

// ------------------------------------ Bonus Food -----------------------------------
class BonusFood : public Food
{
    int timer;

public:
    BonusFood() : timer(0) {}
    void activateBonus(const deque<Point> &snakeBody, int w, int h)
    {
        generate(snakeBody, w, h);
        timer = 50;
    }
    bool isExpired() { return --timer <= 0; }
    void deactivate()
    {
        timer = 0;
        Food::deactivate();
    }
};

// -------------------------------------- Snake ----------------------------------------
class Snake
{
private:
    deque<Point> body;
    Direction dir;

public:
    Snake(int width, int height, int startOffset = 0)
    {
        dir = RIGHT;
        int startX = height / 2 + startOffset;
        int startY = width / 2;
        body.clear();
        body.push_back(Point(startX, startY));
        body.push_back(Point(startX, startY - 1));
        body.push_back(Point(startX, startY - 2));
    }

    void changeDir(Direction newDir)
    {
        if ((dir == UP && newDir == DOWN) ||
            (dir == DOWN && newDir == UP) ||
            (dir == LEFT && newDir == RIGHT) ||
            (dir == RIGHT && newDir == LEFT))
            return;
        dir = newDir;
    }

    // NOTE: wallMode == true -> hitting border = collision (death)
    //       wallMode == false -> wrap-around (portal)
    bool move(Food &food, int &score, int width, int height, bool wallMode)
    {
        Point head = body.front();
        switch (dir)
        {
        case UP:
            head.x--;
            break;
        case DOWN:
            head.x++;
            break;
        case LEFT:
            head.y--;
            break;
        case RIGHT:
            head.y++;
            break;
        }

        // If walls are disabled -> wrap-around (portal)
        if (!wallMode)
        {
            if (head.x <= 0)
                head.x = height - 2;
            else if (head.x >= height - 1)
                head.x = 1;
            if (head.y <= 0)
                head.y = width - 2;
            else if (head.y >= width - 1)
                head.y = 1;
        }
        else
        {
            // With wallMode: hitting border is death
            if (head.x <= 0 || head.x >= height - 1 || head.y <= 0 || head.y >= width - 1)
                return false;
        }

        // Collision with self?
        for (auto &seg : body)
            if (head == seg)
                return false;

        // Advance
        body.push_front(head);

        // Eat food?
        if (food.isActive() && head == food.getPos())
        {
            score++;
            food.deactivate();
            // small eat sound
            Beep(900, 60);
            Beep(1100, 40);
        }
        else
        {
            body.pop_back();
        }

        return true;
    }

    const deque<Point> &getBody() const { return body; }
    Point getHead() const { return body.front(); }
    Direction getDir() const { return dir; }
};

// -------------------------------------- Game -----------------------------------------
class Game
{
private:
    int width, height;
    Snake snake;
    Snake snake2;
    Food food;
    BonusFood bonusFood;
    int score, score2, highScore;
    bool gameOver;
    string loserMsg;
    bool paused;
    bool wallMode; // true => walls active (death on border), false => portal/wrap-around

    vector<int> leaderboard;

    void loadLeaderboard()
    {
        leaderboard.clear();
        ifstream fin("scores.txt");
        int s;
        while (fin >> s)
            leaderboard.push_back(s);
        fin.close();
        if (!leaderboard.empty())
            highScore = *max_element(leaderboard.begin(), leaderboard.end());
    }

    void saveScore(int s)
    {
        leaderboard.push_back(s);
        sort(leaderboard.rbegin(), leaderboard.rend());
        if (leaderboard.size() > 5)
            leaderboard.resize(5);
        ofstream fout("scores.txt");
        for (auto x : leaderboard)
            fout << x << endl;
        fout.close();
    }

public:
    Game(int w = 60, int h = 22) : width(w), height(h), snake(w, h, -2), snake2(w, h, 2)
    {
        srand((unsigned)time(nullptr));
        score = 0;
        score2 = 0;
        highScore = 0;
        gameOver = false;
        loserMsg = "";
        paused = false;
        wallMode = true; // default, can be changed via setWallMode

        loadLeaderboard();
        food.generate(snake.getBody(), width, height);

        // enable ANSI in Windows console for colors
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode))
        {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }

    void setWallMode(bool enabled) { wallMode = enabled; }

    void draw()
    {
        COORD topLeft = {0, 0};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), topLeft);

        string buffer;
        buffer.reserve(width * height * 2);

        // ----- Top border -----
        buffer += WHITE;
        buffer += TL;
        for (int j = 0; j < width - 2; ++j)
        {
            if (wallMode)
                buffer += H;
            else
                buffer += ((j % 2 == 0) ? "·" : " ");
        }
        buffer += TR;
        buffer += "\n";

        // ----- Interior -----
        for (int i = 1; i < height - 1; ++i)
        {
            buffer += WHITE;
            // Left border
            if (wallMode)
                buffer += V;
            else
                buffer += ((i % 2 == 0) ? "·" : " ");

            for (int j = 1; j < width - 1; ++j)
            {
                Point cur(i, j);

                // Head
                if (snake.getHead() == cur)
                {
                    buffer += string(GREEN) + "O" + WHITE;
                }
                else if (snake2.getHead() == cur)
                {
                    buffer += string(GREEN) + "O" + WHITE;
                }
                else
                {
                    bool printed = false;

                    // Body
                    for (auto &seg : snake.getBody())
                    {
                        if (seg == cur && !(seg == snake.getHead()))
                        {
                            buffer += string(GREEN) + "o" + WHITE;
                            printed = true;
                            break;
                        }
                    }
                    if (!printed)
                    {
                        for (auto &seg : snake2.getBody())
                        {
                            if (seg == cur && !(seg == snake2.getHead()))
                            {
                                buffer += string(GREEN) + "o" + WHITE;
                                printed = true;
                                break;
                            }
                        }
                    }
                    if (printed)
                        continue;

                    // Food and bonus
                    if (bonusFood.isActive() && cur == bonusFood.getPos())
                    {
                        buffer += string(CYAN) + "*" + WHITE;
                    }
                    else if (food.isActive() && cur == food.getPos())
                    {
                        buffer += string(RED) + "@" + WHITE;
                    }
                    else
                    {
                        buffer += " ";
                    }
                }
            }

            // Right border
            buffer += WHITE;
            if (wallMode)
                buffer += V;
            else
                buffer += ((i % 2 == 0) ? "·" : " ");

            buffer += "\n";
        }

        // ----- Bottom border -----
        buffer += WHITE;
        buffer += BL;
        for (int j = 0; j < width - 2; ++j)
        {
            if (wallMode)
                buffer += H;
            else
                buffer += ((j % 2 == 0) ? "·" : " ");
        }
        buffer += BR;
        buffer += "\n";

        // ----- Scoreboard -----
        string scoreLine = " P1 Score: " + to_string(score) + " | P2 Score: " + to_string(score2) +
                           " | High Score: " + to_string(highScore) +
                           " | Mode: " + (wallMode ? "WALLS" : "PORTAL");
        int pad = max(0, (width - (int)scoreLine.size()) / 2);
        buffer += string(YELLOW) + string(pad, ' ') + scoreLine + "\n" + RESET;

        // ----- Game Over -----
        if (gameOver)
        {
            string msg = " GAME OVER! " + loserMsg + " Press R to restart or Q to quit ";
            int pad2 = max(0, (width - (int)msg.size()) / 2);
            buffer += string(RED) + string(pad2, ' ') + msg + "\n" + RESET;
        }

        cout << buffer;
    }

    void input()
    {
        if (_kbhit())
        {
            int key = _getch();

            if (key == 'p' || key == 'P')
            {
                paused = !paused;
                return;
            }

            if (!paused)
            {
                if (key == 224 || key == 0)
                {
                    key = _getch();
                    switch (key)
                    {
                    case 72:
                        snake.changeDir(UP);
                        break;
                    case 80:
                        snake.changeDir(DOWN);
                        break;
                    case 75:
                        snake.changeDir(LEFT);
                        break;
                    case 77:
                        snake.changeDir(RIGHT);
                        break;
                    }
                }
                else
                {
                    switch (tolower(key))
                    {
                    case 'w':
                        snake2.changeDir(UP);
                        break;
                    case 's':
                        snake2.changeDir(DOWN);
                        break;
                    case 'a':
                        snake2.changeDir(LEFT);
                        break;
                    case 'd':
                        snake2.changeDir(RIGHT);
                        break;
                    case 'r':
                        if (gameOver)
                            reset();
                        break;
                    case 'q':
                        if (gameOver)
                            exit(0);
                        break;
                    }
                }
            }
        }
    }

    void update()
    {
        if (gameOver)
            return;

        // Attempt to move; pass wallMode to snake move
        bool p1Alive = snake.move(food, score, width, height, wallMode);
        bool p2Alive = snake2.move(food, score2, width, height, wallMode);

        for (auto &seg : snake2.getBody()) {
            if (snake.getHead() == seg) p1Alive = false;
        }
        for (auto &seg : snake.getBody()) {
            if (snake2.getHead() == seg) p2Alive = false;
        }
        if (snake.getHead() == snake2.getHead()) {
            p1Alive = false;
            p2Alive = false;
        }

        if (!p1Alive || !p2Alive)
        {
            // collision occurred
            gameOver = true;
            if (!p1Alive && !p2Alive) loserMsg = "BOTH PLAYERS LOST!";
            else if (!p1Alive) loserMsg = "PLAYER 1 LOST!";
            else loserMsg = "PLAYER 2 LOST!";

            if (score > highScore) highScore = score;
            if (score2 > highScore) highScore = score2;
            saveScore(max(score, score2));

            // Game over tune
            Beep(400, 180);
            Beep(300, 240);
            Beep(200, 320);
            return;
        }

        deque<Point> combinedBody = snake.getBody();
        for (auto& p : snake2.getBody()) {
            combinedBody.push_back(p);
        }

        // If food eaten, spawn a new one (food.deactivate called from move)
        if (!food.isActive())
            food.generate(combinedBody, width, height);

        // Rarely spawn bonus food
        if (rand() % 120 == 0 && !bonusFood.isActive())
            bonusFood.activateBonus(combinedBody, width, height);

        // If snake ate bonus food
        if (bonusFood.isActive())
        {
            if (snake.getHead() == bonusFood.getPos())
            {
                score += 5;
                bonusFood.deactivate();
                Beep(1000, 120);
                Beep(1300, 100);
            }
            else if (snake2.getHead() == bonusFood.getPos())
            {
                score2 += 5;
                bonusFood.deactivate();
                Beep(1000, 120);
                Beep(1300, 100);
            }
        }

        // Expire bonus
        if (bonusFood.isActive() && bonusFood.isExpired())
            bonusFood.deactivate();
    }

    void reset()
    {
        snake = Snake(width, height, -2);
        snake2 = Snake(width, height, 2);
        score = 0;
        score2 = 0;
        gameOver = false;
        loserMsg = "";
        paused = false;
        
        deque<Point> combinedBody = snake.getBody();
        for (auto& p : snake2.getBody()) {
            combinedBody.push_back(p);
        }
        food.generate(combinedBody, width, height);
        bonusFood.deactivate();
    }

    void run()
    {
        // small title/info before starting
        showInitialInstructions();

        while (true)
        {
            if (!paused)
            {
                draw();
                input();
                update();
                int speed = max(40, 150 - max(score, score2) * 5);
                Sleep(speed);
            }
            else
            {
                // paused state
                COORD pos = {0, (SHORT)0};
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
                cout << CYAN << "\n\n   GAME PAUSED. Press 'P' to resume..." << RESET;
                input();
                Sleep(200);
            }
        }
    }

    void showInitialInstructions()
    {
        system("cls");
        cout << GREEN << "\nSNAKE (WASD / Arrow keys) — Mode: " << (wallMode ? "WALLS" : "PORTAL") << RESET << "\n\n";
        cout << YELLOW << "Controls:\n  W / ↑ - Up\n  S / ↓ - Down\n  A / ← - Left\n  D / → - Right\n  P - Pause\n  R - Restart (after Game Over)\n  Q - Quit (after Game Over)\n\n"
             << RESET;
        cout << "Eat '@' to grow. '*' is bonus (+5).\n\nPress any key to start...";
        _getch();
        system("cls");
    }
};

// ----------------------------- UI helpers --------------------------------
void showTitleScreen() {
    system("cls");
    cout << GREEN << R"(

  ╔═══════════════════════════════════════════════════════════════════════════════════════ ╗
  ║                                                                                        ║
  ║    ███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗    ██████╗  █████╗ ███╗   ███╗███████╗   ║
  ║    ██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝   ██╔════╝ ██╔══██╗████╗ ████║██╔════╝   ║
  ║    ███████╗██╔██╗ ██║███████║█████╔╝ █████╗     ██║  ███╗███████║██╔████╔██║█████╗     ║
  ║    ╚════██║██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝     ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝     ║
  ║    ███████║██║ ╚████║██║  ██║██║  ██╗███████╗   ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗   ║
  ║    ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝    ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝   ║
  ║                                                                                        ║
  ╚══════════════════════════════════════════════════════════════════════════════════════  ╝
)" << RESET ;

    cout << YELLOW
         << "==============================================================\n"
         << "                 WELCOME TO THE CLASSIC SNAKE GAME\n"
         << "==============================================================\n\n"
         << "Controls:\n"
         << "   W / ↑    Move Up\n"
         << "   S / ↓    Move Down\n"
         << "   A / ←    Move Left\n"
         << "   D / →    Move Right\n"
         << "   P        Pause\n"
         << "   R        Restart (after Game Over)\n"
         << "   Q        Quit (after Game Over)\n\n"
         << CYAN << "Press any key to start the game..." << RESET;
    _getch();
    system("cls");
}


void selectTheme()
{
    system("cls");
    cout << "Choose a theme:\n1. Classic\n2. Neon\n3. Dark\n> ";
    int choice = 1;
    if (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        choice = 1;
    }
    switch (choice)
    {
    case 2:
        GREEN = "\033[96m";
        RED = "\033[95m";
        break;
    case 3:
        GREEN = "\033[92m";
        RED = "\033[91m";
        break;
    default:
        GREEN = "\033[32m";
        RED = "\033[31m";
        break;
    }
}

void selectGameMode(Game &game)
{
    system("cls");
    cout << "Choose game mode:\n1. Classic (WALLS = death)\n2. Portal  (wrap-around)\n> ";
    int choice = 1;
    if (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        choice = 1;
    }
    game.setWallMode(choice == 1);
}

// ---------------------------------------- main ---------------------------------------
int main()
{
    // Allow UTF-8 sequences (box drawing) in Windows console
    SetConsoleOutputCP(CP_UTF8);

    showTitleScreen();
    selectTheme();

    // Create game with desired size
    Game game(80, 25);

    // Let player choose mode
    selectGameMode(game);

    // Run
    game.run();
    return 0;
}
