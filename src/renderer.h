#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <QtCore/QtGlobal>
#include "vertexarray.h"
#include "indexbuffer.h"
#include "shader.h"

#define ASSERT(x)  Q_ASSERT(x)
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// Clearing the GL errors
void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);

class Renderer
{
public:
    void draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;
    void clear() const;
};

#endif //RENDERER_H
