#pragma once

#include "UserInputHandler.h"
#include "DisplayDriver.h"
#include "Cube.h"
#include <deque>
#include <array>
#include <iomanip>

class DisplayTests {
    public:
        static void startRectTest();
        static void startFullScreenTest();
        static void renderDevStats();
        static void directRenderTest();
        static void test3D();
};