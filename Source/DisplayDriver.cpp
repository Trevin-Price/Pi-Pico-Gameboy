#include "DisplayDriver.h"

/*
-- Documenting issues along the way

    - One issue I had was with something going wrong when it tried to write data to the display, and the issue turned out to be the fact that I was trying to use the 16bit write method when I had not initiated the device for 16 bit transfers.  My two options were to either convert my premade 8bit gamma fixes to 16bit, or to cast my 16 bit data into 8 bit (while retaining 16 bit color by sending each byte at a time).  I decided to go with the latter.  I learned about the 16 bit issue from this site: https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__spi.html#gacff6be19b2de92a34b295597f3ae8572
    - The display has the green and blue bits swapped for some unknown reason
*/


/*
Credit to: https://github.com/shawnhyam/pico/tree/main/ili9341
for already having made this, allowing me to not have to read 250 pages of ili9341 datasheets to find out the initiation process for this display.
*/

uint bufferSize, displayWidth, displayHeight, csPin, rstPin, dcPin, sdiPin, sckPin, sdoPin; // using unsigned ints because though I could cast a normal int, there is no need to because I'm only using positive values.
uint16_t *buffer; // normal color would use 3 bytes (24 bits), but I'm using 2 bytes (16 bits), so I'll need to convert my colors
// while 24 bit color is 8-8-8 (8R bits, 8G bits, 8B bits), 16 bit color is (usually) 5-6-5 (5R bits, 6G bits, 5B bits), though it can also be 5-5-5 with one extra bit

spi_inst_t* spiPort; // the spi0 definition is a pointer

static inline void selectMode(bool value) { // initiates the device to have data written to it
    asm volatile("nop \n nop \n nop"); // apparently used to properly time stuff (though I fail to see how 3 0x90s are gonna help)
    gpio_put(csPin, !value); // set cs to value, putting it either into or out of select mode (low is enabled, high is disabled)
    asm volatile("nop \n nop \n nop");
}

void DisplayDriver::writeCommand(uint8_t cmd) { // write command to device
    selectMode(1); // write mode
    gpio_put(dcPin, 0); // set dcPin to data selection mode
    spi_write_blocking(spiPort, &cmd, 1); // write data
    gpio_put(dcPin, 1); // set dcPin to register mode
    selectMode(0); // out of write mode
}

void DisplayDriver::writeCommandParameter(uint8_t data) { // set parameter from previously executed command
    selectMode(1); // write mode
    spi_write_blocking(spiPort, &data, 1); // write data
    selectMode(0); // out of write mode
}

void DisplayDriver::writeData(void *myBuffer, int bytes) { // Only writes 8 bit data, so I need to write my 16 bit by using two 8 bit packets
    selectMode(1);
    spi_write_blocking(spiPort, (uint8_t*) myBuffer, bytes); // write data to device, and tell it how many bytes are being written
    selectMode(0); // take device out of write mode
}

/*
    - I could use a variety of pin blocks to work the SPI bus
    - Ex: miso = 0, 4, 8, 12, 16, mosi = 3, 7, 11, 15, 19, sck = 2, 6, 10, 14, 18
    - make sure they're using the same block of pins (miso 0 wont work with mosi 11)
*/

DisplayDriver::DisplayDriver(uint x, uint y, uint cs, uint rst, uint dc, uint sdi, uint sck, uint sdo, spi_inst_t* spi) {

    buffer = new uint16_t[x*y];
    displayWidth = x;
    displayHeight = y;
    bufferSize = x * y * sizeof(uint16_t); // this is in bytes

    csPin = cs; // LCD chip select (low = enabled), this essentially tells it when you're going to write data
    rstPin = rst; // LCD reset (low = reset)
    dcPin = dc; // LCD register / data selection (high for register, low for data selection)
    sdiPin = sdi; // mosi (spi write)
    sckPin = sck; // spi clock
    sdoPin = sdo; // miso (spi read)
    spiPort = spi; // this is the specific spi port I will be using (pico can use spi1 or spi0), the pin selection for sck, sdo, and sdi will decide which spi port I'm using, though I still have to specify it.
}

