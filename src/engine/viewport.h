#ifndef VIEWPORT_H
#define VIEWPORT_H

#pragma once
#include <GL/glew.h>

struct Viewport {
    static void update();
    static void update(int width, int height);

    static int width;
    static int height;
};

#endif
