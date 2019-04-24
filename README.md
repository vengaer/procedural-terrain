# Procedural Terrain  
Procedural terrain created using OpenGL.

## Dependencies
- [GLEW](glew.sourceforge.net)
- [GLFW](https://www.glfw.org)
- [GLM](https://glm.g-tru.net)

## Compiling
The easiest way to compile the program is by using the supplied Makefile. In order for it to work, the dependencies listed above need to be accessible through the PATH variable or similarly. The code has been tested with Clang 8.0.0 and GCC 8.3.0.

If not using the Makefile, make sure the src directory and all its subdirectories are added as include directories.

### Customization
- Logging can be enabled and customized by defining one of the following project wide. By default, no logging is performed.
    - LOG\_ERROUT\_DEFAULT - Default logging level, stdin and stdout only, no log file.
    - LOG\_ERROUT\_VERBOSE - Verbose logging, stdin and stdout only, no log file.
    - LOG\_FULL\_DEFAULT - Default logging level, stdin, stdout and log file.
    - LOG\_FULL\_VERBOSE - Verbose logging, stdin, stdout and log file.  
  To explicitly disable logging, define LOG\_DISABLE\_ALL project wide.
- Live reloading of shaders and compression of potential log files is enabled by default. To disable this, define RESTRICT\_THREAD\_USAGE project wide.

## Credits
A few aspects, including the water, were inspired by content on [ThinMatrix' Youtube channel](https://www.youtube.com/user/ThinMatrix).
