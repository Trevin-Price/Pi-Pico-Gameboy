#include "SnakeGame.h"

#define SNAKE_SEGMENT_SIZE 10 // grid is 32x24 (320x240 split into 10x10 sections)
#define GRID_WIDTH (DISPLAY_WIDTH/SNAKE_SEGMENT_SIZE)
#define GRID_HEIGHT (DISPLAY_HEIGHT/SNAKE_SEGMENT_SIZE)
#define BACKGROUND_COLOR Black
#define SNAKE_COLOR Green
#define FOOD_COLOR Red
#define GAME_SPEED 5 // Frames Per Second
#define INPUT_SPEED 70 // Frames Per Second

int SnakeGame::currentDirection;
Vector2 SnakeGame::foodPosition;
std::vector<Vector2> SnakeGame::snakePositions; // starting position

void SnakeGame::startGame() {
    currentDirection = 1;
    foodPosition = Vector2::zero;
    snakePositions = { Vector2::zero, Vector2(1, 0), Vector2(2, 0) };

    DisplayDriver::fillBuffer(BACKGROUND_COLOR);
    DisplayDriver::renderBuffer();
    renderWholeSnake();
    addFood();

    int oldTextLength = 0;
    int frameCount = 0;
    int oldDirection = currentDirection; // keeping the last **rendered** movement
    while (true) {
        sleep_ms(1000/INPUT_SPEED); // input runs at 70 FPS
        frameCount += 1;

        absolute_time_t start = get_absolute_time(); // used for counting fps

        UserInputHandler::updateInput();

        if (UserInputHandler::Buttons[4])
            break; // exit key

        for (int i = 0; i < 4; i++) { // Only uses buttons 0-3 (DPAD)
            if (i+2 != oldDirection && i-2 != oldDirection && UserInputHandler::Buttons[i]) { // make sure they can't do a 180 degree turn in one move
                currentDirection = i;
            }
        }

        if (frameCount < INPUT_SPEED/GAME_SPEED) { // every 14 frames (5 FPS), it renders the game, otherwise it doesn't
            continue; // go back to the start of the loop
        }
        frameCount = 0; // if it got here, the frame count will need to be reset

        Vector2 oldPosition = snakePositions.back();
        Vector2 newPosition = oldPosition;
        oldDirection = currentDirection; // store the last rendered movement direction
        switch (currentDirection)
        {
        case (0): // down button
            newPosition.y += 1;
            break;
        case (1): // right button
            newPosition.x += 1;
            break;
        case (2): // up button
            newPosition.y -= 1;
            break;
        case (3): // left button
            newPosition.x -= 1;
            break;
        }

        if (isSnakeOnSquare(newPosition) || isInvalidPosition(newPosition)) { // check for if the user ran over themself or went out of bounds (I could make it so it just wraps them back to the other side of the game)
            endGame(); // freezes the game, showing "GAME OVER", and waiting until they press the exit button
            break; // go back to the main menu
        } else if (newPosition == oldPosition) {
            // no need to "delete" the old food, as the snake will walk over it, which will cause it to disappear forever
            addFood(); // spawns a new food
        } else
            deleteSnakeTailTip(); // get rid of snake's end, as we're about to move forward one square, and need to keep constant length

        snakePositions.push_back(newPosition); // add the new position at the head of the list
        renderNewSnakeSegment(newPosition); // render the new part of the snake, which is more optimized than re-rendering the whole snake

        absolute_time_t end = get_absolute_time(); // used for counting fps
        std::string text = std::to_string(1000000/absolute_time_diff_us(start, end)).append("FPS"); // fps text

        int newTextLength = static_cast<int>(text.size()); // storing and comparing the FPS text length so we don't draw a rect to cover up old text when it isn't necessary
        if (newTextLength < oldTextLength) {
            DisplayDriver::renderRect(Vector2::zero, FONT_WIDTH*oldTextLength, FONT_HEIGHT, BACKGROUND_COLOR); // covers up the previous text
            DisplayDriver::renderText(Vector2::zero, text, White, BACKGROUND_COLOR); // render fps counter
        } else
            DisplayDriver::renderText(Vector2::zero, text, White, BACKGROUND_COLOR); // render fps counter
        oldTextLength = newTextLength;
    }

    snakePositions = {}; // free memory (a little unnecessary, but just a precaution in case you later run a memory hungry app)
}

void SnakeGame::endGame() {
    DisplayDriver::renderText(Vector2::zero, "GAME OVER", Red, Black);

    while (!UserInputHandler::Buttons[4]) { // wait for the user to press the back button
        UserInputHandler::updateInput();
        sleep_ms(1000/INPUT_SPEED); // we don't need to constantly run this and waste cpu usage, so might as well sleep (no clue if this actually does anything, or if the cpu just runs repeated NOPs)
    }
    // once they press the back button, they will be returned to the main menu
}

void SnakeGame::deleteSnakeTailTip() {
    Vector2 positionToBeErased = snakePositions.front() * SNAKE_SEGMENT_SIZE;
    DisplayDriver::renderRect(positionToBeErased, SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE, BACKGROUND_COLOR);

    std::vector<double> myVec = { 1.0 };
    myVec.erase((std::vector<double>::const_iterator) myVec.begin());

    //snakePositions.erase(snakePositions.begin());
}

bool SnakeGame::isSnakeOnSquare(Vector2 position) { // I should switch to a hash map to optimize it
    for (Vector2 const& segmentPosition: snakePositions) {
        if (segmentPosition.x == position.x && segmentPosition.y == position.y)
            return true;
    }

    return false;
}

bool SnakeGame::isInvalidPosition(Vector2 position) {
    if (position.x >= GRID_WIDTH || position.x < 0 || position.y >= GRID_HEIGHT || position.y < 0)
        return true;
    return false;
}

void SnakeGame::addFood() {
    foodPosition = Vector2(rand() % GRID_WIDTH, rand() % GRID_HEIGHT); // select a random position on the grid
    while (isSnakeOnSquare(foodPosition))
        foodPosition = Vector2(rand() % GRID_WIDTH, rand() % GRID_HEIGHT); // keep regenerating until there is a free square

    DisplayDriver::renderRect(Vector2(foodPosition.x * SNAKE_SEGMENT_SIZE, foodPosition.y * SNAKE_SEGMENT_SIZE), SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE, FOOD_COLOR);
}

void SnakeGame::renderWholeSnake() {
    for (Vector2 const& segmentPosition: snakePositions) {
        DisplayDriver::renderRect(Vector2(segmentPosition.x * SNAKE_SEGMENT_SIZE, segmentPosition.y * SNAKE_SEGMENT_SIZE), SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE, SNAKE_COLOR);
    }
}

void SnakeGame::renderNewSnakeSegment(Vector2 segmentPosition) {
    DisplayDriver::renderRect(Vector2(segmentPosition.x * SNAKE_SEGMENT_SIZE, segmentPosition.y * SNAKE_SEGMENT_SIZE), SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE, SNAKE_COLOR);
}