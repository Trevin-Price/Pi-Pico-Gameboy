#include "DisplayDriver.h"

// need to rewrite drawLine and drawFace, as both of them incorrectly round
// going along with this, I should rewrite my vector class as a template, as the current system is what led to the rounding errors

/*
-- Documenting issues along the way

    - One issue I had was with something going wrong when it tried to write data to the display, and the issue turned out to be the fact that I was trying to use the 16bit write method when I had not initiated the device for 16 bit transfers.  My two options were to either convert my premade 8bit gamma fixes to 16bit, or to cast my 16 bit data into 8 bit (while retaining 16 bit color by sending each byte at a time).  I decided to go with the latter.  I learned about the 16 bit issue from this site: https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__spi.html#gacff6be19b2de92a34b295597f3ae8572
    - The display has the green and blue bits swapped for some unknown reason
    - The problem wasn't the green and blue bits swapped, but rather that my endianness was backwards
*/

/*
Credit to: https://github.com/shawnhyam/pico/tree/main/ili9341
for already having made this, allowing me to not have to read 250 pages of ili9341 datasheets to find out the initiation process for this display.
*/

uint16_t *DisplayDriver::buffer = new uint16_t[DISPLAY_WIDTH * DISPLAY_HEIGHT];


Vector3 halfSize = Vector3(0, 0, 0); // these are used in 3d calculations
Matrix Rx = Matrix(3, 3);
Matrix Ry = Matrix(3, 3);
Matrix Rz = Matrix(3, 3);

static inline void selectMode(bool value)
{                                      // initiates the device to have data written to it
    asm volatile("nop \n nop \n nop"); // apparently used to properly time stuff (though I fail to see how 3 0x90s are gonna help)
    gpio_put(CS_PIN, !value);          // set cs to value, putting it either into or out of select mode (low is enabled, high is disabled)
    asm volatile("nop \n nop \n nop");
}

void DisplayDriver::writeCommand(uint8_t cmd)
{                                             // write command to device
    selectMode(1);                            // write mode
    gpio_put(DC_PIN, 0);                      // set DC_PIN to data selection mode
    spi_write_blocking(SPI_CHANNEL, &cmd, 1); // write data
    gpio_put(DC_PIN, 1);                      // set DC_PIN to register mode
    selectMode(0);                            // out of write mode
}

void DisplayDriver::writeCommandParameter(uint8_t data)
{                                              // set parameter from previously executed command
    selectMode(1);                             // write mode
    spi_write_blocking(SPI_CHANNEL, &data, 1); // write data
    selectMode(0);                             // out of write mode
}

void DisplayDriver::writeData(void *myBuffer, int numberOfBytes)
{ // Only writes 8 bit data, so I need to write my 16 bit by using two 8 bit packets
    selectMode(1);
    spi_write_blocking(SPI_CHANNEL, (uint8_t *)myBuffer, numberOfBytes); // write data to device, and tell it how many bytes are being written
    selectMode(0);                                                       // take device out of write mode
}

/*
    - I could use a variety of pin blocks to work the SPI bus
    - Ex: miso = 0, 4, 8, 12, 16, mosi = 3, 7, 11, 15, 19, sck = 2, 6, 10, 14, 18
    - make sure they're using the same block of pins (miso 0 wont work with mosi 11)
*/

uint DisplayDriver::actualBaudRate = 0; // initialization
uint64_t DisplayDriver::fillAndRenderBufferTime = 0;

