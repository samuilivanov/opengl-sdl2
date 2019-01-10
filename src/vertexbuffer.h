#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H


class VertexBuffer
{
private:
    // unique ID for every object created
    unsigned int m_RendererID;

public:
    VertexBuffer(const void *data, unsigned int size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
};

#endif // VERTEXBUFFER_H
