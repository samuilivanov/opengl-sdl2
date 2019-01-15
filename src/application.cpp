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
#include "vertexarray.h"
#include "vertexbufferlayout.h"
#include "shader.h"

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

    VertexArray va;
    VertexBuffer vb(positions, 4 * 2 * sizeof (float));

    VertexBufferLayout layout;
    layout.push<float>(2);
    va.addBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    Shader shader("/home/samuil/projects/opengl-sdl2/res/shaders/basic.shader");
    shader.bind();
    shader.setUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

    va.unbind();
    vb.unbind();
    ib.unbind();
    shader.unbind();

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

        shader.bind();
        shader.setUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
        va.bind();
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

    return 0;
}