void DisplayDriver::initDisplay()
{
    Rx[0][0] = 1;
    Ry[1][1] = 1;
    Rz[2][2] = 1;

    actualBaudRate = spi_init(SPI_CHANNEL, 75 * 1000000); // initiate the device with buadrate/frequency at 75MHz, though it wont be able to hit this, so it'll limit down a bit lower (62~63MHz in testing)

    gpio_set_function(SDI_PIN, GPIO_FUNC_SPI); // initiate the 3 SPI pins
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SDO_PIN, GPIO_FUNC_SPI);

    // initiate cs pin
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 0); // select mode

    // initiate reset pin
    gpio_init(RST_PIN);
    gpio_set_dir(RST_PIN, GPIO_OUT);
    gpio_put(RST_PIN, 1); // reset pin is 0 when resetting

    // initiate dc pin
    gpio_init(DC_PIN);
    gpio_set_dir(DC_PIN, GPIO_OUT);
    gpio_put(DC_PIN, 1); // data mode

    // reset display (low == enabled)
    sleep_ms(10);
    gpio_put(RST_PIN, 0); // resetting
    sleep_ms(10);         // give time to process the pin input
    gpio_put(RST_PIN, 1); // reset disabled

    writeCommand(1); // 1 == software reset
    sleep_ms(100);   // give time to reset

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
    writeCommandParameter(0b11101000);           // this sets the rotation of the display (page 127 of datasheet, these are 6 bits), 0x48 is default, 0xE0 = 11100000 (flip X, flip Y, swap rows and colums (rotate)), I forgot to swap the declared sizes  of columns and rows and was debugging the issue for a while (lines 133 and 139), this also sets it to BGR mode, which is necessary because my endianness fix made it convert to BGR

    writeCommand(COMMAND_PIXEL_FORMAT);
    writeCommandParameter(0x55); // sets 16 bits per pixel on both DPI and DBI

    // set refresh rate to 70hz
    writeCommand(COMMAND_FRAME_CONTROL);
    writeCommandParameter(0x00);    // see datasheet page 155
    writeCommandParameter(0b11011); // this is the data to set it to 70hz

    // gamma fixes make it so the color is a little more saturated
    writeCommand(COMMAND_SET_GAMMA);
    writeCommandParameter(1); // set gamma to 1

    // correct positive gamma
    writeCommand(COMMAND_POSITIVE_GAMMA);
    writeData(new uint8_t[15]{0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00}, 15); // set positive gamma curve
    // ShawnHyam's original code for the gamma fixes used a constructor for the uint8_t array that is not valid in either cpp as a whole, this specific version of cpp, or this specific compiler.

    // correct negative gamma
    writeCommand(COMMAND_NEGATIVE_GAMMA);
    writeData(new uint8_t[15]{0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f}, 15); // set negative gamma curve
    // ShawnHyam's original code for the gamma fixes used a constructor for the uint8_t array that is not valid in either cpp as a whole, this specific version of cpp, or this specific compiler.

    writeCommand(COMMAND_EXIT_SLEEP); // wake up the display

    writeCommand(COMMAND_DISPLAY_ON); // turn on display

    // setup window after display is on

    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(0x00);              // starts at 0
    writeCommandParameter(0x00);
    writeCommandParameter(DISPLAY_WIDTH >> 8 & 0xFF); // get the upper byte of the size.X
    writeCommandParameter(DISPLAY_WIDTH & 0xFF);      // get the lower byte of the size.X

    writeCommand(COMMAND_ROW_ADDRESS_SET);             // set row address, meaning y pixel
    writeCommandParameter(0x00);                       // the command takes 4 bytes of data
    writeCommandParameter(0x00);                       // the first 2 are the first y coordinate, starting at px 0
    writeCommandParameter(DISPLAY_HEIGHT >> 8 & 0xFF); // the last 2 are the last y coordinate
    writeCommandParameter(DISPLAY_HEIGHT & 0xFF);      // get the lower byte of the size.Y

    writeCommand(COMMAND_MEMORY_WRITE); // set it from setting mode to write mode

    absolute_time_t start = get_absolute_time();
    fillBuffer(White); // fill display buffer
    renderBuffer();    // push buffer
    fillAndRenderBufferTime = absolute_time_diff_us(start, get_absolute_time());
}

void DisplayDriver::fillBuffer(uint16_t color)
{ // memset doesn't work with multiple bytes, but std::fill does
    std::fill(buffer, buffer + (DISPLAY_WIDTH * DISPLAY_HEIGHT), color);
    // I could add a check for if the uint16_t is compatible with memset (2 bytes are identical), but the optimization seems useless considering the rarity of the situation
}

void DisplayDriver::renderBuffer()
{
    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(0x00);              // starts at 0
    writeCommandParameter(0x00);
    writeCommandParameter(DISPLAY_WIDTH >> 8 & 0xFF); // get the upper byte of the size.X
    writeCommandParameter(DISPLAY_WIDTH & 0xFF);      // get the lower byte of the size.X

    writeCommand(COMMAND_ROW_ADDRESS_SET);             // set row address, meaning y pixel
    writeCommandParameter(0x00);                       // the command takes 4 bytes of data
    writeCommandParameter(0x00);                       // the first 2 are the first y coordinate, starting at px 0
    writeCommandParameter(DISPLAY_HEIGHT >> 8 & 0xFF); // the last 2 are the last y coordinate
    writeCommandParameter(DISPLAY_HEIGHT & 0xFF);      // get the lower byte of the size.Y

    writeCommand(COMMAND_MEMORY_WRITE); // set it from setting mode to write mode

    writeData(buffer, BUFFER_SIZE);
}

void DisplayDriver::drawPixel(Vector2 point, uint16_t color)
{
    if (point.Y >= 0 && point.Y < DISPLAY_HEIGHT && point.X >= 0 && point.X < DISPLAY_WIDTH) // memory safety
        buffer[(int)(point.Y * DISPLAY_WIDTH + point.X)] = color;
}

