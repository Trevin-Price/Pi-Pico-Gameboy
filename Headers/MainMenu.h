#pragma once

#include "UserInputHandler.h"
#include "DisplayDriver.h"
#include "DisplayTests.h"
#include "SnakeGame.h"
#include <list>

class MainMenu {
    static int ScrollAmount;
    static std::string Buttons[];
    static void (*ButtonFunctions[])(void);

    public:
        static void startMainLoop();
    private:
        static void renderButtons();
};