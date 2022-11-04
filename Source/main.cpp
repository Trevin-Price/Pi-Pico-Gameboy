#include "DisplayDriver.h"
#include "UserInputHandler.h"
#include "UserInputHandler.cpp" // not entirely sure why I need to link the source file for userinput but not for display

DisplayDriver display = DisplayDriver(320, 240, 22, 21, 20, 19, 18, 16, spi0);
UserInputHandler inputHandler;

int main() { // main entrypoint
    display.initDisplay(); // initiate the display (resolution, pins), pin 22 could swap to pin 17, but I didn't use it because it'd be a little confusing having a digital pin being used in the middle of where I used SP pins
    inputHandler.initInput(); // initiate the device

    int y = 0; // needs to be able to go negative for when it goes too high
    int x = 0; // needs to be able to go negative for when it goes too far left
    uint speed = 5;
    while (true) {
        inputHandler.updateInput();
            
        if (inputHandler.Buttons[0])
            y++;

        if (inputHandler.Buttons[1])
            y--;
        
        if (y > ((int) display.displayHeight-11)) {
            y = 0;
        } else if (y < 0) {
            y = (int) display.displayHeight -11;
        }

        if (x >= (int) display.displayWidth) {
            x = 0;
        }
        
        display.clearBuffer();
        display.drawRect(x, y, 50, 10, Blue);
        display.renderBuffer();
        x += speed;
    }
    return 0;
}