void DisplayDriver::drawHorizontalLine(Vector2 point, int16_t length, uint16_t color)
{ // negative length for negative direction
    if (point.Y < 0 || point.Y >= DISPLAY_HEIGHT)
        return;

    if (length < 0)
    {
        if (point.X + length < 0)
            length = -point.X;
        std::fill(buffer + (uint)(point.Y * DISPLAY_WIDTH + point.X + length), buffer + (uint)(point.Y * DISPLAY_WIDTH + point.X), color);
    }
    else
    {
        if (point.X + length >= DISPLAY_WIDTH)
            length = DISPLAY_WIDTH - point.X - 1;
        std::fill(buffer + (uint)(point.Y * DISPLAY_WIDTH + point.X), buffer + (uint)(point.Y * DISPLAY_WIDTH + point.X + length), color);
    }
}

void DisplayDriver::drawVerticalLine(Vector2 point, int16_t length, const uint16_t color)
{
    if (point.X < 0 || point.X >= DISPLAY_WIDTH)
        return;

    if (length < 0)
    {
        length += 1; // convert from base 1 to base 0
        if (point.Y + length < 0)
            length = -point.Y;

        if (point.Y >= DISPLAY_HEIGHT) {
            length = (point.Y+length)-DISPLAY_HEIGHT+1; // +1 cause base 0
            point.Y = 239;
            if (length >= 0)
                return; // no line to be drawn
        }

        uint16_t *base = &buffer[(uint)((point.Y + length) * DISPLAY_WIDTH + point.X)]; // get a pointer to the first pixel (multiply by display size.X because it's left to right)
        for (int16_t h = length; h <= 0; h++)
        { // iterate through the size.Y, <= because base 0
            *base = color;
            base += DISPLAY_WIDTH;
        }
    }
    else
    {
        length -= 1;                               // convert from base 1 to base 0
        if (point.Y + length >= DISPLAY_HEIGHT)    // limit the line length to be within display limits
            length = DISPLAY_HEIGHT - 1 - point.Y; // -1 converts DISPLAY_HEIGHT from base 1 to base 0
        if (point.Y < 0) {
            length = (point.Y+length);
            point.Y = 0;
            if (length <= 0)
                return; // no line to be drawn
        }

        uint16_t *base = &buffer[(uint)(point.Y * DISPLAY_WIDTH + point.X)]; // get a pointer to the first pixel (multiply by display size.X because it's left to right)
        for (int16_t h = 0; h <= length; h++)
        { // iterate through the size.Y, <= because base 0
            *base = color;
            base += DISPLAY_WIDTH;
        }
    }
}

Vector2 textPos2 = Vector2(0, FONT_HEIGHT*2);

void drawTextNewLine2(std::string text) {
    for (char const &character : text) { // loop through all characters
        if (character != '\n') {
            if (textPos2.X + FONT_WIDTH >= DISPLAY_WIDTH) {
                textPos2.X = 0;
                textPos2.Y += FONT_HEIGHT;
            }
            textPos2.X += FONT_WIDTH;
            DisplayDriver::drawChar(textPos2, character, Green);// draw individual character (returns width)
        }
        else {
            textPos2.Y += FONT_HEIGHT;
            textPos2.X = 0;
        }
    }
    textPos2.Y += FONT_HEIGHT;
    textPos2.X = 0;
}

void DisplayDriver::drawLine(Vector2 start, Vector2 end, uint16_t color)
{
    start.round();
    end.round();

    if (start.X == end.X)
    { // handle vertical straight lines
        drawVerticalLine(start, end.Y - start.Y, color);
        return;
    }
    if (start.Y == end.Y)
    { // handle horizontal flat lines
        drawHorizontalLine(start, end.X - start.X, color);
        return;
    }

    if (start.X > end.X)
        std::swap(start, end);

    double slope = (end.Y - start.Y) / (end.X - start.X);
    double offset = start.Y - slope * start.X;
    
    start.Y = std::min(std::max(start.Y, 0.0), DISPLAY_HEIGHT - 1.0); // constrain Y to display, recalculate X
    end.Y = std::min(std::max(end.Y, 0.0), DISPLAY_HEIGHT - 1.0); // constrain Y to display, recalculate X
    start.X = std::min(std::max((start.Y - offset) / slope, 0.0), DISPLAY_WIDTH - 1.0);
    end.X = std::min(std::max((end.Y - offset) / slope, 0.0), DISPLAY_WIDTH - 1.0);
    
    start.Y = slope * start.X + offset;
    end.Y = slope * end.X + offset;

    start.round();
    end.round();

    // sloped lines are handled here

    int16_t oldY = start.Y; // starting y value
    int16_t y;

    for (int16_t x = start.X; x <= end.X; x++)
    {
        double unroundedY = slope * (double)x + offset;
        if (slope < 0) // this is to fix one stupid issue where if the value was 55.5, instead of rounding to 55, it'd go to 56, which isn't quite what I want when compared to how if it's sloping upwards, it rounds to 56 from 55.5
            if (round(unroundedY) - unroundedY == 0.5)
                unroundedY -= 0.1; // make it round down later

        y = round(unroundedY); // calc y
        
        /*if (x == start.X) 
        { // if it's the first one
            drawPixel(Vector2(x, start.Y), color);
            continue;
        } else if (x == end.X) {
            y = end.Y;
        }*/

        if (y != oldY)
        {
            if (oldY > y) // makes the stepping connect to each other so that it looks smoother
                oldY += 1; // makes line 1 longer
            else
                oldY -= 1; // makes line 1 longer (opposite direction of += 1)
            
            drawVerticalLine(Vector2(x, y), oldY - y, color);
                
            oldY = y;
        } else 
            drawPixel(Vector2(x, y), color);
    }
}

