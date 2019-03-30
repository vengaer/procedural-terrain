#ifndef TILEABLE_NOISE_H
#define TILEABLE_NOISE_H

#pragma once
#include "texture.h"


namespace tileable_noise {

    float generate(float x, float y, unsigned period);
    Texture texture();

}

#endif
