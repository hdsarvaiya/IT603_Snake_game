// Lab 4 test suite for main.cpp.
//
// main.cpp has no header, so the game is pulled in textually. Its main() is
// renamed so this file can supply its own.
//
// Build (from the repo root):
//   g++ -std=c++11 --coverage -O0 -g lab4/tests.cpp -o tests
#define main snake_main
#include "../main.cpp"
#undef main

#include <cstdio>

// ------------------------------- tiny harness ----------------------------------
static int g_failures = 0;
static int g_checks = 0;

#define CHECK(cond, what)                                                        \
    do                                                                           \
    {                                                                            \
        ++g_checks;                                                              \
        if (!(cond))                                                             \
        {                                                                        \
            ++g_failures;                                                        \
            std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, what);         \
        }                                                                        \
    } while (0)

#define RUN(test)                          \
    do                                     \
    {                                      \
        std::printf("[ RUN  ] %s\n", #test); \
        test();                            \
    } while (0)

// ------------------------------- helpers ---------------------------------------
static bool contains(const deque<Point> &body, const Point &p)
{
    for (auto &seg : body)
        if (seg == p)
            return true;
    return false;
}

// Food::generate picks uniformly from cells not in its "snakeBody" argument.
// Passing every interior cell except `target` leaves exactly one candidate,
// so the fruit lands on `target` whatever rand() returns.
static void placeFoodAt(Food &food, Point target, int width, int height)
{
    deque<Point> everythingElse;
    for (int i = 1; i < height - 1; ++i)
        for (int j = 1; j < width - 1; ++j)
            if (!(Point(i, j) == target))
                everythingElse.push_back(Point(i, j));
    food.generate(everythingElse, width, height);
}

static const int BOARD_W = 20;
static const int BOARD_H = 20;
static const bool WALLS = true;
static const bool PORTAL = false;

// ------------------------------- rule 1 ----------------------------------------
// Partial: only the Snake-level half (tail grows, score changes) is reachable.
// The respawn half lives in Game::update, which exposes no food or score.
void rule1_eating_fruit_grows_tail_by_one_and_changes_score()
{
    Snake snake(BOARD_W, BOARD_H); // head (10,10), heading RIGHT
    Food food;
    placeFoodAt(food, Point(10, 11), BOARD_W, BOARD_H);
    int score = 0;
    size_t before = snake.getBody().size();

    bool alive = snake.move(food, score, BOARD_W, BOARD_H, WALLS);

    CHECK(alive, "moving onto fruit is not a collision");
    CHECK(snake.getBody().size() == before + 1, "tail grows by exactly one");
    CHECK(score == 1, "score after one fruit (rule said +10; code does +1)");
    CHECK(!food.isActive(), "eaten fruit is deactivated");
}

// ------------------------------- rule 2 ----------------------------------------
void rule2_walls_on_head_into_wall_is_a_collision()
{
    Snake snake(BOARD_W, BOARD_H); // head (10,10), heading RIGHT; wall column is y == BOARD_W-1
    Food noFood;
    int score = 0;

    for (int y = 11; y <= BOARD_W - 2; ++y)
        CHECK(snake.move(noFood, score, BOARD_W, BOARD_H, WALLS), "moves inside the board survive");

    CHECK(!snake.move(noFood, score, BOARD_W, BOARD_H, WALLS), "head into right wall is a collision");
}

// ------------------------------- rule 3 ----------------------------------------
void rule3_head_into_own_body_is_a_collision()
{
    Snake snake(BOARD_W, BOARD_H); // (10,10) (10,9) (10,8), heading RIGHT
    Food food;
    int score = 0;

    // grow to length 5 so the snake is long enough to bite itself
    placeFoodAt(food, Point(10, 11), BOARD_W, BOARD_H);
    snake.move(food, score, BOARD_W, BOARD_H, WALLS);
    placeFoodAt(food, Point(10, 12), BOARD_W, BOARD_H);
    snake.move(food, score, BOARD_W, BOARD_H, WALLS);
    // body: (10,12) (10,11) (10,10) (10,9) (10,8)

    snake.changeDir(DOWN);
    CHECK(snake.move(food, score, BOARD_W, BOARD_H, WALLS), "turn down");
    snake.changeDir(LEFT);
    CHECK(snake.move(food, score, BOARD_W, BOARD_H, WALLS), "turn left");
    snake.changeDir(UP); // next head cell (10,11) is still body

    CHECK(!snake.move(food, score, BOARD_W, BOARD_H, WALLS), "head into own body is a collision");
}

// ------------------------------- rule 4 ----------------------------------------
void rule4_portal_head_past_right_edge_wraps_to_left_edge()
{
    Snake snake(BOARD_W, BOARD_H);
    Food noFood;
    int score = 0;

    for (int y = 11; y <= BOARD_W - 2; ++y)
        snake.move(noFood, score, BOARD_W, BOARD_H, PORTAL);
    // head is now on the last interior column, (10, BOARD_W-2)

    bool alive = snake.move(noFood, score, BOARD_W, BOARD_H, PORTAL);

    CHECK(alive, "crossing the edge in portal mode is not a collision");
    CHECK(snake.getHead() == Point(10, 1), "head reappears on the first interior column, same row");
}

// ------------------------- rule 1, through the seam -----------------------------
// Answers next() from a script, in order. The game asks; the test decides.
class ScriptedRandom : public RandomSource
{
    deque<int> answers;

public:
    ScriptedRandom(std::initializer_list<int> script) : answers(script) {}
    int next(int) override
    {
        int a = answers.front();
        answers.pop_front();
        return a;
    }
};

// 6x5 board: interior rows 1..3, columns 1..4. Snake starts (2,3) (2,2) (2,1)
// heading RIGHT, so the only free cell on row 2 is (2,4).
static const int SMALL_W = 6;
static const int SMALL_H = 5;
static const int FRUIT_AHEAD_OF_HEAD = 4; // free cells: (1,1..4) then (2,4)
static const int FIFTH_FREE_CELL = 4;     // after eating: (1,1..4) then (3,1)
static const int NO_BONUS = 1;            // anything but 0

void rule1_game_eating_fruit_adds_one_point_and_one_segment()
{
    ScriptedRandom rng{FRUIT_AHEAD_OF_HEAD, FIFTH_FREE_CELL, NO_BONUS};
    Game game(SMALL_W, SMALL_H, rng);

    game.update();

    CHECK(!game.isGameOver(), "eating is not a collision");
    CHECK(game.getScore() == 1, "one regular fruit scores 1 (rule said +10; code does +1)");
    CHECK(game.getSnake().getBody().size() == 4, "tail grows from 3 to 4");
}

void rule1_game_respawned_fruit_is_on_a_free_cell()
{
    ScriptedRandom rng{FRUIT_AHEAD_OF_HEAD, FIFTH_FREE_CELL, NO_BONUS};
    Game game(SMALL_W, SMALL_H, rng);

    game.update();

    // Index 4 over *all* cells would be (2,1), the snake's tail. Over free cells it is (3,1).
    CHECK(game.getFood().isActive(), "a new fruit is spawned after eating");
    CHECK(!contains(game.getSnake().getBody(), game.getFood().getPos()), "new fruit is not on the snake");
    CHECK(game.getFood().getPos() == Point(3, 1), "new fruit is the 5th free cell, skipping the snake");
}

// ------------------------------- rule 5 ----------------------------------------
// Not written: see lab4/REPORT.md section 2.

int main()
{
    RUN(rule1_eating_fruit_grows_tail_by_one_and_changes_score);
    RUN(rule2_walls_on_head_into_wall_is_a_collision);
    RUN(rule3_head_into_own_body_is_a_collision);
    RUN(rule4_portal_head_past_right_edge_wraps_to_left_edge);
    RUN(rule1_game_eating_fruit_adds_one_point_and_one_segment);
    RUN(rule1_game_respawned_fruit_is_on_a_free_cell);

    std::printf("\n%d checks, %d failed\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
