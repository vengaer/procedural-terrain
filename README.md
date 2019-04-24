# Procedural Terrain  
Procedural terrain created using OpenGL

## Dependencies
- GLEW
- GLFW
- GLM

## Compiling
The easiest way to compile the program is by using the supplied Makefile. In order for it to work, the dependencies listed above need to be accessible through the PATH variable or similarly. The code has been tested with Clang 8.0.0 and GCC 8.3.0.

If not using the Makefile, make sure the src directory and all its subdirectories are added as include directories.

### Customizing
- Logging can be customized by defining one of the following project wide
    - LOG\_ERROUT\_DEFULE - Default logging level, stdin and stdout only, no log file.
    - LOG\_ERROUT\_VERBOSE - Verbose logging, stdin and stdout only, no log file.
    - LOG\_FULL\_DEFAULT - Default logging level, stdin, stdout and log file.
    - LOG\_FULL\_VERBOSE - Verbose logging, stdin, stdout and log file.
- Live reloading of shaders and compression of potential log files is enabled by default. To disable this, define RESTRICT\_THREAD\_USAGE project wide.
