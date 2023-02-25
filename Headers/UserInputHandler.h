#pragma once

#include "pico/stdlib.h"


#define BUTTON_COUNT 6

class UserInputHandler {
    static int buttonPins[BUTTON_COUNT]; // make sure to not over allocate (see NUMBER_OF_BUTTONS definition)

    public:
        static bool Buttons[BUTTON_COUNT]; // declare as off
        static void updateInput();
        static void initInput();
};