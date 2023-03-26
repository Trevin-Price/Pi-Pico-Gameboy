#include "UserInputHandler.h"

bool UserInputHandler::Buttons[BUTTON_COUNT] = {0};
uint8_t UserInputHandler::buttonPins[BUTTON_COUNT] = {27, 15, 28, 14, 17, 26}; // up, right, down, left, top right, bottom left

void UserInputHandler::updateInput() {
    for (int i = 0; i < BUTTON_COUNT; i++) { // NUMBER_OF_BUTTONS defined in UserInput.h
        Buttons[i] = gpio_get(buttonPins[i]);
    }
}

void UserInputHandler::initInput() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        gpio_init(buttonPins[i]);
        gpio_set_dir(buttonPins[i], GPIO_IN);
        gpio_pull_down(buttonPins[i]);

        Buttons[i] = gpio_get(buttonPins[i]); // initiate button states
    }
}