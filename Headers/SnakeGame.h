#pragma once

#include "UserInputHandler.h"
#include "DisplayDriver.h"
#include <vector>

class SnakeGame {
public:
    static void startGame();
private:
    static int currentDirection; // 0: up, 1: right, 2: down, 3: left
    static std::vector<Vector2> snakePositions;
    static Vector2 foodPosition;

    static void deleteSnakeTailTip();
    static void renderNewSnakeSegment(Vector2 newSegment);
    static void renderWholeSnake();
    static void addFood();
    static bool isSnakeOnSquare(Vector2 position);
    static bool isInvalidPosition(Vector2 position);
    static void endGame();
};