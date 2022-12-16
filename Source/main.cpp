#include "DisplayDriver.h"
#include "MovingRectTest.h"
#include "MovingRectTest.cpp" // linker issues so I'm manually including the definitions
#include "UserInputHandler.h"
#include "UserInputHandler.cpp" // linker issues so I'm manually including the definitions
#include "GameLoader.h"
#include "GameLoader.cpp"

DisplayDriver display = DisplayDriver(320, 240, 22, 21, 20, 19, 18, 16, spi0);
// I'm tempted to make the display static, though technically someone may later want to wire up two displays at once, so I won't.

DisplayDriver GameLoader::Display = display;

int main() { // main entrypoint
    display.initDisplay(); // initiate the display (resolution, pins), pin 22 could swap to pin 17, but I didn't use it because it'd be a little confusing having a digital pin being used in the middle of where I used SP pins
    UserInputHandler::initInput(); // initiate the device

    //GameLoader::RenderMainMenu();

    //display.testDisplay();

    MovingRectTest::Start(display, 5, Black, Green, White); // yields indefinitely

    return 0;
}
