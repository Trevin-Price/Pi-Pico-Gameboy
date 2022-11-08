#include "DisplayDriver.h"
#include "MovingRectTest.h"
#include "UserInputHandler.h"
#include "UserInputHandler.cpp" // not entirely sure why I need to link the source file for userinput but not for display
#include "MovingRectTest.cpp"

DisplayDriver display = DisplayDriver(320, 240, 22, 21, 20, 19, 18, 16, spi0);
UserInputHandler inputHandler;

int main() { // main entrypoint
    display.initDisplay(); // initiate the display (resolution, pins), pin 22 could swap to pin 17, but I didn't use it because it'd be a little confusing having a digital pin being used in the middle of where I used SP pins
    inputHandler.initInput(); // initiate the device
    MovingRectTest::Start(inputHandler, display, 5, Black, Green); // yields indefinitely
    return 0;
}
