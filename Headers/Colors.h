#pragma once


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