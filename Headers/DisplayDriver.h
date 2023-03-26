#pragma once

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <cstring>
#include <cstdlib>
#include <iostream>
#include "Font.h"
#include "Camera.h"
#include "Colors.h"
#include "Cube.h"
#include <deque>
#include <array>
#include <algorithm> // sorting x ints

// display constants
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define BUFFER_SIZE DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(uint16_t)
#define FRAME_TIME_MS (uint32_t) 1000/70 // instead of typing sleep_ms(1000/70), I can just use this constant

#define CS_PIN 22
#define RST_PIN 21
#define DC_PIN 20
#define SDI_PIN 19
#define SCK_PIN 18
#define SDO_PIN 16
#define SPI_CHANNEL spi0

// command definition
#define COMMAND_SET_GAMMA             0x26
#define COMMAND_POSITIVE_GAMMA        0xE0
#define COMMAND_NEGATIVE_GAMMA        0xE1
#define COMMAND_MEMORY_ACCESS_CONTROL 0x36
#define COMMAND_PIXEL_FORMAT          0x3A
#define COMMAND_FRAME_CONTROL         0xB1
#define COMMAND_EXIT_SLEEP            0x11
#define COMMAND_DISPLAY_ON            0x29
#define COMMAND_COLUMN_ADDRESS_SET    0x2A
#define COMMAND_ROW_ADDRESS_SET       0x2B
#define COMMAND_MEMORY_WRITE          0x2C

class DisplayDriver {
    static uint16_t *buffer; // normal color would use 3 bytes (24 bits), but I'm using 2 bytes (16 bits), so I'll need to convert my colors
    // while 24 bit color is 8-8-8 (8R bits, 8G bits, 8B bits), 16 bit color is (usually) 5-6-5 (5R bits, 6G bits, 5B bits), though it can also be 5-5-5 with one extra bit

    public:
        static uint actualBaudRate; // in hz
        static uint64_t fillAndRenderBufferTime; // time it takes to fill and render the buffer to the display

        static void initDisplay();
        
        static void drawRect(Vector2 point, Vector2 size, uint16_t color);
        static void renderRect(Vector2 point, Vector2 size, uint16_t color);
        static void drawOutlinedRect(Vector2 point, Vector2 size, uint16_t backgroundColor, uint16_t borderThickness, uint16_t borderColor);
        static void renderOutlinedRect(Vector2 point, Vector2 size, uint16_t color, uint16_t borderThickness, uint16_t borderColor);
        

        static void drawPixel(Vector2 point, uint16_t color);
        
        static void drawVerticalLine(Vector2 point, int16_t length, const uint16_t color);
        static void drawHorizontalLine(Vector2 point, int16_t length, uint16_t color);
        static void drawLine(Vector2 start, Vector2 end, uint16_t color);
        static void drawFace(std::array<Vector2, 4> corners, uint16_t color); // corners MUST be in a specific order

        static void renderBuffer();
        static void fillBuffer(uint16_t color);

        static void drawCenteredText(Vector2 point, std::string text, uint16_t textColor);
        static int drawChar(Vector2 point, char c, uint16_t Color);
        static void drawText(Vector2 point, std::string text, uint16_t textColor);
        static void renderText(Vector2 point, std::string text, uint16_t textColor, uint16_t backgroundColor);

        static void ALTdrawCenteredText(Vector2 point, std::string text, uint16_t textColor);
        static int ALTdrawChar(Vector2 point, char c, uint16_t Color);

        static void calculateVerticesFacesAndEdges(std::deque<Cube> *cubes, std::deque<Vector3> *vertices, std::deque<std::array<uint16_t, 2>> *edges, std::deque<std::array<uint16_t, 4>> *faces);
        
    private:
        static void writeData(void *buffer, int numberOfBytes);
        static void writeCommandParameter(uint8_t data);
        static void writeCommand(uint8_t cmd);
};