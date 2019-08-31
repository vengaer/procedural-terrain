# Procedural Terrain  
Purely procedural terrain created using OpenGL.

[![Build Status](https://gitlab.com/vilhelmengstrom/procedural-terrain/badges/master/build.svg)](https://gitlab.com/vilhelmengstrom/procedural-terrain/commits/master)

### Dependencies
- [GLEW](http://glew.sourceforge.net)
- [GLFW](https://www.glfw.org)
- [GLM](https://glm.g-truc.net)
- [STB](https://github.com/nothings/stb) (Included in assets/include)

### Compiling
The easiest way to compile the program is by using the supplied Makefile. In order for it to work, the dependencies listed above need to be accessible through the PATH variable or similarly. The code has been tested with Clang 8.0.0 and GCC 8.3.0.

If not using the Makefile, make sure the src directory, all its subdirectories and assets/include/stb are all added as include directories.

#### Customization
- Logging can be enabled and customized by defining one of the following project wide. By default, no logging is performed.
    - `LOG_ERROUT_DEFAULT` - Default logging level, stdin and stdout only, no log file.
    - `LOG_ERROUT_VERBOSE` - Verbose logging, stdin and stdout only, no log file.
    - `LOG_FULL_DEFAULT` - Default logging level, stdin, stdout and log file.
    - `LOG_FULL_VERBOSE` - Verbose logging, stdin, stdout and log file.  
    - `LOG_DISABLE_ALL` - Explicitly disable all logging (the default).
- Live reloading of shaders and compression of potential log files is enabled by default. To disable this, define RESTRICT\_THREAD\_USAGE project wide.

### Credits
The water was inspired by a series on [ThinMatrix' Youtube channel](https://www.youtube.com/user/ThinMatrix).
