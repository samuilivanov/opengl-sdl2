#include <iostream>

// Glew has to be before SDL2
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <sstream>

// Shoudl probably replace that with the C FILE
#include <fstream>
#include "renderer.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"


struct ShaderProgramScource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramScource ParseShader(const std::string filepath)
{
    std::ifstream stream;
    stream.open(filepath, std::ios::in);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    std::stringstream ss[2];

    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;

            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            ss[static_cast<int>(type)] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(const std::string &source,
                                  unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int32_t lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);

        // Really like this alloca function
        char* message = static_cast<char*>(alloca(
                        static_cast<uint32_t>(lenght) * sizeof (char)));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "Failed to compile!\n" <<
                        (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                        << std::endl;
        std::cout <<message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

// Shader source code
static unsigned int CreateShader(const std::string &vertexShader,
                        const std::string &fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    // TODO: go throud the docs
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    // TODO: glDetackShader should be used here
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main()
{
    // Setting up for FPS 60
    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
    uint32_t frameStart;
    uint32_t frameTime;
    // end

    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 640, 480,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "Could not create window : "
                  << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialise OpenGL with Glew, get the access to OpenGl functions
    if (glewInit() != GLEW_OK) {
        std::cout << "Error\n";
    }
    // print OpenGl version
    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
        -0.5f, -0.5f,   // idx 0
         0.5f, -0.5f,   // idx 1
         0.5f,  0.5f,   // idx 2
        -0.5f,  0.5f    // idx 3
    };

    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    VertexBuffer vb(positions, 4 * 2 * sizeof (float));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                                 sizeof (float) * 2, nullptr));

    IndexBuffer ib(indices, 6);

    ShaderProgramScource source = ParseShader("/home/samuil/projects/opengl-sdl2/res/shaders/basic.shader");

    // Test the parsing code
    std::cout << source.VertexSource << std::endl;
    std::cout << source.FragmentSource << std::endl;
    // end test

    unsigned int shader = CreateShader(source.VertexSource,
                                       source.FragmentSource);
    GLCall(glUseProgram(shader));

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

    // Unbounding
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


    float r = 0.0f;
    float increment = 0.05f;

    while (1) {

        frameStart = SDL_GetTicks();

        // Handle the closing of the window
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }

        } // end of closing the window

        SDL_RenderClear(renderer);

        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glUseProgram(shader));

        // The color of the rect
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        // Bounding the vertex array objects
        GLCall(glBindVertexArray(vao));
        // Bounding the index buffer object
        ib.bind();
        // Draw on the screen
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r > 1.0f) {
            increment = -0.05f;
        } else if (r < 0.0f) {
            increment = 0.05f;
        }

        r += increment;

        SDL_RenderPresent(renderer);

        // Making framerate to be 60fps
        frameTime = SDL_GetTicks() - frameStart;
        if (SCREEN_TICKS_PER_FRAME > frameTime) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTime);
        }
    }

    glDeleteProgram(shader);
    return 0;
}
