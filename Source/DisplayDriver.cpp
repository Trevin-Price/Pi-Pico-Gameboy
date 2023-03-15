#include "DisplayDriver.h"

/*
-- Documenting issues along the way

    - One issue I had was with something going wrong when it tried to write data to the display, and the issue turned out to be the fact that I was trying to use the 16bit write method when I had not initiated the device for 16 bit transfers.  My two options were to either convert my premade 8bit gamma fixes to 16bit, or to cast my 16 bit data into 8 bit (while retaining 16 bit color by sending each byte at a time).  I decided to go with the latter.  I learned about the 16 bit issue from this site: https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__spi.html#gacff6be19b2de92a34b295597f3ae8572
    - The display has the green and blue bits swapped for some unknown reason
    - The problem wasn't the green and blue bits swapped, but rather that my endianness was backwards
*/

// One potential optimization is to use the set collumn and row addresses, which lets me push minimal data and write to certain parts of the display, which seems to be faster than my current method of overwriting the entire buffer every time I want to write a new frame.


/*
Credit to: https://github.com/shawnhyam/pico/tree/main/ili9341
for already having made this, allowing me to not have to read 250 pages of ili9341 datasheets to find out the initiation process for this display.
*/

uint16_t* DisplayDriver::buffer = new uint16_t[DISPLAY_WIDTH * DISPLAY_HEIGHT];

static inline void selectMode(bool value) { // initiates the device to have data written to it
    asm volatile("nop \n nop \n nop"); // apparently used to properly time stuff (though I fail to see how 3 0x90s are gonna help)
    gpio_put(CS_PIN, !value); // set cs to value, putting it either into or out of select mode (low is enabled, high is disabled)
    asm volatile("nop \n nop \n nop");
}

void DisplayDriver::writeCommand(uint8_t cmd) { // write command to device
    selectMode(1); // write mode
    gpio_put(DC_PIN, 0); // set DC_PIN to data selection mode
    spi_write_blocking(SPI_CHANNEL, &cmd, 1); // write data
    gpio_put(DC_PIN, 1); // set DC_PIN to register mode
    selectMode(0); // out of write mode
}

void DisplayDriver::writeCommandParameter(uint8_t data) { // set parameter from previously executed command
    selectMode(1); // write mode
    spi_write_blocking(SPI_CHANNEL, &data, 1); // write data
    selectMode(0); // out of write mode
}

void DisplayDriver::writeData(void *myBuffer, int numberOfBytes) { // Only writes 8 bit data, so I need to write my 16 bit by using two 8 bit packets
    selectMode(1);
    spi_write_blocking(SPI_CHANNEL, (uint8_t*) myBuffer, numberOfBytes); // write data to device, and tell it how many bytes are being written
    selectMode(0); // take device out of write mode
}

/*
    - I could use a variety of pin blocks to work the SPI bus
    - Ex: miso = 0, 4, 8, 12, 16, mosi = 3, 7, 11, 15, 19, sck = 2, 6, 10, 14, 18
    - make sure they're using the same block of pins (miso 0 wont work with mosi 11)
*/

uint DisplayDriver::actualBaudRate = 0; // initialization
uint64_t DisplayDriver::fillAndRenderBufferTime = 0;

