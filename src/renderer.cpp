#include "renderer.h"
#include <iostream>

// Clearing the GL errors
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (" << error << ")" << std::endl;

        // This part doen't really matter with Q_ASSERT, it will print it
        std::cout << "In function : " << function << std::endl << "In file : "
                  << file << std::endl << "In line : " << line << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer()
{

}
