#include "UserInputHandler.h"
#include "MovingRectTest.h"

void MovingRectTest::Start(DisplayDriver display, uint speed, uint16_t backgroundColor, uint16_t rectangleColor, uint16_t borderColor) {
    int y = 0; // needs to be able to go negative for when it goes too high
    int x = 0; // needs to be able to go negative for when it goes too far left
    while (true) {
        
        UserInputHandler::updateInput();
            
        if (UserInputHandler::Buttons[0])
            y++;

        if (UserInputHandler::Buttons[1])
            y--;
        
        if (y >= ((int) display.displayHeight)) {
            y = 0;
        } else if (y < 0) {
            y = ((int) display.displayHeight-1);
        }

        if (x >= (int) display.displayWidth) {
            x = 0;
        }
        
        display.fillBuffer(backgroundColor);
        display.drawRect(x, y, 50, 10, rectangleColor, 1, borderColor); // 1 px border
        display.renderBuffer();
        x += speed;
    }
}