void DisplayDriver::drawFace(std::array<Vector2, 4> corners, uint16_t color)
{ // corners = { bottomLeft, bottomRight, topLeft, topRight }

    bool deb = true;
    for (Vector2& corner : corners) {
        corner.round();
        if (corner.X >= 0 && corner.X < DISPLAY_WIDTH && corner.Y >= 0 && corner.Y < DISPLAY_HEIGHT) {
            deb = false;
            break;
        }
    }
    if (deb)
        return; // all points are off screen
    
    int16_t maxY = round(corners[0].Y), 
            minY = maxY; // have to define because default values may be lower than all else

    // define slopes and offsets to be used later for conversion between x and y coordinates
    // the labels next to the slopes/offsets assume an unrotated camera staring at an unrotated cube
    std::array<double, 4> slopes = {
        (corners[0].Y - corners[1].Y) / (corners[0].X - corners[1].X), // bottom edge
        (corners[1].Y - corners[3].Y) / (corners[1].X - corners[3].X), // right edge
        (corners[3].Y - corners[2].Y) / (corners[3].X - corners[2].X), // top edge
        (corners[2].Y - corners[0].Y) / (corners[2].X - corners[0].X) // left edge
    };
    std::array<double, 4> offsets = {
        (corners[0].Y - corners[0].X * slopes[0]), // bottom edge offset
        (corners[1].Y - corners[1].X * slopes[1]), // right edge offset
        (corners[3].Y - corners[3].X * slopes[2]), // top edge offset
        (corners[2].Y - corners[2].X * slopes[3]) // left edge offset
    };                           

    // declares+defines whether these edges are special cases or not (slope = +-inf)
    // note that this doesn't affect horizontal edges because if they've a slope of 0, they just wont be used (maxY == maxY2)
    std::array<bool, 4> specialCases = {
        (corners[0].X == corners[1].X),
        (corners[1].X == corners[3].X),
        (corners[3].X == corners[2].X),
        (corners[2].X == corners[0].X)
    };
    
    // the max coordinates and above top/bottom max/min definitions are for later
    // they're used to determine how the scanline should be calculated for drawing the face
    for (uint8_t i = 0; i < 4; i++)
    {
        Vector2 corner = corners[i];
        corner.X = std::min(std::max(corner.X, 0.0), DISPLAY_WIDTH - 1.0);
        uint8_t lineNum = i < 2 ? 0 : 2; // if point is bottomleft/right, it does line 0, otherwise it does line 2
        // this makes sure we only use slope/offset of top/bottom line, as this is x oriented
        corner.Y = slopes[lineNum] * corner.X + offsets[lineNum];
        corner.round();

        int16_t Y = corner.Y;
        if (Y > maxY)
            maxY = Y;
        else if (Y < minY)
            minY = Y;
    }
    

    std::deque<int16_t> xIntercepts;

    // round the min and max to display limits
    minY = std::min(std::max((double)minY, 0.0), DISPLAY_HEIGHT - 1.0);
    maxY = std::min(std::max((double)maxY, 0.0), DISPLAY_HEIGHT - 1.0);

    uint16_t *base = &buffer[(int)(minY * DISPLAY_WIDTH)]; // make base starting at min point

    for (uint16_t y = minY; y <= maxY; y++)
    {
        // the below if/else is for calculating xIntercepts
        // xIntercepts get the max and min x values on a horizontal line
        // this is used for scanline calculations

        // you'll notice that this repeated if statement switches between >= and > often
        // this is because there can only be one case for if y == corners[1].Y
        // if there are more than 1 case, then it will find too many xInts, and fail
        if ((y <= corners[1].Y && y >= corners[0].Y) || (y >= corners[1].Y && y <= corners[0].Y))
        {
            if (specialCases[0])
                xIntercepts.emplace_back(corners[1].X);
            else
                xIntercepts.emplace_back((y - offsets[0]) / slopes[0]);
        }

        if ((y < corners[1].Y && y >= corners[3].Y) || (y > corners[1].Y && y <= corners[3].Y))
        {
            if (specialCases[1])
                xIntercepts.emplace_back(corners[1].X);
            else
                xIntercepts.emplace_back((y - offsets[1]) / slopes[1]);
        }

        if ((y <= corners[2].Y && y > corners[3].Y) || (y >= corners[2].Y && y < corners[3].Y))
        {
            if (specialCases[2])
                xIntercepts.emplace_back(corners[2].X);
            else
                xIntercepts.emplace_back((y - offsets[2]) / slopes[2]);
        }

        if ((y < corners[2].Y && y > corners[0].Y) || (y > corners[2].Y && y < corners[0].Y))
        {
            if (specialCases[3])
                xIntercepts.emplace_back(corners[2].X);
            else
                xIntercepts.emplace_back((y - offsets[3]) / slopes[3]+1); // strangely, one edge renders one pixel too far left, needs to be investigated further, but this fixes it for now
        }

        std::sort(xIntercepts.begin(), xIntercepts.end()); // sort smallest to largest

        if (xIntercepts.size() == 2)
        { // should be impossible for this to fail, but can't be too certain
            // constrain to display limits
            for (uint8_t i = 0; i < 2; i++)
                xIntercepts[i] = xIntercepts[i] < 0 ? 0 : (xIntercepts[i] >= DISPLAY_WIDTH ? DISPLAY_WIDTH - 1 : xIntercepts[i]);

            // now that we've calculated the scanline, we can render it in
            for (uint16_t x = xIntercepts[0]; x <= xIntercepts[1]; x++)
                *(base + x) = color; // alternatively i could use subscript operator
        }

        base += DISPLAY_WIDTH; // go down one length (following the iteration)
        xIntercepts.clear();
    }
}

