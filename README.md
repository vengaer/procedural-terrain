# Procedural Terrain  
Purely procedural terrain created using OpenGL. 

[![Build Status](https://gitlab.com/vilhelmengstrom/procedural-terrain/badges/master/pipeline.svg)](https://gitlab.com/vilhelmengstrom/procedural-terrain/commits/master)

Uses periodic noise functions for generating the rocks and the ripples on the water. Reflection and refraction is achieved by blending textures captured with the camera positioned appropriately during prior render passes. The bloom of the sun is achieved through "ping ponging" and downsampling frame buffer objects.
### Dependencies
- [GLEW](http://glew.sourceforge.net)
- [GLFW](https://www.glfw.org)
- [GLM](https://glm.g-truc.net)
- [STB](https://github.com/nothings/stb)[^1]

### Preview (Click to View on Youtube)

[![Preview](http://img.youtube.com/vi/ObkWN27MURQ/0.jpg)](http://www.youtube.com/watch?v=ObkWN27MURQ "Preview")

### Compiler Compatibility

- :heavy_check_mark: GCC
- :heavy_check_mark: Clang
- :heavy_multiplication_x: MSVC[^2]

### Compiling

Ensure that GLEW, GLFW and GLM are available in your `PATH` and simply run
```
make
```

Note that due to the heavy reliance on metaprogramming, it may be desireable to run parallel make to speed up compilation. The following runs make, spawning a number of jobs equal to the number of cores available
```
make -j$(nproc)
```

#### Shader Live Reloading
The application will continuously monitor active shader source files for changes. If a source file is updated, the program automatically reloads that shader. For this to work properly, all uniforms have to be uploaded to the new shader program. Currently, only the model, view and projection matrices are reuploaded automatically, meaning some shaders will not reload properly.

### Credits
The water is a pure-procedural implementation of concepts introduced in a series on [ThinMatrix' Youtube channel](https://www.youtube.com/user/ThinMatrix). 

### Footnotes
[^1]: Included in assets/include
[^2]: Not verified, but due to prior experience of their poor SFINAE-support, I strongly suspect it would not compile. If wanting to run this on windows, look into [MSYS](https://www.msys2.org/) instead.

