#include "MainMenu.h"

int MainMenu::ScrollAmount = 0;
#define OPTION_COUNT 6

std::string MainMenu::Buttons[OPTION_COUNT] = { "Snake", "3D Test", "Line Test", "Color Tester", "Optimized Test", "Extras" };
void (*MainMenu::ButtonFunctions[OPTION_COUNT])(void) = { &DisplayTests::renderDevStats, &DisplayTests::test3D, &DisplayTests::startRectTest, &DisplayTests::startFullScreenTest, &DisplayTests::directRenderTest, &DisplayTests::renderDevStats };

#define BUTTON_X_PADDING 30 // padding on the left and right side of each button
#define BUTTON_Y_OUTER_PADDING 30 // padding on the top and bottom of the list of buttons
#define BUTTON_Y_INNER_PADDING 15 // padding above and below buttons in a list
#define BUTTON_WIDTH (DISPLAY_WIDTH-BUTTON_X_PADDING*2) // width is based on padding amounts
#define BUTTON_HEIGHT 50 // height is adjustable because there is vertical scrolling
#define BUTTON_X BUTTON_X_PADDING // initial X Position (this is redundant, but makes the code easier to understand)
#define BUTTON_Y BUTTON_Y_OUTER_PADDING // initial Y Position
#define BUTTON_BACKGROUND_COLOR White
#define BUTTON_BORDER_COLOR Black
#define BUTTON_HIGHLIGHTED_BORDER_COLOR Blue
#define BUTTON_BORDER_THICKNESS 5

#define MENU_BACKGROUND_COLOR LightGrey

void MainMenu::renderButtons() {
    DisplayDriver::fillBuffer(MENU_BACKGROUND_COLOR); // reset background (coloring over all old buttons)

    int index = 0;
    for (std::string const& buttonText : Buttons) {
        int offset = index - ScrollAmount + 1; // -1 because we want the selected element to be in the middle, so if we're at the top of the list, there should be no elements above the middle
        if (offset == 0 || offset == 2) { // either 1 above, 1 below, or the current selected item
            DisplayDriver::drawOutlinedRect(Vector2(BUTTON_X, BUTTON_Y + offset*(BUTTON_HEIGHT+BUTTON_Y_INNER_PADDING)), Vector2(BUTTON_WIDTH, BUTTON_HEIGHT), White, 5, Black);
            DisplayDriver::drawCenteredText(Vector2(BUTTON_X + BUTTON_WIDTH/2, (BUTTON_Y + offset*(BUTTON_HEIGHT+BUTTON_Y_INNER_PADDING)) + BUTTON_HEIGHT/2), buttonText, Black);
        } else if (offset == 1) {
            DisplayDriver::drawOutlinedRect(Vector2(BUTTON_X, BUTTON_Y + offset*(BUTTON_HEIGHT+BUTTON_Y_INNER_PADDING)), Vector2(BUTTON_WIDTH, BUTTON_HEIGHT), White, 5, Blue); 
            // the middle button is the highlighted one
            DisplayDriver::ALTdrawCenteredText(Vector2(BUTTON_X + BUTTON_WIDTH/2, (BUTTON_Y + offset*(BUTTON_HEIGHT+BUTTON_Y_INNER_PADDING)) + BUTTON_HEIGHT/2), buttonText, Blue);
        }

        index++;
    }
    
    DisplayDriver::renderBuffer();
}

void MainMenu::startMainLoop() {
    renderButtons(); // render

    bool buttonReleased = true; // I dont want it to instantly scroll if you hold it, you should press it each time you want it to scroll down

    while (true) {
        UserInputHandler::updateInput();
        if (UserInputHandler::Buttons[0]) { // down
            if (buttonReleased && ScrollAmount < (OPTION_COUNT-1)) {
                buttonReleased = false;
                ScrollAmount += 1;
                renderButtons();
            }
        } else if (UserInputHandler::Buttons[2]) { // up
            if (buttonReleased && ScrollAmount > 0) {
                buttonReleased = false;
                ScrollAmount -= 1;
                renderButtons();
            }
        } else {
            buttonReleased = true;
        }

        if (UserInputHandler::Buttons[5]) { // select option
            ButtonFunctions[ScrollAmount](); // run the button's code (should yield until the user exits the app)
            renderButtons(); // reload main menu
        }

        sleep_ms(FRAME_TIME_MS); // no need to update faster than the display's refresh rate, that'd just waste cpu usage, and therefor power
    }
}