void DisplayDriver::initDisplay() {
    spi_init(spiPort, 0.5 * 1000000); // initiate the device with buadrate/frequency at 0.5MHz
    spi_set_baudrate(spiPort, 75 * 1000000); // attempt to set the baud rate to 75MHz (too high for the device), and have it return the value it was actually set to (which I could use for debugging if needed)
    // I can probably just initiate the display with the 75mhz baud rate, but I'll seee soon once I've tested.

    gpio_set_function(sdiPin, GPIO_FUNC_SPI); // initiate the 3 SPI pins
    gpio_set_function(sckPin, GPIO_FUNC_SPI);
    gpio_set_function(sdoPin, GPIO_FUNC_SPI);

    // initiate cs pin
    gpio_init(csPin);
    gpio_set_dir(csPin, GPIO_OUT);
    gpio_put(csPin, 0); // select mode

    //initiate reset pin
    gpio_init(rstPin);
    gpio_set_dir(rstPin, GPIO_OUT);
    gpio_put(rstPin, 1); // reset pin is 0 when resetting

    //initiate dc pin
    gpio_init(dcPin);
    gpio_set_dir(dcPin, GPIO_OUT);
    gpio_put(dcPin, 1); // data mode

    // reset display (low == enabled)
    sleep_ms(10);
    gpio_put(rstPin, 0); // resetting
    sleep_ms(10); // give time to process the pin input
    gpio_put(rstPin, 1); // reset disabled

    writeCommand(1); // 1 == software reset
    sleep_ms(100); // give time to reset

    writeCommand(COMMAND_SET_GAMMA);
    writeCommandParameter(1); // set gamma to 1

    // correct positive gamma
    writeCommand(COMMAND_POSITIVE_GAMMA);
    writeData(new uint8_t[15]{ 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 }, 15); // data to correct positive gamma, need to see individually what this data does
    // ShawnHyam's original code for the gamma fixes used a constructor for the uint8_t array that is not valid in either cpp as a whole, this specific version of cpp, or this specific compiler.

    // correct negative gamma
    writeCommand(COMMAND_NEGATIVE_GAMMA);
    writeData(new uint8_t[15]{ 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f }, 15); // data to correct negative gamma, need to see individually what this data does
    // ShawnHyam's original code for the gamma fixes used a constructor for the uint8_t array that is not valid in either cpp as a whole, this specific version of cpp, or this specific compiler.

    writeCommand(COMMAND_MEMORY_ACCESS_CONTROL); // set memory access mode (read and write scanning direction of frame memory)
    writeCommandParameter(0xE0); // this sets the rotation of the display (page 127 of datasheet, these are 6 bits), 0x48 is default, 0xE0 = 11100000 (flip X, flip Y, swap rows and colums (rotate)), I forgot to swap the declared sizes  of columns and rows and was debugging the issue for a while (lines 133 and 139)

    writeCommand(COMMAND_PIXEL_FORMAT);
    writeCommandParameter(0x55); // investigate into the purpose of this code

    // set refresh rate to 70hz
    writeCommand(COMMAND_FRAME_CONTROL);
    writeCommandParameter(0x00); // what is this for?
    writeCommandParameter(0x1B); // this is 27 in dec, why is this here?

    writeCommand(COMMAND_EXIT_SLEEP); // wake up the display

    writeCommand(COMMAND_DISPLAY_ON); // turn on display

    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(0x00); // the command expects 4 bytes of data, so I need to send a few zeroes first
    writeCommandParameter(0x00);
    writeCommandParameter(0x01);
    writeCommandParameter(0x3f); // end column -> 239, as in 240 px wide, starting at px 0

    writeCommand(COMMAND_ROW_ADDRESS_SET); // set row address, meaning y pixel
    writeCommandParameter(0x00); // the command expects 4 bytes of data, so I need to send a few zeroes first
    writeCommandParameter(0x00); 
    writeCommandParameter(0x00); // 0x013f == 319
    writeCommandParameter(0xef); // end page -> 319, as in 320 px wide, starting at px 0

    writeCommand(COMMAND_MEMORY_WRITE); // why?

    writeData(buffer, bufferSize);
}

void fixColor(uint16_t* color) {
    *color = ((*color >> 11 << 11) | ((*color &~(*color >> 5 << 5)) << 6) | (((*color >> 5 << 5)&~(*color >> 11 << 11)) >> 5));
}

void DisplayDriver::fillBuffer(uint16_t color) { // memset doesn't work with multiple bytes
    fixColor(&color);
    
    for (uint x = 0; x < displayWidth; x++) {
        for (uint y = 0; y < displayHeight; y++)
            buffer[y*displayWidth+x] = color;
    }
    //memset(buffer, 0, bufferSize);
}

void DisplayDriver::renderBuffer() {
    writeData(buffer, bufferSize);
}

void DisplayDriver::drawPixel(int x, int y, uint16_t color) {
    buffer[y*displayWidth+x] = color;
}

void DisplayDriver::drawRect(int x, int y, int width, int height, uint16_t backgroundColor, int borderThickness, uint16_t borderColor) {
    fixColor(&backgroundColor);
    fixColor(&borderColor);

    uint16_t *base = &buffer[y*displayWidth+x]; // get a pointer to the first pixel (multiply by display width because it's left to right)

	for (int w = 0; w < width; w++) { // iterate through the width
        uint16_t *loc = base + w; // get a pointer to the pixel at offset w
        
    	for (int h = 0; h < height; h++) { // iterate through the height
            if (w < borderThickness || h < borderThickness || (width-w) < borderThickness || (height-h) < borderThickness)
                *(loc+displayWidth*h) = borderColor; // add displayWidth to loc, dereferencing pointer so I can write to it
            // adding displayWidth because we need to move down one pixel, which is the same as moving across the entire display once
            else
                *(loc+displayWidth*h) = backgroundColor;
        }
	}
}

void DisplayDriver::drawText(int x, int y, int size, char* text) {
    
}