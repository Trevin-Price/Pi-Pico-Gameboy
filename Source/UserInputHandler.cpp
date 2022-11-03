#include "UserInputHandler.h"

bool Buttons = { 0 };
uint buttonPins[NumberOfButtons]; // make sure you don't allocate too many buttons here, as the size must match the declared size in UserInput.h

void UserInputHandler::updateInput() {
    for (int i = 0; i < NumberOfButtons; i++) { // NumberOfButtons defined in UserInput.h
        Buttons[i] = gpio_get(buttonPins[i]);
        if (Buttons[i])
            gpio_put(26, true);
    }
}

void UserInputHandler::initInput() {
    buttonPins[0] = 28;
    buttonPins[1] = 27; // initiation at creation is failing for some reason

    gpio_init(26);
    gpio_set_dir(26, GPIO_OUT);
    for (int i = 0; i < NumberOfButtons; i++) {
        gpio_init(buttonPins[i]);
        gpio_set_dir(buttonPins[i], GPIO_IN);
        gpio_pull_down(buttonPins[i]);

        Buttons[i] = gpio_get(buttonPins[i]); // initiate button states
    }
}