#include "GameLoader.h"

uint GameLoader::Selection = 0;

void GameLoader::RenderMainMenu() {

    Display.fillBuffer(Blue);

    for (int i = 5; i < 105; i++) {
        for (int x = 5; x < 25; x++)
            Display.drawPixel(x, i, White);
    }
    
    //Display.drawRect(0, 0, 1, 100, White, 1, Black);
    
    Display.renderBuffer();
}