#include "UserInputHandler.h"

bool Buttons = { 0 };
uint buttonPins[NumberOfButtons]; // make sure you don't allocate too many buttons here, as the size must match the declared size in UserInput.h

uint oppositeButtons[NumberOfButtons]; // 0 = off, 1 = pin 0

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
    buttonPins[0] = 27;
    buttonPins[1] = 28; // initiation at creation is failing for some reason
    oppositeButtons[0] = 2; // button 1
    oppositeButtons[1] = 1; // button 0

    for (int i = 0; i < NumberOfButtons; i++) {
        gpio_init(buttonPins[i]);
        gpio_set_dir(buttonPins[i], GPIO_IN);
        gpio_pull_down(buttonPins[i]);

        Buttons[i] = gpio_get(buttonPins[i]); // initiate button states
    }
}