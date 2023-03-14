#include "DisplayTests.h"
#include <vector>

void DisplayTests::startFullScreenTest() {
    int red = 31;
    int green = 0;
    int blue = 0;
    int mode = 0;
    int speed = 1;

    while (true) {
        if (mode == 0) {
            if (green + speed > 63 && green != 63)
                green = 63;
            else if (green == 63)
                mode = 1;
            else {
                green += speed;
            }
        } else if (mode == 1) {
            if (red - speed < 0 && red != 0)
                red = 0;
            else if (red == 0)
                mode = 2;
            else {
                red -= speed;
            }
        } else if (mode == 2) {
            if (blue + speed > 31 && blue != 31)
                blue = 31;
            else if (blue == 31)
                mode = 3;
            else {
                blue += speed;
            }
        } else if (mode == 3) {
            if (green - speed < 0 && green != 0)
                green = 0;
            else if (green == 0)
                mode = 4;
            else {
                green -= speed;
            }
        } else if (mode == 4) {
            if (red + speed > 31 && red != 31)
                red = 31;
            else if (red == 31)
                mode = 5;
            else {
                red += speed;
            }
        } else if (mode == 5) {
            if (blue - speed < 0 &&blue != 0)
                blue = 0;
            else if (blue == 0)
                mode = 0;
            else {
                blue -= speed;
            }
        }

        DisplayDriver::fillBuffer(FIX_COLOR((uint16_t) ((red << 11) | (green << 5) | blue)));
        DisplayDriver::renderBuffer();
        UserInputHandler::updateInput();

        if (UserInputHandler::Buttons[4])
            break; // exit game
    }
}

void DisplayTests::startRectTest() {
    int speed = 1;
    uint16_t backgroundColor = White;
    uint16_t rectangleColor = Yellow;
    uint16_t borderColor = Black;
    bool Rainbow = true;

    int y = 0; // needs to be able to go negative for when it goes too high
    int x = 0; // needs to be able to go negative for when it goes too far left
    int red = 31;
    int green = 0;
    int blue = 0;
    int rainbowSpeed = 1;
    int mode = 0;
    int oldTextLength = 0;

    DisplayDriver::fillBuffer(backgroundColor);
    DisplayDriver::renderBuffer();

    while (true) {
        absolute_time_t start = get_absolute_time();

        //DisplayDriver::renderRect(x, y, 50, 10, backgroundColor);

        if (Rainbow) {
            if (mode == 0) {
                if (green + rainbowSpeed > 63 && green != 63)
                    green = 63;
                else if (green == 63)
                    mode = 1;
                else {
                    green += rainbowSpeed;
                }
            } else if (mode == 1) {
                if (red - rainbowSpeed < 0 && red != 0)
                    red = 0;
                else if (red == 0)
                    mode = 2;
                else {
                    red -= rainbowSpeed;
                }
            } else if (mode == 2) {
                if (blue + rainbowSpeed > 31 && blue != 31)
                    blue = 31;
                else if (blue == 31)
                    mode = 3;
                else {
                    blue += rainbowSpeed;
                }
            } else if (mode == 3) {
                if (green - rainbowSpeed < 0 && green != 0)
                    green = 0;
                else if (green == 0)
                    mode = 4;
                else {
                    green -= rainbowSpeed;
                }
            } else if (mode == 4) {
                if (red + rainbowSpeed > 31 && red != 31)
                    red = 31;
                else if (red == 31)
                    mode = 5;
                else {
                    red += rainbowSpeed;
                }
            } else if (mode == 5) {
                if (blue - rainbowSpeed < 0 &&blue != 0)
                    blue = 0;
                else if (blue == 0)
                    mode = 0;
                else {
                    blue -= rainbowSpeed;
                }
            }

            rectangleColor = FIX_COLOR((uint16_t) ((red << 11) | (green << 5) | blue));
        }

        UserInputHandler::updateInput();

        if (UserInputHandler::Buttons[0])
            y++;

        if (UserInputHandler::Buttons[2])
            y--;

        x += speed;

        if (UserInputHandler::Buttons[4])
            break; // if the user presses the back button, the loop ends

        if (y >= ((int) DISPLAY_HEIGHT)) {
            y = 0;
        } else if (y < 0) {
            y = ((int) DISPLAY_HEIGHT-1);
        }

        if (x >= (int) DISPLAY_WIDTH) {
            x = 0;
        }

        DisplayDriver::renderOutlinedRect(Vector2(x, y), 50, 10, rectangleColor, 1, borderColor); // 1 px border
        //DisplayDriver::renderRect(x, y, 50, 10, rectangleColor);

        int64_t deltaT = absolute_time_diff_us(start, get_absolute_time());
        std::string text = std::to_string(1000000/deltaT).append("FPS");

        int newLength = static_cast<int>(text.size());
        if (newLength == oldTextLength) // renderText writes a background, so if the old is the same width as the new, it'll overwrite the old text, otherwise it needs to be done separately
            DisplayDriver::renderText(Vector2::zero, text, Black, backgroundColor);
        else {
            DisplayDriver::renderRect(Vector2::zero, FONT_WIDTH*oldTextLength, FONT_HEIGHT, backgroundColor); // covers up the previous text
            DisplayDriver::renderText(Vector2::zero, text, Black, backgroundColor);
        }
        oldTextLength = newLength;

        int64_t extraTime = FRAME_TIME_MS-absolute_time_diff_us(start, get_absolute_time())/1000; // calculate time until next frame is needed
        if (extraTime > 0) // if we're not late on the frame
            sleep_ms(extraTime); // sleep until next frame
    }
}

