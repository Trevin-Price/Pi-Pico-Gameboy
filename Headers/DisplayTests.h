#pragma once

#include "UserInputHandler.h"
#include "DisplayDriver.h"

class DisplayTests {
    public:
        static void startRectTest();
        static void startFullScreenTest();
        static void renderDevStats();
        static void directRenderTest();
        static void test3D();
};