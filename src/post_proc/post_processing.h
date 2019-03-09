#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#pragma once
#include "bloom.h"
#include "blur.h"
#include "canvas.h"
#include "mix.h"
#include "shader.h"
#include "shader_handler.h"
#include "viewport.h"

class PostProcessing {
    public:
        PostProcessing();
        
        void perform() const;
        static bool enabled();

    private:
        HorizontalBlur h_blur_;
        VerticalBlur v_blur_;
        Bloom bloom_;
        Mix mix_{};
        Canvas<manual_shader_handler> canvas_{};
    
        static bool instantiated_;
};

#endif
