#pragma once

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <cstring>
#include <cstdlib>
#include <stdint.h>

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

// while 24 bit color is 8-8-8 (8R bits, 8G bits, 8B bits), 16 bit color is (usually) 5-6-5 (5R bits, 6G bits, 5B bits), though it can also be 5-5-5 with one extra bit
// common 16 bit color definitions (RGB 565, which is why green goes up to 64, while red and blue only hit 32)
#define Black           0x0000      /*  0,  0,  0 */
#define Navy            0x000F      /*  0,  0, 16 */
#define DarkGreen       0x03E0      /*  0, 32,  0 */
#define DarkCyan        0x03EF      /*  0, 32, 16 */
#define Maroon          0x7800      /* 16,  0,  0 */
#define Purple          0x780F      /* 16,  0, 16 */
#define Olive           0x7BE0      /* 16, 32,  0 */
#define LightGrey       0xC618      /* 24, 48, 24 */
#define DarkGrey        0x7BEF      /* 16, 32, 16 */
#define Blue            0x001F      /*  0,  0, 32 */
#define Green           0x07E0      /*  0, 64,  0 */
#define Cyan            0x07FF      /*  0, 64, 32 */
#define Red             0xF800      /* 32,  0,  0 */
#define Magenta         0xF81F      /* 32,  0, 32 */
#define Yellow          0xFFE0      /* 32, 64,  0 */
#define White           0xFFFF      /* 32, 64, 32 */
#define Orange          0xFD20      /* 32, 41,  0 */
#define GreenYellow     0xAFE5      /* 22, 64,  6 */
#define Pink            0xF81F      /* 32,  0, 32 */

class DisplayDriver {
    uint16_t *buffer;

    public:
        uint displayWidth;
        uint displayHeight;
        void initDisplay();
        void drawRect(int x, int y, int width, int height, uint16_t color);
        void drawPixel(int x, int y, uint16_t color);
        void renderBuffer();
        void setEntireBuffer(uint16_t color);

        void writeData(void *buffer, int bytes);
        void writeCommandParameter(uint8_t data);
        void writeCommand(uint8_t cmd);
        
        DisplayDriver(uint x, uint y, uint cs, uint rst, uint dc, uint sdi, uint sck, uint sdo, spi_inst_t* spi);
};