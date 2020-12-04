#include <renderer/Renderer2D.h>
#include <core/Application.h>
#include <renderer/MeshFactory.h>
#include <renderer/shader/ShaderFactory.h>
#include <maths/matrix/matrix_transform.h>
#include <renderer/RenderCommand.h>

#include <GL/glew.h>

Renderer2DData Renderer2D::data;
Shared<Mesh> Renderer2D::m_textMesh;
Shared<Mesh> Renderer2D::m_framebufferMesh;

void Renderer2D::init()
{
    RenderCommand::setDepthTesting(false);

    auto windowSize = Application::get().getWindow().getSize();

    data.projectionMatrix = math::ortho(0.f, (float)windowSize.x, 0.f, (float)windowSize.y, -1.f, 1.f);
    
    data.textureShader = ShaderFactory::textureShader();
    data.textureShader->bind();
    data.textureShader->setMatrix4("projection", data.projectionMatrix);

    data.textShader = ShaderFactory::textShader();
    data.textShader->bind();
    data.textShader->setMatrix4("projection", data.projectionMatrix);

    m_textMesh = MeshFactory::textMesh();

    data.whiteTexture = Texture2D::create(1, 1);
    uint32_t white = 0xffffff;
    data.whiteTexture->setData(0, 0, 1, 1, &white);

    data.transform = math::mat4();
    data.mesh.vertexArray = VertexArray::create();
    data.mesh.vertexArray->bind();

    BufferLayout layout = {
        { Shader::DataType::Vec3, "aPos"      },
        { Shader::DataType::Vec2, "aTexCoord" },
        { Shader::DataType::Vec4, "aColor"    }
    };

    data.mesh.indexBuffer = IndexBuffer::create(data.MAX_SPRITES * 6, IndexDataType::UInt32);
    int offset = 0;
    for (unsigned int i = 0; i < data.MAX_SPRITES * 6; i += 6)
    {
        data.indices.push_back(offset + 0);
        data.indices.push_back(offset + 1);
        data.indices.push_back(offset + 2);

        data.indices.push_back(offset + 2);
        data.indices.push_back(offset + 3);
        data.indices.push_back(offset + 0);

        offset += 4;
    }

    data.mesh.indexBuffer->setData(&data.indices[0], data.indices.size());

    data.mesh.vertexBuffer = VertexBuffer::create(sizeof(Vertex) * 4 * data.MAX_SPRITES);
    data.mesh.vertexBuffer->setLayout(layout);

    data.mesh.vertexArray->addVertexBuffer(data.mesh.vertexBuffer);
    data.mesh.vertexArray->setIndexBuffer(data.mesh.indexBuffer);
}

void Renderer2D::startBatch(const math::mat4& transform)
{
    data.transform = transform;
    data.drawCalls = 0;
}

void Renderer2D::endBatch()
{
    if (data.vertices.size() > 0)
    {
        flushBatch();
    }
}

void Renderer2D::flushBatch()
{
    if (data.activeTexture == nullptr)
        return;

    RenderCommand::setDepthTesting(false);

    data.mesh.vertexArray->bind();

    data.mesh.vertexBuffer->setData(&data.vertices[0], sizeof(Vertex) * data.vertices.size());
    data.mesh.indexBuffer->setData(&data.indices[0], data.indices.size());

    data.textureShader->bind();
    data.textureShader->setMatrix4("transform", data.transform);
    data.textureShader->setMatrix4("projection", data.projectionMatrix);
    data.activeTexture->bind();

    RenderCommand::renderIndexed(data.mesh.vertexArray, data.vertices.size() * 6/4);
    data.drawCalls++;
    data.vertices.clear();
}

void Renderer2D::renderSprite(const Shared<Texture2D>& texture, const math::vec2& position, const math::vec2& size)
{
    renderSprite(texture, position, size, math::frect(0, 0, texture->getWidth(), texture->getHeight()), 0, math::vec4(1, 1, 1, 1));
}

void Renderer2D::renderSprite(const Shared<Texture2D>& texture, const math::vec2& position, const math::vec2& size, const math::vec4& color)
{
    renderSprite(texture, position, size, math::frect(0, 0, texture->getWidth(), texture->getHeight()), 0, color);
}

void Renderer2D::renderSprite(const Shared<Texture2D>& texture, const math::vec2& position, const math::vec2& size, const math::frect& texRect)
{
    renderSprite(texture, position, size, texRect, 0, math::vec2(), math::vec4(1, 1, 1, 1));
}

void Renderer2D::renderSprite(const Shared<Texture2D>& texture, const math::vec2& position, const math::vec2& size, const math::frect& texRect, float rotation, math::vec4 color)
{
    renderSprite(texture, position, size, texRect, rotation, math::vec2(), color);
}