void DisplayDriver::initDisplay() {
    actualBaudRate = spi_init(SPI_CHANNEL, 75 * 1000000); // initiate the device with buadrate/frequency at 75MHz, though it wont be able to hit this, so it'll limit down a bit lower (62~63MHz in testing)

    gpio_set_function(SDI_PIN, GPIO_FUNC_SPI); // initiate the 3 SPI pins
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SDO_PIN, GPIO_FUNC_SPI);

    // initiate cs pin
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 0); // select mode

    //initiate reset pin
    gpio_init(RST_PIN);
    gpio_set_dir(RST_PIN, GPIO_OUT);
    gpio_put(RST_PIN, 1); // reset pin is 0 when resetting

    //initiate dc pin
    gpio_init(DC_PIN);
    gpio_set_dir(DC_PIN, GPIO_OUT);
    gpio_put(DC_PIN, 1); // data mode

    // reset display (low == enabled)
    sleep_ms(10);
    gpio_put(RST_PIN, 0); // resetting
    sleep_ms(10); // give time to process the pin input
    gpio_put(RST_PIN, 1); // reset disabled

    writeCommand(1); // 1 == software reset
    sleep_ms(100); // give time to reset

    /* unnecessary commands.  Adafruit uses them, no clue why, but they're undocumented (according to them)
    writeCommand(0xEF);// undocumented commands below, used by Adafruit
    writeCommandParameter(0x03);
    writeCommandParameter(0x80);
    writeCommandParameter(0x02);

    writeCommand(0xCF);
    writeCommandParameter(0x00);
    writeCommandParameter(0xC1);
    writeCommandParameter(0x30);

    writeCommand(0xED);
    writeCommandParameter(0x64);
    writeCommandParameter(0x03);
    writeCommandParameter(0x12);
    writeCommandParameter(0x81);

    writeCommand(0xE8);
    writeCommandParameter(0x85);
    writeCommandParameter(0x00);
    writeCommandParameter(0x78);

    writeCommand(0xCB);
    writeCommandParameter(0x039);
    writeCommandParameter(0x2C);
    writeCommandParameter(0x00);
    writeCommandParameter(0x34);
    writeCommandParameter(0x02);

    writeCommand(0xF7);
    writeCommandParameter(0x20);

    writeCommand(0xEA);
    writeCommandParameter(0x00);
    writeCommandParameter(0x00);// end of undocumented commands


    writeCommand(0xC0); // power control 1
    writeCommandParameter(0x23);
    writeCommand(0xC1); // power control 2
    writeCommandParameter(0x10);
    writeCommand(0xC5); // vcom control 1
    writeCommandParameter(0x3e);
    writeCommandParameter(0x28);
    writeCommand(0xC7); // vcom control 2
    writeCommandParameter(0x86);

    writeCommand(0xF2); // enable "3 gamma control"
    writeCommandParameter(0x00);*/

    writeCommand(COMMAND_MEMORY_ACCESS_CONTROL); // set memory access mode (read and write scanning direction of frame memory)
    writeCommandParameter(0b11101000); // this sets the rotation of the display (page 127 of datasheet, these are 6 bits), 0x48 is default, 0xE0 = 11100000 (flip X, flip Y, swap rows and colums (rotate)), I forgot to swap the declared sizes  of columns and rows and was debugging the issue for a while (lines 133 and 139), this also sets it to BGR mode, which is necessary because my endianness fix made it convert to BGR

    writeCommand(COMMAND_PIXEL_FORMAT);
    writeCommandParameter(0x55); // sets 16 bits per pixel on both DPI and DBI

    // set refresh rate to 70hz
    writeCommand(COMMAND_FRAME_CONTROL);
    writeCommandParameter(0x00); // see datasheet page 155
    writeCommandParameter(0b11011); // this is the data to set it to 70hz

    // gamma fixes make it so the color is a little more saturated
    writeCommand(COMMAND_SET_GAMMA);
    writeCommandParameter(1); // set gamma to 1

    // correct positive gamma
    writeCommand(COMMAND_POSITIVE_GAMMA);
    writeData(new uint8_t[15]{ 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 }, 15); // set positive gamma curve
    // ShawnHyam's original code for the gamma fixes used a constructor for the uint8_t array that is not valid in either cpp as a whole, this specific version of cpp, or this specific compiler.

    // correct negative gamma
    writeCommand(COMMAND_NEGATIVE_GAMMA);
    writeData(new uint8_t[15]{ 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f }, 15); // set negative gamma curve
    // ShawnHyam's original code for the gamma fixes used a constructor for the uint8_t array that is not valid in either cpp as a whole, this specific version of cpp, or this specific compiler.

    writeCommand(COMMAND_EXIT_SLEEP); // wake up the display

    writeCommand(COMMAND_DISPLAY_ON); // turn on display

    // setup window after display is on

    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(0x00); // starts at 0
    writeCommandParameter(0x00);
    writeCommandParameter(DISPLAY_WIDTH >> 8 & 0xFF); // get the upper byte of the width
    writeCommandParameter(DISPLAY_WIDTH & 0xFF); // get the lower byte of the width

    writeCommand(COMMAND_ROW_ADDRESS_SET); // set row address, meaning y pixel
    writeCommandParameter(0x00); // the command takes 4 bytes of data
    writeCommandParameter(0x00); // the first 2 are the first y coordinate, starting at px 0
    writeCommandParameter(DISPLAY_HEIGHT >> 8 & 0xFF); // the last 2 are the last y coordinate
    writeCommandParameter(DISPLAY_HEIGHT & 0xFF); // get the lower byte of the height

    writeCommand(COMMAND_MEMORY_WRITE); // set it from setting mode to write mode

    absolute_time_t start = get_absolute_time();
    fillBuffer(White); // fill display buffer
    renderBuffer(); // push buffer
    fillAndRenderBufferTime = absolute_time_diff_us(start, get_absolute_time());
}

