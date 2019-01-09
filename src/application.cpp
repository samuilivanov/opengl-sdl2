#include <iostream>

// Glew has to be before SDL2
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <sstream>
#include <QtCore/QtGlobal>

// Shoudl probably replace that with the c FILE
#include <fstream>

#define ASSERT(x)  Q_ASSERT(x)
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// Clearing the GL errors
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (" << error << ")" << std::endl;

        // This part doen't really matter with Q_ASSERT it will print it
        std::cout << "In function : " << function << std::endl << "In file : "
                  << file << std::endl << "In line : " << line << std::endl;
        return false;
    }
    return true;
}

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

    //auto context = SDL_GL_CreateContext(window);

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

    uint32_t buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof (float),
                        positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                                 sizeof (float) * 2, nullptr));

    uint32_t ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof (uint32_t),
                                            indices, GL_STATIC_DRAW));

    ShaderProgramScource source = ParseShader("/home/samuil/projects/opengl-sdl2/res/shaders/basic.shader");

    // Test the parsing code
    std::cout << source.VertexSource << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource,
                                       source.FragmentSource);
    GLCall(glUseProgram(shader));

    while (1) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }

        }
        SDL_RenderClear(renderer);

        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        SDL_RenderPresent(renderer);
    }

    glDeleteProgram(shader);
    return 0;
}
