#include <iostream>
// Glew has to be before SDL2
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_opengl.h>

static unsigned int CompileShader(const std::string &source,
                                  unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);

        char* message = (char*)alloca(lenght * sizeof (char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "Failed to compile!\n" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
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
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    // TODO: go throud the docs
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // TODO: glDetackShader should be used here
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;

}

int main()
{
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "Could not create window : " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //auto context = SDL_GL_CreateContext(window);

    // Initialise OpenGL with Glew, get the access to OpenGl functions
    if (glewInit() != GLEW_OK) {
        std::cout << "Error\n";
    }
    // print OpenGl version
    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    uint32_t buffer;
    glGenBuffers(1, &buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof (float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof (float) * 2, nullptr);

    std::string vertexShader =
            "#version 300 es\n"
            "\n"
            "layout(location = 0) in vec4 position;\n"
            "\n"
            "void main(void)\n"
            "{\n"
            "   gl_Position = position;\n"
            "}\n";

    std::string fragmentShader =
            "#version 300 es\n"
            "\n"
            "precision lowp float;"
            "layout(location = 0) out vec4 color;\n"
            "\n"
            "void main(void)\n"
            "{\n"
            "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
            "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    while (1) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }

        }
        SDL_RenderClear(renderer);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_RenderPresent(renderer);



    }
    return 0;
}