void DisplayDriver::fillBuffer(uint16_t color) { // memset doesn't work with multiple bytes, but std::fill does
    std::fill(buffer, buffer+(DISPLAY_WIDTH*DISPLAY_HEIGHT), color);
    // I could add a check for if the uint16_t is compatible with memset (2 bytes are identical), but the optimization seems useless considering the rarity of the situation
}

void DisplayDriver::renderBuffer() {
    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(0x00); // starts at 0
    writeCommandParameter(0x00);
    writeCommandParameter(DISPLAY_WIDTH >> 8 & 0xFF); // get the upper byte of the width
    writeCommandParameter(DISPLAY_WIDTH & 0xFF); // get the lower byte of the width

    writeCommand(COMMAND_ROW_ADDRESS_SET); // set row address, meaning y pixel
    writeCommandParameter(0x00); // the command takes 4 bytes of data
    writeCommandParameter(0x00); // the first 2 are the first y coordinate, starting at px 0
    writeCommandParameter(DISPLAY_HEIGHT >> 8 & 0xFF); // the last 2 are the last y coordinate
    writeCommandParameter(DISPLAY_HEIGHT & 0xFF); // get the lower byte of the height

    writeCommand(COMMAND_MEMORY_WRITE); // set it from setting mode to write mode

    writeData(buffer, BUFFER_SIZE);
}

void DisplayDriver::drawPixel(Vector2 point, uint16_t color) {
    buffer[(int) (point.Y*DISPLAY_WIDTH+point.X)] = color;
}


void DisplayDriver::drawRect(Vector2 point, int width, int height, uint16_t backgroundColor) {
    point = Vector2(round(point.X), round(point.Y));
    uint16_t *base = &buffer[(int)(point.Y*DISPLAY_WIDTH+point.X)]; // get a pointer to the first pixel (multiply by display width because it's left to right)

	for (int w = 0; w < width; w++) { // iterate through the width
        if (w + point.X < DISPLAY_WIDTH) {
            uint16_t *loc = base + w; // get a pointer to the pixel at offset w
            
            for (int h = 0; h < height; h++) { // iterate through the height
                if (h + point.Y < DISPLAY_HEIGHT)
                    *(loc+DISPLAY_WIDTH*h) = backgroundColor;
            }
        }
	}
}

