#include "context.h"
#include "viewport.h"

void Viewport::update() {
    auto const* primary_context = static_cast<Context*>(glfwGetWindowUserPointer(glfwGetCurrentContext()))->primary();
    glfwGetWindowSize(static_cast<GLFWwindow*>(*primary_context), &Viewport::width, &Viewport::height);
}

int Viewport::width = 960u;
int Viewport::height = 540u;
