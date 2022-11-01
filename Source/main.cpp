#include "DisplayDriver.h"

DisplayDriver display = DisplayDriver(320, 240, 22, 21, 20, 19, 18, 16, spi0);

// boilerplate display driver code is written, but non functional because I don't have a stable test environment, and I'm waiting for the pi pico pins so that I can make that possible.  Currently the connection between the display and the pi pico is not stable enough to know if my code is the problem.
// Once I get the pins and I solder the pi pico (ensuring proper connections), I will continue my dive into Adafruit's Display Driver code, allowing me to reproduce their driver with limited bloat.  If that is either too difficult, or wouldn't be as optimized if I made it, then I will switch to using Arduino IDE (as opposed to VSCode and a custom compiler)

int main() { // main entrypoint
    display.initDisplay(); // initiate the display (resolution, pins), pin 22 could swap to pin 17, but I didn't use it because it'd be a little confusing having a digital pin being used in the middle of where I used SP pins

    int x = 0;
    while (true) {
        if (x == 320) {
            x = 0;
        } else {
            x++;
        }
        display.clearBuffer();
        display.drawRect(x, 20, 10, 20, White);
    }
    return 0;
}
