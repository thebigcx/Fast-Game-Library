#include "renderer/Quad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Quad::Quad()
{
    
}

Quad::Quad(const Vector2f& position, const Vector2f& size, const Color& color)
: m_color(color)
{
    m_position = position;
    m_size = size;
    m_texRect = FloatRect(0, 0, 1, 1);
}

void Quad::move(Vector2f direction)
{
    m_position += direction;
}

std::array<Vertex, 4> Quad::getVertices() const
{
    // Transform the vertices
    auto transform = getTransform();

    Vertex a;

    auto pos = transform * glm::vec4(0, 0, 0, 1);
    a.position = Vector2f(pos.x, pos.y);

    a.color = m_color;
    a.texCoord = m_texRect.getPosition();

    Vertex b;

    pos = transform * glm::vec4(1, 0, 0, 1);
    b.position = Vector2f(pos.x, pos.y);

    b.color = m_color;
    b.texCoord = Vector2f(m_texRect.x + m_texRect.width, m_texRect.y);

    Vertex c;

    pos = transform * glm::vec4(1, 1, 0, 1);
    c.position = Vector2f(pos.x, pos.y);

    c.color = m_color;
    c.texCoord = Vector2f(m_texRect.x + m_texRect.width, m_texRect.y + m_texRect.height);

    Vertex d;

    pos = transform * glm::vec4(0, 1, 0, 1);
    d.position = Vector2f(pos.x, pos.y);
    d.color = m_color;

    d.texCoord = Vector2f(m_texRect.x, m_texRect.y + m_texRect.height);

    return { a, b, c, d };
}

void Quad::setTextureRect(const FloatRect& rect)
{
    m_texRect = rect;
}

void Quad::setColor(const Color& color)
{
    m_color = color;
}