void DisplayDriver::drawOutlinedRect(Vector2 point, int width, int height, uint16_t backgroundColor, int borderThickness, uint16_t borderColor) {
    uint16_t *base = &buffer[(int) (point.Y*DISPLAY_WIDTH+point.X)]; // get a pointer to the first pixel (multiply by display width because it's left to right)

	for (int w = 0; w < width; w++) { // iterate through the width
        if (w + point.X < DISPLAY_WIDTH) {
            uint16_t *loc = base + w; // get a pointer to the pixel at offset w
            
            for (int h = 0; h < height; h++) { // iterate through the height
                if (h + point.Y < DISPLAY_HEIGHT) {
                    if (w < borderThickness || h < borderThickness || (width-w) <= borderThickness || (height-h) <= borderThickness) // <= cause the greater range never gets hit (0-239, never touches 240)
                        *(loc+DISPLAY_WIDTH*h) = borderColor; // add DISPLAY_WIDTH to loc, dereferencing pointer so I can write to it
                        // adding DISPLAY_WIDTH because we need to move down one pixel, which is the same as moving across the entire display once
                    else
                        *(loc+DISPLAY_WIDTH*h) = backgroundColor;
                }
            }
        }  
	}
}


int DisplayDriver::drawChar(Vector2 point, char c, uint16_t Color) { // font size is currently forced
    const uint8_t (*characterData)[FONT_HEIGHT] = &fontData[static_cast<uint8_t>(c - ' ')]; // this is 12 bytes wide, of which each byte represents one vertical pixel, and holds 8 horizontal pixels
    // c - ' ' because the character list doesn't include non printable ascii, instead opting to start at the space character

    uint16_t* base = &buffer[(int) (point.X + point.Y*DISPLAY_WIDTH)];

    for (uint8_t yCoord = 0; yCoord < FONT_HEIGHT; yCoord++) { // iterates each "row" of the character (each y coordinate)
        uint8_t rowData = (*characterData)[yCoord]; // dereferences the pointer to the uint8_t which stores the 8 x coordinates for each y coordinate
        uint16_t* xRow = base + yCoord*DISPLAY_WIDTH;

        for (uint8_t xCoord = 0; xCoord < FONT_WIDTH; xCoord++) {
            if (rowData & (1 << (8 - xCoord))) { // checks if the bit is a 1 or a 0, if it's 1, write a pixel, if 0, don't
                                // (8 - xCoord) because it has to be left to right
                *(xRow + xCoord) = Color;
            }
        }
    }

    return FONT_WIDTH; // width of each character
}

void DisplayDriver::drawCenteredText(Vector2 point, std::string text, uint16_t textColor) { // centered around (x, y)
    int width = static_cast<int>(text.size()) * FONT_WIDTH; // font is 8 px wide
    point -= Vector2(width/2, FONT_HEIGHT/2);

    for (char const &character : text) { // loop through all characters
        point.X += drawChar(point, character, textColor);// draw individual character (returns width)
    }
}

void DisplayDriver::drawText(Vector2 point, std::string text, uint16_t textColor) { // x, y is the top left corner, renders left to right
    for (char const &character : text) { // loop through all characters
        point.X += drawChar(point, character, textColor);// draw individual character (returns width)
    }
}



// ALT text functions are a hacky fix to make text bigger.  A better fix would be a proper scaling font system, but that sounds annoying to implement, and I'd likely end up rarely using it
int DisplayDriver::ALTdrawChar(Vector2 point, char c, uint16_t Color) { // font size is currently forced
    const uint8_t (*characterData)[FONT_HEIGHT] = &fontData[static_cast<uint8_t>(c - ' ')]; // this is 12 bytes wide, of which each byte represents one vertical pixel, and holds 8 horizontal pixels
    // c - ' ' because the character list doesn't include non printable ascii, instead opting to start at the space character

    uint16_t* base = &buffer[(int) (point.X + point.Y*DISPLAY_WIDTH)];

    for (uint8_t yCoord = 0; yCoord < FONT_HEIGHT*2; yCoord+=2) { // iterates each "row" of the character (each y coordinate)
        uint8_t rowData = (*characterData)[yCoord/2]; // divide by 2 because this font is 2x bigger
        uint16_t* xRow = base + yCoord*DISPLAY_WIDTH;
        uint16_t* xRow2 = base + (yCoord+1)*DISPLAY_WIDTH;

        for (uint8_t xCoord = 0; xCoord < FONT_WIDTH*2; xCoord+=2) { // font is 2x bigger than normal
            if (rowData & (1 << (8 - xCoord/2))) { // checks if the bit is a 1 or a 0, if it's 1, write a pixel, if 0, don't
                *(xRow + xCoord) = Color; // top left
                *(xRow + (xCoord+1)) = Color; // bottom left
                *(xRow2 + xCoord) = Color; // top right
                *(xRow2 + (xCoord+1)) = Color; // bottom right
            }
        }
    }

    return FONT_WIDTH*2; // width of each character
}

