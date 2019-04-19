#include "context.h"
#include "viewport.h"

void Viewport::update() {
    auto const* primary_context = static_cast<Context*>(glfwGetWindowUserPointer(glfwGetCurrentContext()))->primary();
    glfwGetWindowSize(static_cast<GLFWwindow*>(*primary_context), &Viewport::width, &Viewport::height);
}

void Viewport::update(int widht, int height) {
    Viewport::width = widht;
    Viewport::height = height;
}

int Viewport::width = 960u;
int Viewport::height = 540u;
