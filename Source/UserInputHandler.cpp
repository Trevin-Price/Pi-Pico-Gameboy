#include "UserInputHandler.h"

bool UserInputHandler::Buttons[NumberOfButtons] = {0};
uint UserInputHandler::oppositeButtons[NumberOfButtons] = {3, 4, 1, 2, 0, 0}; // up and down, left and right, but not top right and bottom left
uint UserInputHandler::buttonPins[NumberOfButtons] = {27, 15, 28, 14, 17, 26}; // up, right, down, left, top right, bottom left

void UserInputHandler::updateInput() {
    bool newVal = false;
    for (int i = 0; i < NumberOfButtons; i++) { // NumberOfButtons defined in UserInput.h
        newVal = gpio_get(buttonPins[i]);
        if (oppositeButtons[i] && Buttons[oppositeButtons[i]-1]) // check that oppositeButtons[i] != 0 (0 means no opposite button)
            Buttons[i] = false;
        else
            Buttons[i] = newVal;
    }
}

void UserInputHandler::initInput() {
    for (int i = 0; i < NumberOfButtons; i++) {
        gpio_init(buttonPins[i]);
        gpio_set_dir(buttonPins[i], GPIO_IN);
        gpio_pull_down(buttonPins[i]);

        Buttons[i] = gpio_get(buttonPins[i]); // initiate button states
    }
}