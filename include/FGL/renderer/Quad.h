#pragma once

#include <cstdint>

#include "../util/maths/Vector2.h"
#include "../util/Color.h"

class Quad
{
public:
    Quad(const Vector2f& position, const Vector2f& size, const Color& color);

    const Vector2f& getPosition()
    {
        return m_position;
    }

    friend class Batch;

private:
    Vector2f m_position;
    Vector2f m_size;

    static inline const unsigned int m_indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    Vector2f m_positions[4];
    Color m_colors[4];
};