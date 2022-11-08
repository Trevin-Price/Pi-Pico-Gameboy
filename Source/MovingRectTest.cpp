#include "UserInputHandler.h"
#include "MovingRectTest.h"

void MovingRectTest::Start(UserInputHandler inputHandler, DisplayDriver display, uint speed, uint16_t BackgroundColor, uint16_t RectangleColor) {
    int y = 0; // needs to be able to go negative for when it goes too high
    int x = 0; // needs to be able to go negative for when it goes too far left
    while (true) {
        
        inputHandler.updateInput();
            
        if (inputHandler.Buttons[0])
            y++;

        if (inputHandler.Buttons[1])
            y--;
        
        if (y >= ((int) display.displayHeight)) {
            y = 0;
        } else if (y < 0) {
            y = ((int) display.displayHeight-1);
        }

        if (x >= (int) display.displayWidth) {
            x = 0;
        }
        
        display.setEntireBuffer(BackgroundColor);
        display.drawRect(x, y, 50, 10, RectangleColor);
        display.renderBuffer();
        x += speed;
    }
}