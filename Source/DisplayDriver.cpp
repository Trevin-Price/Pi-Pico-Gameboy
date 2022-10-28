#include "DisplayDriver.h"

/*
Credit to: https://github.com/shawnhyam/pico/tree/main/ili9341
for already having made this, allowing me to not have to read 250 pages of ili9341 datasheets to find out the initiation process for this display.
*/

uint bufferSize, displayWidth, displayHeight, csPin, rstPin, dcPin, sdiPin, sckPin, sdoPin; // using unsigned ints because though I could cast a normal int, there is no need to because I'm only using positive values.
//uint16_t *buffer; // normal color would use 3 bytes (24 bits), but I'm using 2 bytes (16 bits), so I'll need to convert my colors
// while 24 bit color is 8-8-8 (8R bits, 8G bits, 8B bits), 16 bit color is (usually) 5-6-5 (5R bits, 6G bits, 5B bits), though it can also be 5-5-5 with one extra bit

//uint16_t buffer[320*240] = { 0 };
uint8_t buffer[320*240] = {0xFF};

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

void DisplayDriver::writeData(uint16_t *buffer, int bytes) { // give it the buffer so it can read it (16 bit support)
    selectMode(1); // put device into write mode
    spi_write16_blocking(spiPort, buffer, bytes); // write data to device, and tell it how many bytes are being written
    selectMode(0); // take device out of write mode
}

void DisplayDriver::writeData8Bit(uint8_t *buffer, int bytes) {
    selectMode(1);
    spi_write_blocking(spiPort, buffer, bytes);
    selectMode(0);
}

/*
    - I could use a variety of pin blocks to work the SPI bus
    - Ex: miso = 0, 4, 8, 12, 16, mosi = 3, 7, 11, 15, 19, sck = 2, 6, 10, 14, 18
    - make sure they're using the same block of pins (miso 0 wont work with mosi 11)
*/

DisplayDriver::DisplayDriver(uint x, uint y, uint cs, uint rst, uint dc, uint sdi, uint sck, uint sdo, spi_inst_t* spi) {

    //buffer = (uint16_t*) malloc(sizeof(uint16_t) * x * y); // allocate display buffer, also have to cast it because this is cpp (not C)
    //memset(buffer, 0xFFFF, x * y); // write display buffer to be all White
    displayWidth = x;
    displayHeight = y;
    bufferSize = x * y; // store this so I don't have to calculate it every time

    csPin = cs; // LCD chip select (low = enabled), this essentially tells it when you're going to write data
    rstPin = rst; // LCD reset (low = reset)
    dcPin = dc; // LCD register / data selection (high for register, low for data selection)
    sdiPin = sdi; // mosi (spi write)
    sckPin = sck; // spi clock
    sdoPin = sdo; // miso (spi read)
    spiPort = spi; // this is the specific spi port I will be using (pico can use spi1 or spi0), the pin selection for sck, sdo, and sdi will decide which spi port I'm using, though I still have to specify it.
}

bool DisplayDriver::initDisplay() {
    spi_init(spiPort, 0.5 * 1000000); // initiate the device with buadrate/frequency at 0.5MHz
    spi_set_baudrate(spiPort, 75 * 1000000); // attempt to set the baud rate to 75MHz (too high for the device), and have it return the value it was actually set to (which I could use for debugging if needed)
    // I can probably just initiate the display with the 75mhz baud rate, but I'll seee soon once I've tested.

    gpio_set_function(sdiPin, GPIO_FUNC_SPI); // initiate the 3 SPI pins
    gpio_set_function(sckPin, GPIO_FUNC_SPI);
    gpio_set_function(sdoPin, GPIO_FUNC_SPI);

    // initiate cs pin
    gpio_init(csPin);
    gpio_set_dir(csPin, GPIO_OUT);
    gpio_put(csPin, 0); // set it into chip select mode

    //initiate reset pin
    gpio_init(rstPin);
    gpio_set_dir(rstPin, GPIO_OUT);
    gpio_put(rstPin, 1); // reset pin is 0 when resetting

    //initiate dc pin
    gpio_init(dcPin);
    gpio_set_dir(dcPin, GPIO_OUT);
    gpio_put(dcPin, 0); // set into data selection mode

    // reset display (low == enabled)
    sleep_ms(10); // not sure why I'm sleeping prior to resetting, this can be investigated for usefulness later
    gpio_put(rstPin, 0); // reset enabled
    sleep_ms(10); // give time to process the pin input
    gpio_put(rstPin, 1); // reset disabled

    writeCommand(1); // 1 == software reset
    sleep_ms(100); // give time to reset

    writeCommand(COMMAND_SET_GAMMA);
    writeCommandParameter(1); // set gamma to 1

    // correct positive gamma
    writeCommand(COMMAND_POSITIVE_GAMMA);
    writeData8Bit(new uint8_t[15]{ 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 }, 15); // data to correct positive gamma, need to see individually what this data does
    // ShawnHyam's original code for the gamma fixes used a constructor for the uint8_t array that is not valid in either cpp as a whole, this specific version of cpp, or this specific compiler.

    // correct negative gamma
    writeCommand(COMMAND_NEGATIVE_GAMMA);
    writeData8Bit(new uint8_t[15]{ 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f }, 15); // data to correct negative gamma, need to see individually what this data does
    // ShawnHyam's original code for the gamma fixes used a constructor for the uint8_t array that is not valid in either cpp as a whole, this specific version of cpp, or this specific compiler.

    writeCommand(COMMAND_MEMORY_ACCESS_CONTROL); // set memory access mode
    writeCommandParameter(0x48); // need to investigate what this parameter does

    writeCommand(COMMAND_PIXEL_FORMAT);
    writeCommandParameter(0x55); // investigate into the purpose of this code

    // set refresh rate to 70hz
    writeCommand(COMMAND_FRAME_CONTROL);
    writeCommandParameter(0x00); // what is this for?
    writeCommandParameter(0x1B); // this is 27 in dec, why is this here?

    writeCommand(COMMAND_EXIT_SLEEP); // wake up the display

    writeCommand(COMMAND_DISPLAY_ON); // turn on display

    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address (whatever that means)
    writeCommandParameter(0x00);
    writeCommandParameter(0x00);
    writeCommandParameter(0x00); // "start column", but why 3 bytes of 0?
    writeCommandParameter(0xef); // end column -> "239"

    writeCommand(COMMAND_PAGE_ADDRESS_SET); // set page address (whatever that means)
    writeCommandParameter(0x00);
    writeCommandParameter(0x00); // "start page", but why 2 bytes of 0?
    writeCommandParameter(0x01); // why?
    writeCommandParameter(0x3f); // end page -> 319 (why?)

    writeCommand(COMMAND_MEMORY_WRITE); // why?

    sleep_ms(10000);

    writeData8Bit(buffer, bufferSize); // initiate the display to white

    return true;
}
/*
bool DisplayDriver::testDisplay() {
    for (uint i = 0; i < bufferSize; i++) {
        buffer[i*sizeof(uint16_t)] = Blue; // set entire buffer to Blue
    }

    writeData(buffer, bufferSize);

    sleep_ms(10000);
    
    for (uint i = 0; i < bufferSize; i++) {
        buffer[i*sizeof(uint16_t)] = Red; // set entire buffer to Red
    }

    writeData(buffer, bufferSize);

    return true;
}*/