# Procedural Terrain  
Purely procedural terrain created using OpenGL.

[![Build Status](https://gitlab.com/vilhelmengstrom/procedural-terrain/badges/master/pipeline.svg)](https://gitlab.com/vilhelmengstrom/procedural-terrain/commits/master)

### Dependencies
- [GLEW](http://glew.sourceforge.net)
- [GLFW](https://www.glfw.org)
- [GLM](https://glm.g-truc.net)
- [STB](https://github.com/nothings/stb) (Included in assets/include)

### Compiling
The easiest way to compile the program is by using the supplied Makefile. In order for it to work, the dependencies listed above (not including STB) need to be accessible through the `PATH` variable or similar. 

If not using the Makefile, make sure the src directory, all its subdirectories and assets/include/stb are all added as include directories.

#### Customization
- Logging can be enabled and customized by defining one of the following project wide. By default, no logging is performed.
    - `LOG_ERROUT_DEFAULT` - Default logging level, stdin and stdout only, no log file.
    - `LOG_ERROUT_VERBOSE` - Verbose logging, stdin and stdout only, no log file.
    - `LOG_FULL_DEFAULT` - Default logging level, stdin, stdout and log file.
    - `LOG_FULL_VERBOSE` - Verbose logging, stdin, stdout and log file.  
    - `LOG_DISABLE_ALL` - Explicitly disable all logging (the default).
- Live reloading of shaders and compression of potential log files is enabled by default. To disable this, define `RESTRICT_THREAD_USAGE` project wide.

#### Shader live reloading
The application will continuously monitor active shader source files for changes. If a source file is updated, the program automatically reloads that shader. For this to work properly, all uniforms have to be uploaded to the new shader program. Currently, only the model, view and projection matrices are reuploaded automatically.

### Credits
The water was inspired by a series on [ThinMatrix' Youtube channel](https://www.youtube.com/user/ThinMatrix).