void DisplayTests::directRenderTest() { // 27047~27048 microseconds per frame originally
    uint16_t backgroundColor = White;
    uint16_t textColor = Black;
    int oldTextLength = 0; // used to fill in the space where the old text was

    DisplayDriver::fillBuffer(backgroundColor);
    DisplayDriver::renderBuffer();
    absolute_time_t start = get_absolute_time();

    while (true) {
        UserInputHandler::updateInput();
        if (UserInputHandler::Buttons[4])
            break; // exit key

        int64_t deltaT = absolute_time_diff_us(start, get_absolute_time());

        std::string text = std::to_string(1000000/deltaT).append("FPS");
        int64_t extraTime = FRAME_TIME_MS-deltaT/1000; // calculate time until next frame is needed
        if (extraTime > 0) // if we're not late on the frame
            sleep_ms(extraTime); // sleep until next frame

        start = get_absolute_time();
        int newLength = static_cast<int>(text.size());
        if (newLength == oldTextLength) // renderText writes a background, so if the old is the same width as the new, it'll overwrite the old text, otherwise it needs to be done separately
            DisplayDriver::renderText(Vector2::zero, text, textColor, backgroundColor);
        else {
            DisplayDriver::renderRect(Vector2::zero, FONT_WIDTH*oldTextLength, FONT_HEIGHT, backgroundColor); // covers up the previous text
            DisplayDriver::renderText(Vector2::zero, text, textColor, backgroundColor);
        }
        oldTextLength = newLength;
    }
}

Vector2 textPos = Vector2::zero;

void drawTextNewLine(std::string text) {
    DisplayDriver::drawText(textPos, text, Black);
    textPos.y += FONT_HEIGHT;
}

void DisplayTests::renderDevStats() {
    DisplayDriver::renderRect(Vector2(0, 0), 5, 5, Red);
    DisplayDriver::fillBuffer(Black);
    drawTextNewLine(((std::string) "Baud Rate: ").append(std::to_string((float)DisplayDriver::actualBaudRate / 1000000)).append(" MHz"));
    drawTextNewLine(((std::string) "Buffer Time: ").append(std::to_string((double)DisplayDriver::fillAndRenderBufferTime / 1000)).append(" ms"));
    drawTextNewLine("Made by Trevin Price");
    test3D();
    DisplayDriver::renderBuffer();
    test3D();

    while (true) {
        sleep_ms(FRAME_TIME_MS);
        UserInputHandler::updateInput();

        if (UserInputHandler::Buttons[4]) {
            textPos.y = 0; // line count
            break; // exit button
        }
    }
}

void DisplayTests::test3D() {
    Camera camera = Camera();
    camera.Position.z = 5; // with 0 rotation, we face the negative z direction, meaning positive X is left, and Y is still up
    camera.CalculateMatrices();

    std::vector<Vector2> vertices = {};
    std::vector<Vector2> vertices2 = {};
    
    Vector2 myPos = Vector2::zero;
    
    if (DisplayDriver::render3DPoint(Vector3(1, 1, 1), camera, &myPos))
        vertices.push_back(myPos);
    /*if (DisplayDriver::render3DPoint(Vector3(1, 1, -1), camera, &myPos))
        vertices.push_back(myPos);
    if (DisplayDriver::render3DPoint(Vector3(-1, 1, 1), camera, &myPos))
        vertices.push_back(myPos);
    if (DisplayDriver::render3DPoint(Vector3(-1, 1, -1), camera, &myPos))
        vertices.push_back(myPos);*/

    
    if (DisplayDriver::render3DPoint(Vector3(1, -1, 1), camera, &myPos))
        vertices2.push_back(myPos);
    /*if (DisplayDriver::render3DPoint(Vector3(1, -1, -1), camera, &myPos))
        vertices2.push_back(myPos);
    if (DisplayDriver::render3DPoint(Vector3(-1, -1, 1), camera, &myPos))
        vertices2.push_back(myPos);
    if (DisplayDriver::render3DPoint(Vector3(-1, -1, -1), camera, &myPos))
        vertices2.push_back(myPos);*/

    for (Vector2 const& pos: vertices) {
        DisplayDriver::drawRect(pos - 2, 4, 4, Red);
        //drawTextNewLine(((std::string)"Coord: ").append(std::to_string(pos.x)).append("x").append(std::to_string(pos.y)));
    }

    for (Vector2 const& pos: vertices2) {
        DisplayDriver::drawRect(pos - 2, 4, 4, Green);
        //drawTextNewLine(((std::string)"Coord: ").append(std::to_string(pos.x)).append("x").append(std::to_string(pos.y)));
    }

    DisplayDriver::renderBuffer();
}