void DisplayDriver::drawRect(Vector2 point, Vector2 size, uint16_t backgroundColor)
{
    point = Vector2(round(point.X), round(point.Y));
    if (point.X < 0)
    {
        size.X += point.X;
        point.X = 0;
    }
    if (point.Y < 0)
    {
        size.Y += point.Y;
        point.Y = 0;
    }
    if (point.X < DISPLAY_WIDTH && point.Y < DISPLAY_HEIGHT && size.X > 0 && size.Y > 0)
    {
        uint16_t *base = &buffer[(int)(point.Y * DISPLAY_WIDTH + point.X)]; // get a pointer to the first pixel (multiply by display size.X because it's left to right)

        for (int w = 0; w < size.X; w++)
        { // iterate through the size.X
            if (w + point.X < DISPLAY_WIDTH)
            {
                uint16_t *loc = base + w; // get a pointer to the pixel at offset w

                for (int h = 0; h < size.Y; h++)
                { // iterate through the size.Y
                    if (h + point.Y < DISPLAY_HEIGHT)
                        *(loc + DISPLAY_WIDTH * h) = backgroundColor;
                }
            }
        }
    }
}

void DisplayDriver::drawOutlinedRect(Vector2 point, Vector2 size, uint16_t backgroundColor, uint16_t borderThickness, uint16_t borderColor)
{
    uint16_t *base = &buffer[(int)(point.Y * DISPLAY_WIDTH + point.X)]; // get a pointer to the first pixel (multiply by display size.X because it's left to right)

    for (int w = 0; w < size.X; w++)
    { // iterate through the size.X
        if (w + point.X < DISPLAY_WIDTH)
        {
            uint16_t *loc = base + w; // get a pointer to the pixel at offset w

            for (int h = 0; h < size.Y; h++)
            { // iterate through the size.Y
                if (h + point.Y < DISPLAY_HEIGHT)
                {
                    if (w < borderThickness || h < borderThickness || (size.X - w) <= borderThickness || (size.Y - h) <= borderThickness) // <= cause the greater range never gets hit (0-239, never touches 240)
                        *(loc + DISPLAY_WIDTH * h) = borderColor;                                                                         // add DISPLAY_WIDTH to loc, dereferencing pointer so I can write to it
                    // adding DISPLAY_WIDTH because we need to move down one pixel, which is the same as moving across the entire display once
                    else
                        *(loc + DISPLAY_WIDTH * h) = backgroundColor;
                }
            }
        }
    }
}

int DisplayDriver::drawChar(Vector2 point, char c, uint16_t Color)
{                                                                                                // font size is currently forced
    const uint8_t(*characterData)[FONT_HEIGHT] = &fontData[static_cast<const uint8_t>(c - ' ')]; // this is 12 bytes wide, of which each byte represents one vertical pixel, and holds 8 horizontal pixels
    // c - ' ' because the character list doesn't include non printable ascii, instead opting to start at the space character

    uint16_t *base = &buffer[(int)(point.X + point.Y * DISPLAY_WIDTH)];

    for (uint8_t yCoord = 0; yCoord < FONT_HEIGHT; yCoord++)
    {                                               // iterates each "row" of the character (each y coordinate)
        uint8_t rowData = (*characterData)[yCoord]; // dereferences the pointer to the uint8_t which stores the 8 x coordinates for each y coordinate
        uint16_t *xRow = base + yCoord * DISPLAY_WIDTH;

        for (uint8_t xCoord = 0; xCoord < FONT_WIDTH; xCoord++)
        {
            if (rowData & (1 << (8 - xCoord)))
            {   // checks if the bit is a 1 or a 0, if it's 1, write a pixel, if 0, don't
                // (8 - xCoord) because it has to be left to right
                *(xRow + xCoord) = Color;
            }
        }
    }

    return FONT_WIDTH; // size.X of each character
}

