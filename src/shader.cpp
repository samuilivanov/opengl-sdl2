#include "shader.h"
#include "renderer.h"

Shader::Shader(const std::string &filename)
    : m_FilePath(filename), m_RendererID(0)
{
    //"/home/samuil/projects/opengl-sdl2/res/shaders/basic.shader"
    ShaderProgramScource source = parseShader(filename);
    m_RendererID = createShader(source.VertexSource,
                                       source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

ShaderProgramScource Shader::parseShader(const std::string filepath)
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

unsigned int Shader::compileShader(const std::string &source,
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

void Shader::unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::setUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::createShader(const std::string &vertexShader,
                        const std::string &fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = compileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

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

unsigned int Shader::getUniformLocation(const std::string &name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }
    GLCall(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));

    m_UniformLocationCache[name] = location;
    return location;
}
