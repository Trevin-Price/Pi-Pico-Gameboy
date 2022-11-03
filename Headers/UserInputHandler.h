#pragma once

#define NumberOfButtons 2

class UserInputHandler {
    uint buttonPins[NumberOfButtons];

    public:
        bool Buttons[NumberOfButtons];
        void updateInput();
        void initInput();
};