void DisplayDriver::drawCenteredText(Vector2 point, std::string text, uint16_t textColor)
{                                                             // centered around (x, y)
    uint width = static_cast<uint>(text.size()) * FONT_WIDTH; // font is 8 px wide
    point -= Vector2(width / 2, FONT_HEIGHT / 2);

    for (char &character : text)
    {                                                     // loop through all characters
        point.X += drawChar(point, character, textColor); // draw individual character (returns size.X)
    }
}

void DisplayDriver::drawText(Vector2 point, std::string text, uint16_t textColor)
{ // x, y is the top left corner, renders left to right
    double origX = point.X;
    for (char &character : text) // loop through all characters
    {
        if (character != '\n' || point.X >= (DISPLAY_WIDTH-FONT_WIDTH)) // \n support, I should write this into every text function, but I'm lazy rn
            point.X += drawChar(point, character, textColor); // draw individual character (returns size.X)
        else {
            point.Y += FONT_HEIGHT;
            point.X = origX;
        }
    }
}

// ALT text functions are a hacky fix to make text bigger.  A better fix would be a proper scaling font system, but that sounds annoying to implement, and I'd likely end up rarely using it
int DisplayDriver::ALTdrawChar(Vector2 point, char c, uint16_t Color)
{                                                                                          // font size is currently forced
    const uint8_t(*characterData)[FONT_HEIGHT] = &fontData[static_cast<uint8_t>(c - ' ')]; // this is 12 bytes wide, of which each byte represents one vertical pixel, and holds 8 horizontal pixels
    // c - ' ' because the character list doesn't include non printable ascii, instead opting to start at the space character

    uint16_t *base = &buffer[(int)(point.X + point.Y * DISPLAY_WIDTH)];

    for (uint8_t yCoord = 0; yCoord < FONT_HEIGHT * 2; yCoord += 2)
    {                                                   // iterates each "row" of the character (each y coordinate)
        uint8_t rowData = (*characterData)[yCoord / 2]; // divide by 2 because this font is 2x bigger
        uint16_t *xRow = base + yCoord * DISPLAY_WIDTH;
        uint16_t *xRow2 = base + (yCoord + 1) * DISPLAY_WIDTH;

        for (uint8_t xCoord = 0; xCoord < FONT_WIDTH * 2; xCoord += 2)
        { // font is 2x bigger than normal
            if (rowData & (1 << (8 - xCoord / 2)))
            {                                    // checks if the bit is a 1 or a 0, if it's 1, write a pixel, if 0, don't
                *(xRow + xCoord) = Color;        // top left
                *(xRow + (xCoord + 1)) = Color;  // bottom left
                *(xRow2 + xCoord) = Color;       // top right
                *(xRow2 + (xCoord + 1)) = Color; // bottom right
            }
        }
    }

    return FONT_WIDTH * 2; // size.X of each character
}

void DisplayDriver::ALTdrawCenteredText(Vector2 point, std::string text, uint16_t textColor)
{                                                                 // alternate is a hacky way to render 2x bigger text
    uint width = static_cast<uint>(text.size()) * FONT_WIDTH * 2; // font is 2x wider than normal
    int startX = point.X - width / 2;                             // subtract half the size.X to reverse the offset
    int startY = point.Y - FONT_HEIGHT;                           // this font is 2x bigger than normal, and so it cancells out the divide by 2

    for (char &character : text)
    {                                                                         // loop through all characters
        startX += ALTdrawChar(Vector2(startX, startY), character, textColor); // draw individual character (returns size.X, which is useless because text only has 2 possible size.Xes right now)
    }
}

void DisplayDriver::renderRect(Vector2 point, Vector2 size, uint16_t rectColor)
{ // custom optimized render function
    if (point.X + size.X > DISPLAY_WIDTH)
        size.X = DISPLAY_WIDTH - point.X;
    if (point.Y + size.Y > DISPLAY_HEIGHT)
        size.Y = DISPLAY_HEIGHT - point.Y;

    uint16_t *newBuffer = new uint16_t[(uint)(size.X * size.Y)];
    std::fill(newBuffer, newBuffer + (uint)(size.X * size.Y), rectColor);

    writeCommand(COMMAND_COLUMN_ADDRESS_SET);          // set column address, meaning x pixel
    writeCommandParameter(((int)point.X) >> 8 & 0xFF); // starting x pos
    writeCommandParameter(((int)point.X) & 0xFF);
    writeCommandParameter(((int)point.X + (uint)size.X - 1) >> 8 & 0xFF);
    writeCommandParameter(((int)point.X + (uint)size.X - 1) & 0xFF); // -1 because it's base 0 (idk, that doesnt intuitively make sense to me, but it works...)

    writeCommand(COMMAND_ROW_ADDRESS_SET);             // set row address, meaning y pixel
    writeCommandParameter(((int)point.Y) >> 8 & 0xFF); // starting y pos
    writeCommandParameter(((int)point.Y) & 0xFF);
    writeCommandParameter(((int)point.Y + (uint)size.Y - 1) >> 8 & 0xFF); // ending y pos
    writeCommandParameter(((int)point.Y + (uint)size.Y - 1) & 0xFF);      // -1 because it's base 0 (idk, that doesnt intuitively make sense to me, but it works...)

    writeCommand(COMMAND_MEMORY_WRITE);

    writeData(newBuffer, size.X * size.Y * sizeof(uint16_t)); // give it the base pointer

    delete[] newBuffer;
    newBuffer = nullptr;
}

