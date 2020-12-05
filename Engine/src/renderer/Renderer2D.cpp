#include <renderer/Renderer2D.h>
#include <core/Application.h>
#include <renderer/MeshFactory.h>
#include <renderer/shader/ShaderFactory.h>
#include <maths/matrix/matrix_transform.h>
#include <renderer/RenderCommand.h>

#include <GL/glew.h>

Renderer2DData Renderer2D::s_data;
Shared<Mesh> Renderer2D::m_textMesh;
Shared<Mesh> Renderer2D::m_framebufferMesh;

void Renderer2D::init()
{
    auto windowSize = Application::get().getWindow().getSize();

    s_data.matrixData = UniformBuffer::create(sizeof(math::mat4) * 2, 2);
    
    s_data.textureShader = ShaderFactory::textureShader();
    s_data.textShader = ShaderFactory::textShader();

    m_textMesh = MeshFactory::textMesh();

    s_data.whiteTexture = Texture2D::create(1, 1);
    uint32_t white = 0xffffff;
    s_data.whiteTexture->setData(0, 0, 1, 1, &white);

    s_data.vertexBase = new Vertex[s_data.MAX_VERTICES];

    s_data.mesh.vertexArray = VertexArray::create();
    s_data.mesh.vertexArray->bind();

    BufferLayout layout = {
        { Shader::DataType::Vec3,  "aPos"      },
        { Shader::DataType::Vec2,  "aTexCoord" },
        { Shader::DataType::Vec4,  "aColor"    },
        { Shader::DataType::Float, "aTexIndex" }
    };
    
    uint32_t* indices = new uint32_t[s_data.MAX_INDICES];

    int offset = 0;
    for (unsigned int i = 0; i < s_data.MAX_SPRITES * 6; i += 6)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;

        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
    }

    s_data.mesh.indexBuffer = IndexBuffer::create(indices, s_data.MAX_SPRITES * 6, IndexDataType::UInt32);

    delete[] indices;
 
    s_data.mesh.vertexBuffer = VertexBuffer::create(sizeof(Vertex) * 4 * s_data.MAX_SPRITES);
    s_data.mesh.vertexBuffer->setLayout(layout);

    s_data.mesh.vertexArray->addVertexBuffer(s_data.mesh.vertexBuffer);
    s_data.mesh.vertexArray->setIndexBuffer(s_data.mesh.indexBuffer);
}

void Renderer2D::shutdown()
{
    delete[] s_data.vertexBase;
}

void Renderer2D::beginScene(OrthographicCamera& camera)
{
    s_data.camera = &camera;
    s_data.drawCalls = 0;

    s_data.matrixData->setData(math::buffer(s_data.camera->getProjectionMatrix()), sizeof(math::mat4), 0);
    s_data.matrixData->setData(math::buffer(s_data.camera->getViewMatrix()), sizeof(math::mat4), sizeof(math::mat4));

    startBatch();
}

void Renderer2D::endScene()
{
    flushBatch();
}

void Renderer2D::startBatch()
{
    s_data.textureSlotIndex = 0;
    s_data.vertexPointer = s_data.vertexBase;
    s_data.indexCount = 0;
}

void Renderer2D::nextBatch()
{
    flushBatch();
    startBatch();
}

void Renderer2D::flushBatch()
{
    if (s_data.indexCount == 0)
        return;

    RenderCommand::setDepthTesting(false);

    s_data.mesh.vertexArray->bind();

    size_t dataSize = (size_t)((uint8_t*)s_data.vertexPointer - (uint8_t*)s_data.vertexBase);
    s_data.mesh.vertexBuffer->setData(s_data.vertexBase, dataSize);

    s_data.textureShader->bind();
    
    for (unsigned int i = 0; i < s_data.textureSlotIndex; i++)
    {
        s_data.textureSlots[i]->bind(i);
    }

    RenderCommand::renderIndexed(s_data.mesh.vertexArray, s_data.indexCount);
    s_data.drawCalls++;
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
    float textureIndex = 0.f;
    for (unsigned int i = 0; i < s_data.textureSlotIndex; i++)
    {
        if (*s_data.textureSlots[i] == *texture)
        {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.f)
    {
        if (s_data.textureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS)
        {
            nextBatch();
        }

        textureIndex = (float)s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex] = texture;
        s_data.textureSlotIndex++;
    }

    if (s_data.indexCount >= s_data.MAX_INDICES)
    {
        flushBatch();
    }

    Transform t = { position, rotation, size, origin };
    math::mat4 transform = t.matrix();
    
    // Populate the vertices array with the sprite's vertices
    for (unsigned int i = 0 ; i < 4 ; i++)
    {
        math::vec4 pos = transform * math::vec4(s_data.quadPositions[i]);
        s_data.vertexPointer->position = math::vec3(pos.x, pos.y, 0);

        // Change "origin" of texCoord, then scale it
        
        // Reciprocal of size (save division operations)
        math::vec2 textureSize(1.f / texture->getWidth(), 1.f / texture->getHeight());
        math::vec2 texCoord = texRect.getPosition() * textureSize;
        texCoord += s_data.quadPositions[i] * (texRect.getSize() * textureSize);

        s_data.vertexPointer->texCoord = texCoord;
        s_data.vertexPointer->color = color;
        s_data.vertexPointer->texIndex = textureIndex;
        s_data.vertexPointer++;
    }

    s_data.indexCount += 6;
}

void Renderer2D::renderQuad(const math::vec2& position, const math::vec2& size, const math::vec4& color)
{
    renderQuad(position, size, 0, color);
}

void Renderer2D::renderQuad(const math::vec2& position, const math::vec2& size, float rotation, const math::vec4& color)
{
    renderSprite(s_data.whiteTexture, position, size, math::frect(0, 0, 1, 1), rotation, color);
}

void Renderer2D::renderText(const std::string& text, const Shared<TrueTypeFont>& font, const math::vec2& position, const math::vec4& color)
{
    renderText(text, font, position, math::vec2(font->getCharacterSize()), color);
}

void Renderer2D::renderText(const std::string& text, const Shared<TrueTypeFont>& font, const math::vec2& position, const math::vec2& size, const math::vec4& color)
{
    RenderCommand::setDepthTesting(false);
    struct GlyphVertex
    {
        math::vec2 position;
        math::vec2 texCoord;
    };

    math::vec2 scale = size / font->getCharacterSize();
    std::vector<uint32_t> indices;

    uint32_t quadIndices[] = {
        0, 1, 2, 2, 3, 0
    };

    // Set the indices
    for (unsigned int i = 0 ; i < text.size() ; i++)
    for (unsigned int j = 0; j < 6; j++)
    {
        indices.push_back(quadIndices[j] + i * 4);
    }
    m_textMesh->vertexArray->bind();
    m_textMesh->indexBuffer->setData(&indices[0], indices.size());

    int x = position.x;
    int y = position.y;

    GlyphVertex* coords = new GlyphVertex[4 * text.size()];

    // Set render states
    s_data.textShader->bind();
    s_data.textShader->setFloat4("textColor", math::vec4(color.r, color.g, color.b, color.a));
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
    m_textMesh->vertexBuffer->setData(coords, sizeof(GlyphVertex) * (4 * text.size()));
    RenderCommand::renderIndexed(m_textMesh->vertexArray, 6 * text.size());

    RenderCommand::setBlend(false);

    delete[] coords;
}

void Renderer2D::render(IRenderable2D& renderable)
{
    renderable.render();
}

