#pragma once

#define NumberOfButtons 2

class UserInputHandler {
    static uint buttonPins[NumberOfButtons]; // make sure to not over allocate (see NumberOfButtons definition)
    static uint oppositeButtons[NumberOfButtons]; // button 1, button 0

    public:
        static bool Buttons[NumberOfButtons]; // declare as off
        static void updateInput();
        static void initInput();
};