void DisplayDriver::renderOutlinedRect(Vector2 point, Vector2 size, uint16_t backgroundColor, uint16_t borderThickness, uint16_t borderColor)
{
    int newWidth = size.X;
    int newHeight = size.Y;
    if (point.X + size.X > DISPLAY_WIDTH)
        newWidth = DISPLAY_WIDTH - point.X;
    if (point.Y + size.Y > DISPLAY_HEIGHT)
        newHeight = DISPLAY_HEIGHT - point.Y;

    uint16_t *newBuffer = new uint16_t[newWidth * newHeight];
    std::fill(newBuffer, newBuffer + newWidth * newHeight, backgroundColor);

    for (int newX = 0; newX < newWidth; newX++)
    {
        for (int newY = 0; newY < newHeight; newY++)
        {
            if (newX < (borderThickness) || newX >= (size.X - borderThickness) || newY < borderThickness || newY >= (size.Y - borderThickness))
                *(newBuffer + newX + newY * newWidth) = borderColor;
        }
    }

    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(((int)point.X) >> 8 & 0xFF);
    writeCommandParameter(((int)point.X) & 0xFF);
    writeCommandParameter(((int)point.X + newWidth - 1) >> 8 & 0xFF); // get the upper byte of the size.X
    writeCommandParameter(((int)point.X + newWidth - 1) & 0xFF);      // get the lower byte of the size.X

    writeCommand(COMMAND_ROW_ADDRESS_SET);                             // set row address, meaning y pixel
    writeCommandParameter(((int)point.Y) >> 8 & 0xFF);                 // the command takes 4 bytes of data
    writeCommandParameter(((int)point.Y) & 0xFF);                      // the first 2 are the first y coordinate, starting at px 0
    writeCommandParameter(((int)point.Y + newHeight - 1) >> 8 & 0xFF); // the last 2 are the last y coordinate
    writeCommandParameter(((int)point.Y + newHeight - 1) & 0xFF);      // get the lower byte of the size.Y

    writeCommand(COMMAND_MEMORY_WRITE);

    writeData(newBuffer, newWidth * newHeight * sizeof(uint16_t)); // give it the base pointer

    delete[] newBuffer;
    newBuffer = nullptr;
}

void DisplayDriver::renderText(Vector2 point, std::string text, uint16_t textColor, uint16_t backgroundColor)
{ // x, y is the top left corner, renders left to right
    uint width = static_cast<int>(text.size()) * FONT_WIDTH;
    uint height = FONT_HEIGHT;

    uint16_t *newBuffer = new uint16_t[width * height]; // allocate a buffer for the entire text size
    std::fill(newBuffer, newBuffer + width * height, backgroundColor);

    writeCommand(COMMAND_COLUMN_ADDRESS_SET); // set column address, meaning x pixel
    writeCommandParameter(((int)point.X) >> 8 & 0xFF);
    writeCommandParameter(((int)point.X) & 0xFF);
    writeCommandParameter(((int)point.X + width - 1) >> 8 & 0xFF);
    writeCommandParameter(((int)point.X + width - 1) & 0xFF);

    writeCommand(COMMAND_ROW_ADDRESS_SET); // set row address, meaning y pixel
    writeCommandParameter(((int)point.Y) >> 8 & 0xFF);
    writeCommandParameter(((int)point.Y) & 0xFF);
    writeCommandParameter(((int)point.Y + height - 1) >> 8 & 0xFF);
    writeCommandParameter(((int)point.Y + height - 1) & 0xFF);

    writeCommand(COMMAND_MEMORY_WRITE);

    int xPos = 0;

    for (char &character : text)
    {                                                                                                  // loop through all characters
        const uint8_t(*characterData)[FONT_HEIGHT] = &fontData[static_cast<uint8_t>(character - ' ')]; // this is 12 bytes wide, of which each byte represents one vertical pixel, and holds 8 horizontal pixels
        // c - ' ' because the character list doesn't include non printable ascii, instead opting to start at the space character
        uint16_t *base = &newBuffer[xPos];
        xPos += FONT_WIDTH;

        for (uint8_t yCoord = 0; yCoord < FONT_HEIGHT; yCoord++)
        {                                               // iterates each "row" of the character (each y coordinate)
            uint8_t rowData = (*characterData)[yCoord]; // dereferences the pointer to the uint8_t which stores the 8 x coordinates for each y coordinate
            uint16_t *xRow = base + yCoord * width;

            for (uint8_t xCoord = 0; xCoord < FONT_WIDTH; xCoord++)
            {
                if (rowData & (1 << (8 - xCoord)))
                {   // checks if the bit is a 1 or a 0, if it's 1, write a pixel, if 0, don't
                    // (8 - xCoord) because it's left to right
                    *(xRow + xCoord) = textColor;
                }
            }
        }
    }

    writeData(newBuffer, width * height * sizeof(uint16_t)); // write the buffer

    delete[] newBuffer; // deallocate to save memory
    newBuffer = nullptr;
}

