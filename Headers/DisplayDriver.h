#pragma once

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <cstring>
#include <cstdio> // used for sprintf
#include <cstdlib>
#include <iostream>
#include "Font.h"
#include "Camera.h"

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

#define FIX_COLOR(color) ((uint16_t) (color >> 8) | (color << 8 & 0xFF00)) // fix endianness.  The display can only receive big endian, while my code was sending small endian.
// this fix was originally done at runtime (not very optimal for performance), but this version makes the compiler do it

// while 24 bit color is 8-8-8 (8R bits, 8G bits, 8B bits), 16 bit color is (usually) 5-6-5 (5R bits, 6G bits, 5B bits), though it can also be 5-5-5 with one extra bit
// common 16 bit color definitions (RGB 565, which is why green goes up to 64, while red and blue only hit 32)
#define Black           FIX_COLOR(0x0000)      /*  0,  0,  0 */
#define Navy            FIX_COLOR(0x000F)      /*  0,  0, 16 */
#define DarkGreen       FIX_COLOR(0x03E0)      /*  0, 32,  0 */
#define DarkCyan        FIX_COLOR(0x03EF)      /*  0, 32, 16 */
#define Maroon          FIX_COLOR(0x7800)      /* 16,  0,  0 */
#define Purple          FIX_COLOR(0x780F)      /* 16,  0, 16 */
#define Olive           FIX_COLOR(0x7BE0)      /* 16, 32,  0 */
#define LightGrey       FIX_COLOR(0x8410)      /* 16, 16, 16 */
#define DarkGrey        FIX_COLOR(0x38E7)      /* 16, 32, 16 */
#define Blue            FIX_COLOR(0x001F)      /*  0,  0, 32 */
#define Green           FIX_COLOR(0x07E0)      /*  0, 64,  0 */
#define Cyan            FIX_COLOR(0x07FF)      /*  0, 64, 32 */
#define Red             FIX_COLOR(0xF800)      /* 32,  0,  0 */
#define Magenta         FIX_COLOR(0xF81F)      /* 32,  0, 32 */
#define Yellow          FIX_COLOR(0xFFE0)      /* 32, 64,  0 */
#define White           FIX_COLOR(0xFFFF)      /* 32, 64, 32 */
#define Orange          FIX_COLOR(0xFD20)      /* 32, 41,  0 */
#define GreenYellow     FIX_COLOR(0xAFE5)      /* 22, 64,  6 */
#define Pink            FIX_COLOR(0xF81F)      /* 32,  0, 32 */

class DisplayDriver {
    static uint16_t *buffer; // normal color would use 3 bytes (24 bits), but I'm using 2 bytes (16 bits), so I'll need to convert my colors
    // while 24 bit color is 8-8-8 (8R bits, 8G bits, 8B bits), 16 bit color is (usually) 5-6-5 (5R bits, 6G bits, 5B bits), though it can also be 5-5-5 with one extra bit

    public:
        static uint actualBaudRate; // in hz
        static uint64_t fillAndRenderBufferTime; // time it takes to fill and render the buffer to the display

        static double xFOV;
        static double focalLength;
        static double skew;
        static Vector2 sensorSize;

        static bool render3DPoint(Vector3 point, Camera camera, Vector2 *pos);
        static void calculateDisplayMatrix();

        static void initDisplay();
        static void drawRect(Vector2 point, int width, int height, uint16_t color);
        static void renderRect(Vector2 point, int width, int height, uint16_t color);
        static void drawOutlinedRect(Vector2 point, int width, int height, uint16_t color, int borderThickness, uint16_t borderColor);
        static void renderOutlinedRect(Vector2 point, int width, int height, uint16_t color, int borderThickness, uint16_t borderColor);
        static void drawPixel(Vector2 point, uint16_t color);
        static void renderBuffer();
        static void fillBuffer(uint16_t color);
        static void drawCenteredText(Vector2 point, std::string text, uint16_t textColor);
        static int drawChar(Vector2 point, char c, uint16_t Color);
        static void drawText(Vector2 point, std::string text, uint16_t textColor);
        static void renderText(Vector2 point, std::string text, uint16_t textColor, uint16_t backgroundColor);
        static void ALTdrawCenteredText(Vector2 point, std::string text, uint16_t textColor);
        static int ALTdrawChar(Vector2 point, char c, uint16_t Color);

        static void writeData(void *buffer, int numberOfBytes);
        static void writeCommandParameter(uint8_t data);
        static void writeCommand(uint8_t cmd);
    private:
        static Matrix displayMatrix;
        static Matrix depthMatrix;
        static Matrix positionMatrix;
};