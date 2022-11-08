#include "GameLoader.h"

uint GameLoader::Selection = 0;

void GameLoader::RenderMainMenu() {

    Display.setEntireBuffer(DarkGrey);
    
    Display.renderBuffer();
}