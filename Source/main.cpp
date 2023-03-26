#include "DisplayDriver.h"
#include "UserInputHandler.h"
#include "MainMenu.h"
#include "DisplayTests.h"

/*

    * Work on either gluing in the headers (defeats the purpose of using headers), or soldering directly to the display/pico, which makes it extremely difficult to reuse.  Gluing may still leave the poor connections while making it more difficult to fix.

    * DisplayDriver initilization should be cleaned up and better documented.
    * Alternative 3D Projection method could be added, simplifying by not using matrices, more for demonstration to presentation viewers.
    * Change out size x and y for a single Vector2
    * Look into cleaning up Vector operators (might not be possible, and it'd only save lines anyways, not perf)
    * While I could rewrite the Vector library to be a single Vector class with adjustable size, I'm not going to bother due to conflicts with std::vector, and the lack of necessity in this usecase
    * Write SnakeGame3D once 3d gfx is done
    * 
    * Fix the 3D Test loading bug
    * Fix SnakeGame2D Not loading
    * These seem like the same issue to me, and something to do with using std::vector<Vector2>

*/

int main() { // main entrypoint
    DisplayDriver::initDisplay(); // initiate the display (resolution, pins), pin 22 could swap to pin 17, but I didn't use it because it'd be a little confusing having a digital pin being used in the middle of where I used SP pins

    UserInputHandler::initInput(); // initiate the device

    MainMenu::startMainLoop();

    return 0;
}