void DisplayDriver::ALTdrawCenteredText(Vector2 point, std::string text, uint16_t textColor) { // alternate is a hacky way to render 2x bigger text
    int width = static_cast<int>(text.size()) * FONT_WIDTH * 2; // font is 2x wider than normal
    int startX = point.X - width/2; // subtract half the width to reverse the offset
    int startY = point.Y - FONT_HEIGHT; // this font is 2x bigger than normal, and so it cancells out the divide by 2

    for (char const &character : text) { // loop through all characters
        startX += ALTdrawChar(Vector2(startX, startY), character, textColor); // draw individual character (returns width, which is useless because text only has 2 possible widthes right now)
    }
}



void DisplayDriver::renderRect(Vector2 point, int width, int height, uint16_t rectColor) { // custom optimized render function
    if (point.X + width > DISPLAY_WIDTH)
        width = DISPLAY_WIDTH-point.X;
    if (point.Y + height > DISPLAY_HEIGHT)
        height = DISPLAY_HEIGHT-point.Y;
    
    uint16_t *newBuffer = new uint16_t[width*height];
    std::fill(newBuffer, newBuffer+width*height, rectColor);

    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(((int) point.X) >> 8 & 0xFF); // starting x pos
    writeCommandParameter(((int) point.X) & 0xFF);
    writeCommandParameter(((int) point.X+width-1) >> 8 & 0xFF);
    writeCommandParameter(((int) point.X+width-1) & 0xFF); // -1 because it's base 0 (idk, that doesnt intuitively make sense to me, but it works...)

    writeCommand(COMMAND_ROW_ADDRESS_SET); // set row address, meaning y pixel
    writeCommandParameter(((int) point.Y) >> 8 & 0xFF); // starting y pos
    writeCommandParameter(((int) point.Y) & 0xFF);
    writeCommandParameter(((int) point.Y+height-1) >> 8 & 0xFF); // ending y pos
    writeCommandParameter(((int) point.Y+height-1) & 0xFF); // -1 because it's base 0 (idk, that doesnt intuitively make sense to me, but it works...)

    writeCommand(COMMAND_MEMORY_WRITE);

    writeData(newBuffer, width*height*sizeof(uint16_t)); // give it the base pointer
    
    delete [] newBuffer;
    newBuffer = nullptr;
}

