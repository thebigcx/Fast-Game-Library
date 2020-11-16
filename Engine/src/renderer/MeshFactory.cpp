#include <renderer/MeshFactory.h>

Shared<Mesh> MeshFactory::textMesh()
{
    auto text = createShared<Mesh>();
    
    text->vertexArray = VertexArray::create();
    text->vertexArray->bind();

    BufferLayout layout = {
        { Shader::DataType::Vec2, "aPos" },
        { Shader::DataType::Vec2, "aTexCoord" }
    };
    text->indexBuffer = IndexBuffer::create(0);

    text->vertexBuffer = VertexBuffer::create(sizeof(float) * 4 * 200);
    text->vertexBuffer->setLayout(layout);
    text->vertexArray->addVertexBuffer(text->vertexBuffer);
    text->vertexArray->setIndexBuffer(text->indexBuffer);

    return text;
}

Shared<Mesh> MeshFactory::quadMesh(float x1, float y1, float x2, float y2)
{
    auto quad = createShared<Mesh>();

    quad->vertexArray = VertexArray::create();
    quad->vertexArray->bind();

    BufferLayout layout = {
        { Shader::DataType::Vec2, "aPos" },
        { Shader::DataType::Vec2, "aTexCoord" }
    };

    uint32_t indices[] = {
        0, 1, 2, 2, 3, 0
    };

    float vertices[] = {
        x1, y1, 0, 0,
        x1, y2, 0, 1,
        x2, y2, 1, 1,
        x2, y1, 1, 0
    };

    quad->indexBuffer = IndexBuffer::create(6);
    quad->indexBuffer->update(indices, 6);

    quad->vertexBuffer = VertexBuffer::create(sizeof(float) * 4 * 4);
    quad->vertexBuffer->update(vertices, 16 * sizeof(float));
    quad->vertexBuffer->setLayout(layout);
    quad->vertexArray->addVertexBuffer(quad->vertexBuffer);
    quad->vertexArray->setIndexBuffer(quad->indexBuffer);

    return quad;
}

Shared<Mesh> MeshFactory::cubeMesh(float size)
{
    auto quad = createShared<Mesh>();

    quad->vertexArray = VertexArray::create();
    quad->vertexArray->bind();

    BufferLayout layout = {
        { Shader::DataType::Vec3, "aPos" },
        { Shader::DataType::Vec2, "aTexCoord" },
        { Shader::DataType::Vec4, "aColor" }
    };

    uint32_t indices[] = {
        0, 1, 2, 2, 3, 0
    };

    float vertices[] = {
        0,    0,    0, 0, 0, 1, 1, 1, 1,
        0,    size, 0, 0, 1, 1, 1, 1, 1,
        size, size, 0, 1, 1, 1, 1, 1, 1,
        size, 0,    0, 1, 0, 1, 1, 1, 1
    };

    quad->indexBuffer = IndexBuffer::create(6);
    quad->indexBuffer->update(indices, 6);

    quad->vertexBuffer = VertexBuffer::create(sizeof(float) * 9 * 4);
    quad->vertexBuffer->update(vertices, 9 * 4 * sizeof(float));
    quad->vertexBuffer->setLayout(layout);
    quad->vertexArray->addVertexBuffer(quad->vertexBuffer);
    quad->vertexArray->setIndexBuffer(quad->indexBuffer);

    return quad;
}