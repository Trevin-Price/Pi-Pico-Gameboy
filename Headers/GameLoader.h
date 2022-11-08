#pragma once

#include "UserInputHandler.h"
#include "DisplayDriver.h"

class GameLoader {
    static uint Selection;

    public:
        static void RenderMainMenu();
        static DisplayDriver Display;
};