void DisplayDriver::renderOutlinedRect(Vector2 point, int width, int height, uint16_t backgroundColor, int borderThickness, uint16_t borderColor) {
    int newWidth = width;
    int newHeight = height;
    if (point.X + width > DISPLAY_WIDTH)
        newWidth = DISPLAY_WIDTH-point.X;
    if (point.Y + height > DISPLAY_HEIGHT)
        newHeight = DISPLAY_HEIGHT-point.Y;

    uint16_t *newBuffer = new uint16_t[newWidth*newHeight];
    std::fill(newBuffer, newBuffer+newWidth*newHeight, backgroundColor);

    for (int newX = 0; newX < newWidth; newX++) {
        for (int newY = 0; newY < newHeight; newY++) {
            if (newX < (borderThickness) || newX >= (width-borderThickness) || newY < borderThickness || newY >= (height-borderThickness))
                *(newBuffer+newX+newY*newWidth) = borderColor;
        }
    }

    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(((int) point.X) >> 8 & 0xFF);
    writeCommandParameter(((int) point.X) & 0xFF);
    writeCommandParameter(((int) point.X+newWidth-1) >> 8 & 0xFF); // get the upper byte of the width
    writeCommandParameter(((int) point.X+newWidth-1) & 0xFF); // get the lower byte of the width

    writeCommand(COMMAND_ROW_ADDRESS_SET); // set row address, meaning y pixel
    writeCommandParameter(((int) point.Y) >> 8 & 0xFF); // the command takes 4 bytes of data
    writeCommandParameter(((int) point.Y) & 0xFF); // the first 2 are the first y coordinate, starting at px 0
    writeCommandParameter(((int) point.Y+newHeight-1) >> 8 & 0xFF); // the last 2 are the last y coordinate
    writeCommandParameter(((int) point.Y+newHeight-1) & 0xFF); // get the lower byte of the height

    writeCommand(COMMAND_MEMORY_WRITE);

    writeData(newBuffer, newWidth*newHeight*sizeof(uint16_t)); // give it the base pointer

    delete [] newBuffer;
    newBuffer = nullptr;
}

void DisplayDriver::renderText(Vector2 point, std::string text, uint16_t textColor, uint16_t backgroundColor) { // x, y is the top left corner, renders left to right
    int width = static_cast<int>(text.size()) * FONT_WIDTH;
    int height = FONT_HEIGHT;

    uint16_t* newBuffer = new uint16_t[width*height]; // allocate a buffer for the entire text size
    std::fill(newBuffer, newBuffer+width*height, backgroundColor);
    
    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(((int) point.X) >> 8 & 0xFF);
    writeCommandParameter(((int) point.X) & 0xFF);
    writeCommandParameter(((int) point.X+width-1) >> 8 & 0xFF);
    writeCommandParameter(((int) point.X+width-1) & 0xFF);

    writeCommand(COMMAND_ROW_ADDRESS_SET); // set row address, meaning y pixel
    writeCommandParameter(((int) point.Y) >> 8 & 0xFF);
    writeCommandParameter(((int) point.Y) & 0xFF);
    writeCommandParameter(((int) point.Y+height-1) >> 8 & 0xFF);
    writeCommandParameter(((int) point.Y+height-1) & 0xFF);

    writeCommand(COMMAND_MEMORY_WRITE);

    int xPos = 0;

    for (char const &character : text) { // loop through all characters
        const uint8_t (*characterData)[FONT_HEIGHT] = &fontData[static_cast<uint8_t>(character - ' ')]; // this is 12 bytes wide, of which each byte represents one vertical pixel, and holds 8 horizontal pixels
        // c - ' ' because the character list doesn't include non printable ascii, instead opting to start at the space character
        uint16_t* base = &newBuffer[xPos];
        xPos += FONT_WIDTH;

        for (uint8_t yCoord = 0; yCoord < FONT_HEIGHT; yCoord++) { // iterates each "row" of the character (each y coordinate)
            uint8_t rowData = (*characterData)[yCoord]; // dereferences the pointer to the uint8_t which stores the 8 x coordinates for each y coordinate
            uint16_t* xRow = base + yCoord*width;

            for (uint8_t xCoord = 0; xCoord < FONT_WIDTH; xCoord++) {
                if (rowData & (1 << (8 - xCoord))) { // checks if the bit is a 1 or a 0, if it's 1, write a pixel, if 0, don't
                                    // (8 - xCoord) because it's left to right
                    *(xRow + xCoord) = textColor;
                }
            }
        }
    }

    writeData(newBuffer, width*height*sizeof(uint16_t)); // write the buffer

    delete [] newBuffer; // deallocate to save memory
    newBuffer = nullptr;
}