void Renderer2D::renderSprite(const Shared<Texture2D>& texture, const math::vec2& position, const math::vec2& size, const math::frect& texRect, float rotation, const math::vec2& origin, math::vec4 color)
{
    if (data.activeTexture == nullptr)
    {
        data.activeTexture = texture;
    }
    if (data.activeTexture->getId() != texture->getId())
    {
        flushBatch();
        data.activeTexture = texture;
    }

    if (data.vertices.size() > data.MAX_SPRITES * 4 - 100)
    {
        flushBatch();
    }

    Transform t = { position, rotation, size, origin };
    math::mat4 transform = t.matrix();
    
    std::array<Vertex, 4> vertices;

    // Populate the vertices array with the sprite's vertices
    for (unsigned int i = 0 ; i < 4 ; i++)
    {
        math::vec4 pos = transform * math::vec4(data.quadPositions[i]);
        vertices[i].position = math::vec3(pos.x, pos.y, 0);

        // Change "origin" of texCoord, then scale it
        
        // Reciprocal of size (save division operations)
        math::vec2 textureSize(1.f / texture->getWidth(), 1.f / texture->getHeight());

        vertices[i].texCoord = texRect.getPosition() * textureSize;
        vertices[i].texCoord += data.quadPositions[i] * (texRect.getSize() * textureSize);
        vertices[i].color = color;
    }

    for (auto& vertex : vertices)
    {
        data.vertices.push_back(vertex);
    }
}

void Renderer2D::renderQuad(const math::vec2& position, const math::vec2& size, const math::vec4& color)
{
    renderQuad(position, size, 0, color);
}

void Renderer2D::renderQuad(const math::vec2& position, const math::vec2& size, float rotation, const math::vec4& color)
{
    renderSprite(data.whiteTexture, position, size, math::frect(0, 0, 1, 1), rotation, color);
}

void Renderer2D::renderText(const std::string& text, const Shared<TrueTypeFont>& font, const math::vec2& position, const math::vec4& color)
{
    renderText(text, font, position, math::vec2(font->getCharacterSize()), color);
}

void Renderer2D::renderText(const std::string& text, const Shared<TrueTypeFont>& font, const math::vec2& position, const math::vec2& size, const math::vec4& color)
{
    struct GlyphVertex
    {
        math::vec2 position;
        math::vec2 texCoord;
    };

    math::vec2 scale = size / font->getCharacterSize();
    std::vector<unsigned int> indices;

    uint32_t quadIndices[] = {
        0, 1, 2, 2, 3, 0
    };

    // Set the indices
    for (unsigned int i = 0 ; i < text.size() ; i++)
    for (unsigned int j = 0; j < 6; j++)
    {
        indices.push_back(quadIndices[j] + i * 4);
    }

    m_textMesh->indexBuffer->setData(&indices[0], indices.size());

    int x = position.x;
    int y = position.y;

    GlyphVertex* coords = new GlyphVertex[4 * text.size()];

    // Set render states
    data.textShader->bind();
    data.textShader->setMatrix4("transform", math::mat4(1.f));
    data.textShader->setFloat4("textColor", math::vec4(color.r, color.g, color.b, color.a));
    m_textMesh->vertexArray->bind();
    font->getTextureAtlas()->bind();

    RenderCommand::setBlend(true);
    RenderCommand::setBlendFunction(BlendFunction::SourceAlpha, BlendFunction::OneMinusSourceAlpha);

    int n = 0;

    // Loop through each character in the string, and add its texCoords to the mesh
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        auto& ch = font->getGlyphs().at(*c);

        math::vec2 pos(x + ch.pos.x * scale.x, -y - ch.pos.y * scale.y);
        math::vec2 size = ch.size * scale;

        x += ch.advance.x * scale.x;
        y += ch.advance.y * scale.y;

        if (!size.x || !size.y)
            continue;

        coords[n++] = { math::vec2(pos.x,          -pos.y),          math::vec2(ch.texOffset,                                      0) };
        coords[n++] = { math::vec2(pos.x + size.x, -pos.y),          math::vec2(ch.texOffset + ch.size.x / font->getAtlasSize().x, 0) };
        coords[n++] = { math::vec2(pos.x + size.x, -pos.y - size.y), math::vec2(ch.texOffset + ch.size.x / font->getAtlasSize().x, ch.size.y / font->getAtlasSize().y) };
        coords[n++] = { math::vec2(pos.x,          -pos.y - size.y), math::vec2(ch.texOffset,                                      ch.size.y / font->getAtlasSize().y) };
    }

    // Draw the text mesh
    m_textMesh->vertexBuffer->bind();
    m_textMesh->vertexBuffer->setData(coords, sizeof(coords));
    RenderCommand::renderIndexed(m_textMesh->vertexArray);

    RenderCommand::setBlend(false);

    delete[] coords;
}

void Renderer2D::render(IRenderable2D& renderable)
{
    renderable.render();
}

