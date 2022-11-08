#include "UserInputHandler.h"

bool UserInputHandler::Buttons[NumberOfButtons] = {0};
uint UserInputHandler::oppositeButtons[NumberOfButtons] = {2, 1};
uint UserInputHandler::buttonPins[NumberOfButtons] = {27, 28};

void UserInputHandler::updateInput() {
    bool newVal = false;
    for (int i = 0; i < NumberOfButtons; i++) { // NumberOfButtons defined in UserInput.h
        newVal = gpio_get(buttonPins[i]);
        if (oppositeButtons[i] && !Buttons[oppositeButtons[i]-1]) // check that oppositeButtons[i] != 0
            Buttons[i] = newVal;
        else
            Buttons[i] = false;
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