const Vector3 stockCubeVertexPositions[8] = {
    Vector3(-1, -1, -1), // 0, 0, 0
    Vector3(1, -1, -1),  // X, 0, 0
    Vector3(-1, 1, -1),  // 0, Y, 0
    Vector3(-1, -1, 1),  // 0, 0, Z
    Vector3(1, 1, -1),   // X, Y, 0
    Vector3(1, -1, 1),   // X, 0, Z
    Vector3(-1, 1, 1),   // 0, Y, Z
    Vector3(1, 1, 1)     // X, Y, Z
};                       // oriented around center

void DisplayDriver::calculateVerticesFacesAndEdges(std::deque<Cube> *cubes, std::deque<Vector3> *vertices, std::deque<std::array<uint16_t, 2>> *edges, std::deque<std::array<uint16_t, 4>> *faces)
{
    uint16_t count = 0;

    for (Cube const &cube : *cubes)
    {
        Vector3 cubeRotationRad = cube.Rotation * deg2rad;
        Rx[1][1] = cos(cubeRotationRad.X);
        Rx[2][1] = -sin(cubeRotationRad.X);
        Rx[1][2] = sin(cubeRotationRad.X);
        Rx[2][2] = cos(cubeRotationRad.X);

        Ry[0][0] = cos(cubeRotationRad.Y);
        Ry[2][0] = sin(cubeRotationRad.Y);
        Ry[0][2] = -sin(cubeRotationRad.Y);
        Ry[2][2] = cos(cubeRotationRad.Y);

        Rz[0][0] = cos(cubeRotationRad.Z);
        Rz[1][0] = -sin(cubeRotationRad.Z);
        Rz[0][1] = sin(cubeRotationRad.Z);
        Rz[1][1] = cos(cubeRotationRad.Z);

        Matrix cubeRotationMatrix = Rz * Ry * Rx;
        Matrix vertexPositionMatrix = Matrix(1, 3);
        halfSize = cube.Size / 2;

        for (Vector3 const &pos : stockCubeVertexPositions)
        {
            vertexPositionMatrix = pos * halfSize;
            vertices->emplace_back(Vector3(cubeRotationMatrix * vertexPositionMatrix) + cube.Position);
        }
        // in the context of edges, count+x is a way to tell which vertices line up.
        // the number x represents which push_back is pushing the vertex.
        // the vertex offset is based on stockCubeVertexPositions's definition

        edges->emplace_back(std::array<uint16_t, 2>{count, static_cast<uint16_t>(count + 1)}); // 0->X
        edges->emplace_back(std::array<uint16_t, 2>{count, static_cast<uint16_t>(count + 2)}); // 0->Y
        edges->emplace_back(std::array<uint16_t, 2>{count, static_cast<uint16_t>(count + 3)}); // 0->Z

        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 1), static_cast<uint16_t>(count + 4)}); // X->XY
        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 1), static_cast<uint16_t>(count + 5)}); // X->XZ

        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 2), static_cast<uint16_t>(count + 4)}); // Y->XY
        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 2), static_cast<uint16_t>(count + 6)}); // Y->YZ

        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 3), static_cast<uint16_t>(count + 6)}); // Z->YZ
        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 3), static_cast<uint16_t>(count + 5)}); // Z->XZ

        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 7), static_cast<uint16_t>(count + 4)}); // XYZ->XY
        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 7), static_cast<uint16_t>(count + 6)}); // XYZ->YZ
        edges->emplace_back(std::array<uint16_t, 2>{static_cast<uint16_t>(count + 7), static_cast<uint16_t>(count + 5)}); // XYZ->XZ

        // in the context of faces, count+x is a way to tell which edges line up.
        // the number x represents which push_back is pushing the edge.
        // For example, count+0 is the 0->X edge

        faces->emplace_back(std::array<uint16_t, 4>{
            count,                            // 0,0 corner
            static_cast<uint16_t>(count + 1), // X, 0 corner
            static_cast<uint16_t>(count + 2), // 0, Y corner
            static_cast<uint16_t>(count + 4)  // X, Y corner
        });                                   // XY face

        count += 8;
    }
}
